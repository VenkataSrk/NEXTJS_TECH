import React from "react";
import styles from './scss/timeline.module.scss'
const Timeline = () => {
    const interestedData = [
        {
            title: "Follow up with customer",
            subtitle: "Moved from Interested to Qualified",
            time: "09:12 AM",
        },
        {
            title: "Send proposal",
            subtitle: "Moved from Qualified to Proposal",
            time: "10:30 AM",
        },
    ];
    return (
        <>
            <div className={styles.timelinecontainer}>
                {interestedData.map((item, index) => (
                    <div className={styles.interestedblock} key={index}>
                        <div className={styles.interestlist}>
                            <div className={styles.interestlistdot}></div>

                            <div className={styles.interestedheader}>
                                <div className={styles.interestheader}>{item.title}</div>
                                <div className={styles.interestsubheader}>{item.subtitle}</div>
                            </div>
                        </div>

                        <div className={styles.interestsubslot}>{item.time}</div>
                    </div>
                ))}
            </div>
        </>
    )
}
export default Timeline;