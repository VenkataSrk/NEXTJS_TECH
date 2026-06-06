
// Import Firebase libraries
importScripts("https://www.gstatic.com/firebasejs/9.6.11/firebase-app-compat.js");
importScripts("https://www.gstatic.com/firebasejs/9.6.11/firebase-messaging-compat.js");

// Initialize Firebase
firebase.initializeApp({
  apiKey: "AIzaSyCsLt2QXdQOMMn4VUNSWeLdPqU3zgUuK58",
  authDomain: "campaignwebpush.firebaseapp.com",
  projectId: "campaignwebpush",
  storageBucket: "campaignwebpush.appspot.com",
  messagingSenderId: "610883259513",
  appId: "1:610883259513:web:0154759264bfa46275d8a8",
  measurementId: "G-PBRTEGFZ8W"
});

// Get Firebase Messaging instance
const messaging = firebase.messaging();

// Handle background messages
messaging.onBackgroundMessage((payload) => {
  console.log("📩 Background notification received:", payload);

  const data = payload.data || {};
  const title = data.title || "Notification";
  const body = data.body || "";
  const icon = data.icon || "/logo192.png";
  const image = data.image || null;
  console.log("🔔 Showing notification:", { title, body, icon, image });
  const options = {
    body,
    icon,
    image,
    badge: "/logo192.png"
  };

  self.registration.showNotification(title, options);
});

// IndexedDB setup
function openDB() {
  return new Promise((resolve, reject) => {
    const request = indexedDB.open("MyDatabase", 1);

    request.onerror = () => reject(request.error);
    request.onsuccess = () => resolve(request.result);

    request.onupgradeneeded = (event) => {
      const db = event.target.result;
      if (!db.objectStoreNames.contains("keyval")) {
        db.createObjectStore("keyval");
      }
    };
  });
}

async function getTokenIndexDB(key) {
  const db = await openDB();
  return new Promise((resolve, reject) => {
    const tx = db.transaction("keyval", "readonly");
    const store = tx.objectStore("keyval");
    const req = store.get(key);

    req.onsuccess = () => resolve(req.result);
    req.onerror = () => reject(req.error);
  });
}

// Handle notification click
self.addEventListener("notificationclick", (event) => {
  console.log("🔔 Notification clicked:", event.notification.data);
  event.notification.close();

  event.waitUntil(
    (async () => {
      if (event.notification.data) {
        try {
          const token = await getTokenIndexDB("tokenId");
          const baseurl = await getTokenIndexDB("baseurl");

          await fetch(`${baseurl}/insert_webpush_onclick`, {
            method: "POST",
            headers: {
              "Content-Type": "application/json",
            },
            body: JSON.stringify({
              domainId: event.notification.data?.domainId,
              campaignChatId: event.notification.data?.campaignChatId,
              tokens: token,
              status: "OPEN",
              description: "notification click open rate",
            }),
          });
        } catch (err) {
          console.error("❌ Failed to update notification status:", err);
        }
      }
      await clients.openWindow(event.notification.data?.webUrl);
    })()
  );
});
