import React from "react";
import { useSelector } from "react-redux";
import Styles from './scss/sales.module.scss';
import Trade from '../../assets/images/home/Icon.svg';
import Linechart from "./charts/linechart";
import Dynamicrechart from "../product/dynamicChart";
import TimeAgo from "./timeAgo";
import trendchartimg from "../../assets/images/product/trendchart.svg"
import downWardArrow from "../../assets/images/product/downWardArrow.svg"
import SkeletonBox from "../marketing/skeletonbox";

const Sales = () => {

    const homeSales = useSelector((state: any) => state.homeStore?.homeSales);
    const loading = useSelector((state: any) => state.homeStore?.homeSalesLoader);
    const salesData = homeSales ?? {};
    const chartData = salesData?.[6]
        ? {
            chartname: salesData?.[6].chartname,
            data: salesData?.[6].data,
            title: "Sales Target Attainment",
        }
        : {} as any;


    if (loading) {
        return (
            <div className={Styles.salecontainer}>
                {/* AI Insights */}
                <div className={Styles.saleblockcontainer}>
                    <SkeletonBox width="100%" height="20px" />
                    <SkeletonBox width="180px" height="14px" />
                    <SkeletonBox width="60%" height="16px" />
                </div>

                {/* Top cards */}
                <div className={Styles.totalsaleblock}>
                    {Array.from({ length: 3 }).map((_, i) => (
                        <div key={i} className={Styles.revenuecontainer}>
                            <SkeletonBox width="150px" height="18px" />
                            <SkeletonBox width="80%" height="22px" />
                            <SkeletonBox width="60%" height="14px" />
                        </div>
                    ))}
                </div>

                {/* Bottom cards */}
                <div className={`${Styles.totalsaleblock} ${Styles.totalsalelist}`}>
                    {Array.from({ length: 2 }).map((_, i) => (
                        <div key={i} className={Styles.revenuecontainer}>
                            <SkeletonBox width="150px" height="18px" />
                            <SkeletonBox width="80%" height="22px" />
                            <SkeletonBox width="60%" height="14px" />
                        </div>
                    ))}
                </div>

                {/* Chart */}
                <div className={Styles.chartline}>
                    <SkeletonBox width="200px" height="20px" />
                    <SkeletonBox width="100%" height="300px" />
                </div>
            </div>
        );
    }
    return (
        <>
            <div className={Styles.salecontainer}>
                <div className={Styles.saleblockcontainer}>
                    <div className={Styles.insightscard}>
                        <div className={Styles.insightscardtitle}>AI Insights</div>
                        {salesData?.[0]?.created_at ? <TimeAgo timestamp={salesData?.[0]?.created_at || ""} /> : '-'}
                    </div>
                    <div className={Styles.ratetitle}>{salesData?.[0]?.Description || '-'}</div>
                </div>
                <div className={Styles.totalsaleblock}>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Total Sales / Revenue</div>
                        <div className={Styles.totalsalesubtitle}>{salesData?.[1]?.["Sub-heading"] || '-'}</div>
                        <div className={Styles.totalsaledesc}>{salesData?.[1]?.Description || '-'}</div>
                        {salesData?.[1]?.Trend && (
                            <div className={Styles.tradelist}>
                                <img src={salesData?.[1]?.Color ? trendchartimg : downWardArrow} alt="Trade" />
                                {salesData?.[1]?.Trend}
                            </div>
                        )}
                    </div>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>New Leads Created</div>
                        <div className={Styles.totalsalesubtitle}>{salesData?.[2]?.["Sub-heading"] || '-'}</div>
                        <div className={Styles.totalsaledesc}>{salesData?.[2]?.Description || '-'}</div>
                    </div>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Deals Closed Won / Lost</div>
                        <div className={Styles.totalsalesubtitle}>{salesData?.[3]?.["Sub-heading"] || '-'}</div>
                        <div className={Styles.totalsaledesc}>{salesData?.[3]?.Description || '-'}</div>
                    </div>
                </div>
                <div className={`${Styles.totalsaleblock} ${Styles.totalsalelist}`}>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Lead Deal Conversion Rate</div>
                        <div className={Styles.totalsalesubtitle}>{salesData?.[4]?.["Sub-heading"] || '-'}</div>
                        <div className={Styles.totalsaledesc}>{salesData?.[4]?.Description || '-'}</div>
                    </div>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Sales Cycle Length (Average Days to Close)</div>
                        <div className={Styles.totalsalesubtitle}>{salesData?.[5]?.["Sub-heading"] || '-'}</div>
                        <div className={Styles.totalsaledesc}>{salesData?.[5]?.Description || '-'}</div>
                    </div>
                </div>
                {chartData?.data && <div className={Styles.chartline}>
                    <div className={Styles.targetitle}>Sales Target Attainment</div>
                    <div className={Styles.axislist}>
                        <div className={Styles.axislistitle}>
                            <span>x axis</span>Month
                        </div>
                        <div className={Styles.axislistitle}>
                            <span>y axis</span>Revenue
                        </div>
                    </div>
                    <div style={{ width: "100%", height: 300, marginTop: "32px" }}>
                        <Dynamicrechart dynamicrechartdata={chartData} />
                    </div>
                </div>}
            </div>
        </>
    );
};

export default Sales;