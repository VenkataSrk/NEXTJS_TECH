import React, { lazy, Suspense, useEffect } from "react";
import { BrowserRouter, Route, Routes, Navigate, useLocation, useNavigate } from 'react-router-dom';
import "./styles/global.scss";
import Layout from "./layout/layout";
import ControlledRoutes from './routes/controlledRoutes';
import Loaders from "./loader/loader";
import { messaging } from "./firebase";
import { onMessage } from "firebase/messaging";
import Cookies from "./utils/cookies";
import SocketService from "./base/service/socketService";
import { useDispatcher } from "./store/redux-store/dispatch";
import { HubspotList } from './modules/hubspot/hubspotList';
import AnalyticsMainScreen from "./modules/analytics/scss/analytics";
import Analytics from "./modules/analytics/scss/analytics";
// import Reports from "./modules/reports/reportsindex";
import Reports from "./modules/reports/reportsindextwo";
import Knowledgebase from "./modules/knowledgebase/knowledgebase";
import Email from "./modules/settings/channelConfiguration/email";
import Apppush from "./modules/settings/channelConfiguration/apppush/apppush";
import WebPushConfig from "./modules/settings/channelConfiguration/webpushconfig/webPushConfig";
import CvmWhatsappConfig from "./modules/settings/channelConfiguration/cvmchannelConfig/cvmWhatsappConfig";
import Smsconfig from "./modules/settings/channelConfiguration/sms/smsconfig";
import Initialhomepagenew from "./modules/home/initialhomepagenew/initialhomepagenew";
import Operationslayout from "./layout/operationshublayout";
import Knowledgebaseneww from "./modules/knowledgebasenew/knowledgebase";
import Agenteditor from "./components/agenticeditor/agenteditor";
import PlanLayout from "./myplans-new/layouts/PlanLayout";
import YourInfo from "./myplans-new/pages/MyPlan/AccountInformation";
import AccountSecurity from "./myplans-new/pages/MyPlan/AccountInformation/security";
import AccountPrivacy from "./myplans-new/pages/MyPlan/AccountInformation/privacy";
import AccountDevices from "./myplans-new/pages/MyPlan/AccountInformation/devices";
import Createrole from "./myplans-new/pages/MyPlan/UserManagement/Role/CreateRole";
import OrderhistoryNew from "./myplans-new/pages/MyPlan/Billing/OrderHistory";
import Rolepermission from "./myplans-new/pages/MyPlan/UserManagement/Role/RolePermission";
import Rolehigherarchy from "./myplans-new/pages/MyPlan/UserManagement/Role/hierarchy";
import Adduser from "./myplans-new/pages/MyPlan/UserManagement/User/adduser";
import Userfile from "./myplans-new/pages/MyPlan/UserManagement/User/user";
import Useractivitylog from "./myplans-new/pages/MyPlan/UserManagement/ActivityLog";
import Teams from "./myplans-new/pages/MyPlan/UserManagement/Role/teams";
import Addon from "./myplans-new/pages/MyPlan/Billing/AddOn/addon";
import ContactCenterSummary from "./myplans-new/pages/MyPlan/Billing/AddOn/autoreneval";
import Paymentform from "./myplans-new/pages/MyPlan/Billing/PaymentOption/paymentform";
import Paymentoption from "./myplans-new/pages/MyPlan/Billing/PaymentOption/paymentoption";
import PaymentSuccess from "./myplans-new/pages/MyPlan/Billing/PaymentSuccess";
import PastCampaign from "./modules/pastCampaign/pastCampaign";
import Archive from "./modules/archive/archive";
import Drafts from "./modules/drafts/drafts";
import Planlayoutnew from "./myplans-new/layouts/planlayoutnew";
import ArchiveConversation from "./modules/archive/archiveconversation";
import Allcontacts from "./modules/allcontacts/allcontacts";
import Spinner from "./components/custom_components/spinner/spinner";
import NavigationHandler from "./layout/navigationHandler";
import OnBoardLayout from "./components/camp/onboarding";
// import LiveCampaignAnalytics from "./modules/analytics/livecampaignanalytics";
import LiveCampaignAnalytics from "./modules/analytics/livecampaignanalytics";
import WhatsappConfigDone from "./components/camp/onboarding/whatsappConfigDone";
import OperationHubWhatsappConfig from "./modules/settings/channelConfiguration/whatsapp/operationHubWhatsappconfig";
import OperationSmsconfig from "./modules/settings/channelConfiguration/sms/operationHubSmsconfig";
import OperationHubApppush from "./modules/settings/channelConfiguration/apppush/operationHubapppush";
import OperationHubWebPushConfig from "./modules/settings/channelConfiguration/webpushconfig/operationHubWebPushConfig";
import OperationHubEmail from "./modules/settings/channelConfiguration/email/operationhubemail";
import Storage from "./myplans-new/pages/MyPlan/AccountInformation/storage";
import Connectors from "./modules/settings/connectors/connectors";
import WhatsappTemplate from "./modules/settings/channelConfiguration/whatsapptemplate/whatsappTemplate";
import Useradmin from "./myplans-new/pages/MyPlan/AccountInformation/useradmin";
import Useradminroles from "./myplans-new/pages/MyPlan/AccountInformation/useradminroles";
import Useradminrolesteams from "./myplans-new/pages/MyPlan/AccountInformation/useradminrolesteams";
import aiMyPlanSocketService from "./base/service/aiMypPanSocketService";

