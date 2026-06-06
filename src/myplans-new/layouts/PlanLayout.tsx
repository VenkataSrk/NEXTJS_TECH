import React, { useEffect, useRef, useState } from "react";
// import styles from "../assets/styles/planlayout.module.scss";
import styles from "./scss/planlayout.module.scss"
// import mainlogo from "../assets/images/whitelogo.svg";
import mainlogo from "./assets/images/whitelogo.svg";
import workspaceleftarrowicon from "../assets/images/agentworkspace/workspaceleftarrowicon.svg";
// import Headers from "../components/header";
import { Outlet, useLocation, useNavigate } from "react-router-dom";
import SimpleCustomEditorPlan from "../../components/agenticeditor/simplecustomeditorplan";
import TopBar from "../../layout/topBar";
// import SimpleCustomEditorPlan from "../components/SimpleCustomEditorPlan";

const PlanLayout: React.FC = () => {
   const [disableEditor, setDisableEditor] = useState(0);
  const editorRef = useRef(null);
  const navigate = useNavigate()
  const location = useLocation();
  // Detect outside click
  useEffect(() => {
    const handleClickOutside = (event) => {
      if (editorRef.current && !editorRef.current.contains(event.target)) {
        setDisableEditor((prev :any)=>prev+1);
      }
    };

    document.addEventListener("mousedown", handleClickOutside);
    return () => document.removeEventListener("mousedown", handleClickOutside);
  }, []);

   useEffect(() => {
        const loader = document.getElementById('contact_center_mainloader');
        if (loader) {
          loader.style.display = 'none';
        }
      }, []);
  return (
    <>
    <div className={`${styles.layoutcenteredtop} myplancss `}>
            {/* <Headers /> */}
            <TopBar/>
          </div>
      <div className={`${styles.layoutmain} main`}>
          
        <div className={`${styles.layoutcentered} centered`}>
          
           <div className={`${styles.layoutsidebarleft} left`}>
        
          <div className={styles.maintitle}>
            <img src={workspaceleftarrowicon} alt="" onClick={() => navigate(-1)} />My Plans
          </div>
          <div className={styles.layoutmenu}>
            <div className={styles.layoutmenutitle}>Account Information</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/yourinfo" ? styles.active : ""}`} onClick={() => navigate("/yourinfo")}>Your Info</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/security" ? styles.active : ""}`} onClick={() => navigate("/security")}>Security</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/privacy" ? styles.active : ""}`} onClick={() => navigate("/privacy")}>Privacy</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/devices" ? styles.active : ""}`} onClick={() => navigate("/devices")}>Devices</div>

            <div className={styles.layoutmenutitle}>User Management</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/roles" ? styles.active : ""}`} onClick={() => navigate("/roles")}>Roles</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/user" ? styles.active : ""}`} onClick={() => navigate("/user")}>Users</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/teams" ? styles.active : ""}`} onClick={() => navigate("/teams")}>Teams</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/activitylog" ? styles.active : ""}`} onClick={() => navigate("/activitylog")}>User Activity Logs</div>

            <div className={styles.layoutmenutitle}>Billing and Subscription</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/Orderhistory" ? styles.active : ""}`} onClick={() => navigate("/Orderhistory")}>Order History</div>
            {/* <div className={`${styles.layoutsubmenu} ${location.pathname === "/paymentoption" ? styles.active : ""}`} onClick={() => navigate("/paymentoption")}>Payment options</div> */}
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/addon" ? styles.active : ""}`} onClick={() => navigate("/addon")}>Add-on's</div>
            <div className={`${styles.layoutsubmenu} ${location.pathname === "/renewal" ? styles.active : ""}`} onClick={() => navigate("/renewal")}>Purchases</div>
          </div>
        </div>
        
       
          <div className={`${styles.layoutcenteredbottom} bottom`}>
<div className={styles.searchconvo} ref={editorRef}>
              <SimpleCustomEditorPlan disableEditor={disableEditor} />
            </div>
           
            <Outlet />
          </div>
           
          {/* <div className={styles.searchconvo}>
            <SimpleCustomEditorPlan />
          </div> */}
         
          
        </div>
      </div>
    </>
  );
};

export default PlanLayout;
