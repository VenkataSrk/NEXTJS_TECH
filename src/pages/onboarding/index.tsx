
import styles from "./scss/newonboard.module.scss";
import logo from "../../assets/images/fulllogo.svg";

import step1 from "../../assets/images/onboard/step1.svg";
import step2 from "../../assets/images/onboard/step2.svg";
import step3 from "../../assets/images/onboard/step7.svg";
import step4 from "../../assets/images/onboard/step5.svg";
import step5 from "../../assets/images/onboard/step6.svg";
import step6 from "../../assets/images/onboard/step4.svg";
import step7 from "../../assets/images/onboard/step3.svg";
import step8 from "../../assets/images/onboard/step8.svg";
import step9 from "../../assets/images/onboard/step9.svg";
import { useRef, useState, useEffect } from "react";
import { useSelector } from "react-redux";
import { useDispatcher } from "../../store/redux-store/dispatch";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { useNavigate, useSearchParams } from "react-router-dom";
import { v4 as uuidv4 } from "uuid";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { excelFormats, getFileSize, imageFormat, pdfFormats, stripHtmlTags, wordFormats } from "../../base/utils";
import { AiOnboardingAction } from "../../store/slice/onboardingSlice/onboardingSlice";
import { getUserOnboardDetails } from "../../base/onboardConfig";
import Excelicon from "../../assets/images/xlsicon.svg";
import Pdficon from "../../assets/images/pdfFile.svg";
import Wordicon from "../../assets/images/documentformate.svg";
import SetUpCommonEditor from "../shared/commonEditor";

