// Import Locator runtime only in development
if (import.meta.env.DEV) {
  import("@locator/runtime");
}
import { StrictMode, useEffect } from 'react'
import { createRoot } from 'react-dom/client'
import './assets/styles/main.scss'
import App from './App.tsx'
import toolkit_store from "../src/store/redux-store/root_store";
import { Provider } from 'react-redux';
import { ErrorBoundary } from 'react-error-boundary';
import { BrowserRouter } from 'react-router-dom';



const AppWrapper = () => {

  // useEffect(() => {
  //   const handleError = (e: any) => {
  //     console.error("handleError indexFile capture")
  //     const target = e?.target || e?.srcElement;
  //     if (target?.tagName === "SCRIPT" && target?.src?.includes("/static/js/")) {
  //       console.error("Chunk load failed:", target?.src);
  //       window.location.reload();
  //     }
  //   };

  //   const handleRejection = (e: any) => {
  //     console.error("Unhandledrejection indexFile capture")
  //     if (e?.reason?.message?.includes("Loading chunk")) {
  //       console.error("Unhandled chunk load error:", e.reason);
  //       window.location.reload();
  //     }
  //   };

  //   window.addEventListener("error", handleError);
  //   window.addEventListener("unhandledrejection", handleRejection);

  //   return () => {
  //     window.removeEventListener("error", handleError);
  //     window.removeEventListener("unhandledrejection", handleRejection);
  //   };
    
  // }, []);

  const onError =(error :any)=>{
     try {
      // const domainId = userDetails?.domainId || userDetails?.domain_id || loginUserDetails?.domainId || loginUserDetails?.domain_id 
      // const userId = userDetails?.userId || loginUserDetails?.userId
      // if(!domainId) return;
      // const body ={ 
      //   message: error?.message || 'Error not capture',
      //   stack: "",
      //   url: window?.location?.href || 'Route not capture',
      //   domainId: `${domainId}_${userId}`,
      // }
      // LoggerService
      //   .post(body, '/logger_error_details')
      //   .catch((error)=> console.error("logger_error_details :: Error", error))
       
    } catch (error) {
      console.log(error)
    }
  }


  return (
    <ErrorBoundary fallbackRender={<div>Something went wrong</div> as any} onError={(error)=> onError(error)}>
          <App />
    </ErrorBoundary>
  );
};

createRoot(document.getElementById('root')!).render(
    <Provider store={toolkit_store}>
       <BrowserRouter>
        <AppWrapper />
      </BrowserRouter>
    </Provider>
)
