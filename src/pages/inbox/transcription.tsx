import react, { use, useRef, useState } from "react"
import styles from "./scss/transcription.module.scss"
import purpleicn from "../../assets/images/inbox/callpurple.svg"
import contactwindow from "../../assets/images/inbox/contactwindow.svg";
import Drawer from "../component/drawer/drawer"
import { useSelector } from "react-redux"
import { useNavigate, useSearchParams } from "react-router-dom"
import { useDispatcher } from "../../store/redux-store/dispatch"
import { findDifferenceBetweenTimeSlots, formatDateTimeLable, getInitials } from "../../base/utils"
import moment from "moment"
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage"
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys"



function Transcription({ offRightSpace, setOffRightSpace }: any) {

    const [showNotes, setShowNotes] = useState(false)
    const [showTransript, setShowTransript] = useState(false)
    const navigate = useNavigate();
    const dispatch = useDispatcher();
    const [params] = useSearchParams();
    const CustomerId = params.get('CustomerId');
    const { perticularInboxData, perticularInboxContactData, customerNotes } = useSelector((state: any) => state.inboxChatStore);
    const { contacts } = perticularInboxContactData ?? {};
    const [showMore, setShowMore] = useState({
        0: true
    })
    const [customerTranscript, setCustomerTrascript] = useState<any>([]);
    const [openMenuId, setOpenMenuId] = useState<any>(null);
    const loginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS) || {};

    const handleShowMore = (index: number) => {
        setShowMore((prev: any) => ({ ...prev, [index]: !prev[index] }))
    }

    const viewTranscript = (message: any) => {
        setShowTransript(true);
        const transcript = Array.isArray(message) && message?.length > 0 ? message : []
        setCustomerTrascript(transcript);
    }

    const formatDate = (dateString: string) => {
        const date = new Date(dateString);

        return date.toLocaleString("en-US", {
            weekday: "long",
            month: "short",
            day: "numeric",
            year: "numeric",
            hour: "numeric",
            minute: "2-digit",
            hour12: true,
        }).replace(",", " -");
    };

    return (<>
        <div className={`${styles.calltranferbody} ${styles.mail}`}>

            <div className={` ${styles.header}`} style={{ justifyContent: "space-between" }}>
                <div style={{ display: "flex", alignItems: "center", gap: 8 }}>
                    <span style={{ display: "grid" }}><img src={purpleicn} /></span>
                    <span className={`content ${styles.subcontent}`}>Inbound - {contacts?.first_name || "New Contact"} {contacts?.last_name || ''}</span>
                </div>
                <span style={{ cursor: "pointer", display: "grid" }}>
                    {CustomerId && !offRightSpace && (
                        <img
                            src={contactwindow}
                            onClick={() => setOffRightSpace?.((prev: boolean) => !prev)}
                            style={{ transform: "rotate(180deg)" }}
                        />
                    )}
                </span>
            </div>

            {Array.isArray(perticularInboxData) && perticularInboxData.map((row: any, index: number) => {

                return (
                    <div className={styles.transcriptionscroller}>
                        <div className={styles.loginid}>{formatDateTimeLable(row.createdAt)}</div>

                        <div className={styles.greyboxaccordin}>
                            <div className={styles.accordintitlwwithchevron}>
                                <span onClick={() => handleShowMore(index)}>
                                    <svg
                                        style={{ transform: showMore[index] ? "rotate(180deg)" : "rotate(0deg)" }}
                                        xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
                                        <path d="M10.5809 5.8065C10.3885 5.8065 10.1961 5.7333 10.0497 5.5858L6.19621 1.7206C6.05461 1.579 5.86611 1.5009 5.66541 1.4999C5.46471 1.4999 5.27581 1.579 5.13371 1.7206L1.28121 5.5858C0.989214 5.8797 0.513614 5.8797 0.220714 5.5878C-0.0727862 5.2948 -0.0736861 4.8202 0.218714 4.5273L4.07171 0.662101C4.49751 0.235301 5.06341 0 5.66551 0C6.26761 0.001 6.83351 0.235401 7.25881 0.662101L11.1123 4.5273C11.4048 4.8203 11.4038 5.2949 11.1103 5.5878C10.9643 5.7333 10.7723 5.8065 10.5809 5.8065Z" fill="#757676" />
                                    </svg>
                                </span>
                                Subject
                            </div>

                            <div className={styles.headeravatorsec}>
                                <div className={styles.list}>
                                    <div className={styles.lefts}>
                                        <div className={styles.avatorcontainer}>
                                            {getInitials(contacts?.first_name, contacts?.last_name)}
                                        </div>

                                        <div className={styles.info}>
                                            <div className={styles.name}>{contacts?.first_name || "New Contact"} {contacts?.last_name || ''}</div>
                                            <div className={styles.phn}>{row?.customerNumber || ''}</div>
                                        </div>
                                    </div>
                                    <div className={styles.rights}>
                                        {row?.sessionStartTime ? findDifferenceBetweenTimeSlots(row?.sessionStartTime, row?.sessionEndTime) : ''}
                                    </div>
                                </div>
                            </div>

                            {
                                showMore[index] &&
                                <>

                                    <div className={styles.summary}>
                                        <div className={styles.quicksummary}>Summary of this audio</div>
                                        {!!row?.RecordingUrl && <audio src={row?.RecordingUrl} controls />}
                                        <div className={`${styles.quicksummary} ${styles.quicksummaryp}`}>
                                            {row?.summary || ''}
                                        </div>
                                    </div>
                                    {/* <div className={styles.content}>
                                     <div className={styles.videoSection}>
                                        <video
                                            src="https://www.w3schools.com/html/mov_bbb.mp4"
                                            controls
                                            preload="auto"
                                        />
                                    </div>
                                </div> */}
                                    <div className={styles.btncontainer}>

                                        {(Array.isArray(customerNotes) && customerNotes?.length > 0) && <button onClick={() => setShowNotes(true)}>View Notes <span className={styles.count}>{customerNotes?.length}</span></button>}
                                        {(Array.isArray(row?.Message) && row?.Message?.length > 0) && <button onClick={() => viewTranscript(row?.Message)}>View Transcriptions</button>}
                                    </div>

                                </>
                            }
                        </div>

                        {/* <div className={styles.callreviewblock}>
                        <div className={styles.callreviewblockmain}>
                            <div className={styles.callreviewleft}>
                            </div>
                            <div>
                                <audio></audio>
                            </div>
                        </div>
                    </div> */}
                    </div>
                )
            })}

        </div>

        {showTransript && (
            <Drawer
                title="Transcript"
                onClose={() => {
                    setShowTransript(false);
                    setCustomerTrascript([]);
                }}
            >
                <div className={styles.chatcontainer}>
                    {customerTranscript?.map((data: any, idx: number) => {
                        const isCustomer = !!data?.isCustomer;
                        const name = isCustomer ? data?.sender?.["Full Name"]?.trim() || "New Customer" : data?.sender?.Name?.trim() || "Agent";
                        const initials = name.split(" ").map((n: string) => n[0]).join("");
                        const text = (data?.message || "").replace(/<[^>]+>/g, "") || "-";

                        const prevData = customerTranscript[idx - 1];
                        const showHeader = !prevData || (prevData?.isCustomer !== isCustomer);

                        return (
                            <div
                                key={data?.messageID ?? data?.id ?? idx}
                                className={isCustomer ? styles.rights : styles.lefts}
                            >
                                {showHeader && (
                                    <div className={styles.namewithtime}>
                                        <div className={styles.namewithavator}>
                                            <div className={styles.avator}>{initials}</div>
                                            {name}
                                        </div>
                                        <span>{moment(data?.sentAt ?? data?.sendAt).format("hh:mm a")}</span>
                                    </div>
                                )}
                                {isCustomer ? (
                                    <div className={styles.bubble}>{text}</div>
                                ) : (
                                    <div
                                        className={styles.bubble}
                                        dangerouslySetInnerHTML={{ __html: data?.message || "-" }}
                                    />
                                )}
                            </div>
                        );
                    })}
                </div>
            </Drawer>
        )}
        {showNotes &&
            <Drawer title={"Notes"} onClose={() => setShowNotes(false)}>
                <div className={styles.notescontainer}>
                    {customerNotes?.map((notes: any) => {
                        return (
                            <div className={styles.notes} key={notes?.nid}>
                                <div className={styles.top}>
                                    <div className={styles.lefts}>
                                        {/* <div className={styles.title}>Regarding request status</div> */}
                                        <div className={styles.duration}>{formatDate(notes?.createdAt)}</div>
                                    </div>
                                    <div style={{ position: "relative" }} className={styles.editbtn}>
                                        {/* <svg
                                            onClick={() =>
                                                setOpenMenuId((prev : any) => (prev === notes?.nid ? null : notes?.nid))
                                            }
                                            xmlns="http://www.w3.org/2000/svg"
                                            width="18"
                                            height="5"
                                            viewBox="0 0 18 5"
                                            fill="none"
                                        >
                                            <path fill-rule="evenodd" clip-rule="evenodd" d="M13.461 2.2435C13.461 3.4774 14.4706 4.487 15.7045 4.487C16.9384 4.487 17.948 3.4774 17.948 2.2435C17.948 1.0096 16.9384 0 15.7045 0C14.4706 0 13.461 1.0096 13.461 2.2435ZM11.2175 2.2435C11.2175 1.0096 10.2079 0 8.974 0C7.7401 0 6.7305 1.0096 6.7305 2.2435C6.7305 3.4774 7.7401 4.487 8.974 4.487C10.2079 4.487 11.2175 3.4774 11.2175 2.2435ZM2.2435 0C3.4774 0 4.487 1.0096 4.487 2.2435C4.487 3.4774 3.4774 4.487 2.2435 4.487C1.0096 4.487 0 3.4774 0 2.2435C0 1.0096 1.0096 0 2.2435 0Z" fill="#23232B" />
                                        </svg> */}
                                        {openMenuId === notes?.nid &&
                                            <div className={"dropdowncompose right"}>
                                                <div className="list"> Edit note</div>
                                                <div className="list"> Delete note</div>


                                            </div>}
                                    </div>
                                </div>
                                <div className={styles.content} dangerouslySetInnerHTML={{ __html: notes?.notes || '' }} />
                                <div className={styles.bottom}>
                                    {/* <span className={styles.bold}>Created by:</span> William */}
                                    <span className={styles.bold}>Created by:</span>  {`${loginCrmUserDetails?.first_name || ""} ${loginCrmUserDetails?.last_name || ""}`}
                                </div>
                            </div>
                        )
                    })}

                </div>
            </Drawer>}
    </>)
} export default Transcription
