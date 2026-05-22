"use-client"
import { useEffect, useMemo, useRef, useState } from 'react';
import styles from './scss/companyinboundcallview.module.scss'
import Arrow from '../../assets/images/contacts/chevronbreadcrumb.svg'
import Plancalendar from '../../assets/images/projectinsights/plancalendar.svg'
import { useNavigate } from 'react-router-dom';
import { useSelector } from 'react-redux';
import { getInitials } from '../../base/utils';

const Companyinboundcallview = () => {
    const navigate = useNavigate();
    const { inboundCallViewData } = useSelector((state: any) => state.inboxChatStore);
    const transcript = Array.isArray(inboundCallViewData?.Message) ? inboundCallViewData?.Message : [];
    const recordingUrl = inboundCallViewData?.RecordingUrl || '';
    const summary = inboundCallViewData?.summary || '-';
    const customerName = inboundCallViewData?.customerName || 'New Contact';
    const [autoScroll, setAutoScroll] = useState(true);
    const [activeIndex, setActiveIndex] = useState<number>(-1);
    const audioRef = useRef<HTMLAudioElement | null>(null);
    const lineRefs = useRef<(HTMLDivElement | null)[]>([]);

    const parseTranscriptSeconds = (tm: string) => {
        if (!tm || typeof tm !== 'string') return NaN;
        const parts = tm.split(':').map((part) => Number(part));
        if (parts.some((num) => Number.isNaN(num))) return NaN;
        if (parts.length === 3) return (parts[0] * 3600) + (parts[1] * 60) + parts[2];
        if (parts.length === 2) return (parts[0] * 60) + parts[1];
        if (parts.length === 1) return parts[0];
        return NaN;
    };

    const sessionStartTs = useMemo(() => {
        const start = inboundCallViewData?.sessionStartTime;
        if (!start) return NaN;
        const time = new Date(start).getTime();
        return Number.isNaN(time) ? NaN : Math.floor(time / 1000);
    }, [inboundCallViewData?.sessionStartTime]);

    const getTranscriptSecond = (item: any, index: number) => {
        const parsed = parseTranscriptSeconds(item?.transcriptTm);
        if (!Number.isNaN(parsed)) return parsed;

        if (!Number.isNaN(sessionStartTs) && (item?.sentAt || item?.sendAt)) {
            const sentTs = Math.floor(new Date(item?.sentAt ?? item?.sendAt).getTime() / 1000);
            if (!Number.isNaN(sentTs)) return Math.max(0, sentTs - sessionStartTs);
        }

        return index;
    };

    const transcriptSeconds = useMemo(() => {
        return transcript.map((item: any, index: number) => getTranscriptSecond(item, index));
    }, [transcript, sessionStartTs]);

    const formatTranscriptTime = (index: number) => {
        const totalSec = transcriptSeconds[index];
        if (Number.isNaN(totalSec)) return '00:00';
        const minutes = Math.floor(totalSec / 60);
        const seconds = totalSec % 60;
        return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
    };

    const getCallDisplayDate = () => {
        if (!inboundCallViewData?.createdAt) return '-';
        const date = new Date(inboundCallViewData.createdAt);
        if (Number.isNaN(date.getTime())) return '-';
        return date.toLocaleString([], { day: '2-digit', month: 'short', hour: '2-digit', minute: '2-digit' });
    };

    useEffect(() => {
        const audio = audioRef.current;
        if (!audio || transcriptSeconds.length === 0) return;

        const onTimeUpdate = () => {
            const current = audio.currentTime;
            let nextActive = -1;

            for (let i = 0; i < transcriptSeconds.length; i++) {
                const start = transcriptSeconds[i];
                const end = transcriptSeconds[i + 1] ?? Number.POSITIVE_INFINITY;
                if (current >= start && current < end) {
                    nextActive = i;
                    break;
                }
            }

            setActiveIndex(nextActive);
            if (autoScroll && nextActive >= 0) {
                lineRefs.current[nextActive]?.scrollIntoView({ behavior: 'smooth', block: 'center' });
            }
        };

        audio.addEventListener('timeupdate', onTimeUpdate);
        return () => audio.removeEventListener('timeupdate', onTimeUpdate);
    }, [transcriptSeconds, autoScroll]);

    const seekToTranscript = (index: number) => {
        const audio = audioRef.current;
        if (!audio) return;
        const second = transcriptSeconds[index];
        if (Number.isNaN(second)) return;
        audio.currentTime = second;
        setActiveIndex(index);
    };

    return (
        <>
            <div className={styles.companynameheader}>
                <div className={styles.topbarheader}>
                    <div className={styles.topbarleft} onClick={() => navigate(-1)} style={{ cursor: 'pointer' }}>
                        <img src={Arrow} alt="Arrow" />
                    </div>
                    <div className={styles.topbarright}>
                        <div className={styles.titleblock}>Inbound Call</div>
                        <div className={styles.subtitle}>{customerName}</div>
                    </div>
                </div>
                <div className={styles.callreviewblock}>
                    {/* <div className={styles.negotiationlist}>
                        <div className={styles.negotiationlisttitle}>Summary</div>
                        <div className={styles.negotiationlistsubject}>
                        <div className={styles.multilingual}>
                            <div className={styles.multilingualcontainer}>
                                <div><img src={Plancalendar} alt="Plancalendar" /></div>
                                <div className={styles.plansuggestion}>
                                    <div className={styles.plansuggestionlist}>Plan negotiation</div>
                                    <div className={styles.plansuggestiontitle}>Inbound call log</div>
                                </div>
                            </div>
                            <div className={styles.plancalendarview}>{getCallDisplayDate()}</div>
                        </div>
                        <div className={styles.plannegotiation}>
                            {summary}
                        </div>
                        </div>
                    </div> */}
                    <div className={styles.callreviewblockmain}>
                        {/* LEFT SIDE */}
                        <div className={styles.callreviewleft}>

                            {/* Summary */}
                            <div className={styles.summary}>
                                <div className={styles.quicksummary}>Summary of this audio</div>
                                <div className={`${styles.quicksummary} ${styles.quicksummaryp}`}>
                                    {summary}
                                </div>
                            </div>

                            <div className={styles.content}>
                                <div className={styles.videoSection}>
                                    {recordingUrl ? (
                                        <>
                                            <audio ref={audioRef} src={recordingUrl} controls preload="metadata" />
                                            <div className={styles.controls}>
                                                <label className={styles.autoscrollToggle}>
                                                    <input
                                                       className="custom-checkbox"

                                                        type="checkbox"
                                                        checked={autoScroll}
                                                        onChange={(event) => setAutoScroll(event.target.checked)}
                                                    />
                                                    Auto-scroll transcript
                                                </label>
                                            </div>
                                        </>
                                    ) : (
                                        <div className={styles.plansubtitle}>Recording not available</div>
                                    )}
                                </div>
                            </div>
                        </div>

                        {/* RIGHT SIDE */}
                        <div className={styles.callreviewright}>
                            <div className={styles.transcripty}>Transcript</div>

                            <div className={styles.list}>
                                {transcript.length > 0 ? transcript.map((item: any, index: number) => {
                                    const senderName = item?.isCustomer ? (item?.sender?.['Full Name'] || 'Customer') : (item?.sender?.Name || 'Agent');
                                    return (
                                        <div
                                            className={`${styles.item} ${activeIndex === index ? styles.activeItem : ''}`}
                                            key={item?.messageID ?? item?.id ?? index}
                                            ref={(el) => { lineRefs.current[index] = el; }}
                                            onClick={() => seekToTranscript(index)}
                                        >
                                            <div className={`${styles.meta} ${styles.metaavatar}`}>
                                                <span>{formatTranscriptTime(index)}</span>
                                                <div className={styles.hourwellse}>
                                                    <div className={styles.metaprofi}>
                                                        <div className={styles.spanvatar}>
                                                            {getInitials(senderName)}
                                                        </div>
                                                        <div className={styles.msgagent}>
                                                            <div className={styles.agentheader}>{senderName}</div>
                                                            {item?.isCustomer ? (
                                                                <div className={styles.plansubtitle}>{(item?.message || '').replace(/<[^>]+>/g, '') || '-'}</div>
                                                            ) : (
                                                                <div className={styles.plansubtitle} dangerouslySetInnerHTML={{ __html: item?.message || '-' }} />
                                                            )}
                                                        </div>
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    );
                                }) : (
                                    <div className={styles.item}>
                                        <div className={styles.plansubtitle}>Transcript not available</div>
                                    </div>
                                )}

                            </div>
                        </div>
                    </div>

                </div>

            </div>
        </>
    )
}
export default Companyinboundcallview;
