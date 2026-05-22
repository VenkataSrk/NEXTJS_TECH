"use-client"
import { useState } from 'react';
import styles from './scss/dealsupcoming.module.scss'
import Arrow from '../../assets/images/contacts/chevronbreadcrumb.svg'
import Cardicon from '../../assets/images/contacts/cardicon.svg'
import Calendar from '../../assets/images/contacts/calendarview.svg'
import Location from '../../assets/images/projectinsights/location.svg'
import Clockupcoming from '../../assets/images/projectinsights/clockcoming.svg'
import Videomeet from '../../assets/images/contacts/videomeet.svg'
import Copied from '../../assets/images/contacts/copiedicon.svg'
import Unifiedcontact from '../../assets/images/contacts/unifiedcontact.svg'
import Startmeet from '../../assets/images/deals/startmeeting.svg'
import Contactlist from '../../assets/images/deals/contactlist.svg'
const Dealsupcoming = () => {
    const cardList = [
        {
            id: 1,
            name: 'Steve Harrington',
            email: 'steveharrington@gmail.com',
            role: 'Customer',
            status: 'Accepted',
            icon: Cardicon,
        },
        {
            id: 2,
            name: 'William Parker',
            email: 'williamparker@gmail.com',
            role: 'Lead',
            status: 'Pending',
            icon: Cardicon,
        },
    ];
    return (
        <>
            <div className={styles.companynameheader}>
                <div className={styles.topbarheader}>
                    <div className={styles.topbarleft}>
                        <img src={Arrow} alt="" />
                    </div>
                    <div className={styles.topbarright}>
                        <div className={styles.topbarcontain}>
                            <div className={styles.titleblock}>Discovery Call with Acme Corp <span>Complete</span></div>
                            <div className={styles.subtitlecard}>10:00 am - 11:00 am - Monday Jan 5, 2026</div>
                        </div>
                        <div className={styles.subtitle}>Initial discovery call to understand their requirements</div>
                    </div>
                </div>
                <div className={styles.videosummary}>
                    {/* <div className={styles.summarytoptitle}>
                        <div className={styles.titleblocksummary}>Summary</div>
                        <div className={styles.card}>
                            <div className={styles.cardtitle}>Video Summary</div>
                            <div className={styles.cardsubtitle}>Customer called about unexpected charge on invoice. Sarah identified a duplicate billing entry, processed refund of ₹12,000, and sent corrected invoice.</div>
                        </div>
                    </div> */}
                    {/* <div className={styles.chargeinvoice}>
                        <div className={styles.callreviewblock}>
                            <div className={styles.callreviewleft}>
                                <div className={styles.summary}>
                                    <div className={styles.quicksummary}>Summary of this audio</div>
                                    <div className={`${styles.quicksummary} ${styles.quicksummaryp}`}>
                                        Customer reported an issue with their recent order. The order status page shows an error, and they are unsure about the current status. Requested assistance.
                                    </div>
                                </div>

                                <div className={styles.content}>
                                    <div className={styles.videoSection}>
                                        <video
                                            src="https://www.w3schools.com/html/mov_bbb.mp4"
                                            controls
                                            preload="auto"
                                        />
                                    </div>
                                </div>
                            </div>

                            <div className={styles.callreviewright}>
                                <div className={styles.transcripty}>Transcript</div>

                                <div className={styles.list}>
                                    <div className={styles.item}>
                                        <div className={`${styles.meta} ${styles.metaavatar}`}>
                                            <span>00:12</span>

                                            <div className={styles.hourwellse}>
                                                <div className={styles.metaprofi}>
                                                    <div className={styles.spanvatar}>W</div>

                                                    <div className={styles.msgagent}>
                                                        <div className={styles.agentheader}>William</div>
                                                        <div className={styles.plansubtitle}>Hi, I need help understanding the pricing.</div>
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    </div>

                                    <div className={styles.item}>
                                        <div className={`${styles.meta} ${styles.metaavatar}`}>
                                            <span>00:28</span>

                                            <div className={styles.hourwellse}>
                                                <div className={styles.metaprofi}>
                                                    <div className={styles.spanvatar}>A</div>

                                                    <div className={styles.msgagent}>
                                                        <div className={styles.agentheader}>Agent (Amelia)</div>
                                                        <div className={styles.plansubtitle}>Sure William, I’ll walk you through the available plans.</div>
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    </div>

                                    <div className={styles.item}>
                                        <div className={`${styles.meta} ${styles.metaavatar}`}>
                                            <span>00:55</span>

                                            <div className={styles.hourwellse}>
                                                <div className={styles.metaprofi}>
                                                    <div className={styles.spanvatar}>B</div>

                                                    <div className={styles.msgagent}>
                                                        <div className={styles.agentheader}>Bot</div>
                                                        <div className={styles.plansubtitle}>Would you like me to send the pricing details to your email?</div>
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    </div>

                                </div>
                            </div>

                        </div>

                    </div> */}
                    <div className={styles.acmecorpcontainer}>
                        <div className={styles.cardblocksetcall}>
                             <div className={styles.callcardtitle}>
                                <div className={styles.calldiscover}>
                                    <div className={styles.callsubtitle}>Discovery Call with Acme Corp <span>Discovery Call</span></div>
                                    <div className={styles.callslot}>Starts in 2 min</div>
                                </div>
                                <div className={styles.discovery}>
                                    <div className={styles.callcardblock}>
                                        <div className={styles.datecontainerleft}>
                                            <img src={Calendar} alt="Calendar" />
                                        </div>
                                        <div className={styles.datecontainerright}>
                                            <div className={styles.discovercall}>Date</div>
                                            <div  className={styles.discovercalldate}>Tuesday, January 6, 2026</div>
                                        </div>
                                    </div>
                                    <div className={styles.callcardblock}>
                                        <div className={styles.datecontainerleft}>
                                            <img src={Clockupcoming} alt="Clockupcoming" />
                                        </div>
                                        <div className={styles.datecontainerright}>
                                            <div className={styles.discovercall}>Date</div>
                                            <div  className={styles.discovercalldate}>Tuesday, January 6, 2026</div>
                                        </div>
                                    </div>
                                    <div className={styles.callcardblock}>
                                        <div className={styles.datecontainerleft}>
                                            <img src={Location} alt="Location" />
                                        </div>
                                        <div className={styles.datecontainerright}>
                                            <div className={styles.discovercall}>Date</div>
                                            <div  className={styles.discovercalldate}>Tuesday, January 6, 2026</div>
                                        </div>
                                    </div>
                                </div>
                             </div>
                        </div>
                        <div className={styles.cardblocksetcall}>
                           <div className={styles.meetinglinkcontainer}>
                               <div className={styles.meetingtitle}>Meeting Link</div>
                               <div className={styles.meetingblock}>
                                   <div className={styles.meetingleftblock}>
                                      <img src={Videomeet} alt="Videomeet" />
                                      <a href='/'>https://meet.google.com/abc-defg-hij</a>
                                   </div>
                                   <div className={styles.copyrightblock}>
                                        <div className={styles.meetingrightblock}>
                                            <img src={Copied} alt="Copied" />
                                            <span>Copy</span>
                                        </div>
                                        <div className={styles.startmeet}>
                                            <img src={Startmeet} alt="Copied" />
                                            <span>Start Meeting Now</span>
                                        </div>
                                   </div>
                               </div>
                               <div className={styles.linksubtitle}>This link will be automatically added to calendar invites</div>
                           </div>
                        </div>
                    </div>
                     <div className={styles.attendeescontainer}>
                        <div className={styles.inviteparticipants}>
                            <div className={styles.invitedparticipantlist}>
                                <div className={styles.attendeestitle}>Invite participants</div>
                                <div className={styles.attendeessubtitle}>3 participants invited</div>
                            </div>
                            <div className={styles.attendees}>  
                                <img src={Contactlist} alt="Copied" />
                                <span>Add Attendee</span>
                            </div>
                        </div>

                        <div className={styles.cardblockmain}>
                            {cardList.map((item) => (
                                <div className={styles.cardblock} key={item.id}>
                                    {/* LEFT */}
                                    <div className={styles.cardcontainerleft}>
                                        <div className={styles.cardsource}>
                                            <img src={item.icon} alt="Cardicon" />
                                        </div>

                                        <div className={styles.cardcustomer}>
                                            <div className={styles.cardcustomername}>{item.name}</div>
                                            <div className={styles.cardcustomeremail}>{item.email}</div>
                                        </div>
                                    </div>

                                    {/* RIGHT */}
                                    <div className={styles.cardcontainerright}>
                                        <div className={styles.leadcard}>{item.role}</div>
                                        <div className={`${styles.accepted} ${styles.pending}`}>{item.status}</div>
                                    </div>
                                </div>
                            ))}
                        </div>
                        {/* <div className={styles.addattendee}>
                              <img src={Unifiedcontact} alt="Unifiedcontact" />
                              <span>Add Attendee</span>
                        </div> */}
                    </div>
                </div>
            </div>
        </>
    )
}
export default Dealsupcoming;