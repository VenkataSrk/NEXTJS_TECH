import React, { ReactNode, useEffect, useRef, useState } from 'react';
import styles from './scss/layout.module.scss';
import TopBar from './topBar';
import LeftNav from './leftNav';
import { Outlet, useLocation, useNavigate } from 'react-router-dom';
import RightNav from './rightNav';
import Agenteditor from '../components/agenticeditor/agenteditor';
import { jwtDecode } from 'jwt-decode';
import { GetItemFromStorage } from '../base/customhooks/useStorage';
import { LocalStorage } from '../base/customhooks/localStorageKeys';
import { useDispatch, useSelector } from 'react-redux';
import { knowledgeDataSliceActions } from '../store/slice/knowledgeBaseChat';
import { config } from '../base/constant';
import { commonDataSliceActions } from '../store/slice/commonSlice/commonSlice';


type layoutprops = {
  children?: ReactNode
}

const Operationslayout = ({ children }: layoutprops) => {
  const navigate = useNavigate();
const dispatch=useDispatch();
  const editorRef = useRef(null);
  const [disableEditor, setDisableEditor] = useState(0);
  const LayoutName: string = "operationsHub";
  const [activeMenu,setActiveMenu] = useState<number>(0);
  const enablePagination = useRef<any>(false);
  const [enablePage,setEnablePage] = useState<any>(true);
  console.log("activeMenu", activeMenu)
  const location = useLocation();

    const loginUserDetails :any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    console.log("logindetails>>",loginUserDetails)
  const domainNotAllowList :any={
    "20667":[6,7,9,10] //welcare account
  }
    const navigateMsg :any = useSelector((state :any)=>state.knowledgeBaseData.navigateMsg)
console.log("navv",navigateMsg)

const routesList :any={
  "knowledgebaseOperationsHub":0,
  "pastCampaign":1,
  "archive":2,
  "allcontacts":3,
  "email_config":6,
  "sms_config":7,
  "whatsapp_config":8,
  "apppush_config":9,
  "webpush_config":10,
  "connectors_config":12,

             
}
  const handleMenu=(path :string,menuIndex :number)=>{
        navigate(`/${path}`);
       
        setActiveMenu(menuIndex)
  }

  const handleEnablePagination=(enable :any)=>{
        enablePagination.current=(enable);
  }


useEffect(() => {
  const routes: any = {
    knowledgebaseOperationsHub: 0,
    pastCampaign: 1,
    archive: 2,
    allcontacts:3,
    reportstwo:4,
  };

  let path :any= window.location.pathname.split('/').pop();  
  path = path?.split('?')[0]?.split('#')[0]; // remove query/hash
  setActiveMenu(routes[path]);
  
}, []);
  // Detect outside click
  useEffect(() => {
    const handleClickOutside = (event) => {
      if (editorRef.current && !editorRef.current.contains(event.target)) {
        setDisableEditor((prev :any)=>prev+1);
        // setEnablePage(!enablePage);
         enablePagination.current=!(enablePagination.current);
      }
    };

    document.addEventListener("mousedown", handleClickOutside);
    return () => document.removeEventListener("mousedown", handleClickOutside);
  }, []);

  //  useEffect(() => {
  //     const loader = document.getElementById('contact_center_mainloader');
  //     if (loader) {
  //       loader.style.display = 'none';
  //     }
  //   }, []);

      useEffect(() => {
      const timer = setTimeout(() => {
        const loaders = document.getElementById('contact_center_mainloader');
        if (loaders) {
          loaders.style.display = 'none';
        }
      }, 1000); // 1 second
    
      // cleanup → remove timer
      return () => clearTimeout(timer);
    }, []);

    useEffect(()=>{
  if(navigateMsg?.count>0){

    // navigate(`/${navigateMsg?.path}`);
    handleMenu(navigateMsg?.path,routesList?.[navigateMsg?.path])
    dispatch(knowledgeDataSliceActions.setNavigate({path:'',count:0}))
  }
},[navigateMsg?.count])

 const whatsappConfig = useSelector((state :any)=>state.commonData.whatsappConfig);

 const wabaIdRef = useRef<string | null>(null);
   const businessIdRef = useRef<string | null>(null);
   const phoneNumberIdRef = useRef<string | null>(null);
 
  //  const loginUserDetails :any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    const launchWhatsAppSignup = () => {
      // console.log("here comes");
      try {
        // setLoader(true);
      
         (window as any).FB.login(
        (response: any) => {
          // setLoader(true);
          if (response.authResponse) {
            const code = response.authResponse.code;
            window.location.href = `${config().BUSINESS_WHATSAPP_CONF_URL}?code=${code}&domainId=${loginUserDetails?.domainId}&domainCountry=${loginUserDetails?.company_country??"United Kingdom"}` +
              `${wabaIdRef.current ? `&wabaId=${wabaIdRef.current}` : ''}` +
              `${businessIdRef.current ? `&businessId=${businessIdRef.current}` : ''}` +
              `${phoneNumberIdRef.current ? `&phoneNumberId=${phoneNumberIdRef.current}` : ''}`;
          }
        },
        {
          config_id: "1109736967317755",
          response_type: "code",
          override_default_response_type: true,
          extras: {
            setup: {},
            featureType: "whatsapp_business_app_onboarding",
            sessionInfoVersion: "3",
          },
        }
      );
      } catch (error :any) {
        // setLoader(false);
        console.log("err",error)
      }
      finally{
        // setLoader(false);
      }
     
    };
