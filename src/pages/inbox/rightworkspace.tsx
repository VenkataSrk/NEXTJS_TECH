
import React, { useEffect, useState } from "react";
import styles from "./scss/inbox.module.scss"
import chevrondown from "../../assets/images/inbox/chevrondown.svg";
import mailicon from "../../assets/images/inbox/mailicon.svg";
import contactwindow from "../../assets/images/inbox/contactwindow.svg";
import bluechevron from "../../assets/images/inbox/bluechevron.svg";
import reply from "../../assets/images/inbox/reply.svg";
import forward from "../../assets/images/inbox/forward.svg";
import ComposeMail from "./composemail";
import threedot from "../../assets/images/inbox/threedot.svg";
import openemail from "../../assets/images/inbox/mailopen.svg";
import newemail from "../../assets/images/inbox/newemail.svg";
import filter from "../../assets/images/inbox/filter.svg";
import { useSearchParams } from "react-router-dom";
import { getContactDetails } from "../../base/service/core_serviceApi";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import { useSelector } from "react-redux";
import { SentimentScore } from "../../base/shared/sentimental";
import { formatDate, formatDateTime } from "../../base/utils";



const RightWorkspace = ({ offRightSpace, setOffRightSpace }: any) => {

    const { perticularInboxContactData } = useSelector((state: any) => state.inboxChatStore);
    const [showmore, setshowmore] = useState({
        about: true,
        lead: false,
        company: false
    });

    const [params] = useSearchParams()
    const dispatch = useDispatcher()
    const CustomerId = params.get('CustomerId')

    useEffect(() => {
        getContact()
    }, [CustomerId])

    const getContact = async () => {
        if (Number(CustomerId)) {
            const res = await getContactDetails(CustomerId, null);
            dispatch(inboxDataSliceActions.setPerticularInboxContactData({
                ...perticularInboxContactData,
                contacts: res?.[0] ?? {}
            }));
        }
    }

    const contactDetails = {
        "Contact Person": `${perticularInboxContactData?.contacts?.first_name || ''} ${perticularInboxContactData?.contacts?.last_name || ''}`.trim(),
        "Email": perticularInboxContactData?.contacts?.primary_email,
        "Phone": perticularInboxContactData?.contacts?.primary_phone_number,
        "Address": perticularInboxContactData?.contacts?.address,
        "Source": perticularInboxContactData?.contacts?.source_type,
        "Created Date": perticularInboxContactData?.contacts?.created_at ? formatDate(perticularInboxContactData?.contacts?.created_at) : '',
        "Owner name": perticularInboxContactData?.contacts?.sales_name || '',
        "Industory": perticularInboxContactData?.contacts?.industry_type || '',
        "Company Size": perticularInboxContactData?.contacts?.company_size,
    }

    const behaviorScore = Number(perticularInboxContactData?.contacts?.behaviors ?? 0);
    const demoScore = Number(perticularInboxContactData?.contacts?.demo_graphics ?? 0);
    const firmoScore = Number(perticularInboxContactData?.contacts?.firmo_graphics ?? 0);
    const LeadScore = Math.round((((behaviorScore + demoScore + firmoScore) / 300) * 100));
    const leadScoreTag = LeadScore < 40 ? "Unqualified" : LeadScore < 70 ? "MQL" : "Qualified";

    return (


        <div className={`${styles.contact} contacthide`}>
            <div className={`p12 ${styles.header}`}>
                <span className={styles.contactwindow}>
                    <img src={contactwindow} onClick={() => setOffRightSpace((prev: boolean) => !prev)} style={{ transform: offRightSpace ? "rotate(0deg)" : "rotate(180deg)" }} />
                </span>
                <span className={`content ${styles.subcontent}`}>
                    {perticularInboxContactData?.channelType}
                    {" - "}
                    {contactDetails?.["Contact Person"] || "New Customer"}
                </span>
            </div>
            <div className={`${styles.bodyinnercontainer} p12`}>

                <div className={`${styles.bodyinner}`}>
                    <div className={`${styles.expandsec} mb16 ${showmore.about ? styles.none : ""}`} onClick={() => setshowmore(({ ...showmore, about: !showmore.about }))} >
                        <div className={styles.lefts}>
                            <img src={chevrondown} style={{ rotate: showmore.about ? "180deg" : "0deg" }} />
                            About this contact
                        </div>
                    </div>
                    {showmore.about && (
                        <div className={styles.hidendshowbox}>
                            {Object.entries(contactDetails).map(([key, value]) => {
                                return (
                                    <div className={`${styles.block} mb16`} key={key}>
                                        <label>{key}</label>
                                        <h5>{value || '-'}</h5>
                                    </div>
                                )
                            })}
                        </div>
                    )}
                </div>

                <div className={`${styles.bodyinner}`}>
                    <div className={`${styles.expandsec} mb16 ${showmore.lead ? styles.none : ""}`} onClick={() => setshowmore(({ ...showmore, lead: !showmore.lead }))}>
                        <div className={styles.lefts}>
                            <img src={chevrondown} style={{ rotate: showmore.lead ? "180deg" : "0deg" }} />
                            Lead score
                        </div>

                    </div>
                    {
                        showmore.lead && <div className={`${styles.hidendshowbox} ${styles.card}`}>
                            <div className={styles.headerrr}>
                                <h5>Total score</h5>
                                {!!perticularInboxContactData?.contacts?.lead_status && <span className={`${styles.badge} ${styles.green}`}>{perticularInboxContactData?.contacts?.lead_status }</span>}
                            </div>

                            <SentimentScore value={LeadScore} />

                            <p className="content">
                                {perticularInboxContactData?.contacts?.additional_info?.lead_score_summary || '-'}
                            </p>
                        </div>
                    }
                </div>

                <div className={`${styles.bodyinner} `}>
                    <div className={`${styles.expandsec} mb16 ${showmore.company ? styles.none : ""}`} onClick={() => setshowmore(({ ...showmore, company: !showmore.company }))}>
                        <div className={styles.lefts}>
                            <img src={chevrondown} style={{ rotate: showmore.company ? "180deg" : "0deg" }} />
                            Company details
                        </div>

                    </div>
                    {
                        showmore.company && <div className={styles.hidendshowbox}>
                            <div className={`${styles.block} mb16`}>
                                <label>Company Name</label>
                                <h5>{perticularInboxContactData?.contacts?.company_name || '-'}</h5>
                            </div>
                            <div className={`${styles.block} mb16`}>
                                <label>Company Size</label>
                                <h5>{perticularInboxContactData?.contacts?.company_size || '-'}</h5>
                            </div>
                        </div>
                    }
                </div>

            </div>

        </div>


    )
}

export default RightWorkspace;
