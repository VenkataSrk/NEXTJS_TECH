"use-client"
import { useEffect, useState } from 'react';
import styles from './scss/videodetails.module.scss'
import Arrow from '../../assets/images/contacts/chevronbreadcrumb.svg'
import Cardicon from '../../assets/images/contacts/cardicon.svg'
import Calendar from '../../assets/images/contacts/calendarview.svg'
import Videomeet from '../../assets/images/contacts/videomeet.svg'
import Copied from '../../assets/images/contacts/copiedicon.svg'
import Unifiedcontact from '../../assets/images/contacts/unifiedcontact.svg'
import { getPerticularInboxSessionDetails } from '../../base/service/core_serviceApi';
import { useNavigate, useSearchParams } from 'react-router-dom';
import { useDispatcher } from '../../store/redux-store/dispatch';
import moment from 'moment';

const Videoviewdetails = () => {
    const [params] = useSearchParams()
    const customerId = params.get('coid');
    const channelType = params.get('channelType');
    const sessionId = params.get('sessionId');
    const VideoHistoryId = params.get('VideoHistoryId');
    const dispatch = useDispatcher()
    const navigate = useNavigate()
    const [perticularDetails, setPerticularDetails] = useState<any>({})

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

    useEffect(() => {
        getPerticularInboxSessionDetailsApi()
    }, [])

    const getPerticularInboxSessionDetailsApi = async () => {
        const res =  await getPerticularInboxSessionDetails({
            channelType: channelType,
            sessionId: sessionId,
            historyId: VideoHistoryId,
        }, dispatch, false)

        if(res) {
            setPerticularDetails(res?.[0] ?? {});
        }
    }

    const goBack = () => {
        navigate(-1)
    }

    const getScheduleTime = (row: any) => {
        try {
            if(row?.channelType === 'Video' && row?.schedule_start){
                const start =  row?.schedule_start ? moment(row?.schedule_start).format("hh:mm A") : '';
                const end =  row?.schedule_end ? moment(row?.schedule_end).format("hh:mm A") : '' ;
                const date = row?.schedule_start ? moment(row?.schedule_start).format("dddd MMM D, YYYY") : '';
                return `${start} - ${end} - ${date}`
            }
            return moment(row?.created_date).format("dddd MMM D, YYYY hh:mm A")
        } catch (error) {
            return moment(row?.created_date).format("dddd MMM D, YYYY hh:mm A") 
        }
    }


    return (
        <>
            <div className={styles.companynameheader}>
                <div className={styles.topbarheader}>
                    <div className={styles.topbarleft}>
                        <img src={Arrow} alt="" onClick={goBack}/>
                    </div>
                    <div className={styles.topbarright}>
                        <div className={styles.topbarcontain}>
                            <div className={styles.titleblock}>{perticularDetails?.meeting_title || '-'} <span>{perticularDetails?.tags || '-'}</span></div>
                            <div className={styles.subtitlecard}>{getScheduleTime(perticularDetails)}</div>
                        </div>
                        <div className={styles.subtitle}>Initial discovery call to understand their requirements</div>
                    </div>
                </div>
                <div className={styles.videosummary}>
                    <div className={styles.summarytoptitle}>
                        <div className={styles.titleblocksummary}>Summary</div>
                        <div className={styles.card}>
                            <div className={styles.cardtitle}>Video Summary</div>
                            <div className={styles.cardsubtitle}>
                                {perticularDetails?.summary || '-'}
                            </div>
                        </div>
                    </div>

                    {/**  Completed  */}
                    {/* <div className={styles.chargeinvoice}>
                        <div className={styles.callreviewblock}>
                            <div className={styles.callreviewleft}>
                                <div className={styles.summary}>
                                    <div className={styles.quicksummary}>Summary of this audio</div>
                                    <div className={`${styles.quicksummary} ${styles.quicksummaryp}`}>
                                        {perticularDetails?.audioSummary || '-'}
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
                    </div> */}  {/* --------  completed ------------*/}

                    {/* <div className={styles.acmecorpcontainer}>
                        <div className={styles.cardblocksetcall}>
                             <div className={styles.callcardtitle}>
                                <div className={styles.callsubtitle}>Discovery Call with Acme Corp <span>Discovery Call</span></div>
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
                                            <img src={Calendar} alt="Calendar" />
                                        </div>
                                        <div className={styles.datecontainerright}>
                                            <div className={styles.discovercall}>Date</div>
                                            <div  className={styles.discovercalldate}>Tuesday, January 6, 2026</div>
                                        </div>
                                    </div>
                                    <div className={styles.callcardblock}>
                                        <div className={styles.datecontainerleft}>
                                            <img src={Calendar} alt="Calendar" />
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
                                   <div className={styles.meetingrightblock}>
                                      <img src={Copied} alt="Copied" />
                                      <span>Copy</span>
                                   </div>
                               </div>
                               <div className={styles.linksubtitle}>This link will be automatically added to calendar invites</div>
                           </div>
                        </div>
                    </div> */}

                     <div className={styles.attendeescontainer}>
                        <div className={styles.attendeestitle}>Attendees</div>
                        <div className={styles.attendeessubtitle}>{perticularDetails?.urmeet_user?.length || 0} participants invited</div>

                        <div className={styles.cardblockmain}>
                            {Array.isArray(perticularDetails?.urmeet_user) && perticularDetails?.urmeet_user?.map((item :any) => (
                                <div className={styles.cardblock} key={item.id}>
                                    {/* LEFT */}
                                    <div className={styles.cardcontainerleft}>
                                        {!!item.profile_img && <div className={styles.cardsource}>
                                            <img src={item.profile_img} alt="" />
                                        </div>}

                                        <div className={styles.cardcustomer}>
                                            <div className={styles.cardcustomername}>{item.name}</div>
                                            <div className={styles.cardcustomeremail}>{item.email}</div>
                                        </div>
                                    </div>

                                    {/* RIGHT */}
                                    <div className={styles.cardcontainerright}>
                                        {!!item.role && <div className={styles.leadcard}>{item.role || '-'}</div>}
                                        {!!item.status && <div className={`${styles.accepted} ${styles.pending}`}>{item.status}</div>}
                                    </div>
                                </div>
                            ))}
                        </div>

                        {/* <div className={styles.addattendee}>
                              <img src={Unifiedcontact} alt="Unifiedcontact" />
                              <span>Add Attendee</span>
                        </div> */}
                        
                    </div>
                    
                    {/* <div className={styles.attendeescontainer}>
                        <div className={styles.attendeestitle}>Notes</div>

                        <div className={styles.cardblockmain}>
                             <div className={styles.notetitlecard}>
                                  <div className={styles.notescontainer}>
                                      <div className={styles.cardicon}>
                                        <img src={Cardicon} alt="Cardicon" />
                                        <div className={styles.notescard}>Sarah Johnson</div>
                                      </div>
                                      <div className={styles.notescardline}>29 Dec, 02:15 AM</div>
                                  </div>
                                  <div className={styles.notessubtitles}>
                                      <div className={styles.notesrequest}>Note title</div>
                                      <div className={styles.subrequest}>Spoke with the customer regarding request status. Confirmed the request is currently under review. Advised expected completion within 24–48 hours. No further action required at this time.</div>
                                  </div>
                             </div>
                        </div>
                    </div>
                    
                    <div className={styles.attendeescontainer}>
                        <div className={styles.attendeestitle}>Action items</div>
                        <div className={styles.cardblockmain}>
                            <div className={styles.notetitlecard}>
                                  <div className={styles.notescontainer}>
                                      <div className={styles.cardicon}>
                                        <img src={Cardicon} alt="Cardicon" />
                                        <div className={styles.notescard}>Sarah Johnson</div>
                                      </div>
                                      <div className={styles.notescardline}>@ 4:45</div>
                                  </div>
                                  <div className={`${styles.notessubtitles} ${styles.subnotetitle}`}>
                                      <div className={styles.notesrequest}><input     className="custom-checkbox" type="checkbox" name="" id="" /></div>
                                      <div className={styles.subrequest}>Spoke with the customer regarding request status. Confirmed the request is currently under review. Advised expected completion within 24–48 hours. No further action required at this time.</div>
                                  </div>
                            </div>
                            <div className={styles.notetitlecard}>
                                  <div className={styles.notescontainer}>
                                      <div className={styles.cardicon}>
                                        <img src={Cardicon} alt="Cardicon" />
                                        <div className={styles.notescard}>Sarah Johnson</div>
                                      </div>
                                      <div className={styles.notescardline}>@ 4:45</div>
                                  </div>
                                  <div className={`${styles.notessubtitles} ${styles.subnotetitle}`}>
                                      <div className={styles.notesrequest}><input     className="custom-checkbox" type="checkbox" name="" id="" /></div>
                                      <div className={styles.subrequest}>Spoke with the customer regarding request status. Confirmed the request is currently under review. Advised expected completion within 24–48 hours. No further action required at this time.</div>
                                  </div>
                            </div>
                        </div>
                    </div> */}
                </div>
            </div>
        </>
    )
}
export default Videoviewdetails;