useEffect(() => {
        console.log("sdk call")
     // Load the Facebook SDK
     try {
        
         if(whatsappConfig){
    
             const loadFbSdk = () => {
               (window as any).fbAsyncInit = function () {
                 (window as any).FB.init({
                   appId: "2792648964217350",
                   autoLogAppEvents: true,
                   xfbml: true,
                   version: "v22.0",
                 });
               };
               
               const script = document.createElement("script");
               script.src = "https://connect.facebook.net/en_US/sdk.js";
               script.async = true;
               script.defer = true;
               script.crossOrigin = "anonymous";
               document.body.appendChild(script);
             };
         
             loadFbSdk();
            
         const timer =setTimeout(()=>{
        
             if ((window as any).FB) {
                        launchWhatsAppSignup();
                        clearTimeout(timer);
                        return;
                    }
         },2000)
        
             const handleReceiveEmbeddedSignupData = ({ waba_id, business_id, phone_number_id }: { waba_id: string, business_id: string, phone_number_id: string }) => {
               wabaIdRef.current = waba_id;
               businessIdRef.current = business_id;
               phoneNumberIdRef.current = phone_number_id;
             };
         
             // Handle messages from Facebook
             const handleMessage = (event: MessageEvent) => {
               if (
                 event.origin !== "https://www.facebook.com" &&
                 event.origin !== "https://web.facebook.com"
               ) {
                 return;
               }
         
               try {
                 const data = JSON.parse(event.data);
                 if (data.type === "WA_EMBEDDED_SIGNUP") {
                   if (data.event === "FINISH_WHATSAPP_BUSINESS_APP_ONBOARDING") {
                     console.log(
                       "Phone number ID:",
                       data.data.phone_number_id,
                       "WhatsApp business account ID:",
                       data.data.waba_id,
                       "Business ID:",
                       data.data.business_id
                     );
                     const waba_id = data.data.waba_id;
                     const business_id = data.data.business_id;
                     const phone_number_id = data.data.phone_number_id;
                     handleReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
                   } else if (data.event === "FINISH") {
                     console.log(
                       "Phone number ID:",
                       data.data.phone_number_id,
                       "WhatsApp business account ID:",
                       data.data.waba_id,
                       "Business ID:",
                       data.data.business_id
                     );
                     const waba_id = data.data.waba_id;
                     const business_id = data.data.business_id;
                     const phone_number_id = data.data.phone_number_id;
                     handleReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
                   } else if (data.event === "CANCEL") {
                     console.warn("Cancel at:", data.data.current_step);
                   } else if (data.event === "ERROR") {
                     console.error("Error:", data.data.error_message);
                   }
                 }
               } catch {
                 console.log("Non JSON Responses", event.data);
               }
               
             };
         
             window.addEventListener("message", handleMessage);
           
               
    
             return () => {
               window.removeEventListener("message", handleMessage);
            //    setWhatsAppConfig(false); 
              dispatch(commonDataSliceActions.setWhatsAppConfig(false));
             };
            
         }

     } catch (error :any) {
        console.log("err:",error)
     }finally{
      dispatch(commonDataSliceActions.setWhatsAppConfig(false));
     }
   
   }, [whatsappConfig]);


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
                  <span style={{ marginLeft: "12px" }}>Operations Hub</span>
                </div>
                <span style={{ display: "grid", top: "15px", position: "absolute", cursor: "pointer" }}><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
                  <path d="M16.2724 5.33001H1.94332L5.99312 1.28021C6.28612 0.987213 6.28612 0.512613 5.99312 0.219713C5.70012 -0.0731875 5.22552 -0.0732875 4.93262 0.219713L0.662618 4.48921C0.232918 4.91891 -0.00238182 5.48871 1.81797e-05 6.09371C0.00291818 6.69331 0.239718 7.25481 0.662618 7.66991L4.93262 11.9404C5.07912 12.0869 5.27102 12.1601 5.46292 12.1601C5.65482 12.1601 5.84672 12.0869 5.99322 11.9404C6.28622 11.6474 6.28622 11.1728 5.99322 10.8799L1.94392 6.83011H16.2725C16.6866 6.83011 17.0225 6.49421 17.0225 6.08011C17.0225 5.66601 16.6865 5.33001 16.2724 5.33001Z" fill="black" />
                </svg></span>
              </div>
                <div className={styles.ophubsidescroller}>

             
              <div className={styles.prevcamcontainer}>
                <li className={activeMenu==0?styles.active:""} onClick={()=>handleMenu('knowledgebaseOperationsHub',0)} >Knowledge base</li>
                <li className={activeMenu==1?styles.active:""}  onClick={()=>handleMenu('pastCampaign',1)}>Past Campaign</li>
                  <li  className={activeMenu==2?styles.active:""} onClick={()=>handleMenu('archive',2)}>Archive Campaigns</li>
                   <li  className={activeMenu==3?styles.active:""} onClick={()=>handleMenu('allcontacts',3)}>Contacts</li>
                   {/* <li  className={activeMenu==4?styles.active:""} onClick={()=>handleMenu('reportstwo',4)}>Legacy Reports</li> */}
              </div>

              {loginUserDetails?.domainId!="20667"&&<> <div style={{ position: "relative" }}>
                <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  <span >Data Ingestion</span>
                </div>
            
              </div>
               <div className={styles.prevcamcontainer}>
                <li className={activeMenu==12?styles.active:""} onClick={()=>handleMenu('connectors_config',12)} >Connectors</li>
              </div></>
              }

              <div style={{ position: "relative" }} >
                <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  <span >Channel Configuration</span>
                </div>
            
              </div>

              <div className={styles.prevcamcontainer}>
             
               {!domainNotAllowList?.[`${loginUserDetails?.domainId}`]?.includes(6) && <li className={activeMenu==6?styles.active:""} onClick={()=>handleMenu('email_config',6)} >Email</li>}
              
                {!domainNotAllowList?.[`${loginUserDetails?.domainId}`]?.includes(7) &&<li className={activeMenu==7?styles.active:""}  onClick={()=>handleMenu('sms_config',7)}>SMS</li>}
                   {!domainNotAllowList?.[`${loginUserDetails?.domainId}`]?.includes(8) && <li  className={activeMenu==8?styles.active:""} onClick={()=>handleMenu('whatsapp_config',8)}>WhatsApp</li>}
                    {!domainNotAllowList?.[`${loginUserDetails?.domainId}`]?.includes(9) && <li  className={activeMenu==9?styles.active:""} onClick={()=>handleMenu('apppush_config',9)}>App Push</li>}
                     {!domainNotAllowList?.[`${loginUserDetails?.domainId}`]?.includes(10) &&<li  className={activeMenu==10?styles.active:""} onClick={()=>handleMenu('webpush_config',10)}>Web Push</li>}
             
                   {/* <li  className={activeMenu==11?styles.active:""} onClick={()=>handleMenu('inapp_config',11)}>In-App</li> */}
              </div>

              {/* <div style={{ position: "relative" }} >
                <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  <span >Templates</span>
                </div>

                
            
              </div>
              <div className={styles.prevcamcontainer}>
                <li className={activeMenu==13?styles.active:""} onClick={()=>handleMenu('whatsapp_template',13)} >Whatsapp</li>
               
              </div> */}
                 </div>
            </li>
          </ul>
        </div>
        
        <div style={{ position: "relative" }} className={`${styles.ltscentersec}`}>
          {/* {children} */}
          <Outlet />
          <div className={`${styles.agenticeditorcontainer} ${styles.overalleditai }`} ref={editorRef}>
            <Agenteditor disableEditor={disableEditor} layoutName={"operationsHub"} handleEnablePagination={(enable :any)=>handleEnablePagination(enable)}/>
          </div>
        </div>
        <div className={styles.ltsrightnavsec}>
         
        </div>
        
         
      </section>
    </div>
  )
}

export default Operationslayout
