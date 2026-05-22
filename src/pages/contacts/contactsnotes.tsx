import React, { useEffect, useState } from "react";
import styles from './scss/contactsnote.module.scss';
import Arrow from '../../assets/images/projectinsights/smsbreadcrumb.svg'
import Attachmenticon from '../../assets/images/projectinsights/attachmenticon.svg'
import { useNavigate, useSearchParams } from "react-router-dom";
import { getCustomerNotesApi } from "../../base/service/core_serviceApi";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { useSelector } from "react-redux";

const Contactnotes = () => {

    const navigate = useNavigate();
    const { customerNotes } = useSelector((store: any) => store.contactStore)

    const formatDateTime = (isoString: string) => {
        const date = new Date(isoString);

        const options: Intl.DateTimeFormatOptions = {
            weekday: "long",
            month: "short",
            day: "numeric",
            year: "numeric",
        };

        const formattedDate = date.toLocaleDateString("en-US", options);

        let hours = date.getHours();
        const minutes = date.getMinutes().toString().padStart(2, "0");
        const ampm = hours >= 12 ? "pm" : "am";

        hours = hours % 12 || 12;

        return `${formattedDate} - ${hours}:${minutes} ${ampm}`;
    };

    return (
        <>
            <div className={styles.smscontainer}>
                <div className={styles.breadcrumb}>
                    <img src={Arrow} alt="Arrow" onClick={() => navigate(-1)} />
                    <div>
                        <div className={styles.breadcrumbtitle}>Advised expected completion</div>
                        <div className={styles.breadcrumbsubtitle}>{formatDateTime(customerNotes?.createdAt)} <span> Created by: </span>{customerNotes?.agent_name}</div>
                    </div>
                </div>
                <div className={styles.notescontainer}>
                    <div className={styles.reviewtitle} dangerouslySetInnerHTML={{ __html: customerNotes?.title || '' }} />
                   {customerNotes?.imageurl && <div className={styles.attachmenticon}>
                        <img src={customerNotes?.imageurl} alt="Notes image" />
                    </div>}
                </div>
            </div>
        </>
    )
}
export default Contactnotes