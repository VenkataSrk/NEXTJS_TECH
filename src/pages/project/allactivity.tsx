"use-client"
import { useState } from 'react';
import styles from './scss/allactivity.module.scss'
import Allrequirement from './allrequirement';
import Bycustomer from './bycustomer';
import Bycompany from './bycompany';
import Timeline from './timeline';

const Allactivity = () => {
    const [activeTab, setActiveTab] = useState("allrequirement");
    return (
        <>
            <div className={styles.allactivity}>
                <div className={styles.allactivitycontainer}>
                    <div className={styles.activitytitle}>All activities</div>
                    <div className={styles.activitysubtitle}>Content needed</div>
                </div>
                <div className={styles.filters}>
                     <span
                        className={`${styles.filterchip} ${activeTab === "timeline" ? styles.active : ""
                            }`}
                        onClick={() => setActiveTab("timeline")}
                    >
                        Timeline
                    </span>
                    <span
                        className={`${styles.filterchip} ${activeTab === "allrequirement" ? styles.active : ""
                            }`}
                        onClick={() => setActiveTab("allrequirement")}
                    >
                        All Requirements
                    </span>

                    <span
                        className={`${styles.filterchip} ${activeTab === "customer" ? styles.active : ""
                            }`}
                        onClick={() => setActiveTab("customer")}
                    > By Customer</span>

                    <span
                        className={`${styles.filterchip} ${activeTab === "company" ? styles.active : ""
                            }`}
                        onClick={() => setActiveTab("company")}
                    >
                        By Company
                    </span>
                </div>
                <div className={styles.tabContent}>
                    {activeTab === "timeline" && <div><Timeline /></div>}
                    {activeTab === "allrequirement" && <div><Allrequirement /></div>}
                    {activeTab === "customer" && <div><Bycustomer /></div>}
                    {activeTab === "company" && <div><Bycompany /></div>}
                </div>
            </div>
        </>
    )
}
export default Allactivity;