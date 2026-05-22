import React, { useState } from "react";
import styles from './scss/contactsmsview.module.scss';
import Arrow from '../../assets/images/projectinsights/smsbreadcrumb.svg'
import Chevron_up from '../../assets/images/projectinsights/chevronup.svg'
const Contactsmsview = () => {
    const [isOpen, setIsOpen] = useState(false);
    return(
        <>
           <div className={styles.smscontainer}>
                <div className={styles.breadcrumb}>
                    <img src={Arrow} alt="Arrow" />
                    <div className={styles.breadcrumbtitle}>SMS</div>
                </div>
                <div className={styles.smsviewcontainer}>
                    <div className={styles.smsblock}>
                        <div className={styles.smsblockleft}>
                            <div className={styles.smstitleblock}>
                                <img src={Chevron_up} alt="" onClick={() => setIsOpen(!isOpen)}/>
                                SMS - Customer reported an issue
                            </div>
                            <div className={styles.smstitleslot}>Jan 03, 2026 at 8:03 AM </div>
                        </div>
                        {isOpen && (
                        <div className={styles.smsblockright}>
                            <div className={styles.smscontact}>
                                <div className={styles.smscontactitle}>SH</div>
                                <div className={styles.smscontactsubtitle}>Steve Harrington</div>
                            </div>
                            <div className={styles.smsdescription}>Customer reported an issue with their recent order. The order status page shows an error, and they are unsure about the current status. Requested assistance.</div>
                        </div>
                        )}
                    </div>
                     <div className={styles.smsblock}>
                        <div className={styles.smsblockleft}>
                            <div className={styles.smstitleblock}>
                                <img src={Chevron_up} alt="" onClick={() => setIsOpen(!isOpen)}/>
                                SMS - Customer reported an issue
                            </div>
                            <div className={styles.smstitleslot}>Jan 03, 2026 at 8:03 AM </div>
                        </div>
                        {isOpen && (
                        <div className={styles.smsblockright}>
                            <div className={styles.smscontact}>
                                <div className={styles.smscontactitle}>SH</div>
                                <div className={styles.smscontactsubtitle}>Steve Harrington</div>
                            </div>
                            <div className={styles.smsdescription}>Customer reported an issue with their recent order. The order status page shows an error, and they are unsure about the current status. Requested assistance.</div>
                        </div>
                        )}
                    </div>
                     <div className={styles.smsblock}>
                        <div className={styles.smsblockleft}>
                            <div className={styles.smstitleblock}>
                                <img src={Chevron_up} alt="" onClick={() => setIsOpen(!isOpen)}/>
                                SMS - Customer reported an issue
                            </div>
                            <div className={styles.smstitleslot}>Jan 03, 2026 at 8:03 AM </div>
                        </div>
                        {isOpen && (
                        <div className={styles.smsblockright}>
                            <div className={styles.smscontact}>
                                <div className={styles.smscontactitle}>SH</div>
                                <div className={styles.smscontactsubtitle}>Steve Harrington</div>
                            </div>
                            <div className={styles.smsdescription}>Customer reported an issue with their recent order. The order status page shows an error, and they are unsure about the current status. Requested assistance.</div>
                        </div>
                        )}
                    </div>
                </div>
           </div>
        </>
    )
}
export default Contactsmsview