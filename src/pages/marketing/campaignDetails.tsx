import styles from "./scss/marketing.module.scss";
import SetUpCommonEditor from "./commoneditor";

export default function MarketingCampaignDetails() {

    return (
        <div className={styles.marketmain}>
            <div className={styles.marketmaininner}>

                <div className={styles.topblockarea}>
                    <div className={styles.campaigndetail}>
                        <div className={styles.campaigndetailleft}>
                            <div className={styles.title}>Andromeda Drive - Ent<span>Primary Goal: Boost Online Sales</span></div>
                            <div className={styles.tab}>
                                <div className={styles.tabinner}>Email</div>
                                <div className={styles.tabinner}>SMS</div>
                            </div>
                        </div>
                        <div className={styles.campaigndetailright}>
                            EMAIL/SMS
                        </div>
                    </div>
                    <div className={styles.marketblock}>
                        <div className={styles.title}>Campaign progress</div>
                        <div className={styles.campaignrate}>
                            80%
                            <div className={styles.line}>
                                <div className={styles.innerline} style={{ width: "70%" }}></div>
                            </div>
                        </div>
                        <p>Goal achieved so far</p>
                    </div>

                    <div className={styles.marketblockone}>
                        <div className={styles.marketblockoneinner}><span>Open Rate</span>$45234<span>Subject line effectiveness</span></div>
                        <div className={styles.marketblockoneinner}><span>Click-Through Rate</span>$45234<span>Content relevance</span></div>
                        <div className={styles.marketblockoneinner}><span>Conversion Rate</span>$45234<span>Goal completion</span></div>
                        <div className={styles.marketblockoneinner}><span>Bounce Rate</span>$45234<span>List quality</span></div>
                        <div className={styles.marketblockoneinner}><span>Deliverability Rate</span>$45234<span>Inbox reach</span></div>
                        <div className={styles.marketblockoneinner}><span>Unsubscribe Rate</span>$45234<span>Content alignment</span></div>
                        <div className={styles.marketblockoneinner}><span>Spam Complaint Rate</span>$45234<span>List health</span></div>
                    </div>

                    <div className={styles.marketblock}>
                        <div className={styles.title}>Key Insights</div>
                        <p><strong>Open Rate</strong> (22.4%) is above industry average, indicating effective subject line and sender reputation.</p>
                        <p><strong>CTR</strong> (3.8%) shows strong content engagement - your offer and CTAs are resonating with the audience.</p>
                        <p><strong>Conversion Rate</strong> (2.1%) is solid and indicates good landing page alignment.</p>
                        <p><strong>List Health</strong> is excellent with low bounce (1.2%), unsubscribe (0.3%), and spam complaint (0.05%) rates.</p>
                    </div>

                    <div className={styles.marketblock}>
                        <div className={styles.title}>AI Recommendations</div>
                        <p>✓ Continue current subject line strategy - open rates are trending positively</p>
                        <p>✓ A/B test CTA placement to push CTR above 4%</p>
                        <p>✓ Analyze converting segments for better targeting</p>
                        <p>✓ Maintain send frequency - unsubscribe rate is healthy</p>
                    </div>
                </div>

                <div className={styles.editorbottom}>
                    <SetUpCommonEditor />
                </div>
            </div>
        </div>
    );
}
