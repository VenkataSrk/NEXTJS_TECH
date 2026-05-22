"use-client"
import { useEffect, useRef, useState } from 'react';
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
import { useNavigate, useSearchParams } from 'react-router-dom';
import { getPerticularInboxSessionDetails } from '../../base/service/core_serviceApi';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { getScheduleTime } from '../../base/utils';
import moment from 'moment';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { v4 as uuidv4 } from "uuid";
import MeetingTimer from './components/MeetingTimer';
import aiChatAssistSocketservice from '../../services/aiChatService';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import { contactDataSliceActions } from '../../store/slice/contactSlice';
import { useSelector } from 'react-redux';

const ScheduledMeeting = () => {

    const navigate = useNavigate();
    const dispatch = useDispatcher();
    const [params] = useSearchParams()
    const channelType = params.get('channelType');
    const sessionId = params.get('sessionId');
    const customerId = params.get("coid");
    const companyId = params.get("company_id")
    const companyName = params.get("companyId")
    const VideoHistoryId = params.get('VideoHistoryId');

    const { contactPerticularActivity } = useSelector((store: any) => store?.contactStore)

    // const [perticularDetails, setPerticularDetails] = useState<any>({});


    const [isCopied, setIsCopied] = useState(false);
    const copyResetRef = useRef<number | null>(null);
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);

    useEffect(() => {
        getPerticularInboxSessionDetailsApi()
    }, [])

    useEffect(() => {
        return () => {
            if (copyResetRef.current) {
                window.clearTimeout(copyResetRef.current);
            }
            dispatch(contactDataSliceActions.setContactPerticularActivity({}));
        };
    }, []);

    const getPerticularInboxSessionDetailsApi = async () => {
        const res = await getPerticularInboxSessionDetails({
            channelType: channelType,
            sessionId: sessionId,
            historyId: VideoHistoryId,
        }, dispatch, false)

        if (res) {
            // setPerticularDetails(res?.[0] ?? {});
            dispatch(contactDataSliceActions.setContactPerticularActivity(res?.[0] ?? {}));
        }
    }


    const goBack = () => {
        navigate(-1);
    }

    const handleCopyMeetingLink = () => {
        const url = contactPerticularActivity?.meet_url;
        if (!url) return;

        navigator.clipboard.writeText(url);
        setIsCopied(true);

        if (copyResetRef.current) {
            clearTimeout(copyResetRef.current);
        }

        copyResetRef.current = setTimeout(() => {
            setIsCopied(false);
        }, 2000);
    };

    const handleStartMeeting = () => {
        const url = contactPerticularActivity?.meet_url;
        if (!url) return;
        window.open(url, '_blank', 'noopener,noreferrer');
    };

    const formatDate = (dateString) => {
        return new Date(dateString).toLocaleDateString("en-US", {
            weekday: "long",
            year: "numeric",
            month: "long",
            day: "numeric",
        });
    };

    const handleNewConversation = () => {
        const sessionId: any = uuidv4() + 'contact';
        let body: any = {
            query: 'addAttendee',
            sessionId: sessionId,
            roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
            org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
            userId: `${LoginCcaasUserDetails?.userId || ''}`,
            domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
            module: '',
            ext: `${LoginCcaasUserDetails?.ext || ''}`,
            agent: 'contact',
            isBot: 0,
            customerId: `${customerId || ''}`,
            companyId: `${companyId || ''}`,
            companyName: `${companyName || ''}`,
            uploadUrl: [],
            meetingId: contactPerticularActivity?.meetingId,
        }
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/contact/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "contact" }));
        navigate(`/contact/chat-conversation?sessionId=${sessionId}`);
        aiChatAssistSocketservice.io.emit('crm_chat', body);
    }

    return (
        <>
            <div className={styles.companynameheader}>
                <div className={styles.topbarheader}>
                    <div className={styles.topbarleft} onClick={() => goBack()}>
                        <img src={Arrow} alt="" />
                    </div>
                    <div className={styles.topbarright}>
                        <div className={styles.topbarcontain}>
                            <div className={styles.titleblock}>{contactPerticularActivity?.meeting_title || '-'} <span>{contactPerticularActivity?.tags || '-'}</span></div>
                            <div className={styles.subtitlecard}>{getScheduleTime(contactPerticularActivity)}</div>
                        </div>
                        <div className={styles.subtitle}>{contactPerticularActivity?.summary || '-'}</div>
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
                                    <div className={styles.callsubtitle}>{contactPerticularActivity?.meeting_title || '-'} <span>{contactPerticularActivity?.tags || '-'}</span></div>
                                    {contactPerticularActivity?.schedule_start && (
                                        <div className={styles.callslot}>
                                            <MeetingTimer startTime={contactPerticularActivity?.schedule_start} />
                                        </div>
                                    )}
                                </div>
                                <div className={styles.discovery}>
                                    <div className={styles.callcardblock}>
                                        <div className={styles.datecontainerleft}>
                                            <img src={Calendar} alt="Calendar" />
                                        </div>
                                        <div className={styles.datecontainerright}>
                                            <div className={styles.discovercall}>Date</div>
                                            <div className={styles.discovercalldate}>{formatDate(contactPerticularActivity?.schedule_start)}</div>
                                        </div>
                                    </div>
                                    <div className={styles.callcardblock}>
                                        <div className={styles.datecontainerleft}>
                                            <img src={Clockupcoming} alt="Clockupcoming" />
                                        </div>
                                        <div className={styles.datecontainerright}>
                                            <div className={styles.discovercall}>Time</div>
                                            <div className={styles.discovercalldate}>
                                                {contactPerticularActivity?.schedule_start && contactPerticularActivity?.schedule_end
                                                    ? `${moment(contactPerticularActivity.schedule_start).format("hh:mm A")} - ${moment(contactPerticularActivity.schedule_end).format("hh:mm A")}`
                                                    : ''}
                                            </div>
                                        </div>
                                    </div>
                                    <div className={styles.callcardblock}>
                                        <div className={styles.datecontainerleft}>
                                            <img src={Location} alt="Location" />
                                        </div>
                                        <div className={styles.datecontainerright}>
                                            <div className={styles.discovercall}>Location</div>
                                            <div className={styles.discovercalldate}>{contactPerticularActivity?.meeting_platform || '-'}</div>
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
                                        <a className="truncate2" href={contactPerticularActivity?.meet_url || '#'} target="_blank" rel="noreferrer">
                                            {contactPerticularActivity?.meet_url}
                                        </a>
                                    </div>
                                    <div className={styles.copyrightblock}>
                                        <div className={styles.meetingrightblock} onClick={handleCopyMeetingLink}>
                                            <img src={Copied} alt="Copied" />
                                            <span>{isCopied ? 'Copied' : 'Copy'}</span>
                                        </div>
                                        <div className={styles.startmeet} onClick={handleStartMeeting}>
                                            <img src={Startmeet} alt="Meeting Link" />
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
                                <div className={styles.attendeessubtitle}>
                                    {contactPerticularActivity?.urmeet_user?.length} participant{contactPerticularActivity?.urmeet_user?.length === 1 ? '' : 's'} invited
                                </div>
                            </div>
                            <div className={styles.attendees} onClick={handleNewConversation}>
                                <img src={Contactlist} alt="Copied" />
                                <span>Add Attendee</span>
                            </div>
                        </div>
                        <div className={styles.cardblockmain}>
                            {Array.isArray(contactPerticularActivity?.urmeet_user) && contactPerticularActivity?.urmeet_user?.map((item: any) => (
                                <div className={styles.cardblock} key={item.id}>
                                    <div className={styles.cardcontainerleft}>
                                        {!!item.profile_img && <div className={styles.cardsource}>
                                            <img src={item.profile_img} alt="" />
                                        </div>}

                                        <div className={styles.cardcustomer}>
                                            <div className={styles.cardcustomername}>{item.name}</div>
                                            <div className={styles.cardcustomeremail}>{item.email}</div>
                                        </div>
                                    </div>

                                    <div className={styles.cardcontainerright}>
                                        {!!item.role && <div className={styles.leadcard}>{item.role || '-'}</div>}
                                        {!!item.status && <div className={`${styles.accepted} ${styles.pending}`}>{item.status}</div>}
                                    </div>
                                </div>
                            ))}
                        </div>
                        {/* <div className={styles.cardblockmain}>
                            {cardList.map((item) => (
                                <div className={styles.cardblock} key={item?.id}>
                                    <div className={styles.cardcontainerleft}>
                                        <div className={styles.cardsource}>
                                            <img src={item?.icon} alt="Cardicon" />
                                        </div>

                                        <div className={styles.cardcustomer}>
                                            <div className={styles.cardcustomername}>{item?.name}</div>
                                            <div className={styles.cardcustomeremail}>{item?.email}</div>
                                        </div>
                                    </div>

                                    <div className={styles.cardcontainerright}>
                                        <div className={styles.leadcard}>{item?.role}</div>
                                        <div className={`${styles.accepted} ${styles.pending}`}>{item?.status}</div>
                                    </div>
                                </div>
                            ))}
                        </div> */}
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
export default ScheduledMeeting;
