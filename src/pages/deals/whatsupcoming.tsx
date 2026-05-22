import React from "react";
import styles from "./scss/dealslist.module.scss"
import mailicon from "../../assets/images/inbox/mailicon.svg"
import Whatsupcoming from "../contacts/whatupcoming";
import Clock from '../../assets/images/contacts/clockicon.svg'
function Upcoming() {
      const activityLogs = [
            {
                time: '11:00 AM',
                icon: Clock,
                title: 'Web Registration',
                subtitle: 'Welcome journey will begin after account verification.'
            },
            {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },
            {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },
            {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },
            {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },
               {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },
               {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },   {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },   {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            },
               {
                time: '11:30 AM',
                icon: Clock,
                title: 'Email Verified',
                subtitle: 'User successfully verified email address.'
            }
        ];
    return (<>
        <div className={`${styles.dealslistcontainer} ${styles.upcomingcontainer} maincontainer`} >
            <div className={styles.card}>
                <div className={styles.top}>
                    <div className={styles.lefts}>
                        <div className={styles.namewithbadge}>
                            <h5>Zoom meet - Discovery Call</h5>

                        </div>
                        <div className={styles.secondline}>

                            <p>Initial discovery call to understand their requirements</p>
                        </div>
                    </div>
                    <div className={styles.rights}>
                        <div className={`${styles.scenerio} ${styles.red}`}>Yet to start</div>
                        <div className={styles.timer}><span><svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                            <path d="M6.66667 0C3 0 0 3 0 6.66667C0 10.3333 3 13.3333 6.66667 13.3333C10.3333 13.3333 13.3333 10.3333 13.3333 6.66667C13.3333 3 10.3333 0 6.66667 0ZM6.66667 12.3333C3.53333 12.3333 1 9.8 1 6.66667C1 3.53333 3.53333 1 6.66667 1C9.8 1 12.3333 3.53333 12.3333 6.66667C12.3333 9.8 9.8 12.3333 6.66667 12.3333Z" fill="#3F3F49" />
                            <path d="M6.66667 1.93333C6.4 1.93333 6.13333 2.13333 6.13333 2.46667V5.86667C6.13333 6.06667 6 6.2 5.8 6.2H3.13333C2.86667 6.2 2.6 6.4 2.6 6.73333C2.6 7.06667 2.8 7.26667 3.13333 7.26667H5.8C6.53333 7.26667 7.13333 6.66667 7.13333 5.93333V2.4C7.2 2.13333 6.93333 1.93333 6.66667 1.93333Z" fill="#3F3F49" />
                        </svg></span>10:00 am - 11:00 am - Monday Jan 5, 2026</div>
                    </div>

                </div>
                <div className={styles.bottom}>
                    <div className={styles.lefts}>
                        <div style={{ display: "grid" }}><img src={mailicon} /></div>
                        <p><span className={styles.boldtxt}>Created by:</span>William</p>
                        <p><span className={styles.boldtxt}>Participants:</span> <div className="iconStack">
                            <img src={mailicon} />
                            <img src={mailicon} />
                            <img src={mailicon} />
                        </div></p>

                    </div>
                    <div className={styles.rights}>
                        <div className={styles.btncontainer}>
                            <button>Start Meeting</button>
                            <button>Copy link</button>
                        </div>

                    </div>
                </div>
            </div>
           <div className={styles.upcomingactivity}>
                           <div className={styles.upcomingtitle}>Upcoming Activities</div>
                           <div className={styles.activitylogcontain}>
                               {activityLogs.map((item, index) => (
                                   <div className={styles.activitylogset} key={index}>
                                       <div className={styles.activitytitle}>{item.time}</div>
           
                                       <div className={styles.activitycontainer}>
                                           <div className={styles.activityleft}>
                                               <img src={item.icon} alt="Clock" />
                                           </div>
           
                                           <div className={styles.activityright}>
                                               <div className={styles.activetitle}>{item.title}</div>
                                               <div className={styles.activesubtitle}>{item.subtitle}</div>
                                           </div>
                                       </div>
                                   </div>
                               ))}
                           </div>
                       </div>
            <div className={styles.commonbox}>
                 <div className={styles.headerr}>
                                <div className={styles.heading}>Action items</div>
                                
                            </div>
                            <div className={styles.cardwrapper}>
   <div className={`${styles.card} ${styles.mentionscard}`}>
                <div className={styles.top}>
                    <div className={styles.namewithmention}>
                        <div className={styles.imgwithname}><img src={mailicon} />Steve Harrington</div>
                        <span>@ 4:45</span>
                    </div>

                </div>
                <div className={styles.bottom}>
                    <label className={`content ${styles.checkboxwithcontent}`}>
                    <input     className="custom-checkbox" type="checkbox" /> Spoke with the customer regarding request status. Confirmed the request is currently under review. Advised expected completion within 24–48 hours. No further action required at this time.
                </label>
                </div>
            </div>
                            </div>
          
            </div>
           
        </div>
    </>)
} export default Upcoming