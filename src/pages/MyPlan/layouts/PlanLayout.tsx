import React, { useEffect, useState } from "react";
import styles from "../../setup/scss/set.module.scss";
import SetUpCommonEditor from "../../shared/commonEditor";
import { GetItemFromStorage } from "../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../base/custom_hooks/localStorageKeys";
import { useLocation, useNavigate, useSearchParams } from "react-router-dom";
import YourInfo from "../pages/AccountInformation";
import AccountSecurity from "../pages/AccountInformation/security";
import AccountPrivacy from "../pages/AccountInformation/privacy";
import Storage from "../pages/AccountInformation/storage";
import AccountDevices from "../pages/AccountInformation/devices";
import Rolefirst from "../pages/UserManagement/roleteams";
import Teams from "../pages/UserManagement/teams";
import Createrole from "../pages/UserManagement/CreateRole";
import Useractivityview from "../pages/UserManagement/useractivityview";
import SimpleCustomEditorPlan from "../components/SimpleCustomEditorPlan";
import Useractivitylog from "../pages/UserManagement/useractivitylog";
import Orderhistory from "../pages/Billing/OrderHistory";
import Addon from "../pages/Billing/addon";
import Billingndsubscription from "../pages/Billing/Billingndsubscription";
import Agentroleview from "../pages/UserManagement/agentroleview";
import Roleagent from "../pages/UserManagement/roleagent";
import { v4 as uuidv4 } from "uuid";
import { stripHtmlTags } from "../../../base/utils";
import aiChatAssistSocketservice from "../../../services/aiChatService";
import { useDispatcher } from "../../../store/redux-store/dispatch";
import { MyplanProfileActions } from "../../../store/slice/my_plans/profile";
import { commonDataSliceActions } from "../../../store/slice/commonSilce/commonSlice";
import menuiconimg from "../../../assets/images/menuicon.png";
import closeicon from "../../../assets/images/smallclose.svg"

