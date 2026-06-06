import CryptoJS from 'crypto-js';

const toEncrypt: string[] = [];

export const GetItemFromStorage = (key: string) => {
    const initialValue = null;
    if (typeof window === 'undefined') return initialValue;
    try {
        const item = window.localStorage.getItem(key);
        if (toEncrypt.includes(key) && item) {
            const secretKey = import.meta.env.REACT_APP_LOCAL_STORAGE_SECRET_KEY;
            if (!secretKey) throw new Error('Missing encryption key');
            const decrypted = CryptoJS.AES.decrypt(item, secretKey).toString(CryptoJS.enc.Utf8);
            if (decrypted) {
                return JSON.parse(decrypted);
            } else {
                clearSessionData();
                window.location.href = '/login';
            }
        }
        return item ? JSON.parse(item) : initialValue;
    } catch (error) {
        console.error('GetItemFromStorage error:', error);
        return initialValue;
    }
};

export const SetItemInStorage = (key: string, value: any) => {
    try {
        if (typeof window !== 'undefined') {
            if (toEncrypt.includes(key)) {
                const secretKey = import.meta.env.REACT_APP_LOCAL_STORAGE_SECRET_KEY;
                if (!secretKey) throw new Error('Missing encryption key');
                const encrypted = CryptoJS.AES.encrypt(JSON.stringify(value), secretKey).toString();
                window.localStorage.setItem(key, encrypted);
            } else {
                window.localStorage.setItem(key, JSON.stringify(value));
            }
        }
    } catch (error) {
        console.error('SetItemInStorage error:', error);
    }
};

export const ClearStorage = () => {
    try {
        if (typeof window !== 'undefined') {
            window.localStorage.clear();
        }
    } catch (error) {
        console.error('ClearStorage error:', error);
    }
};

const clearSessionData = () => {
    localStorage.clear();
    sessionStorage.clear();

    const host = window.location.hostname;
    const domainParts = host.split('.');
    domainParts.shift();
    const domain = domainParts.join('.');

    document.cookie.split(';').forEach((ck) => {
        document.cookie = ck
            .replace(/^ +/, '')
            .replace(/=.*/, `=;expires=${new Date().toUTCString()};path=/${domain}`);
    });
};

// 🔹 Open (or create) IndexedDB
function openDB(): Promise<IDBDatabase> {
  return new Promise((resolve, reject) => {
    const request = indexedDB.open("MyDatabase", 1);

    request.onerror = () => reject(request.error);
    request.onsuccess = () => resolve(request.result);

    request.onupgradeneeded = (event: any) => {
      const db = event.target.result;
      if (!db.objectStoreNames.contains("keyval")) {
        db.createObjectStore("keyval");
      }
    };
  });
}

// 🔹 Save value to IndexedDB
export async function setTokenIndexDB(key: string, value: any): Promise<boolean> {
  const db = await openDB();

  return new Promise((resolve, reject) => {
    const tx = db.transaction("keyval", "readwrite");
    const store = tx.objectStore("keyval");
    const req = store.put(value, key);

    req.onsuccess = () => resolve(true);
    req.onerror = () => reject(req.error);
  });
}

// 🔹 Retrieve value from IndexedDB
export async function getTokenIndexDB(key: string): Promise<any> {
  const db = await openDB();

  return new Promise((resolve, reject) => {
    const tx = db.transaction("keyval", "readonly");
    const store = tx.objectStore("keyval");
    const req = store.get(key);

    req.onsuccess = () => resolve(req.result);
    req.onerror = () => reject(req.error);
  });
}