export default function OnBoardLayout() {
    const ONBOARDING_SESSION_KEY = "onboardSessionId";
    const chatEndRef = useRef<HTMLDivElement | null>(null);
    const boxRef = useRef<HTMLDivElement | null>(null);
    const [height, setHeight] = useState(0);
    const dispatch = useDispatcher();
    const navigate = useNavigate();
    const [searchParams, setSearchParams] = useSearchParams();
    const loginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const { onboardingStaus, onboardMessages, aiAgentChatTyping } = useSelector((store: any) => store.onboarding);
    const chatsessionId = searchParams.get(ONBOARDING_SESSION_KEY);
    const activeStep = onboardingStaus?.find((s: any) => s?.isCompleted === 0) || onboardingStaus?.[onboardingStaus?.length - 1];
    const currentStep = Number(activeStep?.step || 1);
    const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);

    useEffect(() => {
        getUserOnboardDetails(dispatch);
    }, [])

    useEffect(() => {
        if (!boxRef.current) return;

        const observer = new ResizeObserver((entries) => {
            for (let entry of entries) {
                setHeight(Math.round(entry.contentRect.height));
            }
        });

        observer.observe(boxRef.current);

        return () => observer.disconnect();
    }, []);

    useEffect(() => {
        if (chatEndRef.current) {
            chatEndRef.current.scrollIntoView({ behavior: "smooth" });
        }
    }, [onboardMessages.length, aiAgentChatTyping?.[`${chatsessionId}`]]);

    useEffect(()=> {
        if(!onboardMessages?.length && accessToken) {
            if(aiChatAssistSocketservice?.io){
                initiate()
            }
        }
    },[accessToken])


    const initiate = () => {
        let newSessonId = chatsessionId;
        if (!newSessonId) {
            newSessonId = uuidv4() + "onboarding";
            createNewChat(newSessonId);
        }
        const body = {
            query: "",
            sessionId: newSessonId,
            roleId: loginCcaasUserDetails?.roleId?.toString() ?? "",
            org_role_id_crm: loginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
            userId: loginCcaasUserDetails?.UserID?.toString() ?? "",
            domainId: loginCcaasUserDetails?.domainId?.toString() ?? "",
            module: "",
            ext: loginCcaasUserDetails?.ext?.toString() ?? "",
            agent: "onboarding",
            files: [],
            landingPage: 1,
        };

        aiChatAssistSocketservice?.io?.emit("crm_chat", body);
        dispatch(AiOnboardingAction.setOnboardTyping({ sessionId: newSessonId, typing: true }));
    }

    const createNewChat = (sessionId = uuidv4()) => {
        const params = new URLSearchParams(searchParams);
        params.set(ONBOARDING_SESSION_KEY, sessionId);
        setSearchParams(params, { replace: true });
    };

    const onChange = (value: any, files: any) => {
        const hasText = value?.trim();
        const hasFiles = files && files.length > 0;
        if (!hasText && !hasFiles) return;

        let newSessonId = chatsessionId;
        if (!newSessonId) {
            newSessonId = uuidv4() + "onboarding";
            createNewChat(newSessonId);
        }

        const body = {
            query: stripHtmlTags(value?.trim()),
            sessionId: newSessonId,
            roleId: loginCcaasUserDetails?.roleId?.toString() ?? "",
            org_role_id_crm: loginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
            userId: loginCcaasUserDetails?.UserID?.toString() ?? "",
            domainId: loginCcaasUserDetails?.domainId?.toString() ?? "",
            module: "",
            ext: loginCcaasUserDetails?.ext?.toString() ?? "",
            agent: "onboarding",
            files: files || [],
            landingPage: 0,
        };

        aiChatAssistSocketservice.io.emit("crm_chat", body);
        dispatch(AiOnboardingAction.setOnboardMessages({ ...body, isBot: 0, query: body?.query }));
        dispatch(AiOnboardingAction.setOnboardTyping({ sessionId: newSessonId, typing: true }));
    };

    const images: any = {
        1: step1,
        2: step2,
        3: step3,
        4: step4,
        5: step5,
    };

    const getImage = (step: number) => images[step] || step1;
    const navigateToHome = () => {
        navigate('/home/sales')
    }

    return (

        <>

            <div className={styles.mainlayout}>
                <div className={styles.mainlogo}>
                    <img src={logo} alt="" />
                </div>

                <div className={styles.mainblock}>
                    <div className={styles.mainblockleft}>
                        <div className={styles.mainblocklefttop}>
                            <div className={styles.titlearea}>
                                <div className={styles.titlearealeft}>Setup Journey<span>Move through each step at your pace and track your progress here.</span></div>
                                <div className={styles.titlearearight} onClick={() => navigateToHome()}>Cancel</div>
                            </div>
                        </div>
                        <div className={styles.mainblockleftmid} style={{ height: `calc(100dvh - ${height}px - 196px)` }}>
                            <div className={styles.bubbleblock}>
                                {
                                    onboardMessages.map((item: any, index: number) => {
                                        const isBot = Number(item?.isBot);

                                        if (isBot === 0) {
                                            return (
                                                <div className={styles.bubbleblockagent} key={index}>

                                                    {item?.files?.length > 0 && (
                                                        <div className={styles.bubbleblockagenttextblock}>
                                                            <div className={styles.bubbleblockagenttext}>
                                                                {item?.files?.map((file: any, i: number) => {
                                                                    const fileName = file?.name || file?.filename || "Attachment";
                                                                    const fileSize = getFileSize(Number(file?.size) || 0) || "";
                                                                    const fileUrl = file?.url || file?.path;
                                                                    const mimetype = file?.mimetype;

                                                                    let iconSrc = Pdficon; // default icon

                                                                    if (imageFormat.includes(mimetype)) {
                                                                        iconSrc = fileUrl; // show actual image preview
                                                                    } else if (excelFormats.includes(mimetype)) {
                                                                        iconSrc = Excelicon;
                                                                    } else if (wordFormats.includes(mimetype)) {
                                                                        iconSrc = Wordicon;
                                                                    } else if (pdfFormats.includes(mimetype)) {
                                                                        iconSrc = Pdficon;
                                                                    }
                                                                    return (
                                                                        <div className={styles.chateditorfile} key={file?.uuid || i}>
                                                                            <a
                                                                                href={fileUrl}
                                                                                target="_blank"
                                                                                rel="noopener noreferrer"
                                                                            >
                                                                                <img
                                                                                    src={iconSrc}
                                                                                    alt="File attachment"
                                                                                    className={styles.fileIcon}
                                                                                />
                                                                            </a>

                                                                            <div className={styles.right}>
                                                                                <p>{fileName}</p>
                                                                                <p className={styles.filesize}>{fileSize}</p>
                                                                            </div>
                                                                        </div>
                                                                    );
                                                                })}
                                                                <div dangerouslySetInnerHTML={{ __html: item?.query }} />
                                                            </div>
                                                        </div>
                                                    )}

                                                    {/* Message Text */}
                                                    {(item?.query && item?.files?.length === 0) && (
                                                        <div className={styles.bubbleblockagenttextblock}>
                                                            <div
                                                                className={styles.bubbleblockagenttext}
                                                                dangerouslySetInnerHTML={{ __html: item?.query }}
                                                            />
                                                        </div>
                                                    )}

                                                </div>
                                            );
                                        }

                                        return (
                                            <div className={styles.bubbleblockcustomer} key={index}>
                                                <div className={styles.bubbleblockcustomertextblock}>
                                                    <div
                                                        className={styles.bubbleblockcustomertext}
                                                        dangerouslySetInnerHTML={{ __html: item?.response }}
                                                    />
                                                </div>
                                            </div>
                                        );
                                    })
                                }

                                {!!aiAgentChatTyping?.[`${chatsessionId}`] && (
                                    <div className="msgbubble left">
                                        <div className="chatContent">
                                            {aiAgentChatTyping?.[`${chatsessionId}`] ?? "Typing..."}
                                        </div>
                                    </div>
                                )}

                                <div ref={chatEndRef} />
                            </div>
                        </div>
                        <div className={styles.mainblockleftbottom} ref={boxRef}>
                            <SetUpCommonEditor onChange={onChange} />
                        </div>
                    </div>

                    <div className={styles.mainblockright}>
                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>

                            <div className={styles.subtitle}>
                                <span>Step {currentStep} of 5</span>
                                {activeStep?.label}
                            </div>

                            <p>{activeStep?.desc}</p>

                            <img src={getImage(activeStep?.step)} alt="" />
                        </div>

                        {/* <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 2 of 9</span>Lead Generation & Entry Points</div>
                            <ul>
                                <li>Lead sources - Website, Ads, WhatsApp, Email, Phone, Social, Partners / referrals, Offline</li>
                                <li>Inbound vs outbound mix</li>
                                <li>Lead capture methods</li>
                                <li>Campaign attribution preference</li>
                                <li>Volume expectations per source</li>
                            </ul>
                            <img src={step2} alt="" />
                        </div>

                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 3 of 9</span>Sales Process & Pipeline</div>
                            <ul>
                                <li>Sales stages</li>
                                <li>Stage order</li>
                                <li>Loss reasons</li>
                                <li>Deal ownership rules</li>
                            </ul>
                            <img src={step3} alt="" />
                        </div>

                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 4 of 9</span>Lead, Account & Contact Data</div>
                            <ul>
                                <li>lead name</li>
                                <li>Budget / pricing range</li>
                                <li>Requirement or use case</li>
                                <li>Decision maker identification</li>
                                <li>Custom identifiers (GST, customer ID, region codes)</li>
                            </ul>
                            <img src={step4} alt="" />
                        </div>

                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 5 of 9</span>Communication Channels & Conversations</div>
                            <ul>
                                <li>Email</li>
                                <li>Voice</li>
                                <li>Web chat</li>
                                <li>Social platforms</li>
                                <li>Google Ads, LinkedIn Ads</li>
                            </ul>
                            <img src={step5} alt="" />
                        </div>

                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 6 of 9</span>Users, Roles & Responsibilities</div>
                            <ul>
                                <li>User roles (Sales, Manager, Admin, Finance, Support)</li>
                                <li>Role responsibilities</li>
                                <li>Data visibility rules</li>
                                <li>Edit / delete permissions</li>
                                <li>Deal approval authority</li>
                                <li>Discount approval rules</li>
                                <li>Escalation hierarchy</li>
                            </ul>
                            <img src={step6} alt="" />
                        </div>

                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 7 of 9</span>Products, Services & Pricing</div>
                            <ul>
                                <li>Product / service catalog</li>
                                <li>Pricing model - One-time, Subscription, Usage-based</li>
                                <li>Packaging & plans</li>
                                <li>Discount limits</li>
                                <li>Taxes & compliance rules</li>
                                <li>Currency handling</li>
                                <li>Quote → order → invoice flow</li>
                            </ul>
                            <img src={step7} alt="" />
                        </div>

                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 8 of 9</span>Revenue & Billing Logic</div>
                            <ul>
                                <li>Revenue recognition type</li>
                                <li>Payment methods</li>
                                <li>Billing frequency</li>
                                <li>Partial payments (Y/N)</li>
                                <li>Refund / cancellation rules</li>
                                <li>Renewal logic (if applicable)</li>
                            </ul>
                            <img src={step8} alt="" />
                        </div>

                        <div className={styles.mainblockrightblock}>
                            <div className={styles.title}>Your Setup Path</div>
                            <div className={styles.subtitle}><span>Step 9 of 9</span>Data Migration</div>
                            <ul>
                                <li>Existing tools used</li>
                                <li>Data sources (CRM, spreadsheets, messaging tools)</li>
                                <li>Field mapping preferences</li>
                                <li>Duplicate handling rules</li>
                                <li>Data retention policy</li>
                            </ul>
                            <img src={step9} alt="" />
                        </div> */}
                    </div>
                </div>
            </div>
        </>
    );
}
