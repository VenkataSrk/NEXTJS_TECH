import { Routes, Route, Navigate, useNavigate, useLocation, Outlet } from "react-router-dom";
import Dashboard from './pages/dashboard';
import NormalLayouts from './layout/NormalLayout';
import Contacts from './pages/contacts';
import Contactactivityview from './pages/contacts/contactactivityview';
import Inbox from './pages/inbox/inbox';
import Inboundcallview from "./pages/contacts/inboundcallview";
import Videoviewdetails from "./pages/contacts/videoviewdetails";
import OnBoardLayout from "./pages/onboarding";
import SetUpMain from "./pages/setup";
import Product from "./pages/product/product";
import ProductInsights from "./pages/product/insights";
import ProductTracker from "./pages/product/tracker";
import ProductCatalog from "./pages/product/catalog";
import Projectsinsights from "./pages/project/insights";
import Myactivity from "./pages/project/myactivity";
import Allactivity from "./pages/project/allactivity";
import Myactivityview from "./pages/project/myactivityview";
import MarketingInsights from "./pages/marketing";
import MarketingCampaign from "./pages/marketing/campaign";
import MarketingCampaignDetails from "./pages/marketing/campaignDetails";
import MarketingScheduleCampaign from "./pages/marketing/scheduleCampaign";
import MarketingPastCampaign from "./pages/marketing/pastCampaign";
import MarketingPendingCampaign from "./pages/marketing/pendingCampaign";
import MarketingCampaignConversation from "./pages/marketing/campaignConversation";
import CallWidgets from "./pages/component/callWidget";
import Deals from "./pages/deals/deals";
import Projectdrawer from "./pages/project/drawers";
import Companyinboundcallview from "./pages/contacts/companyinboundcallview";
import Login, { ConnectSockets } from "./pages/login";
import Chatconversation from "./pages/chat/chatconversation";
import { useEffect, useMemo, useRef } from "react";
import aiChatAssistSocketservice from "./services/aiChatService";
import { GetItemFromStorage } from "./base/custom_hooks/useStorage";
import { LocalStorage } from "./base/custom_hooks/localStorageKeys";
import { useDispatcher } from "./store/redux-store/dispatch";
import MarketingTemplateCampaignConversation from "./pages/marketing/templateConversation";
import jwtDecode from "jwt-decode";
import presenceService from "./base/sockets/presence";
import socketService from "./base/sockets/socketService";
import { CallService } from "./base/service/call_service";
import IncomingCallNotify from "./pages/component/incomingcall/incomingcallnotify";
import { useSelector } from "react-redux";
import { setQueueList as fetchQueueList, getDomainEmailConfig } from "./base/service/core_serviceApi";
import { commonDataSliceActions } from "./store/slice/commonSilce/commonSlice";
import Contactname from "./pages/contacts/contactname";
import ContactsList from "./pages/contacts/contactList";
import CompanyList from "./pages/contacts/companyList";
import Companyname from "./pages/contacts/companyname";
import ContactConversation from "./pages/contacts/contactConversation";
import MyPlanConversation from "./pages/MyPlan/myPlanConversation.tsx";
import SetUpConversation from "./pages/setup/setupConversation.tsx";
import Dealscontent from "./pages/deals";
import Dealslist from "./pages/deals/dealslist";
import Activitydeals from "./pages/deals/activitydeals";
import Invoice from "./pages/deals/invoice";
import DealsLayout from "./pages/deals/dealsLayout";
import DealsConversation from "./pages/deals/dealsConversation";
import HomeLayout from "./pages/dashboard/homeLayout";
import DashboardConversation from "./pages/dashboard/dashboardConversation";
import ProjectsLayout from "./pages/project";
import ProjectConversation from "./pages/project/projectConversation";
import ProductsLayout from "./pages/product";
import ProductConversation from "./pages/product/productConversation";
import Productname from "./pages/product/productname";
import { getCurrentUserCallerID } from "./base/utils";
import Contactsmsview from "./pages/contacts/contactsmsview";
import Contactnotes from "./pages/contacts/contactsnotes";
import Allrequirement from "./pages/project/allrequirement";
import Dealscontact from './pages/contacts/deals'
import inboxAiChatAssistSocketservice from "./services/inboxAiChatService";
import Whatsupcoming from "./pages/deals/whatsupcoming";
import Upcoming from "./pages/deals/whatsupcoming";
import Meeting from "./pages/deals/meeting";
import ScheduledMeeting from "./pages/contacts/scheduledmeeting.tsx";
import Dealsupcoming from "./pages/deals/dealsupcoming.tsx.tsx";
import Homeproducts from "./pages/home/homeproducts.tsx";
import Emailininbox from "./pages/contacts/emailininbox.tsx";
// import Emailverification from "./login/emailverification.tsx";
// import Forgotpasswordnew from "./login/forgotpasswordnew.tsx";
// import Newpassword from "./login/newpassword.tsx";
import Sales from "./pages/home/sale.tsx";
import Marketing from "./pages/home/marketing.tsx";
import Commerce from "./pages/home/commerce.tsx";
import Project from "./pages/home/project.tsx";
import AccountDevices from "./pages/MyPlan/pages/AccountInformation/devices.tsx";
import YourInfo from "./pages/MyPlan/pages/AccountInformation/index.tsx";
import AccountPrivacy from "./pages/MyPlan/pages/AccountInformation/privacy.tsx";
import AccountSecurity from "./pages/MyPlan/pages/AccountInformation/security.tsx";
import Storage from "./pages/MyPlan/pages/AccountInformation/storage.tsx";
import Addon from "./pages/MyPlan/pages/Billing/addon.tsx";
import Purchases from "./pages/MyPlan/pages/Billing/Billingndsubscription.tsx";
import Orderhistory from "./pages/MyPlan/pages/Billing/OrderHistory.tsx";
import Useractivitylog from "./pages/MyPlan/pages/UserManagement/useractivitylog.tsx";
import PlanLayout from "./pages/MyPlan/layouts/PlanLayout.tsx";
import Roleagent from "./pages/MyPlan/pages/UserManagement/roleagent.tsx";
import Rolefirst from "./pages/MyPlan/pages/UserManagement/roleteams.tsx";
import Agentroleview from "./pages/MyPlan/pages/UserManagement/agentroleview.tsx";
import { getAllUsers } from "./store/slice/users/user_management.ts";
import { getCallConfigurationList, getEmailConfigDomain, getSmsConfigurationList } from "./base/service/setup.ts";
import Upgradelist from "./pages/upgradeplan/upgradelist.tsx";
import DraggApp from "./pages/component/draggableaichat/files/draggablemain.tsx";


