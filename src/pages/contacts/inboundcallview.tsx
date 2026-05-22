"use-client"
import styles from './scss/inboundcallview.module.scss'
import Arrow from '../../assets/images/contacts/chevronbreadcrumb.svg'
const Inboundcallview = () => {
    return (
        <>
            <div className={styles.companynameheader}>
                <div className={styles.topbarheader}>
                    <div className={styles.topbarleft}>
                        <img src={Arrow} alt="Arrow" />
                    </div>
                    <div className={styles.topbarright}>
                        <div className={styles.titleblock}>Inbound Call</div>
                        <div className={styles.subtitle}>Interaction Log microcopy</div>
                    </div>
                </div>
                <div className={styles.callreviewblock}>

                    {/* LEFT SIDE */}
                    <div className={styles.callreviewleft}>

                        {/* Summary */}
                        <div className={styles.summary}>
                            <div className={styles.quicksummary}>Summary of this audio</div>
                            <div className={`${styles.quicksummary} ${styles.quicksummaryp}`}>
                                Customer reported an issue with their recent order. The order status page shows an error, and they are unsure about the current status. Requested assistance.
                            </div>
                        </div>

                        <div className={styles.content}>
                            {/* Video Section */}
                            <div className={styles.videoSection}>
                                <video
                                    src="https://www.w3schools.com/html/mov_bbb.mp4"
                                    controls
                                    preload="auto"
                                />
                            </div>
                        </div>
                    </div>

                    {/* RIGHT SIDE */}
                    <div className={styles.callreviewright}>
                        <div className={styles.transcripty}>Transcript</div>

                        <div className={styles.list}>
                            <div className={styles.item}>
                                <div className={`${styles.meta} ${styles.metaavatar}`}>
                                    <span>00:12</span>

                                    <div className={styles.hourwellse}>
                                        <div className={styles.metaprofi}>
                                            {/* <div className={styles.spanvatar}>W</div> */}

                                            <div className={styles.msgagent}>
                                                {/* <div className={styles.agentheader}>William</div> */}
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
                                            {/* <div className={styles.spanvatar}>A</div> */}

                                            <div className={styles.msgagent}>
                                                {/* <div className={styles.agentheader}>Agent (Amelia)</div> */}
                                                <div className={styles.plansubtitle}>Sure William, I’ll walk you through the available plans.</div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>

                            {/* Transcript item 3 */}
                            <div className={styles.item}>
                                <div className={`${styles.meta} ${styles.metaavatar}`}>
                                    <span>00:55</span>

                                    <div className={styles.hourwellse}>
                                        <div className={styles.metaprofi}>
                                            {/* <div className={styles.spanvatar}>B</div> */}

                                            <div className={styles.msgagent}>
                                                {/* <div className={styles.agentheader}>Bot</div> */}
                                                <div className={styles.plansubtitle}>Would you like me to send the pricing details to your email?</div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>

                        </div>
                    </div>

                </div>

            </div>
        </>
    )
}
export default Inboundcallview;