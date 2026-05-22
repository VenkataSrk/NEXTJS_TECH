import styles from "./scss/marketing.module.scss";
import SetUpCommonEditor from "./commoneditor";
import { useSelector } from "react-redux";
import SkeletonBox from "./skeletonbox";

export default function MarketingInsights() {
    const insightData = useSelector((state: any) => state.marketingChatStore?.marketingInsight) ?? {};
    return (
        <>
            <div className={`${styles.marketmain} maincontainer`}>
                <div className={styles.marketmaininner}>
                    {
                        <div className={styles.topblockarea}>
                            <div className={styles.campaignblockmain}>

                                <div className={`${styles.marketblock} marketblockininsights`}>
                                    {insightData?.Executive_summary ?
                                        <div dangerouslySetInnerHTML={{ __html: insightData?.Executive_summary }} />
                                        :
                                        <div className={"skeletonTextGroup"}>
                                            <SkeletonBox width="80%" height="14px" />
                                            <SkeletonBox width="60%" height="14px" />
                                            <SkeletonBox width="90%" height="14px" />
                                            <SkeletonBox width="90%" height="14px" />
                                            <SkeletonBox width="90%" height="14px" />
                                        </div>
                                    }
                                    {/* <div className={styles.title}>Executive Summary - April 2024</div>
                            <p>This month shows strong growth across all channels with total revenue of $189,470, representing a 20% increase from March. Your ROAS of 4.2x indicates that for every dollar spent on marketing, you're generating $4.20 in revenue - an 18.5% improvement from last month.</p>

                            <div className={styles.subtitle}>Channel Performance Highlights:</div>
                            <p>Google Ads continues to be your top revenue driver with $71,000 generated, though Meta Ads shows the highest ROAS at 5.69x. Email campaigns deliver the best conversion rate at 1.8% with the lowest customer acquisition cost, making it highly efficient for nurturing existing leads.</p> */}
                                </div>

                                <div className={styles.marketblockone}>
                                    {(insightData?.metrics && Object.keys(insightData?.metrics)?.length > 0) ? Object.keys(insightData?.metrics)?.map((item: any) => {
                                        return (
                                            <div className={styles.marketblockoneinner}><span>{item?.charAt(0)?.toUpperCase() + item?.slice(1)?.replaceAll("_", " ")}</span>{insightData?.metrics[item]}</div>
                                        )
                                    }) :

                                        <>
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                            <SkeletonBox width='100%' height="94px" />
                                        </>

                                    }
                                    {/* <div className={styles.marketblockoneinner}><span>Total Clicks</span>$45234</div>
                            <div className={styles.marketblockoneinner}><span>Total Impressions</span>$45234</div>
                            <div className={styles.marketblockoneinner}><span>Total Conversations</span>$45234</div>
                            <div className={styles.marketblockoneinner}><span>Avg CTR</span>$45234</div>
                            <div className={styles.marketblockoneinner}><span>Avg CPC</span>$45234</div>
                            <div className={styles.marketblockoneinner}><span>ROAS</span>$45234</div>
                            <div className={styles.marketblockoneinner}><span>Conversion Rate</span>$45234</div> */}
                                </div>

                                <div className={`${styles.marketblock} marketblockininsights`}>
                                    {insightData?.strategic_recommamdation ?

                                        <div dangerouslySetInnerHTML={{ __html: insightData?.strategic_recommamdation }} />
                                        :
                                        <div className={"skeletonTextGroup"}>
                                            <SkeletonBox width="80%" height="14px" />
                                            <SkeletonBox width="60%" height="14px" />
                                            <SkeletonBox width="90%" height="14px" />
                                            <SkeletonBox width="90%" height="14px" />
                                            <SkeletonBox width="90%" height="14px" />
                                            <SkeletonBox width="90%" height="14px" />
                                        </div>
                                    }
                                    {/* <div className={styles.title}>Strategic Recommendations</div>
                            <div className={styles.subtitle}>Scale Meta Ads Investment:</div>
                            <p>With the highest ROAS (5.69x) and growing efficiency, consider increasing Meta Ads budget by 20-30% to capitalize on this high-performing channel.</p>

                            <div className={styles.subtitle}>Refresh Creative Assets:</div>
                            <p>The CTR decline suggests potential ad fatigue. Develop new creative variations and A/B test messaging to restore engagement levels.</p>

                            <div className={styles.subtitle}>Optimize Email Nurture Sequences:</div>
                            <p>Email shows best conversion efficiency. Expand automated nurture sequences to convert more leads at lower acquisition costs.</p>

                            <div className={styles.subtitle}>LinkedIn Lead Quality Analysis:</div>
                            <p>While LinkedIn has lower volume, evaluate if these leads have higher lifetime value or faster sales cycles to justify the higher CPL.</p> */}
                                </div>
                            </div>
                        </div>
                    }
                </div>
            </div>
            <div className={`${styles.editorbottom} maincontainer `}>
                <SetUpCommonEditor pageSource={"marketing"} />
            </div>
        </>
    );
}
