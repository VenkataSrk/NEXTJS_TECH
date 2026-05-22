import React from "react";
import styles from "./scss/verifyprivate.module.scss";
const Verifyprivate = () => {
    const forwardData = [
        {
            title: "Forward Email",
            subtitle:
                "We’ve confirmed that email forwarding is active for your support email address.",
            status: "Forwarding active",
        },
        {
            title: "SPF Configuration",
            subtitle:
                "Your SPF record has been validated, authorizing both inbound and outbound emails for your domain.",
            status: "SPF record validated",
        },
        {
            title: "DNS Configuration",
            subtitle:
                "Your domain is verified, allowing outgoing emails to be sent from addresses at your domain.",
            status: "Forwarding active",
        },
    ];
    return (
        <>
            <div className={styles.verifyprivate}>
                <div className="titlecontainer">
                    <div className="titleheader">Verify Private Domain Email</div>
                    <div className="subtitle">Verify and configure your private domain email settings.</div>
                </div>
                <div className={styles.forwardemail}>
                    {forwardData.map((item, index) => (
                        <div key={index} className={styles.forwardlist}>
                            <div>
                                <div className={styles.forwardtitle}>{item.title}</div>
                                <div className={styles.forwardsubtitle}>{item.subtitle}</div>
                            </div>

                            <div className={styles.fowardiconbtn}>
                                <svg
                                    xmlns="http://www.w3.org/2000/svg"
                                    width="14"
                                    height="14"
                                    viewBox="0 0 14 14"
                                    fill="none"
                                >
                                    <path
                                        d="M6.66667 0C2.98473 0 0 2.98473 0 6.66667C0 10.3486 2.9848 13.3333 6.66667 13.3333C10.3485 13.3333 13.3333 10.3485 13.3333 6.66667C13.3333 2.98487 10.3485 0 6.66667 0ZM9.85247 4.81073L6.83453 8.74107C6.56927 9.0868 6.16753 9.299 5.73233 9.3244C5.70333 9.32633 5.67407 9.327 5.64507 9.327C5.24047 9.327 4.8524 9.16427 4.5686 8.87193L3.5188 7.79053C3.3264 7.5926 3.331 7.27553 3.5292 7.08353C3.7274 6.89153 4.0442 6.89667 4.2362 7.09393L5.286 8.17533C5.38887 8.2808 5.5204 8.33747 5.67433 8.3264C5.82147 8.31793 5.95167 8.24893 6.04153 8.13173L9.05947 4.2014C9.22747 3.98267 9.54253 3.94167 9.76033 4.1096C9.9794 4.2776 10.0207 4.59133 9.85247 4.81073Z"
                                        fill="black"
                                    />
                                </svg>
                                {item.status}
                            </div>
                        </div>
                    ))}
                </div>
            </div>
        </>
    )
}
export default Verifyprivate;