const getSetupSteps = (displayName: string) => [
  {
    key: "Your info",
    title: "Your info",
    title1: `Welcome, ${displayName}`,
    content: "Manage the personal details used across all your Worktual applications",
    component: <YourInfo />,
  },
  {
    key: "lead",
    title: "Security",
    title1: "Security",
    content: "Settings and recommendations to help you keep your account secure",
    component: <AccountSecurity />
  },
  {
    key: "Storage",
    title: "Storage",
    title1: "Storage",
    content: "To capture the end-user's consent preferences, ensure that both the audio and video call are recorded for verification purposes",
    component: <Storage />,
  },
  {
    key: "dataandprivacy",
    title: "Data and privacy",
    title1: "Data and privacy settings",
    content: "Manage how your data is used, shared, and protected across the services.",
    component: <AccountPrivacy />,
  },
  {
    key: "Devices",
    title: "Devices",
    title1: "Devices",
    content: "Manage connected devices and active sessions to keep your account secure.",
    component: <AccountDevices />,
  }

];
const COMMUNICATION_STEPS = [
  {
    key: "Roles",
    title: "Roles",
    title1: "Roles",
    content: "Create and Manage roles for the Worktual products",
    component: <Createrole />,
  },
  {
    key: "Users",
    title: "Users",
    title1: "Users",
    content: "View all the activities done across Worktual platform",
    component: <Useractivityview />,
  },
  {
    key: "Teams",
    title: "Teams",
    title1: "Teams",
    content: "Organize users for task assignment and streamline the business operations.",
    component: <Teams />,
  },
  {
    key: "User activity logs",
    title: "User activity logs",
    title1: "User activity logs",
    content: "View all the activities done across Worktual platform",
    component: <Useractivitylog />,
  },
];
const BILLING_SUBSCRIPTION = [
  {
    key: "Order History",
    title: "Order History",
    title1: "Order History",
    content: "View a list of all your past purchases and details.",
    component: <Orderhistory />,
  },
  {
    key: "Add-on’s",
    title: "Add-on’s",
    title1: "Add-on’s",
    content: "Purchase and manage additional features to extend functionality and scale your service.",
    component: <Addon />,
  },
  {
    key: "Billing & subscription",
    title: "Billing & subscription",
    title1: "Billing & subscription",
    content: "About billing & subscription",
    component: <Billingndsubscription />,
  },
];
export default function SetUpMain() {
  const [activeStep, setActiveStep] = useState("Your info");
  const [hideSetupHeader, setHideSetupHeader] = useState(false);
  const location = useLocation();
  const navigate = useNavigate();
  const loginUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const loginCrmDetails: any = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);

  const loginUser =
    (Array.isArray(loginUserDetails) ? loginUserDetails[0] : loginUserDetails) ||
    (Array.isArray(loginCrmDetails) ? loginCrmDetails[0] : loginCrmDetails) ||
    {};

  const firstName =
    loginUser?.Firstname ||
    loginUser?.first_name ||
    loginUser?.firstName ||
    "";
  const lastName =
    loginUser?.Surname ||
    loginUser?.last_name ||
    loginUser?.lastName ||
    "";
  const fullName = `${firstName} ${lastName}`.trim();
  const displayName = fullName || loginUser?.name || loginUser?.username || "User";
  const SETUP_STEPS = getSetupSteps(displayName);

  const activeSetup = SETUP_STEPS.find(
    (item) => item.key === activeStep
  );

  const activeCommunication = COMMUNICATION_STEPS.find(
    (item) => item.key === activeStep
  );

  const activeBilling = BILLING_SUBSCRIPTION.find(
    (item) => item.key === activeStep
  );


  const activeData = activeSetup || activeCommunication || activeBilling || SETUP_STEPS[0];
  const isCommunicationStep = COMMUNICATION_STEPS.some(
    (item) => item.key === activeStep
  );
  const innerRoutePath = location.pathname;
  const innerRouteComponent = innerRoutePath === "/Useradminroles"
    ? <Rolefirst />
    : innerRoutePath === "/useradmin"
      ? <Agentroleview />
      : innerRoutePath === "/useradminrolesteams"
        ? <Roleagent />
        : null;

  useEffect(() => {
    if (activeStep !== "account") {
      setHideSetupHeader(false);
    }
  }, [activeStep]);

  useEffect(() => {
    if (innerRoutePath === "/Useradminroles") {
      setActiveStep("Roles");
    } else if (innerRoutePath === "/useradmin") {
      setActiveStep("Users");
    } else if (innerRoutePath === "/useradminrolesteams") {
      setActiveStep("Teams");
    }
  }, [innerRoutePath]);

  const renderedComponent =
    innerRouteComponent ? innerRouteComponent :
      activeStep === "account" && React.isValidElement(activeData.component)
        ? React.cloneElement(activeData.component as React.ReactElement<any>, {
          onDetailViewChange: setHideSetupHeader,
        })
        : activeData.component;
  const shouldHideHeader = hideSetupHeader || !!innerRouteComponent;

  const handleStepChange = (key: string) => {
    setActiveStep(key);
    if (isMobile) {
      setMenuicon(false);
    }

    if (innerRouteComponent) {
      navigate("/myplans");
    }
  };

  const [searchParams, setSearchParams] = useSearchParams()
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const chatsessionId = searchParams.get("sessionId");
  const dispatch = useDispatcher()

  const onChange = (value: any, files: any) => {
    const sessionId: any = uuidv4() + "setup";
    const body: any = {
      query: stripHtmlTags(value?.trim()),
      sessionId,
      roleId: `${LoginCcaasUserDetails?.roleId || ""}`,
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: `${LoginCcaasUserDetails?.userId || ""}`,
      domainId: `${LoginCcaasUserDetails?.domainId || ""}`,
      module: "",
      ext: `${LoginCcaasUserDetails?.ext || ""}`,
      agent: "myplans",
      isBot: 0,
      customerId: "",
      files: files ?? [],
    };

    aiChatAssistSocketservice.io.emit("crm_chat", body);
    dispatch(MyplanProfileActions.setSetupAiAgentChat({ ...body, isBot: 0, query: body?.query }))
    dispatch(MyplanProfileActions.setSetupAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
    dispatch(commonDataSliceActions.setTabMenuPush({
      path: `/myplans/chat-conversation?sessionId=${sessionId}`,
      menuName: "New Conversation",
      close: 1,
      type: "myplans",
    })
    );
    navigate(`/myplans/chat-conversation?sessionId=${sessionId}`);
  }

  const [menuicon, setMenuicon] = useState(false);

  const menuiconclick = () => {
    setMenuicon(prev => !prev);
  };
  const [isMobile, setIsMobile] = useState(window.innerWidth <= 1023);

  useEffect(() => {
    const handleResize = () => setIsMobile(window.innerWidth <= 1023);
    window.addEventListener("resize", handleResize);
    return () => window.removeEventListener("resize", handleResize);
  }, []);


  return (
    <div className={`${styles.setmain} setmain`}>
      <div className={`${styles.setmainleft} ${isMobile ? (menuicon ? styles.showSidebar : styles.hideSidebar) : ""}`}>
        <div className={styles.titlebars}>
          <div className={styles.settitle}>My Plans</div>
          <img src={menuicon ? closeicon : menuiconimg} alt="" onClick={menuiconclick} className={menuicon ? styles.menuiconfix : styles.menuicon} />
        </div>
        <div
          className={styles.leftscroller}>
          <div className={styles.settitle}>Account information</div>
          <div className={styles.setblock}>
            {SETUP_STEPS.map((item) => (
              <div
                key={item.key}
                className={`${styles.setblockinner} ${activeStep === item.key ? styles.active : ""
                  }`}
                onClick={() => handleStepChange(item.key)}
              >
                {item.title}
              </div>
            ))}
          </div>
          <div className={styles.settitle}>User Management</div>
          <div className={styles.setblock}>
            {COMMUNICATION_STEPS.map((item) => (
              <div
                key={item.key}
                className={`${styles.setblockinner} ${activeStep === item.key ? styles.active : ""
                  }`}
                onClick={() => handleStepChange(item.key)}
              >
                {item.title}

              </div>
            ))}
          </div>
          <div className={styles.settitle}>Billing and subscription</div>
          <div className={styles.setblock}>
            {BILLING_SUBSCRIPTION.map((item) => (
              <div key={item.key} className={`${styles.setblockinner} ${activeStep === item.key ? styles.active : ""}`} onClick={() => handleStepChange(item.key)}>
                {item.title}
              </div>
            ))}
          </div>
        </div>
      </div>
      <div className={styles.setmainright}>
        <div className={`maincontainer ${styles.newmaincontt}`}>


          <div className={` ${styles.setmainrightinner} ${isCommunicationStep && styles.setborder}`}>
            <>
              {!shouldHideHeader && (
                <div className={styles.settitle}>
                  {activeData.title1}
                  <div className={styles.description}> {activeData.content}</div>
                </div>
              )}
              {renderedComponent}

            </>
          </div>

          <div className={`${styles.editorbottom} ${styles.editorplanlayout}`}>
            <SetUpCommonEditor onChange={onChange} />
          </div>
        </div>



      </div>
    </div>
  );
}