function App() {
  const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
  const navigate = useNavigate();
  // const { pathname } = useLocation();
  const dispatch = useDispatcher();
  const callServiceRef = useRef<any>(null);
  const popUpDetails: any = useSelector((state: any) => state.callPopUpDetails);
  const showDraggableChat :any =useSelector((state :any)=>state.commonData.popupChat)
  const isQueueList = useSelector((state: any) => state.commonData?.AllQueueList?.length > 0);
  const { acceptCallRoom, isCalling, incomingCallDetail } = popUpDetails;
  const decodedLoginToken: any = useMemo(() => {
    if (!accessToken) return null;
    try {
      return jwtDecode(accessToken);
    } catch (error) {
      return null;
    }
  }, [accessToken]);
  const pathName = window.location.pathname
  const isAuthRoute = pathName === "/login" || pathName === "/" ;
  const socketDomainId = decodedLoginToken?.domainId ?? decodedLoginToken?.domain_id;
  const socketExt = decodedLoginToken?.ext;
  

  useEffect(() => {
    if (accessToken) {
      ConnectSockets(accessToken, navigate, dispatch);
      const { createCallConnection } = CallService(dispatch, navigate);
      createCallConnection();
      dispatch(getAllUsers());
      getCallConfigurationList();
      getSmsConfigurationList();
      getEmailConfigDomain();
      return;
    }
  }, [accessToken]);

  // useEffect(() => {
  //   if (!accessToken || isAuthRoute) {
  //     void presenceService.destroy();
  //     return;
  //   }
  //   if (!socketDomainId || !socketExt) return;
  //   if (presenceService?.io) return;
  //   new presenceService(accessToken, navigate, dispatch, 0);
  // }, [accessToken, isAuthRoute, socketDomainId, socketExt, navigate, dispatch]);

  // useEffect(() => {
  //   if (!accessToken || isAuthRoute) {
  //     void socketService.destroy();
  //     return;
  //   }
  //   if (!socketDomainId || !socketExt) return;
  //   if (socketService?.io) return;
  //   new socketService(accessToken, navigate, dispatch);
  // }, [accessToken, isAuthRoute, socketDomainId, socketExt, navigate, dispatch]);

  // useEffect(() => {
  //   if (!accessToken || isAuthRoute) {
  //     callServiceRef.current?.unregister?.();
  //     callServiceRef.current = null;
  //     return;
  //   }
  //   if (callServiceRef.current) return;
  //   const callService = CallService(dispatch, navigate);
  //   callServiceRef.current = callService;
  //   void callService.createCallConnection();
  // }, [accessToken, isAuthRoute, dispatch, navigate]);
  const showCallWidget = (!acceptCallRoom && !!incomingCallDetail?.guid && accessToken && pathName !== "/login" && pathName !== "/");

  const loadQueueList = async () => {
    if (isQueueList) return;
    const response: any = await fetchQueueList();
    const queueList = Array.isArray(response?.getQueueLists) ? response.getQueueLists : [];
    dispatch(commonDataSliceActions.setQueueList(queueList));
  };


  useEffect(() => {

    getCurrentUserCallerID()
    if(!accessToken || pathName === "/login" || pathName === "/"){
      return;
    }

    loadQueueList();
    getDomainEmailConfig(dispatch)

    return () => {}

  }, [accessToken, dispatch]);

  const showIncomingCallNotify = Boolean(
    acceptCallRoom &&
    pathName !== "/login" &&
    pathName !== "/" &&
    accessToken
  );

  useEffect(() => {
    const ringer = document.getElementById("ringer") as HTMLAudioElement | null;
    if (!ringer) return;
    const baseUrl = import.meta.env.BASE_URL ?? "/";
    const normalizedBase = baseUrl.endsWith("/") ? baseUrl.slice(0, -1) : baseUrl;
    ringer.src = `${normalizedBase}/ringtone/ringback--linux.mp3`;
    ringer.load();
  }, [accessToken]);

  useEffect(() => {
    return () => {
      const ringer = document.getElementById("ringer") as HTMLAudioElement | null;
      try {
        if (ringer && typeof ringer.pause === "function") {
          ringer.pause();
          ringer.currentTime = 0;
        } else {
          console.warn("Ringer element not found or not an audio element.");
        }
      } catch (error) {
        console.error("Failed to pause ringer:", error);
      }
    };
  }, []);


  return (
    <>
      <Routes>
        <Route element={<NormalLayouts />}>
          <Route path="/contact" element={<Contacts />} />
          <Route path='/contactactivityview' element={<Contactactivityview />} />
          <Route path='/inbox' element={<Inbox />} />
          <Route path='/inboundcallview' element={<Inboundcallview />} />
          
          <Route path='/setup' element={<SetUpMain />} />
          <Route path="/setup/chat-conversation" element={<SetUpConversation />} />

          <Route path="contactsmsviewlist" element={<Contactsmsview />} />
          <Route path='/projectdrawer' element={<Projectdrawer />} />
          <Route path='/projectsinsights' element={<Projectsinsights />} />
          <Route path='/myactivity' element={<Myactivity />} />
          <Route path='/allactivity' element={<Allactivity />} />
          <Route path='/myactivityview' element={<Myactivityview />} />
          <Route path='/allrequirement' element={<Allrequirement />} />

          <Route path="/myplans" element={<PlanLayout />} />
          <Route path="/myplans/chat-conversation" element={<MyPlanConversation />} />
          {/* <Route path="/details" element={<Chatconversation />} /> */}
          <Route path="/Useradminroles" element={<PlanLayout />} />
          <Route path="/useradmin" element={<PlanLayout />} />
          <Route path="/useradminrolesteams" element={<PlanLayout />} />

          <Route path="/marketing" element={<Outlet />}>
            <Route index element={<MarketingInsights />} />
            <Route path="insights" element={<MarketingInsights />} />
            <Route path="campaign" element={<MarketingCampaign />} />
            <Route path="campaign-details" element={<MarketingCampaignDetails />} />
            <Route path="schedule-campaign" element={<MarketingScheduleCampaign />} />
            <Route path="past-campaign" element={<MarketingPastCampaign />} />
            <Route path="pending-campaign" element={<MarketingPendingCampaign />} />
            <Route path="campaign-conversation" element={<MarketingCampaignConversation />} />
            <Route path="template-conversation" element={<MarketingTemplateCampaignConversation />} />
            <Route path='upgradeplan' element={<Upgradelist />} />
          </Route>


          <Route path="/contact">
              <Route element={<Contacts />}>
              
              <Route index element={<ContactsList />} />
              <Route path="contactlist" element={<ContactsList />} />
              <Route path="companyList" element={<CompanyList />} />
              <Route path="contactdetais" element={<Contactname />} />
              <Route path="companydetais" element={<Companyname />} />
              <Route path="dealscontact" element={<Dealscontact />} />
              <Route path='videoviewdetails' element={<Videoviewdetails />} />
              <Route path="scheduledmeeting" element={<ScheduledMeeting/>} />
              <Route path='companyinboundcallview' element={<Companyinboundcallview />} />
              <Route path="contactnotes" element={<Contactnotes />} />
              <Route path="contactemail" element={<Emailininbox />} />
            </Route>

            <Route path='upgradeplan' element={<Upgradelist />} />
            <Route path="chat-conversation" element={<ContactConversation />} />

          </Route>

          <Route path="/deals">
            <Route element={<DealsLayout />}>
              <Route index element={<Dealslist />} />
              <Route path="dealslist" element={<Dealslist />} />
              <Route path="activitydeals" element={<Activitydeals />} />
              <Route path="dealscontent" element={<Dealscontent />} />
              <Route path="invoice" element={<Invoice />} />
              <Route path="whatsupcoming" element={<Upcoming/>} />
              <Route path="dealsupcoming" element={<Dealsupcoming/>} />
              <Route path="meeting" element={<Meeting/>} />
              <Route path="emailInbox" element={<Emailininbox />} />
              <Route path='dealvideoviewdetails' element={<Videoviewdetails />} />
              <Route path="dealscheduledmeeting" element={<ScheduledMeeting/>} />
              <Route path='dealcompanyinboundcallview' element={<Companyinboundcallview />} />
            </Route>
            <Route path='upgradeplan' element={<Upgradelist />} />
            <Route path="chat-conversation" element={<DealsConversation />} />
          </Route>

          <Route path="/home">
            <Route element={<HomeLayout />}>
              {/* <Route index element={<Dashboard />} />
              <Route path="dashboard" element={<Dashboard />} /> */}
              <Route index element={<Sales />} />
              <Route path='sales' element={<Sales />} />
              <Route path='marketing' element={<Marketing />} />
              <Route path='commerce' element={<Commerce />} />
              <Route path='project' element={<Project />} />
            </Route>
            <Route path='upgradeplan' element={<Upgradelist />} />
            <Route path="chat-conversation" element={<DashboardConversation />} />
          </Route>

          <Route path="/projects">
            <Route element={<ProjectsLayout/>}>
              <Route index element={<Projectsinsights/>} />
              <Route path='projectinsights' element={<Projectsinsights/>} />
              <Route path='projectdrawer' element={<Projectdrawer />} />
              <Route path='aichat' element={<Chatconversation />} />
            </Route>

            <Route path='upgradeplan' element={<Upgradelist />} />
            <Route path="chat-conversation" element={<ProjectConversation />} />
          </Route>


          <Route path="/products">
            <Route element={<ProductsLayout />}>
              <Route element={<Product />}>
              <Route index element={<Product />} />
                <Route path="insights" element={<ProductInsights />} />
                <Route path="tracker" element={<ProductTracker />} />
                <Route path="catalog" element={<ProductCatalog />} />
              </Route>
              <Route path="product-details" element={<Productname />} />
            </Route>
            <Route path='upgradeplan' element={<Upgradelist />} />
            <Route path="chat-conversation" element={<ProductConversation />} />
          </Route>

        </Route>
        <Route path="/*" element={<Navigate to="/login" />} />
        <Route path="/login" element={<Login />} />
        {/* <Route path="/email" element={<Emailverification/>} /> 
        <Route path="/newpassword" element={<Newpassword/>} /> 
        <Route path="/forgotpassword" element={<Forgotpasswordnew/>} />  */}
         
        <Route path='/onboarding' element={<OnBoardLayout />} />
        <Route path='/roleagent' element={<Roleagent />} />
        <Route path='/rolefirst' element={<Rolefirst />} />
        <Route path='/agentroleview' element={<Agentroleview />} />
      </Routes>
      {showIncomingCallNotify && <IncomingCallNotify />}
      {showCallWidget && <CallWidgets />}
     { showDraggableChat && <DraggApp/>}
      <audio id="ringer" preload="auto" loop />
    </>
  )
}

export default App
