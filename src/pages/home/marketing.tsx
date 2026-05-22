import React from "react";
import { useSelector } from "react-redux";
import Styles from "./scss/marketing.module.scss";
import FunnelChart from "./charts/funnelchart";
import DonutChart from "./donutchart";
import SkeletonBox from "../marketing/skeletonbox";
import Trade from '../../assets/images/home/Icon.svg'
import TimeAgo from "./timeAgo";

type MarketingMetric = {
    Label?: string;
    Description?: string;
    "Sub-heading"?: string;
    "Avg per Campaign"?: number;
    "Overall Conversion"?: string;
};

type MarketingChartItem = {
    name: string;
    value: number;
};

type MarketingChart = {
    chartname?: string;
    data?: MarketingChartItem[];
};

const Marketing = () => {
    const marketingData: MarketingMetric[] = useSelector((state: any) => state.homeStore?.homeMarketing) ?? [];
    const loading: boolean = useSelector((state: any) => state.homeStore?.homeMarketingLoader);
    const reachChartData: MarketingChartItem[] = (marketingData?.[6] as MarketingChart)?.data || [];
    const funnelData: MarketingChartItem[] = (marketingData?.[7] as MarketingChart)?.data || [];

    const metricCards = marketingData?.slice(0, 6) || [];
    const aiInsights: any = marketingData?.[8] ?? {};
    const donutColors = ["#1A73E8", "#F4B400", "#DB4437", "#34A853", "#9AA0A6"];

    const getMetricSubtitle = (item: MarketingMetric) => {
        return item?.["Sub-heading"] || item?.["Overall Conversion"] || "-";
    };

    if (loading) {
        return (
            <div className={Styles.marketingleads}>
                <div className={Styles.saleblockcontainer}>
                    <div className={Styles.insightscard}>
                        <SkeletonBox width="120px" height="18px" />
                        <SkeletonBox width="140px" height="14px" />
                    </div>
                    <SkeletonBox width="100%" height="16px" />
                    <SkeletonBox width="85%" height="16px" />
                </div>

                <div className={Styles.totalsaleblock}>
                    {Array.from({ length: 6 }).map((_, index) => (
                        <div className={Styles.revenuecontainer} key={`marketing-card-skeleton-${index}`}>
                            <SkeletonBox width="150px" height="18px" />
                            <SkeletonBox width="80%" height="22px" />
                            <SkeletonBox width="100%" height="14px" />
                            <SkeletonBox width="70%" height="14px" />
                        </div>
                    ))}
                </div>

                <div className={Styles.funnelcontainer}>
                    <div className={Styles.leadconversation}>
                        <SkeletonBox width="220px" height="20px" />
                    </div>
                    <div className={Styles.convolist}>
                        <SkeletonBox width="320px" height="240px" borderRadius="16px" />
                        <div className={Styles.funneldatalist}>
                            {Array.from({ length: 5 }).map((_, index) => (
                                <div className={Styles.datahistory} key={`funnel-legend-skeleton-${index}`}>
                                    <SkeletonBox width="180px" height="16px" />
                                </div>
                            ))}
                        </div>
                    </div>
                </div>

                <div className={Styles.funnelcontainer}>
                    <div className={Styles.leadconversation}>
                        <SkeletonBox width="180px" height="20px" />
                    </div>
                    <div className={Styles.convolist}>
                        <SkeletonBox width="300px" height="240px" borderRadius="16px" />
                        <div className={Styles.funneldatalist}>
                            {Array.from({ length: 4 }).map((_, index) => (
                                <div className={Styles.datahistory} key={`donut-legend-skeleton-${index}`}>
                                    <SkeletonBox width="160px" height="16px" />
                                </div>
                            ))}
                        </div>
                    </div>
                </div>
            </div>
        );
    }

    return (


        <div className={Styles.marketingleads}>
            <div className={Styles.saleblockcontainer}>
                <div className={Styles.insightscard}>
                    <div className={Styles.insightscardtitle}>AI Insights</div>
                    {aiInsights?.created_at ? <TimeAgo timestamp={aiInsights?.created_at || ""} /> : '-'}
                </div>
                <div className={Styles.ratetitle}>{aiInsights?.Description || "-"}</div>
            </div>

            <div className={Styles.totalsaleblock}>
                {metricCards.map((item, index) => (
                    <div className={Styles.revenuecontainer} key={`${item?.Label || "metric"}-${index}`}>
                        <div className={Styles.totalsaletitle}>{item?.Label || "-"}</div>
                        <div className={Styles.totalsalesubtitle}>{getMetricSubtitle(item)}</div>
                        {item?.["Avg per Campaign"] && (
                            <div className={Styles.totalsaledesc}>
                                Avg per Campaign: {item["Avg per Campaign"]}
                            </div>
                        )}
                        <div className={Styles.totalsaledesc}>{item?.Description || "-"}</div>
                    </div>
                ))}

            </div>

            <div className={Styles.funnelcontainer}>
                <div className={Styles.leadconversation}>
                    Lead-to-Customer Conversion
                </div>
                <div className={Styles.convolist}>
                    <FunnelChart data={funnelData} />
                    <div className={Styles.funneldatalist}>
                        {funnelData.map((item, index) => (
                            <div className={Styles.datahistory} key={`${item.name}-${index}`}>
                                <div
                                    className={Styles.dot}
                                    style={{
                                        backgroundColor:
                                            ["#7423D7", "#8650DE", "#9A6BFA", "#B894FF", "#E6D8FF"][index] ||
                                            "#E6D8FF",
                                    }}
                                ></div>
                                {item.name}
                                <span>{item.value}%</span>
                            </div>
                        ))}
                    </div>
                </div>
            </div>

            <div className={Styles.funnelcontainer}>
                <div className={Styles.leadconversation}>
                    New Leads by Source
                </div>
                <div className={Styles.convolist}>
                    <DonutChart data={reachChartData} colors={donutColors} centerText="Total" />
                    <div className={Styles.funneldatalist}>
                        {reachChartData?.map((item, index) => (
                            <div className={Styles.datahistory} key={`${item.name}-${index}`}>
                                <div
                                    className={Styles.paidads}
                                    style={{
                                        backgroundColor: donutColors[index] || donutColors[donutColors.length - 1],
                                    }}
                                ></div>
                                {item?.name}: <span>{item?.value}%</span>
                            </div>
                        ))}
                    </div>
                </div>
            </div>
        </div>

    );
};

export default Marketing;
