import React from "react";
import styles from "./scss/smtpemail.module.scss";
const Smtpemail = () => {

    return (
        <>
            <div className={styles.smtpemailcontainer}>
                <div className="titlecontainer">
                    <div className="titleheader">SMTP Email</div>
                    <div className="subtitle">Set up your SMTP server for outbound emails.</div>
                </div>
                <div className={styles.smtpinnerblock}>
                    <div className={styles.setmainblockmain}>
                        <div className={styles.label}>Sender email</div>
                        <div className={styles.value}>support@yourdomain.com</div>
                    </div>

                    <div className={styles.setmainblockmain}>
                        <div className={styles.label}>User name</div>
                        <div className={styles.value}>SMTP account username</div>
                    </div>

                    <div className={styles.setmainblockmain}>
                        <div className={styles.label}>Password</div>
                        <div className={styles.value}>SMTP password</div>
                    </div>

                    <div className={styles.setmainblockmain}>
                        <div className={styles.label}>Host</div>
                        <div className={styles.value}>smtp.mailprovider.com</div>
                    </div>

                    <div className={styles.setmainblockmain}>
                        <div className={styles.label}>Port</div>
                        <div className={styles.value}>465</div>
                    </div>
                </div>
            </div>
        </>
    )
}
export default Smtpemail;
