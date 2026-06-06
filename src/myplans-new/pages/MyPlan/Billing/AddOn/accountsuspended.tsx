import React, { useState } from "react";
import styles from '../../../../assets/styles/autoreneval.module.scss';
import { useNavigate } from "react-router-dom";
import { assets } from "../../../../../styles/assets/assets";
const Accountsuspended = () => {
    const navigate = useNavigate();
    const [open, setOpen] = useState(false);
    return (
        <>
            <div>
                <div className={styles.topbarview}>
                    <div className={styles.worktualtopbar}>
                        <img src={assets.worktual_logo} alt="" className={styles.topbarwi} />
                    </div>
                    <div className={`${styles.topbarright} notifycl`}>
                        <ul>
                            <li style={{ display: 'flex', cursor: 'pointer' }}>
                                {/* <img src={assets.Detailsicon} alt="notification" onClick={() => setOpen(true)} /> */}
                            </li>
                            <li className={styles.profileWrapper}>
                                <div className={styles.profilesec}>
                                    <span>DH</span>
                                </div>
                            </li>
                        </ul>
                    </div>
                </div>
                {/* Sidebar */}
                <div className={`${styles.offcanvas} ${open ? styles.open : ""}`}>
                    <div className={styles.readytri}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="72" height="43" viewBox="0 0 72 43" fill="none">
                            <path d="M35.5068 0L71.0139 42.75H-0.000205994L35.5068 0Z" fill="white" />
                        </svg>
                    </div>

                    <div className={styles.offcanvasheader}>
                        <div className={styles.readyhelp}>
                            <h3>Help</h3>
                            <p>Need assistance? Lola is ready to help you</p>
                        </div>
                        <button className={styles.closebtn}>
                            ✕
                        </button>
                    </div>
                    <div className={styles.offcanvasbody}>
                        <div className={styles.iconicmsg}>
                            <div
                            >
                                <div className={styles.chatmaincontainer}>
                                    <div className={styles.chatleftside}>
                                        <div className={styles.lolaheading}>
                                            <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24"
                                                viewBox="0 0 24 24" fill="none">
                                                <path d="M23.8951 4.37152V16.0324H15.8702V12.4304C15.8702 10.0176 13.9208 8.05979 11.5176 8.05979H7.93115V0H19.5425C21.9457 0 23.8951 1.9569 23.8951 4.37152Z" fill="black" />
                                                <path d="M15.8689 16.0312V24.0001L7.93359 16.0312H15.8689Z" fill="black" />
                                                <path d="M15.8698 12.4292C15.8698 10.0155 13.9213 8.05859 11.5181 8.05859H7.93158L7.93441 8.06144H0V16.0302H7.93441V8.06238L15.8688 16.0312V12.4292H15.8698Z" fill="black" />
                                            </svg>
                                            <h3>Lola</h3>
                                        </div>
                                    </div>
                                </div>
                                <div className={styles.chatrightside}>
                                    <div className={styles.handlingconvo}>hgfh</div>
                                </div>
                            </div>
                            <div className={`${styles.bubble} ${styles.leftmsgbuble}`}>
                                <h3 className={styles.detailist}>
                                    Typing
                                    <span className={styles.messagethinkloadr}></span>
                                </h3>
                            </div>
                        </div>
                    </div>

                </div>
            <div className={styles.wrapper}>
                <div className={styles.card}>
                    <div className={styles.icon}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="52" height="52" viewBox="0 0 52 52" fill="none">
                            <path d="M46.4492 34.8916L32.1059 10.0399C29.3976 5.33826 22.5942 5.33826 19.8859 10.0399L5.54256 34.8916C2.83423 39.5933 6.21423 45.4866 11.6526 45.4866H40.3392C45.7776 45.4866 49.1792 39.5933 46.4492 34.8916ZM27.1442 36.7333C26.8409 37.0366 26.4292 37.2099 25.9959 37.2099C25.7792 37.2099 25.5842 37.1666 25.3676 37.0799C25.1726 36.9933 24.9992 36.8849 24.8476 36.7333C24.5442 36.4299 24.3709 36.0183 24.3709 35.5849C24.3709 35.1516 24.5442 34.7399 24.8476 34.4366C24.9992 34.2849 25.1726 34.1549 25.3676 34.0899C25.9742 33.8299 26.6892 33.9816 27.1442 34.4366C27.4476 34.7399 27.6209 35.1516 27.6209 35.5849C27.6209 36.0183 27.4476 36.4299 27.1442 36.7333ZM27.6209 31.0566C27.6209 31.9449 26.8842 32.6816 25.9959 32.6816C25.1076 32.6816 24.3709 31.9449 24.3709 31.0566V16.4316C24.3709 15.5433 25.1076 14.8066 25.9959 14.8066C26.8842 14.8066 27.6209 15.5433 27.6209 16.4316V31.0566Z" fill="#F4B400" />
                        </svg>
                    </div>

                    <h2 className={styles.title}>Account Suspended</h2>

                    <p className={styles.description}>
                        Your account has been temporarily suspended due to an unpaid invoice.
                        Please complete the payment below to restore access.
                    </p>

                    <div className={styles.balanceSection}>
                        <div className={styles.pricesec}>
                            <p className={styles.label}>Outstanding balance</p>
                            <h1 className={styles.amount}>₹12,000</h1>
                        </div>


                        <p className={styles.invoice}>Invoice #INV-20342</p>
                        <p className={styles.dueDate}>Due date: Feb 12, 2026</p>
                    </div>

                    <button className={styles.payButton}>Pay Now</button>

                    <p className={styles.note}>
                        Your account will be restored automatically after payment.
                    </p>

                    <div className={styles.links}>
                        <button>View Invoice</button>
                        <button>Update Payment Method</button>
                        <button>Contact Support</button>
                    </div>
                </div>
            </div>
        </div >
        </>
    )
}
export default Accountsuspended