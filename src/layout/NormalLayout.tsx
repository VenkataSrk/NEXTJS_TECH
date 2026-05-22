import { useState, useRef, useEffect, useMemo, useCallback } from "react";
import { Outlet, useLocation, useNavigate, useSearchParams } from "react-router-dom";
import styles from "./scss/normallayout.module.scss";
import logo from "../assets/images/mainlogosmall.png";
import fulllogo from "../assets/images/fulllogo.svg";
import inbox_icon from "../assets/images/menu/minbox.svg";
import contact_icon from "../assets/images/menu/mcontact.svg";
import deals_icon from "../assets/images/menu/mshake.svg";
import products_icon from "../assets/images/menu/mproduct.svg";
import projects_icon from "../assets/images/menu/mproject.svg";
import marketing_icon from "../assets/images/menu/mcampaign.svg";
import smallclose from "../assets/images/smallclose.svg";
import smallclosewhite from "../assets/images/smallclosewhite.svg";
import operations from "../assets/images/menu/operation.svg";
import history from "../assets/images/history.svg";
import create from "../assets/images/create.svg";
import Notification from "../pages/notification/notification";
import Chatconversation from "../pages/chat/chatconversation";
import { Cookie } from "../base/utils";
import { ClearStorage, GetItemFromStorage } from "../base/custom_hooks/useStorage";
import { useAppSelector, useDispatcher } from "../store/redux-store/dispatch";
import { LOGIN_USER } from "../store/constants/reducer_names";
import StatusDropdown from "../pages/component/statusDropdown";
import { LocalStorage } from "../base/custom_hooks/localStorageKeys";
import jwtDecode from "jwt-decode";
import { getAgentStatusDetails, getCheckEmailconfig, getNotificationApi, insertLogoutLog, insertUserActivityLogInLog } from "../base/service/core_serviceApi";
import presenceService from "../base/sockets/presence";
import socketService from "../base/sockets/socketService";
import { commonDataSliceActions } from "../store/slice/commonSilce/commonSlice";
import { AgentStateAction } from "../store/slice/agentStatusSlice/agent_state";
import { useSelector } from "react-redux";
import { v4 as uuidv4 } from "uuid";
import { MarketingAgentChatAction } from "../store/slice/marketingSlice/marketingSlice";
import { getOnboardingCompletion, getUserOnboardDetails } from "../base/onboardConfig";
import { ACTIONS_ACCESS, config, levelOfAccess, pothysDomainId, welCareDomainId } from "../base/constant";
import store from "../store/redux-store/root_store";
import aiChatAssistSocketservice from "../services/aiChatService";
import inboxAiChatAssistSocketservice from "../services/inboxAiChatService";
import { Toastbox } from "../base/shared/toastmsg";
import premiumSvg from "../assets/images/premium.svg";
import { CoreServices } from "../base/service/apiservices";
import axios from "axios";
import { useFetchMarketingInsight, useFetchProductInsights, useFetchProjectInsight, useGetCustomerDeals, useGetHomeMarketing, useGetHomeProject, useGetHomeSales, useGetMainContactList, useGetProductList } from "./initialload";

const hiddenTopBar = ["inbox", 'upgradeplan']

const menuIconMap = {
  inbox: inbox_icon,
  contact: contact_icon,
  deals: deals_icon,
  products: products_icon,
  projects: projects_icon,
  marketing: marketing_icon,
  setup: operations,
};

