import React, { ReactNode, useEffect, useRef, useState } from 'react';
import styles from '../../layout/scss/layout.module.scss';
import { Outlet, useLocation, useNavigate } from 'react-router-dom';
import Agenteditor from '../../components/agenticeditor/agenteditor';
import TopBar from '../../layout/topBar';
import aiMyPlanSocketService from '../../base/service/aiMypPanSocketService';
import { useDispatcher } from '../../store/redux-store/dispatch';
import Cookies from '../../utils/cookies';



type layoutprops = {
  children?: ReactNode
}

const Planlayoutnew = ({ children }: layoutprops) => {
   const [disableEditor, setDisableEditor] = useState(0);
  const editorRef = useRef(null);
  const navigate = useNavigate()
  const location = useLocation();
  const dispatch = useDispatcher();
   const token = Cookies.get();
  const LayoutName :string="planLayout";
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
        new aiMyPlanSocketService(token?.ssoToken,navigate, dispatch);
      }, []);
  return (
    <div className={styles.layoutsec}>
    <header className={styles.ltstopbar}>
        <TopBar />
      </header>
      <section className={`${styles.ltsmainsec} ${styles.ltsmainoperation}`}>
        <div className={styles.ltsleftnavsec}>
          <ul className={`${styles.ltnverticallists} ${styles.campaignhistory}`}>
            <li className={styles.ltnlists}>
              <div style={{ position: "relative", padding: "0 0 0 12px" }} onClick={()=>navigate('/home')}>
                <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  <span style={{ marginLeft: "12px" }}>My Plans</span>
                </div>
                <span style={{ display: "grid", top: "15px", position: "absolute", cursor: "pointer" }}><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
                  <path d="M16.2724 5.33001H1.94332L5.99312 1.28021C6.28612 0.987213 6.28612 0.512613 5.99312 0.219713C5.70012 -0.0731875 5.22552 -0.0732875 4.93262 0.219713L0.662618 4.48921C0.232918 4.91891 -0.00238182 5.48871 1.81797e-05 6.09371C0.00291818 6.69331 0.239718 7.25481 0.662618 7.66991L4.93262 11.9404C5.07912 12.0869 5.27102 12.1601 5.46292 12.1601C5.65482 12.1601 5.84672 12.0869 5.99322 11.9404C6.28622 11.6474 6.28622 11.1728 5.99322 10.8799L1.94392 6.83011H16.2725C16.6866 6.83011 17.0225 6.49421 17.0225 6.08011C17.0225 5.66601 16.6865 5.33001 16.2724 5.33001Z" fill="black" />
                </svg></span>
              </div>

              
            </li>
          </ul>
         <div className={styles.leftnavscroller}>

        
            <ul className={`${styles.ltnverticallists} ${styles.campaignhistory}`}>
            <li className={styles.ltnlists}>
              <div style={{ position: "relative"}} >
                <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  <span >Account Information</span>
                </div>
               
              </div>

              <div className={styles.prevcamcontainer}>


                   
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/yourinfo" ? styles.active : ""}`} onClick={() => navigate("/yourinfo")}>Your Info</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/security" ? styles.active : ""}`} onClick={() => navigate("/security")}>Security</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/privacy" ? styles.active : ""}`} onClick={() => navigate("/privacy")}>Data and Privacy</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/devices" ? styles.active : ""}`} onClick={() => navigate("/devices")}>Devices</li>
             {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/storage" ? styles.active : ""}`} onClick={() => navigate("/storage")}>Storage</li> */}

            {/* <div className={styles.layoutmenutitle}>User Management</div> */}
            {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/roles" ? styles.active : ""}`} onClick={() => navigate("/roles")}>Roles</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/user" ? styles.active : ""}`} onClick={() => navigate("/user")}>Users</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/teams" ? styles.active : ""}`} onClick={() => navigate("/teams")}>Teams</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/activitylog" ? styles.active : ""}`} onClick={() => navigate("/activitylog")}>User Activity Logs</li> */}

            {/* <div className={styles.layoutmenutitle}>Billing and Subscription</div> */}
            {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/Orderhistory" ? styles.active : ""}`} onClick={() => navigate("/Orderhistory")}>Order History</li> */}
            {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/paymentoption" ? styles.active : ""}`} onClick={() => navigate("/paymentoption")}>Payment options</li> */}
            {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/addon" ? styles.active : ""}`} onClick={() => navigate("/addon")}>Add-on's</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/renewal" ? styles.active : ""}`} onClick={() => navigate("/renewal")}>Purchases</li> */}



              </div>
            </li>
          </ul>
            <ul className={`${styles.ltnverticallists} ${styles.campaignhistory}`}>
            <li className={styles.ltnlists}>
              <div style={{ position: "relative" }} >
                <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  <span >User Management</span>
                </div>
               
              </div>

              <div className={styles.prevcamcontainer}>


                   
            

           
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/roles" ? styles.active : ""}`} onClick={() => navigate("/roles")}>Roles</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/user" ? styles.active : ""}`} onClick={() => navigate("/user")}>Users</li>
            {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/teams" ? styles.active : ""}`} onClick={() => navigate("/teams")}>Teams</li> */}
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/activitylog" ? styles.active : ""}`} onClick={() => navigate("/activitylog")}>User Activity Logs</li>

            {/* <div className={styles.layoutmenutitle}>Billing and Subscription</div>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/Orderhistory" ? styles.active : ""}`} onClick={() => navigate("/Orderhistory")}>Order History</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/paymentoption" ? styles.active : ""}`} onClick={() => navigate("/paymentoption")}>Payment options</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/addon" ? styles.active : ""}`} onClick={() => navigate("/addon")}>Add-on's</li>
            <li className={`${styles.layoutsubmenu} ${location.pathname === "/renewal" ? styles.active : ""}`} onClick={() => navigate("/renewal")}>Purchases</li> */}



              </div>
            </li>
          </ul>
           <ul className={`${styles.ltnverticallists} ${styles.campaignhistory}`}>
            <li className={styles.ltnlists}>
              <div style={{ position: "relative" }} >
                <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  <span >Billing and Subscription</span>
                </div>
               
              </div>

              <div className={styles.prevcamcontainer}>


             

        
            {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/Orderhistory" ? styles.active : ""}`} onClick={() => navigate("/Orderhistory")}>Order History</li>
          <li className={`${styles.layoutsubmenu} ${location.pathname === "/addon" ? styles.active : ""}`} onClick={() => navigate("/addon")}>Add-on's</li> */}
                <li className={`${styles.layoutsubmenu} ${location.pathname === "/renewal" ? styles.active : ""}`} onClick={() => navigate("/renewal")}>Billing & Subscription</li> 
            
            


            {/* <li className={`${styles.layoutsubmenu} ${location.pathname === "/paymentoption" ? styles.active : ""}`} onClick={() => navigate("/paymentoption")}>Payment options</li>  */}

              </div>
            </li>
          </ul>
           </div>
        </div>
        <div  style={{position:"relative"}} className={`${styles.ltscentersec} `}>
            <div className={styles.planlayoutscroller}>
<Outlet />
            </div>
         
         <div className={`${styles.agenticeditorcontainer} ${styles.overalleditai }`} ref={editorRef}>
<Agenteditor disableEditor={disableEditor} layoutName={LayoutName} />
        </div>
        </div>
        <div className={styles.ltsrightnavsec}>
         
        </div>
        
         
      </section>
    </div>
  )
}

export default Planlayoutnew
