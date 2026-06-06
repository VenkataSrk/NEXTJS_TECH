import React, {Suspense, lazy} from "react";
import ReactDOM from "react-dom/client";
// import "./index.css";
// import App from './App'
import reportWebVitals from "./reportWebVitals";
import toolkit_store from "./store/redux-store/root_store";
import { MemoryRouter as Router, Route, Routes } from "react-router-dom";
import { Provider } from "react-redux";
import "regenerator-runtime/runtime";
const App = lazy(()=>import('./App'));

try {
  if (process.env.NODE_ENV === 'production') {
    console.log = () => {};
  }
} catch (err) {
  console.error('Environment check error:', err);
};

window.React = React;
window.ReactDOM = ReactDOM;
const root = ReactDOM.createRoot(document.getElementById("chat_bot"));
if(root){
root.render(
<Provider store={toolkit_store}>
    <Router>
      <Suspense fallback={null}>
        <Routes>
          <Route element={<App />} path="/" />
          <Route element={<App />} path="/chatbot" />
        </Routes>
      </Suspense>
    </Router>
  </Provider>
)
}

// If you want to start measuring performance in your app, pass a function
// to log results (for example: reportWebVitals(console.log))
// or send to an analytics endpoint. Learn more: https://bit.ly/CRA-vitals
reportWebVitals();