export default function NormalLayouts() {

  /**
   * @description Preloading — warms data ahead of routes whose pages don't fetch on mount.
   */
  useGetCustomerDeals();
  useFetchProjectInsight();
  useFetchProductInsights();
  useGetProductList();
  useFetchMarketingInsight();
  useGetMainContactList()
  // Home
  useGetHomeSales();
  useGetHomeMarketing();
  useGetHomeProject();

  // -----------------------------------------------------------------------
    
  const { pathname } = useLocation();
  const profileRef = useRef<HTMLDivElement>(null);
  const notifyRef = useRef<HTMLDivElement>(null);
  const queryParams = useSearchParams();
  const navigate = useNavigate();
  const dropdownRef = useRef<any>(null);
  const topLayerLeftRef = useRef<HTMLDivElement | null>(null);
  const prevTopLayerCountRef = useRef(0);
  const [notifyopen, setnotifyopen] = useState(false);
  // const [tabs, setTabs] = useState<any>([]);
  // const [activeTab, setActiveTab] = useState<any>(null);
  const currentActiveRoute = pathname?.split('/')[1];
  const [openDropdown, setOpenDropdown] = useState(false);
  const [openProfileDropdown, setOpenProfileDropdown] = useState(false);
  const { status: statusMenuList, currentStatus } = useSelector((state: any) => state.agentState);
  const permissionsData = useSelector((state: any) => state?.loginUser?.rolesandpermissions);
  const [agentStatusData, setAgentStatusData] = useState<any[]>([]);
  const { notificationUnreadCount } = useSelector((state: any) => state.homeStore);
  const dispatch = useDispatcher();
  const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
  const loginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS) || {};
  const currentPath: any = pathname?.split('/')?.[1];
  const childcurrentPath: any = pathname?.split('/')?.[2];
  const tabMenu: any = useSelector((state: any) => state?.commonData?.tabMenu);
  const currentTopLayerTabs = tabMenu?.[currentPath] ?? [];
  console.log("loginCrmUserDetails", loginCrmUserDetails)
  const isAdmin = loginCrmUserDetails?.org_role_id_crm == 3;
  const { onboardingStaus } = useSelector((store: any) => store.onboarding);
  const [isSetupCompleted, setIsSetupCompleted] = useState<number>(0);
  const [setupPercentage, setSetupPercentage] = useState<number>(0);
  const [isLoading, setIsLoading] = useState(true);
  const { appTabs, activeTab, dropdownTabs } = useSelector((state: any) => state.commonData);
  const decodedLoginToken: any = useMemo(() => {
    if (!accessToken) return null;
    try {
      return jwtDecode(accessToken);
    } catch (error) {
      console.error("Invalid login token:", error);
      return null;
    }
  }, [accessToken]);
  const socketDomainId = decodedLoginToken?.domainId ?? decodedLoginToken?.domain_id;
  const socketExt = decodedLoginToken?.ext ?? decodedLoginToken?.sipLoginId ?? decodedLoginToken?.sip_login_id;

  const statusOptions = useMemo(() => {
    try {
      const restricted = new Set(["busy", "away", "offline"]);
      const allowed = new Set(["ready", "not ready", "offline", "busy", "away"]);

      const arr: any = [];
      for (const item of agentStatusData || []) {
        const value = String(item?.statusName ?? item?.value ?? "").toLowerCase();
        if (!allowed.has(value)) continue;

        arr.push({
          ...item,
          label: item?.statusName ?? item?.label,
          value,
          colorCode: item?.colorCode,
          disabled: restricted.has(value),
        });
      }

      arr.sort((a: any, b: any) => (b.value === "ready") - (a.value === "ready") || (b.value === "not ready") - (a.value === "not ready"));

      return arr.length
        ? arr
        : [
          { label: "Ready", value: "ready", colorCode: "#22C55E" },
          { label: "Not Ready", value: "not ready", colorCode: "#EF4444" },
          { label: "Offline", value: "offline", colorCode: "#9CA3AF", disabled: true },
        ];

    } catch (err) {
      console.error(err);
      return [];
    }
  }, [agentStatusData]);

  const statusTextMap: any = {
    "ready": "All Set! You’re Ready",
    "not ready": "Are you ready?",
    "transfer only": "I’m ready to take over!",
    "busy": "In progress—stay focused",
    "away": "Resume when ready",
  };

  const currentStatusColor = statusOptions.find((i) => i.label == currentStatus?.statusName)?.colorCode || '#EF4444'
  const currentStatusKey = currentStatus?.statusName?.toLowerCase() || 'not ready';
  const currentStatusText = statusTextMap[currentStatusKey] ?? "All Set! You’re Ready";

  const doSelectTheAgentStatus = async (currentState: any) => {
    sessionStorage.setItem('presence', currentState.statusName)
    try {
      const payload = {
        domainId: socketDomainId,
        isVoice: currentState?.isVoice ?? 0,
        isVoiceCallTransfer: currentState?.isVoiceCallTransfer ?? 0,
        isChat: currentState?.isChat ?? 0,
        isChatTransfer: currentState?.isChatTransfer ?? 0,
        isEmail: currentState?.isEmail ?? 0,
        isEmailTransfer: currentState?.isEmailTransfer ?? 0,
        stateTime: Math.floor(Date.now() / 1000),
        statusName: currentState?.statusName ?? currentState?.label ?? "Not Ready",
        ext: socketExt,
        email: decodedLoginToken?.Email ?? decodedLoginToken?.username,
        userId: loginCrmUserDetails?.UserID ?? decodedLoginToken?.userId,
        routingProfileId: loginCrmUserDetails?.routing_profile_id ?? 0,
        userName: loginCrmUserDetails?.UserName ?? decodedLoginToken?.profileName,
        roleId: loginCrmUserDetails?.org_role_id_crm ?? decodedLoginToken?.roleId,
        agentStatus: 1,
        skill: [],
        routingMethod: 0,
        inboundCall: loginCrmUserDetails?.inboundCall,
        outboundCampaignCall: 0,
        forced_presence: 0,
        isCrm: 1
      };
      presenceService?.io?.emit("updatestate_backend", payload);
    } catch (error) {
      console.log("doSelectTheAgentStatus error", error);
    }
  };

  useEffect(() => {
    let cancelled = false;
    const fetchData = async () => {
      try {
        const res = await getAgentStatusDetails();
        if (!cancelled) {
          const statusList = Array.isArray(res) ? res : [];
          setAgentStatusData(statusList);
          dispatch(AgentStateAction.addStatusList(statusList));
        }
      } catch (err) {
        if (!cancelled) {
          console.error("Error fetching agent status:", err);
          setAgentStatusData([]);
        }
      }
    };
    fetchData();
    getCheckEmailconfig(dispatch)
    return () => {
      cancelled = true;
    };
  }, []);

  useEffect(() => {
    if (!decodedLoginToken) {
      navigate("/login");
      return;
    }
    getNotificationApi(null, dispatch);
  }, []);

  useEffect(() => {
    const handleClickOutside = (e) => {
      if (dropdownRef.current && !dropdownRef.current.contains(e.target)) {
        setOpenDropdown(false);
      }
    };
    document.addEventListener("mousedown", handleClickOutside);
    return () => document.removeEventListener("mousedown", handleClickOutside);
  }, []);

  const handleTabClick = (tab: any) => {
    const target = tab?.disabled ? `${tab.id}/upgradeplan?type=${tab.id}` : tab.path;
    if (pathname === target) return;
    navigate(target);
    dispatch(commonDataSliceActions.setActiveAppTab(tab.id));
  };

  const hasInitializedTabs = useRef(false);

  const filteredTabs = useMemo(() => {
    const { permissions } = permissionsData || {};
    if (!permissions) return [];
  
    if (isAdmin) {
      return dropdownTabs.map((t) => ({
        ...t,
        disabled: !levelOfAccess(t.moduleId, ACTIONS_ACCESS.VIEW, permissions),
      }));
    }
  
    return dropdownTabs.filter((t) =>
      levelOfAccess(t.moduleId, ACTIONS_ACCESS.VIEW, permissions)
    );
  }, [permissionsData, dropdownTabs, isAdmin]);

  useEffect(() => {
    if (hasInitializedTabs.current) return;
    if (!permissionsData?.permissions) return;
    if (appTabs?.length) return;
  
    hasInitializedTabs.current = true;
  
    dispatch(commonDataSliceActions.setAppTabs(filteredTabs));
  }, [permissionsData, appTabs, filteredTabs, dispatch]);


  const getDropdownTabs = useMemo(() => {
    return filteredTabs.filter(
      (t) => !appTabs?.some((tab) => tab?.path === t.path)
    );
  }, [filteredTabs, appTabs]);

  const addTabFromDropdown = (item: any) => {
    const existingTab = appTabs.find((t: any) => t.path === item.path);

    if (existingTab) {
      dispatch(commonDataSliceActions.setActiveAppTab(existingTab?.id));
      navigate(existingTab?.path);
      setOpenDropdown(false);
      return;
    }

    // const newTab = {
    //   id: item.path,
    //   title: item.title,
    //   path: item.path,
    // };

    if(item?.disabled) {
      navigate(`${item.id}/upgradeplan?type=${item.id}`);
    }else {
      navigate(item.path);
    }

    dispatch(commonDataSliceActions.addAppTab(item));
    setOpenDropdown(false);
  };

  /* ---------- Close tab ---------- */
  // const closeTab = (id) => {
  //   const filteredTabs = tabs.filter((t) => t.id !== id);
  //   setTabs(filteredTabs);

  //   if (id === activeTab) {
  //     setActiveTab(null);
  //     navigate("/home");
  //   }
  // };

  const closeTab = (id: any) => {
    const isActive = id === activeTab;

    dispatch(commonDataSliceActions.removeAppTab(id));

    if (isActive) {
      navigate("/home/sales");
    }
  };

    const inActivelogout=async(email :any)=>{
    try {
      const response = await axios.post(
        `${config().CORESERVICE_URL}/logout`,
        {
          email,
        }
      );

      console.log("Success:", response.data);
     
    } catch (error: any) {
      console.error("Error:", error.response?.data || error.message);
    }
  }

  const signOut = async () => {
    try {
      void socketService.destroy();
      void presenceService.destroy();
      void aiChatAssistSocketservice.destroy();
      void inboxAiChatAssistSocketservice.destroy();
      insertUserActivityLogInLog()
      inActivelogout(decodedLoginToken?.username)
      Cookie.clear();
      ClearStorage();
      localStorage.clear();
      sessionStorage.clear();
      dispatch(commonDataSliceActions.reset());
      dispatch(MarketingAgentChatAction.reset());
      dispatch({ type: `${LOGIN_USER}/logout` });
      navigate("/login", { replace: true });
    } catch (error) {
      navigate("/login", { replace: true });
    }
  }


  const handleMenuTabClose = (item: any, isActivePath: any) => {
    console.log('isActivePath>>>>>>', isActivePath, item)

    const pathGroup = item?.path?.split('/')?.[1];
    if (pathGroup === 'marketing') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "marketing" }));
      if (isActivePath) {
        navigate('/marketing/insights');
      }
    } else if (pathGroup === 'contact') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "contact" }));
      if (isActivePath) {
        navigate('/contact/contactlist');
      }
    } else if (pathGroup === 'deals') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "deals" }));
      if (isActivePath) {
        navigate('/deals');
      }
    }
    else if (pathGroup === 'home') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "home" }));
      if (isActivePath) {
        navigate('/home/sales');
      }
    }
    else if (pathGroup === 'projects') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "projects" }));
      if (isActivePath) {
        navigate('/projects/projectinsights');
      }
    }
    else if (pathGroup === 'products') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "products" }));
      if (isActivePath) {
        navigate('/products/insights');
      }
    }
    else if (pathGroup === 'setup') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "setup" }));
      if (isActivePath) {
        navigate('/setup');
      }
    }
    else if (pathGroup === 'myplans') {
      dispatch(commonDataSliceActions.setRemoveTabMenu({ index: item?.index, type: "myplans" }));
      if (isActivePath) {
        navigate('/myplans');
      }
    }

    console.log("pathGroup", pathGroup)

    // const comparePath: any = item?.path?.split('marketing')?.[1];

    // switch (pathGroup) {
    //   case 'marketing': {
    //     dispatch(commonDataSliceActions.setRemoveTabMenuMarketing(item?.index));
    //     if (comparePath == window.location.href.split('marketing')?.[1]) {
    //       console.log("comparepath", comparePath, window.location.href.split('marketing')?.[1])
    //       navigate('/marketing/insights');
    //     }
    //     break;
    //   }

    //   case 'contact': {
    //  dispatch(commonDataSliceActions.setRemoveTabMenu({index: item?.index, type: "contact"}));
    //     if (comparePath == window.location.href.split('contact')?.[1]) {
    //        navigate('/contact/contactlist');
    //     }
    //     break;
    //   }

    //   default:
    //     break;
    // }
  }

  const handleNewConversation = () => {
    const pathGroup = pathname?.split('/')?.[1];
    switch (pathGroup) {
      case 'marketing': {
        const sessionId: any = uuidv4() + 'marketing';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/marketing/campaign-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "marketing" }));
        navigate(`/marketing/campaign-conversation?sessionId=${sessionId}`);
      }
        break;

      case 'contact': {
        const sessionId: any = uuidv4() + 'contact';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/contact/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "contact" }));
        navigate(`/contact/chat-conversation?sessionId=${sessionId}`);
      }
        break;

      case 'deals': {
        const sessionId: any = uuidv4() + 'deals';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/deals/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "deals" }));
        navigate(`/deals/chat-conversation?sessionId=${sessionId}`);
      }
        break;
      case 'home': {
        const sessionId: any = uuidv4() + 'home';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/home/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "home" }));
        navigate(`/home/chat-conversation?sessionId=${sessionId}`);
      }
        break;
      case 'projects': {
        const sessionId: any = uuidv4() + 'projects';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/projects/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "projects" }));
        navigate(`/projects/chat-conversation?sessionId=${sessionId}`);
      }
        break;
      case 'products': {
        const sessionId: any = uuidv4() + 'products';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/products/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "products" }));
        navigate(`/products/chat-conversation?sessionId=${sessionId}`);
      }
        break;
      case 'setup': {
        const sessionId: any = uuidv4() + 'setup';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/setup/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "setup" }));
        navigate(`/setup/chat-conversation?sessionId=${sessionId}`);
      }
        break;
      case 'myplans': {
        const sessionId: any = uuidv4() + 'myplans';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/myplans/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "myplans" }));
        navigate(`/myplans/chat-conversation?sessionId=${sessionId}`);
      }
        break;

      default:
        break;
    }
  }

  // useEffect(() => {
  //   if (isAiChatNavigate) {
  //     addTabFromDropdown({ id: "aichat", title: "AI Chat", path: "/aichat", img: menu6 });
  //     dispatch(commonDataSliceActions.setAiChatNavigate(false));
  //   }
  // }, [isAiChatNavigate])

  const navigateOnboard = () => {
    navigate('/onboarding')
  }

  useEffect(() => {
    getOnboardingData()
  }, [])

  const getOnboardingData = async () => {
    const onboardRes: any = await getUserOnboardDetails(dispatch);
    setIsSetupCompleted(onboardRes?.isCompleted);
    setIsLoading(false);
  };

  const getPercentage = (onboardingStatus: any) => {
    const res = getOnboardingCompletion(onboardingStatus);
    setSetupPercentage(res);
  };

  useEffect(() => {
    getPercentage(onboardingStaus)
  }, [onboardingStaus])

  useEffect(() => {
    const currentCount = currentTopLayerTabs.length;
    if (currentCount > prevTopLayerCountRef.current && topLayerLeftRef.current) {
      topLayerLeftRef.current.scrollTo({
        left: topLayerLeftRef.current.scrollWidth,
        behavior: "smooth",
      });
    }
    prevTopLayerCountRef.current = currentCount;
  }, [currentTopLayerTabs.length]);

  const isActive = currentActiveRoute === "home";

  useEffect(() => {
    const handleClickOutside = (event: MouseEvent) => {
      if (profileRef.current && !profileRef.current.contains(event.target as Node)) {
        setOpenProfileDropdown(false);
      }
    };

    document.addEventListener('mousedown', handleClickOutside);
    return () => document.removeEventListener('mousedown', handleClickOutside);
  }, []);

  useEffect(() => {
    const handleClickOutside = (event: MouseEvent) => {
      if (notifyRef.current && !notifyRef.current.contains(event.target as Node) && notifyopen) {
        setnotifyopen(false);
      }
    };
    document.addEventListener('mousedown', handleClickOutside);
    return () => document.removeEventListener('mousedown', handleClickOutside);
  }, [notifyopen]);

  // for horizontal scroll
  const [showLeftArrow, setShowLeftArrow] = useState(false);
  const [showRightArrow, setShowRightArrow] = useState(false);

  const checkOverflow = useCallback(() => {
    try {
      const el = topLayerLeftRef.current;
      if (!el) return;
      const isOverflowing = el.scrollWidth > el.clientWidth;
      const atStart = el.scrollLeft <= 2;
      const atEnd = el.scrollLeft + el.clientWidth >= el.scrollWidth - 2;
      setShowLeftArrow(isOverflowing && !atStart);
      setShowRightArrow(isOverflowing && !atEnd);
    } catch (error) {
      console.log(error)
    }
  }, []);

  useEffect(() => {
    try {
      const el = topLayerLeftRef.current;
      if (!el) return;
      checkOverflow();
      el.addEventListener("scroll", checkOverflow);
      window.addEventListener("resize", checkOverflow);
      return () => {
        el.removeEventListener("scroll", checkOverflow);
        window.removeEventListener("resize", checkOverflow);
      };
    } catch (error) {
      console.log(error)
    }
  }, [checkOverflow, tabMenu, currentPath]);

  const handleScrollRight = () => {
    try {
      topLayerLeftRef.current?.scrollBy({ left: 200, behavior: "smooth" });
      // setTimeout(checkOverflow, 350);
    } catch (error) {
      console.log(error)
    }
  };

  const handleScrollLeft = () => {
    try {
      topLayerLeftRef.current?.scrollBy({ left: -200, behavior: "smooth" });
      setTimeout(checkOverflow, 350);
    } catch (error) {
      console.log(error)
    }
  };

  const [isMobile, setIsMobile] = useState(window.innerWidth < 1023);

  useEffect(() => {
    const handleResize = () => setIsMobile(window.innerWidth < 1023);
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);

    const updateHeartBeat=async()=>{
      try {
          const body :any={
            email:loginCrmUserDetails?.Email
          }
          await CoreServices.post(body,'/updateHeartBeat').catch((err :any)=>{
            console.log("err",err)
          })
      } catch (error) {
        console.log("err",error)
      } 
    }


  useEffect(() => {
    let isMounted = true;

    const run = async () => {
      if (!isMounted) return;

      try {
        await updateHeartBeat();
      } catch (error) {
        console.log("err:", error);
      }

      if (isMounted) {
        setTimeout(run, 60000);
      }
    };
    
    run();

    return () => {
      isMounted = false;
    };
  }, []);

  return (
    <div className={styles.overallmain}>
      <Toastbox />
      <div className={styles.mainlayout}>
        <div className={styles.mainlogo}>
          <img src={logo} alt="" />
        </div>


        <div className={styles.mainmenu}>
          <div className={styles.mainmenuleft}>
            <div className={`${styles.mainhome} ${styles.tab} ${isActive ? styles.active : ""}`} onClick={() => navigate("/home/sales")}>
              <svg xmlns="http://www.w3.org/2000/svg" width="20" height="18" viewBox="0 0 20 18" fill="none">
                <path d="M19.3086 8.00177L11.6646 0.741975C10.6207 -0.247325 8.92236 -0.247325 7.87746 0.741975L0.233462 8.00177C-0.0668378 8.28697 -0.0790377 8.76157 0.206162 9.06227C0.353662 9.21758 0.551862 9.29567 0.750062 9.29567C0.935562 9.29567 1.12166 9.22727 1.26666 9.08957L1.66456 8.71167V16.0515C1.66456 17.0349 2.44976 17.8357 3.41456 17.8357H6.51126C7.47606 17.8357 8.26126 17.0349 8.26126 16.0515V13.8552H11.2813V16.0515C11.2813 17.0349 12.0665 17.8357 13.0313 17.8357H16.1275C17.0923 17.8357 17.8775 17.0349 17.8775 16.0515V8.71167L18.2754 9.08957C18.5752 9.37278 19.0498 9.36207 19.3359 9.06227C19.6211 8.76147 19.6088 8.28687 19.3086 8.00177ZM16.3774 16.0516C16.3774 16.2078 16.2651 16.3358 16.1274 16.3358H13.0312C12.8935 16.3358 12.7812 16.2079 12.7812 16.0516V13.1053C12.7812 12.6912 12.4453 12.3553 12.0312 12.3553H7.51116C7.09706 12.3553 6.76116 12.6912 6.76116 13.1053V16.0516C6.76116 16.2078 6.64886 16.3358 6.51116 16.3358H3.41446C3.27676 16.3358 3.16446 16.2079 3.16446 16.0516V7.28697L8.91006 1.82997C9.37636 1.38657 10.1655 1.38657 10.6318 1.82997L16.3774 7.28697V16.0516Z" fill="#fff" />
              </svg>
            </div>
            {appTabs.map((tab) => (
              <div
                key={tab.id}
                className={`${styles.tab} ${tab?.path?.split('/')[1] === currentActiveRoute ? styles.active : ""}`}
                onClick={() => handleTabClick(tab)}
              >
                <div className={styles.title}>{tab.title}</div>
                <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none" onClick={(e) => { e.stopPropagation(); closeTab(tab.id); }}>
                  <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#2D2B2F" />
                </svg>
              </div>
            ))}

            {getDropdownTabs?.length > 0 && <div className={styles.dropdown} ref={dropdownRef}>
              <div className={styles.addicon} onClick={() => setOpenDropdown(!openDropdown)}>
                <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                  <path d="M13.2803 8.23032H9.7295V4.68052C9.7295 4.26642 9.3936 3.93052 8.9795 3.93052C8.5654 3.93052 8.2295 4.26642 8.2295 4.68052V8.23032H4.6797C4.2656 8.23032 3.9297 8.56623 3.9297 8.98032C3.9297 9.39442 4.2656 9.73032 4.6797 9.73032H8.2295V13.2806C8.2295 13.6947 8.5654 14.0306 8.9795 14.0306C9.3936 14.0306 9.7295 13.6947 9.7295 13.2806V9.73032H13.2803C13.6944 9.73032 14.0303 9.39442 14.0303 8.98032C14.0303 8.56623 13.6944 8.23032 13.2803 8.23032Z" fill="white" />
                  <path d="M16.42 1.63462C15.0147 0.301125 13.0489 0.0755251 11.1348 0.0306251C9.545 -0.0103749 8.4219 -0.0098751 6.836 0.0301249C4.92 0.0760249 2.9542 0.301625 1.5469 1.63653C0.0762002 3.04373 0.0362001 5.14042 0.00390005 6.82502L0 10.9676L0.00390005 11.1458C0.0361001 12.8309 0.0762 14.9285 1.5488 16.326C2.955 17.66 4.9209 17.8856 6.834 17.93C7.6289 17.9505 8.3067 17.9608 8.9844 17.9608C9.6621 17.9608 10.3399 17.9505 11.1328 17.9305C13.0488 17.8851 15.0146 17.6595 16.4199 16.326C17.8857 14.9339 17.9306 12.7796 17.9599 11.3387V6.60682C17.9306 5.18103 17.8858 3.02672 16.42 1.63462ZM16.46 11.3231C16.4278 12.8797 16.3223 14.3509 15.3868 15.2381C14.2989 16.2713 12.6026 16.3953 11.0958 16.431C9.5372 16.4701 8.4337 16.4706 6.8712 16.4305C5.3663 16.3953 3.671 16.2713 2.5821 15.2381C1.5635 14.2723 1.5313 12.596 1.503 11.1175L1.5001 7.03353L1.503 6.85332C1.5313 5.37383 1.5635 3.69662 2.5821 2.72242C3.67 1.69023 5.3663 1.56572 6.8731 1.52952C7.6524 1.51002 8.3184 1.49972 8.9844 1.49972C9.6504 1.49972 10.3164 1.51003 11.0977 1.53003C12.6026 1.56523 14.2989 1.68973 15.3868 2.72242C16.3204 3.60812 16.4278 5.07592 16.46 6.63013V11.3231Z" fill="white" />
                </svg>
              </div>

              {/* {openDropdown && (
                <div className={styles.dropdownmenu}>
                  {getDropdownTabs?.map((item) => (
                    <div key={item.id} className={styles.dropdownmenuname} onClick={() => addTabFromDropdown(item)}><img src={item.img} alt="" />{item.title}</div>
                  ))}
                </div>
              )} */}

              {openDropdown && (
                isMobile ? (
                  <div className={styles.mobileMenuOverlay}>
                    <div className={styles.mobileMenu}>

                      <div className={styles.mobileHeader}>
                        <img src={fulllogo} alt="" />
                        <img src={smallclosewhite} onClick={() => setOpenDropdown(false)} />
                      </div>

                      {/* {getDropdownTabs?.map((item) => (
                        <div
                          key={item.id}
                          className={styles.mobileMenuItem}
                          onClick={() => {
                            addTabFromDropdown(item);
                            setOpenDropdown(false);
                          }}
                        >
                          <img src={item.img} alt="" />
                          {item.title}
                        </div>
                      ))} */}


                      {appTabs?.length > 0 && (
                        <>
                          {appTabs.map((tab) => {
                            const key = tab.path?.split("/")?.[1]; // extract module
                            const icon = menuIconMap[key];

                            return (
                              <div
                                key={tab.id}
                                className={styles.mobileMenuItem}
                                onClick={() => {
                                  handleTabClick(tab);
                                  setOpenDropdown(false);
                                }}
                              >
                                {icon && <img src={icon} alt="" />}
                                {tab.title}
                              </div>
                            );
                          })}
                        </>
                      )}


                      {getDropdownTabs?.length > 0 && (
                        <>
                          {getDropdownTabs.map((item) => (
                            <div
                              key={item.id}
                              className={styles.mobileMenuItem}
                              onClick={() => {
                                addTabFromDropdown(item);
                                setOpenDropdown(false);
                              }}
                            >
                              <img src={item.img} alt="" />
                              {item.title}
                            </div>
                          ))}
                        </>
                      )}

                    </div>
                  </div>
                ) : (
                  <div className={styles.dropdownmenu}>
                    {getDropdownTabs?.map((item) => (
                      <div
                      style={{display:"flex", justifyContent:"space-between"}}
                        key={item.id}
                        className={styles.dropdownmenuname}
                        onClick={() => addTabFromDropdown(item)}
                      >
                        <div style={{display:"flex", gap:"10px", alignItems:"center"}}>

                        <img src={item.img} alt="" />
                        {item.title}</div> 
                        {item?.disabled && <img src={premiumSvg} alt="" />}
                      </div>
                    ))}
                  </div>
                )
              )}
            </div>}
          </div>

          <div className={styles.mainmenuright}>
            {(!isLoading && isAdmin && isSetupCompleted !== 1) &&
              <div className={styles.completevalue} onClick={navigateOnboard}>
                <div className={styles.setitle}>Complete setup</div>
                <CircularProgress value={setupPercentage} />
              </div>}

            <div className={styles.notify} onClick={(e: any) => { e.stopPropagation(); setnotifyopen(prev => !prev) }} ref={notifyRef} >
              {notificationUnreadCount > 0 && <span className={styles.counterspan}>{notificationUnreadCount > 99 ? "99+" : notificationUnreadCount}</span>}
              <svg
                xmlns="http://www.w3.org/2000/svg" width="17" height="20" viewBox="0 0 17 20" fill="none">
                <path d="M10.1996 17.2214H6.33237C5.87547 17.2214 5.58867 17.7998 5.81067 18.2673C6.28947 19.2758 7.20957 19.9582 8.26597 19.9582C9.32237 19.9582 10.2425 19.2758 10.7214 18.2673C10.9433 17.7998 10.6566 17.2214 10.1996 17.2214Z" fill="white" />
                <path d="M16.0254 13.6967C15.4698 13.2026 15.0023 12.643 14.6644 11.9789C14.2471 11.1591 14.0522 10.2892 14.0558 9.369C14.0591 8.4982 14.0828 7.6251 14.0295 6.7572C13.8873 4.4491 11.8976 2.1583 9.76417 1.5682C9.73917 1.5614 9.72067 1.5579 9.69677 1.5518C9.69947 1.5159 9.71327 1.4839 9.71327 1.4473C9.71327 0.648 9.06537 0 8.26597 0C7.46657 0 6.81867 0.6479 6.81867 1.4473C6.81867 1.4839 6.83487 1.5154 6.83757 1.5513C6.81277 1.5576 6.79347 1.5611 6.76767 1.5681C4.63447 2.1582 2.64467 4.449 2.50267 6.7571C2.44917 7.625 2.47287 8.4981 2.47617 9.3689C2.47987 10.2891 2.28487 11.1589 1.86767 11.9788C1.52957 12.6429 1.06217 13.2024 0.50687 13.6966C0.0642698 14.0899 -0.0958301 14.5884 0.0556699 15.1656C0.23377 15.8446 0.79837 16.2388 1.56847 16.2388C2.68467 16.239 3.80087 16.239 4.91727 16.239C6.03367 16.239 7.14977 16.239 8.26607 16.2388C9.38227 16.239 10.4986 16.239 11.6149 16.239C12.7312 16.239 13.8474 16.239 14.9637 16.2388C15.734 16.2388 16.2984 15.8445 16.4765 15.1656C16.6278 14.5885 16.4678 14.09 16.0254 13.6967ZM1.59097 14.739C2.28807 14.1025 2.81647 13.4214 3.20447 12.6594C3.72117 11.644 3.98087 10.5351 3.97617 9.3635L3.97227 8.81C3.96637 8.1279 3.96077 7.4836 3.99987 6.8493C4.09847 5.2458 5.57877 3.4535 7.16767 3.0139C7.30657 2.9756 7.42167 2.9551 7.52067 2.9392C7.77197 2.8945 8.02257 2.8686 8.26607 2.8625C8.50947 2.8688 8.76117 2.8947 9.04247 2.9443C9.11007 2.955 9.22507 2.9755 9.36427 3.0141C10.9531 3.4536 12.4335 5.2458 12.5323 6.8493C12.5712 7.4828 12.5656 8.1266 12.5597 8.8083L12.5558 9.3632C12.5512 10.5353 12.8109 11.6442 13.3275 12.6593C13.7097 13.4103 14.2287 14.0829 14.9383 14.7389L1.59097 14.739Z" fill="white" />
              </svg>
              {notifyopen && <Notification close={setnotifyopen} />}
            </div>

            <div
              style={{ position: "relative" }}
              className={styles.userprofile}
              ref={profileRef}
              onClick={() => setOpenProfileDropdown((prev) => !prev)}
            >
              <h3 style={{ color: "white" }}>
                {(decodedLoginToken?.first_name || decodedLoginToken?.profileName || "")?.charAt(0).toUpperCase()}
              </h3>
              <span
                className={`${styles.stsdott} ${styles.smalldot}`}
                style={{ backgroundColor: currentStatusColor }}
              ></span>
              <div className='dropdownprof' onClick={(e) => e.stopPropagation()}>
                {openProfileDropdown &&
                  <div className={styles.profiledropdown}>
                    <div className={styles.profileMenu}>
                      <div className={styles.avatarSection}>
                        <div className={styles.avatar}>{(decodedLoginToken?.first_name || decodedLoginToken?.profileName || "")?.charAt(0)?.toUpperCase()}
                          <span
                            className={styles.stsdott}
                            style={{ backgroundColor: currentStatusColor }}
                          ></span></div>
                        <h3>{decodedLoginToken?.first_name || decodedLoginToken?.profileName || ""}</h3>
                        <p className={styles.mailid}>{(decodedLoginToken?.Email) ?? ""}</p>
                      </div>
                      <div className={styles.stssection}>
                        <span className={styles.allsettxt}>
                          {currentStatusText}
                        </span>

                        <StatusDropdown
                          value={currentStatus}
                          options={statusOptions}
                          onChange={(selectedOption: any) => {
                            dispatch(commonDataSliceActions.setAgentStatus(selectedOption));
                            dispatch(AgentStateAction.setCurrentStatus({ status: selectedOption?.statusName }));
                            doSelectTheAgentStatus(selectedOption);
                          }}
                        />
                      </div>
                      <div className={styles.menu}>
                        <div className={styles.menuItem} onClick={() => navigate("/myplans")}>
                          <div className={styles.lefts} >
                            <svg xmlns="http://www.w3.org/2000/svg" width="21" height="20" viewBox="0 0 21 20" fill="none">
                              <path d="M19.3178 9.6815C20.0004 9.0038 20.2348 8.0165 19.9286 7.1044C19.6229 6.1923 18.8407 5.5458 17.8881 5.4169L14.4677 4.9521C14.1694 4.9121 13.9101 4.7265 13.7739 4.458L12.2177 1.3779C11.788 0.5263 10.9321 0 9.9799 0C9.9721 0 9.9638 1.65892e-07 9.956 0.00100017C8.9941 0.00980017 8.1372 0.5528 7.7197 1.419L6.2212 4.5274C6.0903 4.7989 5.8345 4.9903 5.5366 5.0362L2.126 5.5645C1.1758 5.711 0.406299 6.3721 0.117699 7.2891C-0.171401 8.2071 0.0810995 9.1905 0.7759 9.8555L3.2447 12.2168C3.4786 12.4404 3.587 12.7637 3.5352 13.083L2.9893 16.4551C2.8355 17.4053 3.2266 18.3418 4.0103 18.8994C4.4463 19.2099 4.9532 19.3682 5.4634 19.3682C5.8696 19.3682 6.2779 19.2676 6.6533 19.0655L9.6929 17.4307C9.9571 17.2891 10.2769 17.2852 10.545 17.4229L13.6148 19C14.4693 19.4385 15.481 19.3574 16.254 18.7852C17.027 18.213 17.4 17.2686 17.2286 16.3223L16.6139 12.9268C16.5602 12.6299 16.6559 12.3262 16.8698 12.1133L19.3178 9.6815Z" fill="#404040" />
                            </svg>My Plans
                          </div>
                          <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                          </svg>
                        </div>
                        <div className={styles.menuItem} onClick={signOut}>
                          <div className={styles.lefts}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="19" height="17" viewBox="0 0 19 17" fill="none">
                              <path d="M14.0236 2.74238L18.5807 7.29941C18.8479 7.5666 19 7.93398 19 8.3125C19 8.69102 18.8479 9.0584 18.5807 9.32559L14.0236 13.8826C13.7861 14.1201 13.467 14.25 13.133 14.25C12.4391 14.25 11.875 13.6859 11.875 12.992V10.6875H7.125C6.46816 10.6875 5.9375 10.1568 5.9375 9.5V7.125C5.9375 6.46816 6.46816 5.9375 7.125 5.9375H11.875V3.63301C11.875 2.93906 12.4391 2.375 13.133 2.375C13.467 2.375 13.7861 2.50859 14.0236 2.74238ZM5.9375 2.375H3.5625C2.90566 2.375 2.375 2.90566 2.375 3.5625V13.0625C2.375 13.7193 2.90566 14.25 3.5625 14.25H5.9375C6.59434 14.25 7.125 14.7807 7.125 15.4375C7.125 16.0943 6.59434 16.625 5.9375 16.625H3.5625C1.5957 16.625 0 15.0293 0 13.0625V3.5625C0 1.5957 1.5957 0 3.5625 0H5.9375C6.59434 0 7.125 0.530664 7.125 1.1875C7.125 1.84434 6.59434 2.375 5.9375 2.375Z" fill="#DE003D" />
                            </svg>Logout
                          </div>
                        </div>

                      </div>
                    </div>
                  </div>}
              </div>
            </div>
          </div>
        </div>
      </div>
      <div className={styles.mainlayoutdiv}>

        {/* {!hiddenTopBar.includes(currentPath) && (
          <div className={styles.toplayer}>
            <div className={styles.toplayerleft} ref={topLayerLeftRef}>
              {tabMenu?.[currentPath]?.map((item: any, index: any) => {
                const isActivePath = item?.path?.split(currentPath)?.[1] == window.location.href?.split(currentPath)?.[1]
                const xlocation = decodeURIComponent(
                  window.location.pathname + window.location.search
                );
                console.log("xlocation", xlocation, item?.path);
                const isActivePath = item?.path === xlocation;

                return (
                  <div
                    key={index}
                    className={`${styles.toplayerleftname} ${isActivePath ? styles.active : ""}`}
                  >
                    <span onClick={() => navigate(item.path)}>{item.menuName}</span>
                    {item?.close ? <img src={smallclose} alt="" onClick={() => handleMenuTabClose(item, isActivePath)} /> : ""}
                    <p >{item?.path?.split(currentPath)?.[1]==window.location.href?.split(currentPath)?.[1]?"P":"N "}</p>
                  </div>
                )
              }
              )}
            </div>
            <div className={styles.toplayerright}>
              <img src={history} alt="" />
              <img src={create} alt="" onClick={() => handleNewConversation()} />
            </div>
          </div>
        )} */}

        {!hiddenTopBar.includes(currentPath) && 
          !hiddenTopBar.includes(childcurrentPath) && (

          <div className={styles.toplayer} style={{ position: "relative" }}>
            {showLeftArrow && (
              <div className={`${styles.scrollArrowBtn} ${styles.left}`} onClick={handleScrollLeft}>
                <svg width="14" height="14" viewBox="0 0 14 14" fill="none">
                  <path
                    d="M9 2.5L4.5 7L9 11.5"
                    stroke="currentColor"
                    strokeWidth="1.5"
                    strokeLinecap="round"
                    strokeLinejoin="round"
                  />
                </svg>
              </div>
            )}

            <div className={styles.toplayerleft} ref={topLayerLeftRef}>
              {tabMenu?.[currentPath]
                ?.filter((item: any) => {
                  if (
                    [welCareDomainId, pothysDomainId].includes(Number(loginCrmUserDetails?.domainId))
                    && (item?.path === "/home/commerce" || item?.path === "/home/project")
                  ) {
                    return false;
                  }
                  return true;
                }).map((item: any, index: any) => {
                  const xlocation = decodeURIComponent(
                    window.location.pathname + window.location.search
                  );
                  const isActivePath = item?.path === xlocation;

                  return (
                    <div
                      key={index}
                      className={`${styles.toplayerleftname} ${isActivePath ? styles.active : ""}`}
                    >
                      <span onClick={() => navigate(item.path)}>{item.menuName}</span>
                      {item?.close ? (
                        <img
                          src={smallclose}
                          alt=""
                          onClick={() => handleMenuTabClose(item, isActivePath)}
                        />
                      ) : ""}
                    </div>
                  );
                })}
            </div>

            {showRightArrow && (
              <div className={`${styles.scrollArrowBtn} ${styles.right} `} onClick={handleScrollRight}>
                <svg width="14" height="14" viewBox="0 0 14 14" fill="none">
                  <path
                    d="M5 2.5L9.5 7L5 11.5"
                    stroke="currentColor"
                    strokeWidth="1.5"
                    strokeLinecap="round"
                    strokeLinejoin="round"
                  />
                </svg>
              </div>
            )}

            <div className={styles.toplayerright}>
              <img src={create} alt="" onClick={() => handleNewConversation()} />
            </div>

          </div>

        )}
      </div>
      <div className={`${styles.mainoutlet} mainoutlet `}>
        <Outlet />

      </div>
    </div>
  );
}

export const CircularProgress = ({ value = 0 }: any) => {
  const size = 40;
  const strokeWidth = 2;
  const radius = (size - strokeWidth) / 2;
  const center = size / 2;
  const circumference = 2 * Math.PI * radius;
  const offset = circumference * (1 - value / 100);

  return (
    <div className={styles.progresswrapper}>
      <svg width={size} height={size}>
        <circle
          cx={center}
          cy={center}
          r={radius}
          stroke="#2a2a2a"
          strokeWidth={strokeWidth}
          fill="none"
        />
        <circle
          cx={center}
          cy={center}
          r={radius}
          stroke="#fff"
          strokeWidth={strokeWidth}
          fill="none"
          strokeLinecap="round"
          strokeDasharray={circumference}
          strokeDashoffset={offset}
          transform={`rotate(-90 ${center} ${center})`}
        />
      </svg>

      <div className={styles.progresstext}>{value}<span>%</span></div>
    </div>
  );
};
