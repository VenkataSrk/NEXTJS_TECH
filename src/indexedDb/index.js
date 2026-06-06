// indexedDB.js
import { openDB } from 'idb';

const DB_NAME = 'chatbot-storage';
const STORE_NAME = 'Ccass-team';

const initDB = async () => {
  return openDB(DB_NAME, 1, {
    upgrade(db) {
      db.createObjectStore(STORE_NAME);
    },
  });
};

export const setIndexDbItem = async (key, value) => {
  const db = await initDB();
  const tx = db.transaction(STORE_NAME, 'readwrite');
  await tx.objectStore(STORE_NAME).put(value, key);
  await tx.done;
};

export const getIndexDbItem = async (key) => {
  const db = await initDB();
  return db.transaction(STORE_NAME).objectStore(STORE_NAME).get(key);
};

export const removeIndexDbItem = async (key) => {
  const db = await initDB();
  const tx = db.transaction(STORE_NAME, 'readwrite');
  await tx.objectStore(STORE_NAME).delete(key);
  await tx.done;
};

export const clearIndexDb = async () => {
  const db = await initDB();
  const tx = db.transaction(STORE_NAME, 'readwrite');
  await tx.objectStore(STORE_NAME).clear();
  await tx.done;
};
