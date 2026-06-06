import { getApp, getApps, initializeApp } from "firebase/app";
import { getMessaging, getToken, isSupported, onMessage } from "firebase/messaging";

export class WebPushSdk {
  constructor(config) {
    this.config = config;
    this.app = null;
    this.messaging = null;

    this.log("constructor", "SDK initialized", {
      serviceWorkerPath: config.serviceWorkerPath ?? "/firebase-messaging-sw.js",
      serviceWorkerScope: config.serviceWorkerScope ?? null,
    });
  }

  log(step, message, data) {
    if (data !== undefined) {
      console.log(`[WebPushSDK] ${step}: ${message}`, data);
      return;
    }
    console.log(`[WebPushSDK] ${step}: ${message}`);
  }

  ensureFirebaseApp() {
    this.log("ensureFirebaseApp", "Checking Firebase app");
    if (this.app) {
      this.log("ensureFirebaseApp", "Using cached app");
      return this.app;
    }

    const hasExistingApp = getApps().length > 0;
    this.app = hasExistingApp ? getApp() : initializeApp(this.config.firebaseConfig);
    this.log("ensureFirebaseApp", "Firebase app ready", { hasExistingApp });
    return this.app;
  }

  async registerServiceWorker() {
    this.log("registerServiceWorker", "Starting registration");
    if (typeof window === "undefined" || !("serviceWorker" in navigator)) {
      this.log("registerServiceWorker", "Service worker not supported");
      return null;
    }

    const registration = await navigator.serviceWorker.register(
      this.config.serviceWorkerPath ?? "/firebase-messaging-sw.js",
      {
        scope: this.config.serviceWorkerScope,
      }
    );
    this.log("registerServiceWorker", "Registered", { scope: registration.scope });
    return registration;
  }

  async getMessagingClient() {
    this.log("getMessagingClient", "Preparing messaging");
    if (this.messaging) {
      this.log("getMessagingClient", "Using cached messaging");
      return this.messaging;
    }

    if (typeof window === "undefined") {
      this.log("getMessagingClient", "Window unavailable");
      return null;
    }

    const supported = await isSupported();
    this.log("getMessagingClient", "Support check done", { supported });
    if (!supported) {
      return null;
    }

    this.messaging = getMessaging(this.ensureFirebaseApp());
    this.log("getMessagingClient", "Messaging initialized");
    return this.messaging;
  }

  async requestPermission() {
    this.log("requestPermission", "Checking permission");
    if (typeof window === "undefined" || typeof Notification === "undefined") {
      this.log("requestPermission", "Notification API unavailable");
      return "denied";
    }

    if (Notification.permission === "granted") {
      this.log("requestPermission", "Already granted");
      return Notification.permission;
    }

    const permission = await Notification.requestPermission();
    this.log("requestPermission", "Permission resolved", { permission });
    return permission;
  }

  async getWebPushToken(options = {}) {
    this.log("getWebPushToken", "Token flow started", options);
    const shouldRequestPermission = options.requestPermission ?? true;

    if (shouldRequestPermission) {
      const permission = await this.requestPermission();
      if (permission !== "granted") {
        this.log("getWebPushToken", "Stopped: permission denied", { permission });
        return null;
      }
    } else if (typeof Notification !== "undefined" && Notification.permission !== "granted") {
      this.log("getWebPushToken", "Stopped: permission not granted and prompt disabled");
      return null;
    }

    const messagingClient = await this.getMessagingClient();
    if (!messagingClient) {
      this.log("getWebPushToken", "Stopped: messaging unavailable");
      return null;
    }

    const registration = await this.registerServiceWorker();
    this.log("getWebPushToken", "Calling firebase getToken");
    const token = await getToken(messagingClient, {
      vapidKey: this.config.vapidKey,
      serviceWorkerRegistration: registration ?? undefined,
    });
    this.log("getWebPushToken", "getToken completed", {
      hasToken: !!token,
      tokenPreview: token ? `${token.slice(0, 20)}...` : null,
    });

    if (token && options.onToken) {
      this.log("getWebPushToken", "Calling onToken callback");
      await options.onToken(token);
    }

    return token || null;
  }

