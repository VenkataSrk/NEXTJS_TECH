import React, { useEffect, useState } from "react";
import styles from "./scss/set.module.scss";

import BusinessSetup from "./business-identity";
import LeadSetup from "./lead";
import SalesSetup from "./sales";
import AccountSetup from "./account";
import ChannelSetup from "./channel";
import ProductSetup from "./product";
import RevenueSetup from "./revenue";
import DataSetup from "./data";
import Emailsetup from "./emailsetup";
import Verifyprivate from "./verifyprivate";
import Smtpemail from "./smtpemail";
import Callchannel from "./callchannel";
import Smschannel from "./smschannel";
import SetUpCommonEditor from "../shared/commonEditor";
import { stripHtmlTags } from "../../base/utils";
import { useNavigate, useSearchParams } from "react-router-dom";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { contactDataSliceActions } from "../../store/slice/contactSlice";
import { SetupSliceActions } from "../../store/slice/setupSlice/setupSlice";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { v4 as uuidv4 } from "uuid";
import menuiconimg from "../../assets/images/menuicon.png";
import closeicon from "../../assets/images/smallclose.svg"

const SETUP_STEPS = [
    {
        key: "business_profile",
        title: "Business Profile",
        title1: "Company & Business Profile",
        content: "Configure your company profile and business settings.",
        component: <BusinessSetup />,
    },
    {
        key: "integration",
        title: "Integration",
        title1: "Integration & Entry Points",
        content: "Manage and configure your lead capture channels",
        component: <LeadSetup />,
    },

    {
        key: "business_hours",
        title: "Business hours & holidays ",
        title1: "Set your business hours, time zones, and holiday schedules",
        content: "Set your business hours, time zones, and holiday schedules",
        component:
            <SalesSetup />,
    },
    {
        key: "team_managemant",
        title: "Team Management",
        title1: "Team Management",
        content: "Set up teams and control queue distribution.",
        component: <AccountSetup />,
    },
];
const COMMUNICATION_STEPS = [
    {
        key: "Email",
        title: "Email",
        title1: "Email",
        content: "Manage your email addresses",
        component: <Emailsetup />,
    },
    // {
    //     key: "verify",
    //     title: "Verify Private Domain Email",
    //     component: <Verifyprivate />,
    // },
    // {
    //     key: "SMTP",
    //     title: "SMTP Email",
    //     component: <Smtpemail />,
    // },
    {
        key: "Call",
        title: "Call",
        title1: "Call",
        content: "Configure call numbers and routing settings",
        component: <Callchannel />,
    },
    {
        key: "SMS",
        title: "SMS",
        title1: "SMS",
        content: "Set up and control SMS messaging settings.",
        component: <Smschannel />,
    },
];

export default function SetUpMain() {
    const [activeStep, setActiveStep] = useState("business_profile");
    const [hideSetupHeader, setHideSetupHeader] = useState(false);

    const [searchParams, setSearchParams] = useSearchParams()
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const chatsessionId = searchParams.get("sessionId");
    const dispatch = useDispatcher()
    const navigate = useNavigate();

    const activeSetup = SETUP_STEPS.find(
        (item) => item.key === activeStep
    );

    const activeCommunication = COMMUNICATION_STEPS.find(
        (item) => item.key === activeStep
    );

    const activeData = activeSetup || activeCommunication || SETUP_STEPS[0];
    const isCommunicationStep = COMMUNICATION_STEPS.some(
        (item) => item.key === activeStep
    );

    useEffect(() => {
        if (activeStep !== "business_hours") {
            setHideSetupHeader(false);
        }
    }, [activeStep]);

    const renderedComponent =
        activeStep === "business_hours" && React.isValidElement(activeData.component)
            ? React.cloneElement(activeData.component as React.ReactElement<any>, {
                onDetailViewChange: setHideSetupHeader,
            })
            : activeData.component;



    const onChange = (value: any, files: any) => {
        console.log("onChangeonChange", onChange)
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
            agent: "setup",
            isBot: 0,
            customerId: "",
            files: files ?? [],
            current_page: activeStep ?? ""
        };

        aiChatAssistSocketservice.io.emit("crm_chat", body);
        dispatch(SetupSliceActions.setSetupAiAgentChat({ ...body, isBot: 0, query: body?.query }))
        dispatch(SetupSliceActions.setSetupAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
        dispatch(commonDataSliceActions.setTabMenuPush({
            path: `/setup/chat-conversation?sessionId=${sessionId}`,
            menuName: "New Conversation",
            close: 1,
            type: "setup",
        })
        );

        navigate(`/setup/chat-conversation?sessionId=${sessionId}`);
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
                    <div className={styles.settitle}>Operations Hub</div>
                    <img src={menuicon ? closeicon : menuiconimg} alt="" onClick={menuiconclick} className={menuicon ? styles.menuiconfix : styles.menuicon} />
                </div>
                <div className={styles.leftscroller}>
                    <div className={styles.setblock}>
                        {SETUP_STEPS.map((item) => (
                            <div
                                key={item.key}
                                className={`${styles.setblockinner} ${activeStep === item.key ? styles.active : ""}`}
                                onClick={() => {setActiveStep(item.key); setMenuicon(false);}}
                            >
                                {item.title}
                            </div>
                        ))}
                    </div>
                    <div className={styles.settitle}>Communication Channels</div>
                    <div className={styles.setblock}>
                        {COMMUNICATION_STEPS.map((item) => (
                            <div
                                key={item.key}
                                className={`${styles.setblockinner} ${activeStep === item.key ? styles.active : ""}`}
                                onClick={() => {setActiveStep(item.key); setMenuicon(false);}}
                            >
                                {item.title}
                            </div>
                        ))}
                    </div>
                </div>
            </div>
            <div className={styles.setmainright}>
                <div className={`maincontainer ${styles.newmaincontt}`}>
                    <div className={` ${styles.setmainrightinner} ${isCommunicationStep && styles.setborder}`}>
                        {!hideSetupHeader && (
                            <div className={styles.mainsettitle}>
                                {activeData.title1}
                                <div className={styles.maindescription}> {activeData.content}</div>
                            </div>
                        )}
                        {renderedComponent}
                    </div>
                    <div className={`${styles.editorbottom} `}>
                        <SetUpCommonEditor onChange={onChange} />
                    </div>
                </div>
            </div>
        </div>
    );
}