import styles from "./scss/callwidget.module.scss";
import callsource from "../../assets/images/callwidget/calliconwidget.svg";
import callend from "../../assets/images/callwidget/callend.svg";
import callicon1 from "../../assets/images/callwidget/callicon1.svg";
import callicon1active from "../../assets/images/callwidget/callicon1active.svg";
import callicon2 from "../../assets/images/callwidget/callicon2.svg";
import callicon2active from "../../assets/images/callwidget/callicon2active.svg";
import callicon3 from "../../assets/images/callwidget/callicon3.svg";
import callicon3active from "../../assets/images/callwidget/callicon3active.svg";
import callicon4 from "../../assets/images/callwidget/callicon4.svg";
import callicon4active from "../../assets/images/callwidget/callicon4active.svg";
import callicon5 from "../../assets/images/callwidget/callicon5.svg";
import callicon5active from "../../assets/images/callwidget/callicon5active.svg";
import CallWidgetEditor from "./callwidgeteditor";
import { MouseEvent as ReactMouseEvent, useEffect, useMemo, useRef, useState } from "react";
import { useSelector } from "react-redux";
import Agenttransfer from "./agenttransfer/agenttransfer";
import { useNavigate } from "react-router-dom";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { CallService } from "../../base/service/call_service";


export default function CallWidgets() {
    const dispatch = useDispatcher();
    const navigate = useNavigate();
    const { incomingCallDetail, callDetails, isCalling, currentCallDuration, dialedNumber }: any = useSelector((state: any) => state.callPopUpDetails);
    const [notes, setNotes] = useState(false);
    const [isRecording, setIsRecording] = useState(false);
    const [isSpeakerOn, setIsSpeakerOn] = useState(true);
    const [isDragging, setIsDragging] = useState(false);
    const [position, setPosition] = useState(() => ({
        x: Math.max(window.innerWidth - 382, 12),
        y: 72,
    }));
    const dragStateRef = useRef({ dragging: false, offsetX: 0, offsetY: 0 });
    const { EndCallfunction, CallMuteFunction, CallUnMuteFunction, CallHoldFunction, CallUnHoldFunction } = useMemo(() => CallService(dispatch, navigate), [dispatch, navigate]);
    const activeCall = incomingCallDetail && Object.keys(incomingCallDetail).length > 0 ? incomingCallDetail : Array.isArray(callDetails) && callDetails.length > 0 ? callDetails[callDetails.length - 1] : null;
    const fullName = `${activeCall?.contactDetails?.first_name ?? ""} ${activeCall?.contactDetails?.last_name ?? ""}`.trim();
    const hasCustomerName = Boolean(activeCall?.fromCallerName || fullName);
    const callerName = activeCall?.fromCallerName || fullName || activeCall?.from || "New Customer";
    const statusValue = String(activeCall?.callStatus ?? "").toLowerCase();
    const callStatusText = statusValue === "accepted" || statusValue === "connected" ? "Connected" : statusValue === "ringing" ? "Ringing" : statusValue === "dialing" ? "Dialing" : "Connecting...";
    const isConnected = statusValue === "accepted" || statusValue === "connected";
    const callDuration = isConnected ? activeCall?.callduration || currentCallDuration || "00:00:00" : "00:00:00";
    const callTag = hasCustomerName ? "Existing Customer" : "New Customer";

    const noteclick = () => {
        setNotes((prev) => !prev);
    };
    const endCallClick = () => {
        const callToEnd =
            activeCall?.useragent
                ? activeCall
                : [incomingCallDetail, ...(Array.isArray(callDetails) ? callDetails : [])].find((item: any) => item?.useragent);
        if (!callToEnd) return;
        EndCallfunction(callToEnd);
    };
    const muteToggleClick = () => {
        if (!activeCall?.useragent) return;
        if (activeCall?.muteState) {
            CallUnMuteFunction(activeCall);
            return;
        }
        CallMuteFunction(activeCall);
    };
    const holdToggleClick = () => {
        if (!activeCall?.useragent) return;
        if (activeCall?.holdState) {
            CallUnHoldFunction(activeCall);
            return;
        }
        CallHoldFunction(activeCall);
    };
    const recordingToggleClick = () => {
        if (!isConnected) return;
        setIsRecording((prev) => !prev);
    };
    const speakerToggleClick = () => {
        if (!activeCall?.useragent) return;
        const nextState = !isSpeakerOn;
        setIsSpeakerOn(nextState);
        const remoteMedia = document.getElementById("remoteVideo") as HTMLMediaElement | null;
        if (remoteMedia) {
            remoteMedia.muted = !nextState;
            remoteMedia.volume = nextState ? 1 : 0;
        }
    };
    const clampPosition = (x: number, y: number) => {
        const width = 370;
        const height = 220;
        const maxX = Math.max(12, window.innerWidth - width - 12);
        const maxY = Math.max(12, window.innerHeight - height - 12);
        return {
            x: Math.min(Math.max(12, x), maxX),
            y: Math.min(Math.max(12, y), maxY),
        };
    };
    const onDragStart = (e: ReactMouseEvent<HTMLDivElement>) => {
        if (e.button !== 0) return;
        dragStateRef.current.dragging = true;
        dragStateRef.current.offsetX = e.clientX - position.x;
        dragStateRef.current.offsetY = e.clientY - position.y;
        setIsDragging(true);
    };

    useEffect(() => {
        const onMove = (e: MouseEvent) => {
            if (!dragStateRef.current.dragging) return;
            const nextX = e.clientX - dragStateRef.current.offsetX;
            const nextY = e.clientY - dragStateRef.current.offsetY;
            setPosition(clampPosition(nextX, nextY));
        };
        const onUp = () => {
            if (!dragStateRef.current.dragging) return;
            dragStateRef.current.dragging = false;
            setIsDragging(false);
        };
        window.addEventListener("mousemove", onMove);
        window.addEventListener("mouseup", onUp);
        return () => {
            window.removeEventListener("mousemove", onMove);
            window.removeEventListener("mouseup", onUp);
        };
    }, []);

    return (
        <div
            className={`${styles.maincall} ${isDragging ? styles.dragging : ""}`}
            style={{ left: `${position.x}px`, top: `${position.y}px`, right: "auto" }}
        >
            <div className={styles.maincallinner}>
                <div className={styles.callblocktopmain} onMouseDown={onDragStart}>
                    <div className={styles.callblocktop}>
                        {/* <div className={styles.callblocktopleftcontainer}> */}
                        <div className={styles.callblocktopleft}>
                            <img src={callsource} alt="" />{callerName}
                        </div>
                            {/* {!!dialedNumber && <div className={styles.callblocktopleft}>{dialedNumber || ''}</div>}
                        </div> */}
                        <div className={styles.callblocktopright}>
                            {!isConnected && callStatusText}
                            {/* <div className={styles.onhold}>On Hold</div> */}
                            {isConnected && <div className={styles.callcount}><span></span>{callDuration}</div>}
                            <img src={callend} alt="" onClick={endCallClick} style={{ cursor: "pointer" }} />
                        </div>
                    </div>
                    <div className={styles.callblockbottom}>
                        <div className={styles.callblockbottomtag}>{callTag}</div>
                    </div>
                </div>
                <div className={styles.callblockbottommain}>
                    <div className={styles.callicon}>
                        <div className={`${styles.calliconinner} ${activeCall?.muteState ? styles.active : ""}`} onClick={muteToggleClick}>
                            <img src={activeCall?.muteState ? callicon1active : callicon1} alt="" style={{ cursor: "pointer" }} />
                        </div>
                        <div className={`${styles.calliconinner} ${activeCall?.holdState ? styles.active : ""}`} onClick={holdToggleClick}>
                            <img src={activeCall?.holdState ? callicon2active : callicon2} alt="" style={{ cursor: "pointer" }} />
                        </div>
                        <div className={`${styles.calliconinner} ${isRecording ? styles.active : ""}`} onClick={recordingToggleClick}>
                            <img src={isRecording ? callicon3active : callicon3} alt="" style={{ cursor: isConnected ? "pointer" : "not-allowed" }} />
                        </div>
                        <div className={`${styles.calliconinner} ${notes ? styles.active : ""}`} onClick={noteclick}>
                            <img src={notes ? callicon4active : callicon4} alt="" />
                        </div>
                        <div className={`${styles.calliconinner} ${isSpeakerOn ? styles.active : ""}`} onClick={speakerToggleClick}>
                            <img src={isSpeakerOn ? callicon5active : callicon5} alt="" style={{ cursor: "pointer" }} />
                        </div>
                    </div>
                    {notes && (
                        <div className={styles.calleditor}>
                            <CallWidgetEditor />
                        </div>
                    )}
                </div>
            </div>
        </div>
    );
}
