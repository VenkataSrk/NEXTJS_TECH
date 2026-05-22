import React, { useState } from "react";
import styles from "../contacts/scss/emailininbox.module.scss"
import chevrondown from "../../assets/images/inbox/chevrondown.svg";
import reply from "../../assets/images/inbox/reply.svg";
import forward from "../../assets/images/inbox/forward.svg";
import deleteem from "../../assets/images/inbox/delete.svg";
import { useNavigate, useSearchParams } from "react-router-dom";
import { useSelector } from "react-redux";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { ACTIONS_ACCESS, levelOfAccess, MENUS } from "../../base/constant";
import { moveEmailToTrashApi } from "../../base/service/core_serviceApi";
import { contactDataSliceActions } from "../../store/slice/contactSlice";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
function Emailininbox() {

    const navigate = useNavigate();
    const { customerEmail } = useSelector((store: any) => store.contactStore)
    const dispatch = useDispatcher();
    const { permissions } = useSelector((store: any) => store?.loginUser?.rolesandpermissions)
    const hasMailDeleteAccess = levelOfAccess(MENUS.INBOX, ACTIONS_ACCESS.DELETE, permissions)
    const { appTabs } = useSelector((state: any) => state.commonData);
    const [params] = useSearchParams()
    const customerId = params.get('coid');
    const [showDetail, setShowDetail] = useState(false);

    const formatDate = (dateString) => {
        const date = new Date(dateString);

        const datePart = date.toLocaleDateString("en-US", {
            month: "short",
            day: "2-digit",
            year: "numeric",
        });

        const timePart = date.toLocaleTimeString("en-US", {
            hour: "numeric",
            minute: "2-digit",
            hour12: true,
        });

        return `${datePart} at ${timePart}`;
    };

    const openInboxTab = (mailData?: any, isReply = true) => {
        try {
            dispatch(inboxDataSliceActions.setCleanReplyForword());

            const tabId = "inbox";
            const path = `/inbox?channelType=Email&CustomerId=${mailData?.CustomerId || 0}`;
            const title = "Inbox";
            dispatch(inboxDataSliceActions.clearAiSuggestions());
            dispatch(inboxDataSliceActions.setComposeMailToggle(true));
            dispatch(inboxDataSliceActions.setForwardedMessage(!isReply));
            dispatch(inboxDataSliceActions.setReplyMessage(isReply));

            if (isReply) {
                dispatch(
                    inboxDataSliceActions.setReplyMailDetails({
                        from: mailData?.customer_name || "New Customer",
                        fromEmail: mailData?.FromEmail || "",
                        date: mailData?.createdAt,
                        subject: mailData?.EmailSubject || "No Subject",
                        toMail: mailData?.ToEmail || "",
                        message: mailData?.Message?.userText?.message?.message || "",
                    })
                );
            } else {
                dispatch(
                    inboxDataSliceActions.setForwardMailDetails({
                        from: mailData?.customer_name || "New Customer",
                        fromEmail: mailData?.FromEmail || "",
                        date: mailData?.createdAt,
                        subject: mailData?.EmailSubject || "No Subject",
                        toMail: mailData?.ToEmail || "",
                        message: mailData?.Message?.userText?.message?.message || "",
                    })
                );
            }

            const existing = appTabs?.find((t: any) => t.id === tabId);
            if (!existing) {
                dispatch(commonDataSliceActions.addAppTab({ id: tabId, title, path: "/inbox" }));
            }

            dispatch(commonDataSliceActions.setActiveAppTab(tabId));
            navigate(path);

        } catch (error) {
            console.log(error);
        }
    };

    const openReplyCompose = (mailData?: any) => openInboxTab(mailData, true);
    const openCompose = (mailData?: any) => openInboxTab(mailData, false);

    const getEmailHistoryId = (item: any) => item?.EmailHistoryId || null;
    const getSmsHistoryId = (item: any) => item?.SMSHistoryId || null;
    const getCallHistoryId = (item: any) => item?.CallHistoryId || null;
    const getDraftId = (item: any) => item?.draftId || item?.draft_id || null;

    const moveEmailToTrash = async (mailData?: any) => {
        const emailhistoryId = getEmailHistoryId(mailData);
        const smsHistoryId = getSmsHistoryId(mailData);
        const callHistoryId = getCallHistoryId(mailData);
        const body = {
            email_history_ids: emailhistoryId,
            sms_history_ids: smsHistoryId,
            call_history_ids: callHistoryId,
            isTrash: 1,
        }
        const response = await moveEmailToTrashApi(body)
        if (response?.statusCode === 200 && response?.response?.[0]?.errcode) {
            dispatch(inboxDataSliceActions.setMoveToTrash(body))
            dispatch(contactDataSliceActions.setCustomerEmail({}));
            navigate(`/contact/contactdetais?coid=${customerId}`)
        }
    }

    const contactName = customerEmail?.contactData?.first_name ?  (`${customerEmail?.contactData?.first_name || ""} ${customerEmail?.contactData?.last_name || ""}`.trim()) :  "New Customer";


    return (<>
        <div className={`maincontainer ${styles.emailininboxbody}`}>
            <div className={styles.stickyheader}>
                <div className={styles.lefts} onClick={() => navigate(-1)}>
                    <svg xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none">
                        <path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#404040" />
                    </svg>
                </div>
                <div className={styles.rights}>
                    <div className={styles.title}>Email</div>
                    {/* <div className={styles.desc}>Interaction Log microcopy</div> */}
                </div>
            </div>
            <div className={styles.emailcontentwrapper}>
                <div className={`${styles.fullviewofemail} mb16`} >
                    <div className={styles.expandsec} >
                        <div className={styles.lefts} onClick={() => setShowDetail(!showDetail)}>
                            <img src={chevrondown} className={styles.chevrondown} style={{ transform: showDetail ? "rotate(180deg)" : "rotate(0deg)" }} />
                            Email {customerEmail?.EmailSubject?.trim() ? `- ${customerEmail?.EmailSubject}` : ''}
                        </div>
                        <div className={styles.rights}>{formatDate(customerEmail?.createdAt)}</div>
                    </div>
                    {showDetail && <div className={styles.hidendshowbox}>
                        <div className={`${styles.avatorwithactions} ptb16`}>
                            <div className={styles.lefts}>
                                <div className={styles.avatar}>
                                    {contactName?.trim()
                                        ? contactName
                                            .split(' ')
                                            .map(word => word.charAt(0))
                                            .slice(0, 2)
                                            .join('')
                                            .toUpperCase()
                                        : 'N'}
                                </div>
                                <div className={styles.namewithact}>
                                    <h5> {contactName || 'New Customer'}</h5>
                                    {/* <span
                                                                    className={styles.dtls}
                                                                    onClick={() => setShowDetail((prev) => ({ ...prev, [msgIndex]: !prev[msgIndex] }))}
                                                                >
                                                                    {showDetail[msgIndex] ? "Hide" : "Show"}  Details {" "}
                                                                    <img style={{ transform: showDetail[msgIndex] ? "rotate(180deg)" : "rotate(0deg)" }} src={bluechevron} />
                                                                </span> */}
                                </div>
                            </div>
                            <div className={styles.rights}>
                                <div className={styles.actions} onClick={() => openReplyCompose(customerEmail)}>
                                    <img src={reply} />Reply
                                </div>
                                <div className={styles.actions} onClick={() => openCompose(customerEmail)}>
                                    <img src={forward} />Forward
                                </div>

                                {hasMailDeleteAccess && (
                                    <div className={styles.actions} onClick={() => moveEmailToTrash(customerEmail)}>
                                        <img src={deleteem} />Delete
                                    </div>)}

                            </div>
                        </div>
                        {/* {showDetail[msgIndex] && ( */}
                        <div className={styles.emailcontent}>
                            {/* <h5>Hi team,</h5> */}
                            {/* <div
                                                                dangerouslySetInnerHTML={{
                                                                    __html: (msg?.message || "").replace(
                                                                        /\n/g,
                                                                        "<br />"
                                                                    ),
                                                                }}
                                                            /> */}
                            <div dangerouslySetInnerHTML={{ __html: customerEmail?.Message.userText?.message?.message || '' }} />
                        </div>
                        {/* )} */}
                    </div>}
                </div>
            </div>
        </div>

    </>)
} export default Emailininbox