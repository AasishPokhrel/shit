import React from "react";
import ReactDOM from "react-dom/client";
import App from "./App.tsx";
import "./index.css";

// Remove loading shit
const loadingElement = document.querySelector(".loading-shit");
if (loadingElement) {
  loadingElement.remove();
}

// Holy shit, let's render this masterpiece
ReactDOM.createRoot(document.getElementById("root")!).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
);