  getBrowserName() {
    if (typeof navigator === "undefined") {
      return "Unknown";
    }

    const ua = navigator.userAgent;
    if (ua.includes("Chrome") && !ua.includes("Edg") && !ua.includes("OPR")) return "Chrome";
    if (ua.includes("Edg")) return "Edge";
    if (ua.includes("Firefox")) return "Firefox";
    if (ua.includes("Safari") && !ua.includes("Chrome")) return "Safari";
    if (ua.includes("OPR") || ua.includes("Opera")) return "Opera";
    return "Unknown";
  }

  async insertFcmToken(options) {
    const { token, browser, domainId, customer, insertFcm } = options;
    this.log("insertFcmToken", "Posting token to API", {
      endpoint: insertFcm.endpoint,
      domainId,
      browser: browser || this.getBrowserName(),
    });

    const headers = {
      "Content-Type": "application/json",
      ...(insertFcm.headers || {}),
    };

    const payload = {
      TokenId: token,
      browser: browser || this.getBrowserName(),
      ...(domainId !== undefined ? { domainId } : {}),
      ...(customer || {}),
    };

    const response = await fetch(insertFcm.endpoint, {
      method: "POST",
      headers,
      body: JSON.stringify(payload),
      credentials: insertFcm.credentials,
    });
    this.log("insertFcmToken", "API response", { ok: response.ok, status: response.status });

    return response.ok;
  }

  async subscribeAndInsertFcm(options) {
    this.log("subscribeAndInsertFcm", "Combined flow started", {
      endpoint: options.insertFcm.endpoint,
      domainId: options.domainId,
      baseUrl: options.baseUrl,
    });

    const token = await this.getWebPushToken(options);
    if (!token) {
      this.log("subscribeAndInsertFcm", "No token available");
      return { token: null, inserted: false };
    }

    let inserted = false;
    try {
      inserted = await this.insertFcmToken({
        token,
        browser: options.browser,
        domainId: options.domainId,
        customer: options.customer,
        insertFcm: options.insertFcm,
      });
    } catch (error) {
      this.log("subscribeAndInsertFcm", "Insert call exception", error);
      inserted = false;
    }

    if (options.setIndexDbValue) {
      this.log("subscribeAndInsertFcm", "Saving token/baseurl using external IndexedDB helper");
      await options.setIndexDbValue("tokenId", token);
      if (options.baseUrl) {
        await options.setIndexDbValue("baseurl", options.baseUrl);
      }
    } else {
      this.log("subscribeAndInsertFcm", "Saving token/baseurl using SDK IndexedDB helper");
      await this.setIndexDbValue("tokenId", token);
      if (options.baseUrl) {
        await this.setIndexDbValue("baseurl", options.baseUrl);
      }
    }

    this.log("subscribeAndInsertFcm", "Combined flow completed", { inserted, hasToken: true });
    return { token, inserted };
  }

  async subscribeWithLoginStyleConfig(options) {
    this.log("subscribeWithLoginStyleConfig", "Login-style flow started", options);
    const baseUrl = (options.baseUrl || "").replace(/\/+$/, "");
    const insertPath = options.insertFcmPath || "/insert_fcm_token_config";
    const endpoint = options.baseUrl
      ? `${baseUrl}${insertPath.startsWith("/") ? insertPath : `/${insertPath}`}`
      : insertPath;
    this.log("subscribeWithLoginStyleConfig", "Resolved endpoint", { endpoint });

    return this.subscribeAndInsertFcm({
      requestPermission: options.requestPermission,
      browser: options.browser || this.getBrowserName(),
      domainId: options.domainId,
      customer: options.customer,
      baseUrl: options.baseUrl,
      setIndexDbValue: options.setIndexDbValue,
      insertFcm: {
        endpoint,
      },
    });
  }

  openDB() {
    this.log("openDB", "Opening IndexedDB");
    return new Promise((resolve, reject) => {
      const request = indexedDB.open("MyDatabase", 1);

      request.onerror = () => {
        this.log("openDB", "Open failed", request.error);
        reject(request.error);
      };
      request.onsuccess = () => {
        this.log("openDB", "Open success");
        resolve(request.result);
      };
      request.onupgradeneeded = (event) => {
        const db = event.target.result;
        this.log("openDB", "Upgrade needed");
        if (!db.objectStoreNames.contains("keyval")) {
          db.createObjectStore("keyval");
          this.log("openDB", "Created object store keyval");
        }
      };
    });
  }

