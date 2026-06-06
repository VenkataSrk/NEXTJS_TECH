import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import App from './App';
import './styles/global.scss';
import "react-datepicker/dist/react-datepicker.css";
import toolkit_store from "../src/store/redux-store/root_store";


import { Provider } from 'react-redux';

if ("serviceWorker" in navigator) {
  window.addEventListener("load", async () => {
    try {
      // 🧹 Unregister old or duplicate service workers
      const registrations = await navigator.serviceWorker.getRegistrations();
      for (const registration of registrations) {
        if (!registration.active?.scriptURL.includes("firebase-messaging-sw.js")) {
          console.log("🗑 Removing old SW:", registration.active?.scriptURL);
          await registration.unregister();
        }
      }

      // ✅ Register only firebase-messaging-sw.js
      const registration = await navigator.serviceWorker.register("/firebase-messaging-sw.js");
      console.log("✅ Service Worker registered:", registration.scope);

    } catch (err) {
      console.error("❌ Service Worker registration failed:", err);
    }
  });
}

createRoot(document.getElementById('root')!).render(

  <Provider store={toolkit_store}>
    <App />
  </Provider>

)
