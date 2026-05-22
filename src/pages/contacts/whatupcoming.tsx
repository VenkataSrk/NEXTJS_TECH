import styles from './scss/whatsupcoming.module.scss'
import Clock from '../../assets/images/contacts/clockicon.svg'
import mailicon from "../../assets/images/inbox/mailicon.svg"
import Whatsupcoming from "../contacts/whatupcoming";
import { useEffect, useRef, useState } from 'react';
import { getScheduleMeetingApi } from '../../base/service/core_serviceApi';
import { useSearchParams } from 'react-router-dom';
import { useDispatch, useSelector } from 'react-redux';
import Nodata from '../component/nodata';
import { getScheduleTime } from '../../base/utils';
import MeetingTimer from './components/MeetingTimer';
import { contactDataSliceActions } from '../../store/slice/contactSlice';


function Upcoming() {

    const { contactScheduledata } = useSelector((store: any) => store.contactStore)
    const [copiedMeetingId, setCopiedMeetingId] = useState<any>(null);
    const copyResetRef = useRef<any>(null);
    const [paramns] = useSearchParams()
    const dispatch = useDispatch()
    const customerId = paramns.get('coid')

    // const activityLogs = [
    //     {
    //         time: '11:00 AM',
    //         icon: Clock,
    //         title: 'Web Registration',
    //         subtitle: 'Welcome journey will begin after account verification.'
    //     },
    //     {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     },
    //     {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     },
    //     {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     },
    //     {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     },
    //     {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     },
    //     {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     }, {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     }, {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     },
    //     {
    //         time: '11:30 AM',
    //         icon: Clock,
    //         title: 'Email Verified',
    //         subtitle: 'User successfully verified email address.'
    //     }
    // ];
    useEffect(()=>{
        getScheduleData()
    },[customerId])

    useEffect(() => {
        return () => {
            if (copyResetRef.current) {
                window.clearTimeout(copyResetRef.current);
            }
        };
    }, []);

    const [loader, setLoader] = useState(false)
    const getScheduleData = async () => {
        dispatch(contactDataSliceActions.setContactScheduledata([]));
        setLoader(true)
        await getScheduleMeetingApi({
            customerId: customerId,
        }, dispatch)
        setLoader(false)
    }


    const handleCopyMeetingLink = (url: any, meetingId: string | number) => {
        if (!url) return;
        navigator.clipboard.writeText(url);
        setCopiedMeetingId(meetingId);
        if (copyResetRef.current) {
            window.clearTimeout(copyResetRef.current);
        }
        copyResetRef.current = window.setTimeout(() => {
            setCopiedMeetingId(null);
        }, 2000);
    };

    const handleStartMeeting = (url: string) => {
        if (!url) return;
        window.open(url, '_blank', 'noopener,noreferrer');
    };

    return (<>
        {!loader && contactScheduledata?.length !== 0 && <div className={`${styles.dealslistcontainer} ${styles.upcomingcontainer} maincontainer`} >
            
            {Array.isArray(contactScheduledata) && contactScheduledata?.length > 0 &&
                contactScheduledata?.map((data: any, index: number) => {
                    const meetingId = data?.meetingId ?? data?.VideoHistoryId ?? index;
                    return (
                        <div className={styles.card} key={meetingId}>
                            <div className={styles.top}>
                                <div className={styles.lefts}>
                                    <div className={styles.namewithbadge}>
                                        <h5>{data?.meeting_platform || '-'} - {data?.meeting_title || '-'}</h5>
                                    </div>
                                    <div className={styles.secondline}>
                                        <p>{data?.meeting_description || '-'}</p>
                                    </div>
                                </div>
                                <div className={styles.rights}>
                                    <div className={`${styles.scenerio} ${styles.red}`}>
                                        <MeetingTimer startTime={data?.schedule_start} />
                                    </div>
                                    <div className={styles.timer}><span><svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
                                        <path d="M6.66667 0C3 0 0 3 0 6.66667C0 10.3333 3 13.3333 6.66667 13.3333C10.3333 13.3333 13.3333 10.3333 13.3333 6.66667C13.3333 3 10.3333 0 6.66667 0ZM6.66667 12.3333C3.53333 12.3333 1 9.8 1 6.66667C1 3.53333 3.53333 1 6.66667 1C9.8 1 12.3333 3.53333 12.3333 6.66667C12.3333 9.8 9.8 12.3333 6.66667 12.3333Z" fill="#3F3F49" />
                                        <path d="M6.66667 1.93333C6.4 1.93333 6.13333 2.13333 6.13333 2.46667V5.86667C6.13333 6.06667 6 6.2 5.8 6.2H3.13333C2.86667 6.2 2.6 6.4 2.6 6.73333C2.6 7.06667 2.8 7.26667 3.13333 7.26667H5.8C6.53333 7.26667 7.13333 6.66667 7.13333 5.93333V2.4C7.2 2.13333 6.93333 1.93333 6.66667 1.93333Z" fill="#3F3F49" />
                                    </svg></span>{getScheduleTime({ ...data, channelType: "Video" })}</div>
                                </div>

                            </div>
                            <div className={styles.bottom}>
                                <div className={styles.lefts}>
                                    {/* <div style={{ display: "grid" }}><img src={mailicon} /></div> */}
                                    {/* <p><span className={styles.boldtxt}>Created by:</span>William</p> */}
                                    {/* <p><span className={styles.boldtxt}>Participants:</span> <div className="iconStack">
                                        <img src={mailicon} />
                                        <img src={mailicon} />
                                        <img src={mailicon} />
                                    </div></p> */}

                                </div>
                                <div className={styles.rights}>
                                    <div className={styles.btncontainer}>
                                        <button onClick={() => handleCopyMeetingLink(data?.meet_url, meetingId)} className={styles.whitebtn}>
                                         <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
                                                <g clip-path="url(#clip0_7635_21028)">
                                                <path d="M19.667 4.47334C18.5288 3.39324 16.957 3.21064 15.4292 3.17454C14.1851 3.14234 13.3042 3.14234 12.064 3.17454C10.5352 3.21064 8.96342 3.39234 7.82522 4.47334C7.35402 4.92014 7.06632 5.46164 6.88462 6.03044C5.84252 6.22774 5.01792 6.60684 4.38912 7.20374C3.13862 8.39024 3.10302 10.2174 3.07712 11.5523L3.07422 14.9644L3.07712 15.1119C3.10302 16.4459 3.13862 18.273 4.38912 19.4693C5.57662 20.5963 7.22162 20.7867 8.82072 20.8248C9.47452 20.8414 10.0326 20.8492 10.5902 20.8492C11.1478 20.8492 11.7064 20.8414 12.3583 20.8248C13.9589 20.7867 15.6039 20.5963 16.7914 19.4693C17.4974 18.7999 17.8021 17.9312 17.9461 17.1514C18.5758 16.96 19.1648 16.6648 19.6669 16.188C20.8632 15.0542 20.8988 13.3296 20.9223 12.1734V8.47324C20.8989 7.33264 20.8633 5.60814 19.667 4.47334ZM19.4099 8.06274C18.6534 8.05424 18.1072 8.02224 17.6767 7.89814C16.4565 7.56124 15.5356 6.43814 15.437 5.16184C15.4236 5.00024 15.4313 4.83834 15.4337 4.67674C16.8325 4.71214 17.9151 4.87794 18.6347 5.56124C19.2355 6.13154 19.3698 7.05804 19.4099 8.06274ZM15.7593 18.3815C14.9854 19.1159 13.8262 19.2897 12.3218 19.3249C11.0425 19.3581 10.1406 19.3581 8.85742 19.3249C7.35452 19.2897 6.19532 19.1149 5.42382 18.3835C4.62592 17.6208 4.60012 16.2722 4.57712 15.0837L4.57422 11.7312L4.57712 11.5818C4.60002 10.3933 4.62592 9.04664 5.42132 8.29184C5.73032 7.99884 6.12022 7.78844 6.60292 7.63824C6.57942 7.98764 6.57232 8.32524 6.56632 8.63454L6.56342 11.8796L6.56632 12.0202C6.59072 13.2946 6.62442 15.0407 7.82462 16.1882C8.96282 17.2683 10.5346 17.4509 12.0624 17.487C12.6845 17.5026 13.2152 17.5104 13.746 17.5104C14.2763 17.5104 14.8075 17.5026 15.4276 17.487C15.727 17.4799 16.0276 17.4641 16.3268 17.4401C16.1937 17.8339 16.01 18.1437 15.7593 18.3815ZM18.6348 15.1003C17.9073 15.7907 16.8125 15.9538 15.3911 15.987C14.1743 16.0173 13.3179 16.0173 12.0991 15.987C10.6792 15.9538 9.58452 15.7907 8.85942 15.1022C8.11232 14.3874 8.08792 13.1149 8.06592 11.9918L8.06352 8.80634L8.06592 8.66274C8.08792 7.54064 8.11232 6.26824 8.85792 5.56114C9.58452 4.87074 10.6792 4.70764 12.1006 4.67444C12.709 4.65884 13.2271 4.65104 13.7461 4.65104C13.7984 4.65104 13.8529 4.65204 13.9053 4.65224C13.9026 4.86364 13.8947 5.07604 13.9121 5.28384C14.061 7.21744 15.4106 8.86004 17.2612 9.37074C17.8579 9.54284 18.5225 9.58214 19.4223 9.59144V12.1578C19.3984 13.3375 19.3199 14.4509 18.6348 15.1003Z" fill="#23232B"/>
                                                </g>
                                                <defs>
                                                <clipPath id="clip0_7635_21028">
                                                <rect width="24" height="24" fill="white"/>
                                                </clipPath>
                                                </defs>
                                            </svg>
                                            {copiedMeetingId === meetingId ? 'Copied' : 'Copy link'}
                                        </button>
                                        <button onClick={() => handleStartMeeting(data?.meet_url)}>
                                            <svg xmlns="http://www.w3.org/2000/svg" width="19" height="19" viewBox="0 0 19 19" fill="none">
                                                <path d="M17.4748 8.2154C17.0606 8.2142 16.7238 8.5489 16.7227 8.9632C16.7173 10.7967 16.7119 12.6297 16.7065 14.463C16.7033 15.703 15.6964 16.7069 14.4566 16.7067H3.75C2.5076 16.7043 1.5022 15.6991 1.4999 14.4567V3.7501C1.5022 2.5077 2.5076 1.5023 3.75 1.4999H9.1033C9.5174 1.4999 9.8531 1.1642 9.8531 0.7501C9.8531 0.3358 9.5174 0.000299931 9.1033 0.000299931L3.75 9.98974e-05C1.6786 0.000599897 0.0005 1.6788 0 3.7501V14.4566C0.0004 16.5279 1.6786 18.2061 3.75 18.2064H14.4565C16.5235 18.2062 18.1999 16.5348 18.2063 14.4674C18.2117 12.6341 18.217 10.8007 18.2224 8.9676C18.2236 8.5533 17.8889 8.2166 17.4748 8.2154Z" fill="white"/>
                                                <path d="M11.4772 7.8062L16.7226 2.5606V5.293C16.7226 5.7071 17.0584 6.043 17.4726 6.043C17.8868 6.043 18.2226 5.7071 18.2226 5.293V1.75C18.2221 0.783 17.4394 0.0005 16.4726 0H12.9296C12.5154 0 12.1796 0.3357 12.1796 0.75C12.1796 1.1641 12.5154 1.5 12.9296 1.5L15.662 1.4998L10.4165 6.7454C10.1237 7.0384 10.1237 7.5132 10.4165 7.8062C10.7093 8.0989 11.1843 8.0989 11.4772 7.8062Z" fill="white"/>
                                            </svg>
                                            Start Meeting
                                        </button>
                                    </div>
                                </div>
                            </div>
                        </div>
                    )
                })
            }

            {/* <div className={styles.upcomingactivity}>
                <div className={styles.upcomingtitle}>Upcoming Activities</div>
                <div className={styles.activitylogcontain}> */}
                    {/* {activityLogs.map((item, index) => (
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
                    ))} */}
                    {/* <Nodata />

                </div>
            </div> */}
            {/* <div className={styles.commonbox}>
                <div className={styles.headerr}>
                    <div className={styles.heading}>Action items</div>
                </div>

                <div className={styles.cardwrapper}> */}
                    {/* <div className={`${styles.card} ${styles.mentionscard}`}>
                        
                        <div className={styles.top}>
                            <div className={styles.namewithmention}>
                                <div className={styles.imgwithname}><img src={mailicon} />Steve Harrington</div>
                                <span>@ 4:45</span>
                            </div>
                        </div>

                        <div className={styles.bottom}>
                            <label className={`content ${styles.checkboxwithcontent}`}>
                                <input type="checkbox" /> Spoke with the customer regarding request status. Confirmed the request is currently under review. Advised expected completion within 24–48 hours. No further action required at this time.
                            </label>
                        </div>
                        
                    </div> */}
                    {/* <Nodata />
                </div>

            </div> */}

        </div>}
        {!loader && contactScheduledata?.length === 0 &&<div style={{height: "calc(100vh - 367px)"}}> <Nodata /></div>}
        {/* {loader && (
            <div>
                
            </div>
        )} */}

    </>)
} export default Upcoming