  async setIndexDbValue(key, value) {
    this.log("setIndexDbValue", "Writing IndexedDB key", { key });
    const db = await this.openDB();
    return new Promise((resolve, reject) => {
      const tx = db.transaction("keyval", "readwrite");
      const store = tx.objectStore("keyval");
      const req = store.put(value, key);
      req.onsuccess = () => {
        this.log("setIndexDbValue", "Write success", { key });
        resolve(true);
      };
      req.onerror = () => {
        this.log("setIndexDbValue", "Write failed", { key, error: req.error });
        reject(req.error);
      };
    });
  }

  async getAndPersistWebPushToken(options = {}) {
    this.log("getAndPersistWebPushToken", "Token + persist flow started");
    const token = await this.getWebPushToken(options);
    if (!token) {
      this.log("getAndPersistWebPushToken", "No token available");
      return null;
    }

    await this.persistWebPushToken({
      token,
      browser: options.browser,
      baseUrl: options.baseUrl,
      domainId: options.domainId,
      insertTokenToDb: options.insertTokenToDb,
      setIndexDbValue: options.setIndexDbValue,
      onTokenPersisted: options.onTokenPersisted,
    });

    this.log("getAndPersistWebPushToken", "Token + persist flow completed");
    return token;
  }

  async persistWebPushToken(options) {
    this.log("persistWebPushToken", "Persist started", {
      domainId: options.domainId,
      hasInsertCallback: !!options.insertTokenToDb,
      hasIndexDbCallback: !!options.setIndexDbValue,
    });
    const {
      token,
      browser,
      baseUrl,
      insertTokenToDb,
      setIndexDbValue,
      onTokenPersisted,
    } = options;

    if (insertTokenToDb) {
      this.log("persistWebPushToken", "Calling insertTokenToDb callback");
      await insertTokenToDb({ token, browser });
    }

    if (setIndexDbValue) {
      this.log("persistWebPushToken", "Saving token/baseurl via provided callback");
      await setIndexDbValue("tokenId", token);
      if (baseUrl) {
        await setIndexDbValue("baseurl", baseUrl);
      }
    }

    if (onTokenPersisted) {
      this.log("persistWebPushToken", "Calling onTokenPersisted callback");
      await onTokenPersisted(token);
    }
    this.log("persistWebPushToken", "Persist completed");
  }

  async listenForegroundMessages(handler) {
    this.log("listenForegroundMessages", "Attaching listener");
    const messagingClient = await this.getMessagingClient();
    if (!messagingClient) {
      this.log("listenForegroundMessages", "Messaging unavailable, noop returned");
      return () => {};
    }

    this.log("listenForegroundMessages", "Listener attached");
    return onMessage(messagingClient, handler);
  }
}

const defaultFirebaseConfig = {
  apiKey: "AIzaSyCsLt2QXdQOMMn4VUNSWeLdPqU3zgUuK58",
  authDomain: "campaignwebpush.firebaseapp.com",
  projectId: "campaignwebpush",
  storageBucket: "campaignwebpush.appspot.com",
  messagingSenderId: "610883259513",
  appId: "1:610883259513:web:0154759264bfa46275d8a8",
  measurementId: "G-PBRTEGFZ8W",
};

export const campaignWebPushSdk = new WebPushSdk({
  firebaseConfig: defaultFirebaseConfig,
  vapidKey: "BD-Y7Qftct_1IUMGgtN3TgZ80z7PJqNDegqq3RefslFL_3EkDZjbXbKAg_4sveEZY_Fm8EdJMhwT2FTd5semeus",
  serviceWorkerPath: "/firebase-messaging-sw.js",
});

if (typeof window !== "undefined") {
  console.log("[WebPushSDK] globalInit: attaching WorktualWebPushSDK to window");
  window.WorktualWebPushSDK = {
    version: "1.0.0",
    createWebPushSdk: (config) => new WebPushSdk(config),
    defaultSdk: campaignWebPushSdk,
    subscribeWithCampaignDefaults: (options) =>
      campaignWebPushSdk.subscribeWithLoginStyleConfig(options),
  };
}