const Brandelevate = lazy(() => import("./modules/brandelevate/brandelevate"));
const Settings = lazy(() => import("./modules/settings"));
const Home = lazy(() => import("./modules/home/home"));
const Hubspot = lazy(() => import("./modules/hubspot/hubspotindex"));
const AddWorktualAddress = lazy(() => import("./modules/settings/channelConfiguration/email/addWorktualAddress"));
const Contacts = lazy(() => import("./modules/contacts/contacts"));
const ImportHistory = lazy(() => import("./modules/settings/apiconfiguration/importhistory"));
const Login = lazy(() => import("./components/loginnew/login"));
const SetupSmtpEmail = lazy(() => import("./modules/settings/channelConfiguration/email/setupSmtpEmail"));
const Purchasemodal = lazy(() => import("./modules/purchasemodal/purchasemodal"));

const App = () => {
  const dispatch = useDispatcher();
  
  
  useEffect(() => {
    const unsubscribe = onMessage(messaging, (payload: any) => {
      console.log("🔔 Foreground notification received:", payload);
      const { title, body } = payload.data || payload.notification || {};
      if (title && body && Notification.permission === "granted") {
        new Notification(title, { body, icon: "/logo192.png" });
      }
    });
    return () => unsubscribe();
    
  }, []);

  //   const inActivelogout=async(email :any)=>{
  //   try {
  //     const response = await axios.post(
  //       `${import.meta.env.VITE_APP_ENV === "development" ? "https://qacampaignapi.worktual.co.uk/campaignService/v1/logout" : "https://campaignapi.worktual.co.uk/campaignService/v1/logout"}`,
  //       {
  //         email,
  //       }
  //     );

  //     console.log("Success:", response.data);
     
  //   } catch (error: any) {
  //     console.error("Error:", error.response?.data || error.message);
  //   }
  // }

//   useEffect(() => {
//   const handleUnload = () => {
//     const data = JSON.stringify({
//       token: localStorage.getItem("accessToken"),
//     });

//     navigator.sendBeacon(
//       "https://your-api.com/logout",
//       data
//     );
//   };

//   window.addEventListener("unload", handleUnload);

//   return () => {
//     window.removeEventListener("unload", handleUnload);
//   };
// }, []);

  // useEffect(() => {
  //   const loader = document.getElementById('contact_center_mainloader');
  //   if (loader) {
  //     loader.style.display = 'none';
  //   }
  // }, []);

  useEffect(() => {
    const token = Cookies.get();
    console.log("token from cookies:", token);

    try {
      if (token?.ssoToken && typeof token?.ssoToken === "string") {
        new SocketService(token?.ssoToken, dispatch);
      } else {
        console.warn("⚠️ No valid token found for socket connection");
      }
    } catch (err) {
      console.error("❌ Error connecting socket:", err);
    }

    return () => {
      SocketService.destroy();
    };
  }, []);


  return (
    <BrowserRouter>
      <NavigationHandler/>
      <Suspense fallback={null}>
        <Routes>
          <Route element={<ControlledRoutes />}>

            <Route path="/addWorktualAddress" element={<Layout><Suspense fallback={<Loaders />}><AddWorktualAddress /></Suspense></Layout>} />
            <Route path="/setupSmtpEmail" element={<Layout><Suspense fallback={<Loaders />}><SetupSmtpEmail /></Suspense></Layout>} />
            <Route path="/settings" element={<Layout><Suspense fallback={<Loaders />}><Settings /></Suspense></Layout>} />
            <Route path="/home" element={<Layout><Suspense fallback={<Spinner />}><Home /></Suspense></Layout>} />
            
            <Route path="/analyticssummary" element={<Layout><Suspense fallback={<Spinner />}><LiveCampaignAnalytics /></Suspense></Layout>} />
          
            <Route path="/onboard" element={<Suspense fallback={<Spinner />}><OnBoardLayout /></Suspense>} />
            {/* <Route path="/home" element={<Layout><Suspense fallback={<Loaders />}><Home /></Suspense></Layout>} /> */}
            
            <Route path="/brandelevate" element={<Layout><Suspense fallback={<Loaders />}><Brandelevate /></Suspense></Layout>} />
            <Route path="/purchasemodal" element={<Layout><Suspense fallback={<Loaders />}><Purchasemodal /></Suspense></Layout>} />
            {/* <Route path="/hubspotconnection" element={<Layout><Suspense fallback={<Loaders />}><Hubspot /></Suspense></Layout>} /> */}
            {/* <Route path="/hubspotconnectionlist" element={<Layout><Suspense fallback={<Loaders />}><HubspotList /></Suspense></Layout>} /> */}
            <Route path="/contacts" element={<Layout><Suspense fallback={<Loaders />}><Contacts /></Suspense></Layout>} />
            <Route path="/importHistory" element={<Layout><Suspense fallback={<Loaders />}><ImportHistory /></Suspense></Layout>} />
            <Route path="/analytics" element={<Layout><Suspense fallback={<Loaders />}><Analytics /></Suspense></Layout>} />
            {/* <Route path="/reports" element={<Layout><Suspense fallback={<Loaders />}><Reports /></Suspense></Layout>} /> */}

            <Route path="/drafts" element={<Layout><Suspense fallback={<Loaders />}><Drafts /></Suspense></Layout>} />
            <Route path="/Knowledgebase" element={<Layout><Suspense fallback={<Loaders />}><Knowledgebase/></Suspense></Layout>} />
             {/* <Route path="/operationshublayout" element={<Operationslayout></Operationslayout>}/> */}
             <Route  element={<Operationslayout/>}>
               <Route path="/knowledgebaseOperationsHub" element={<Knowledgebaseneww/>}/>
               <Route path="/pastCampaign" element={<PastCampaign/>}/>
                <Route path="/archive" element={<Archive/>}/>
                <Route path="/archiveconv" element={<ArchiveConversation/>}/>
                <Route path="/allcontacts" element={<Allcontacts/>}/>
                <Route path="/reportstwo" element={<Reports />} />
                <Route path="/email_config" element={<OperationHubEmail/>}/>
                <Route path="/sms_config" element={<OperationSmsconfig/>}/>
                <Route path="/whatsapp_config" element={< OperationHubWhatsappConfig/>}/>
                <Route path="/apppush_config" element={<OperationHubApppush/>}/>
                <Route path="/webpush_config" element={<OperationHubWebPushConfig/>}/>
                
                <Route path="/connectors_config" element={<Connectors />}/>
                <Route path="/hubspotconnection" element={<Hubspot />} />
                <Route path="/whatsapp_template" element={<WhatsappTemplate />} />
                <Route path="/hubspotconnectionlist" element={<HubspotList />} />
                <Route path="/useradminroles" element={<Useradminroles />} />
                <Route path="/useradmin" element={<Useradmin />} />
                <Route path="/useradminrolesteams" element={<Useradminrolesteams />} />

                {/* <Route path="/inapp_config" element={<Email/>}/> */}
             </Route>
             {/* <Route path="/agenteditor" element={<Operationslayout><Agenteditor/></Operationslayout>}/> */}
          <Route element={<PlanLayout />}>
          
          
          </Route>

             <Route element={<Planlayoutnew />}>

                      <Route path="/yourinfo" element={<YourInfo />} />
                      <Route path="/security" element={<AccountSecurity />} />
                      <Route path="/privacy" element={<AccountPrivacy />} />
                      <Route path="/devices" element={<AccountDevices />} />
                      <Route path="/storage" element={<Storage/>}/>

                      <Route path="/roles" element={<Createrole />} />
                      <Route path="/Orderhistory" element={<OrderhistoryNew />} />
                      <Route path="/createrole" element={<Createrole />} />
                      <Route path="/roles/useradminroles" element={<Useradminroles />} />
                      <Route path="/role-hierarchy" element={<Rolehigherarchy />} />
                      <Route path="/role-permission" element={<Rolepermission />} />
                      
                      <Route path="/adduser" element={<Adduser />} />
                      <Route path="/user" element={<Userfile />} />
                      <Route path="/activitylog" element={<Useractivitylog/>} />
                      <Route path="/teams" element={<Teams />} />

                      <Route path="/addon" element={<Addon />} />
                      <Route path="/renewal" element={<ContactCenterSummary />} />
                      <Route path="/payment" element={<Paymentform />} />
                      <Route path="/paymentoption" element={<Paymentoption />} />
                      <Route path="/paymentsuccess" element={<PaymentSuccess />} />
                     
                      
            </Route>
          </Route>                
          <Route path="/login" element={<Login />} />         
          <Route path="/*" element={<Navigate to="/login" />} />
          <Route path="/cvmcampaign" element={<Home />} />
          <Route path="/cvmanalytics" element={<Analytics />} />

          <Route path="/cvmemailconfig" element={<Email />} />
          <Route path="/cvmsmsconfig" element={<Smsconfig/>} />
          <Route path="/cvmapppushconfig" element={<Apppush />} />
          <Route path="/cvmwebpushconfig" element={<WebPushConfig />} />
          <Route path="/cvmwhatsappconfig" element={<CvmWhatsappConfig/>} />
          <Route path="/cvmaddWorktualAddress" element={<AddWorktualAddress />} />
          <Route path="/cvmsetupSmtpEmail" element={<SetupSmtpEmail />} />
          <Route path="/whatsappconfig" element={<WhatsappConfigDone/>}/>
          
        </Routes>       
      </Suspense>
    </BrowserRouter>
  );
}

export default App;
