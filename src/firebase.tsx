import { initializeApp } from "firebase/app";
import { getMessaging } from "firebase/messaging";

const firebaseConfig = {
  apiKey: "AIzaSyCsLt2QXdQOMMn4VUNSWeLdPqU3zgUuK58",
  authDomain: "campaignwebpush.firebaseapp.com",
  projectId: "campaignwebpush",
  storageBucket: "campaignwebpush.appspot.com",
  messagingSenderId: "610883259513",
  appId: "1:610883259513:web:0154759264bfa46275d8a8",
  measurementId: "G-PBRTEGFZ8W"
};

// Initialize Firebase
const firebaseApp = initializeApp(firebaseConfig);

// Initialize Messaging
const messaging = getMessaging(firebaseApp);

export { firebaseApp, messaging };