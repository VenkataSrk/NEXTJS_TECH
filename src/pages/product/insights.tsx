import React from "react";
import { useSelector } from "react-redux";
import styles from "./scss/tracker.module.scss"
import chartimg from "../../assets/images/product/trendchartimg.svg"
import prod from "../../assets/images/product/product.png"
import uparrowprod from "../../assets/images/product/uparrowprod.svg"
import yellowdisclaimer from "../../assets/images/product/disclaimer.svg"
import mailicon from "../../assets/images/inbox/mailicon.svg"
import prodimg from "../../assets/images/product/prodimgf.png"
import trendchartimg from "../../assets/images/product/trendchart.svg"
import downWardArrow from "../../assets/images/product/downWardArrow.svg"
import DataTable from "react-data-table-component";
import Dynamicrechart from "./dynamicChart";
import {
    XAxis,
    YAxis,
    ResponsiveContainer,
    CartesianGrid,
    BarChart,
    Bar,
} from "recharts";
import {
    LineChart,
    Line,

} from "recharts";
import {
    PieChart,
    Pie,
    Cell,

} from "recharts";
import { getCurrencyByDomain } from "../../base/utils";
import Nodata from "../component/nodata";
import SkeletonBox from "../marketing/skeletonbox";
import Tooltip from "../component/tooltip/tooltip";

function Insights() {

    const { productInsights, productInsightsLoader } = useSelector((state: any) => state.productStore);
    const { performanceRanking = {}, productRevenue = null, revenueGrowth = null } = productInsights || {};
    const loading = productInsightsLoader || { performance: false, productRevenue: false, revenueGrowth: false };
    const currency = getCurrencyByDomain();
    const growthIcon = productRevenue?.percentage < 0 ? downWardArrow : trendchartimg;

    const perfomanceList = Array.isArray(performanceRanking?.product_performance_ranking) ? performanceRanking?.product_performance_ranking : [];
    const topThreeRanking = perfomanceList?.slice(0, 3);
    const topThreeRankingDisplay = topThreeRanking?.length === 3 ? [topThreeRanking[2], topThreeRanking[0], topThreeRanking[1]] : topThreeRanking;
    const remainingRanking = perfomanceList?.slice(3);


    const remainingRankingTable = (remainingRanking || []).map((item: any) => {
        const rankValue = item?.rank ?? "--";
        const rankLabel = `#${String(rankValue).padStart(2, "0")}`;
        const unitsValue = item?.units_sold ?? 0;
        const revenueValue = item?.revenue ?? 0;
        const growthValue = item?.growth_percent ?? 0;
        const growthIcon = Number(item?.growth_percent ?? 0) < 0
            ? downWardArrow
            : trendchartimg;

        return {
            name: <>{rankLabel} {item?.product_name || "—"}</>,
            units: <>{unitsValue} units</>,
            revenue: <>{currency}{Math.round(revenueValue)} revenue</>,
            trend: (
                <div className={styles.contwithimg}>
                    <span><img src={growthIcon} /></span> {growthValue}% vs last month
                </div>
            )
        };
    });

    const hasData = (val: any) => Array.isArray(val?.data) && val.data.length > 0;
    const isAllLoadingDone = !loading.performance && !loading.productRevenue && !loading.revenueGrowth;
    const showPerformanceSkeleton = loading.performance && perfomanceList?.length === 0;
    const showRevenueSkeleton = loading.productRevenue && !hasData(productRevenue);
    const showGrowthSkeleton = loading.revenueGrowth && !hasData(revenueGrowth);
    const showNoData = isAllLoadingDone && perfomanceList?.length === 0 && !hasData(productRevenue) && !hasData(revenueGrowth);

    const rankColums = [
        {
            name: "",
            selector: row => row.name,
            cell: row => (
                <span
                    className={styles.clickblecol}
                >
                    {row.name}
                </span>
            )
        },
        {
            name: "",
            selector: row => row.units,
        },
        {
            name: "",
            selector: row => row.revenue,
        },
        {
            name: "",
            selector: row => row.trend,
        }
    ];

    const dataline = [
        { month: "Jan", value: 15000 },
        { month: "Feb", value: 20000 },
        { month: "Mar", value: 22000 },
        { month: "Apr", value: 20000 },
        { month: "May", value: 18000 },
        { month: "Jun", value: 19000 },
        { month: "Jul", value: 25000 },
        { month: "Aug", value: 27000 },
        { month: "Sep", value: 25000 },
        { month: "Oct", value: 23000 },
        { month: "Nov", value: 26000 },
        { month: "Dec", value: 30000 }
    ];
    const data = [
        { name: "Not Renewed", value: 26, color: "#4C84FF" },
        { name: "Renewed", value: 74, color: "#D63CE3" }
    ];


    const databar = [
        { name: "Value", value: 40 },
        { name: "Value", value: 32 },
        { name: "Value", value: 48 },
        { name: "Value", value: 36 },
        { name: "Value", value: 28 },
        { name: "Value", value: 45 },
        { name: "Value", value: 38 },
    ];

    return (<>
        <div className={`${styles.trackerbodyinner} trackbodyinner`}>
            {(!loading.performance && perfomanceList?.length > 0) &&
                <>
                    <div className={styles.topsec}>
                        <div className={styles.title}>Product Performance Ranking</div>
                        <p>Displays the ranking of products based on their overall sales performance, including revenue generated and units sold during the selected period.</p>
                    </div>
                    <div className={styles.section}>
                        <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                        <p>Quick highlights from your product performance data to help identify top-performing products and emerging trends.</p>
                    </div>
                    <div className={styles.leaderboardtable}>
                        <div className={styles.greybox}>
                            {topThreeRankingDisplay && topThreeRankingDisplay?.map((rank: any, idx: any) => {
                          
                                const growthValue = Number(rank?.growth_percent ?? 0);
                                const growthIcon = growthValue < 0 ? downWardArrow : trendchartimg;
                                return (
                                    <div className={`${styles.prod1} ${idx === 1 ? styles.zoom :""}`} key={idx}>
                                        <div className={styles.top}>
                                            <div className={styles.countdiv}>{rank?.rank}</div>
                                               {
                                                (rank?.product_name || "").trim().length > 25 &&
                                                    window.innerWidth > 768 ? (
                                                        <Tooltip text={rank?.product_name} className="">
                                                            <div className="truncate2">
                                                                {rank?.product_name}
                                                            </div>
                                                        </Tooltip>
                                                    ) : (
                                                        <div className="truncate2">
                                                            {rank?.product_name}
                                                        </div>
                                                    )
                                                }
                                            </div>
                                        <div className={styles.imgcontainer}>
                                            <img src={rank?.prod_image} />
                                        </div>
                                        <div className={styles.bottom}>
                                            <p>{rank?.units_sold} Units • {currency}{Math.round(rank?.revenue)} revenue</p>
                                            <div className={styles.charttxt}>
                                                <div className={styles.contwithimg}><span><img src={growthIcon} /></span> {rank?.growth_percent ?? '-'}%</div> vs last month
                                            </div>
                                        </div>
                                    </div>
                                )
                            })}


                        </div>
                        <div className="worktualtable headernone">
                            <DataTable columns={rankColums} data={remainingRankingTable} fixedHeader responsive />
                        </div>
                        {/* <div className={styles.viewallbtn}>
                            View all <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
                                <path d="M16.3604 4.48975L12.0899 0.21975C11.7969 -0.07325 11.3223 -0.07325 11.0294 0.21975C10.7365 0.51275 10.7364 0.98735 11.0294 1.28025L15.0797 5.33005H0.75C0.3359 5.33005 0 5.66595 0 6.08005C0 6.49415 0.3359 6.83005 0.75 6.83005H15.0791L11.0293 10.8799C10.7363 11.1729 10.7363 11.6475 11.0293 11.9404C11.1758 12.0869 11.3672 12.1601 11.5596 12.1601C11.752 12.1601 11.9434 12.0869 12.0899 11.9404L16.3555 7.67475C16.7832 7.25485 17.0196 6.69335 17.0225 6.09365C17.0254 5.48865 16.7901 4.91945 16.3604 4.48975Z" fill="#23232B" />
                            </svg></span>
                        </div> */}
                    </div>
                </>
            }

            {showPerformanceSkeleton && (
                <>
                    {/* Title */}
                    <SkeletonBox width="250px" height="24px" />
                    <SkeletonBox width="60%" height="16px" />

                    {/* Smart insight section */}
                    <div style={{ marginTop: 20 }}>
                        <SkeletonBox width="120px" height="20px" />
                        <SkeletonBox width="70%" height="14px" />
                    </div>

                    {/* Top 3 cards */}
                    <div style={{ display: "flex", gap: 16, marginTop: 20 }}>
                        {Array.from({ length: 3 }).map((_, idx) => (
                            <div key={idx} style={{ flex: 1 }}>
                                <SkeletonBox width="100%" height="140px" />
                            </div>
                        ))}
                    </div>

                    {/* Table rows */}
                    <div style={{ marginTop: 20 }}>
                        {Array.from({ length: 5 }).map((_, idx) => (
                            <SkeletonBox
                                key={idx}
                                width="100%"
                                height="50px"
                            />
                        ))}
                    </div>
                </>
            )}


            {(!loading.productRevenue && productRevenue && Array.isArray(productRevenue?.data) && productRevenue.data.length > 0) &&
                <> <div className={styles.topsec}>
                    <div className={styles.title}>Total Revenue by Product</div>
                    <p>Shows the total revenue generated by each product, helping identify which products contribute most to overall sales.</p>
                </div>
                    <div className={styles.section}>
                        <div className={styles.contwithimg}><span><img src={growthIcon} /></span>{" "}{productRevenue?.percentage || 0} vs last month</div>
                        <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                        <p>Quick highlights from your product performance data to help identify top-performing products and emerging trends.</p>
                    </div>
                    <div className={styles.commonbox}>
                        <div className={styles.labelsection}>
                            <div className={styles.labelwithlegend}>
                                <div className={styles.label}>Xaxis:</div>
                                <span>Product</span>
                            </div>
                            <div className={styles.labelwithlegend}>
                                <div className={styles.label}>Yaxis:</div>
                                <span>Price</span>
                            </div>
                        </div>
                        <div>
                            <Dynamicrechart dynamicrechartdata={productRevenue} />
                        </div>
                    </div>
                </>
            }

            {showRevenueSkeleton && (
                <>
                    {/* Title */}
                    <SkeletonBox width="260px" height="24px" />
                    <div style={{ marginTop: 8 }}>
                        <SkeletonBox width="65%" height="16px" />
                    </div>

                    {/* Insight section */}
                    <div style={{ marginTop: 20 }}>
                        <SkeletonBox width="180px" height="18px" />
                        <div style={{ marginTop: 10 }}>
                            <SkeletonBox width="120px" height="20px" />
                        </div>
                        <div style={{ marginTop: 8 }}>
                            <SkeletonBox width="70%" height="14px" />
                        </div>
                    </div>

                    {/* Chart box */}
                    <div style={{ marginTop: 20 }}>
                        {/* Labels */}
                        <div style={{ display: "flex", gap: 20, marginBottom: 12 }}>
                            <SkeletonBox width="120px" height="14px" />
                            <SkeletonBox width="120px" height="14px" />
                        </div>

                        {/* Chart area */}
                        <SkeletonBox width="100%" height="280px" />
                    </div>
                </>
            )}

            {(!loading.revenueGrowth && revenueGrowth && Array.isArray(revenueGrowth?.data) && revenueGrowth.data.length > 0) &&
                <>
                    <div className={styles.topsec}>
                        <div className={styles.title}>Product Revenue Growth</div>
                        <p>Shows the total revenue generated by each product, helping identify which products contribute most to overall sales.</p>
                    </div>
                    <div className={styles.section}>
                        <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                        <p>Quick highlights from your product performance data to help identify top-performing products and emerging trends.</p>
                    </div>
                    <div className={styles.commonbox} style={{padding:"20px"}}>
                        <div style={{ width: "100%", height: 314,  }}>
                            <Dynamicrechart dynamicrechartdata={revenueGrowth} />

                        </div>
                    </div>
                </>
            }

            {showGrowthSkeleton && (
                <>
                    {/* Title */}
                    <SkeletonBox width="260px" height="24px" />
                    <div >
                        <SkeletonBox width="65%" height="16px" />
                    </div>

                    {/* Insight section */}
                    <div>
                        <SkeletonBox width="180px" height="18px" />
                        <div style={{ marginTop: 10 }}>
                            <SkeletonBox width="120px" height="20px" />
                        </div>
                        <div style={{ marginTop: 8 }}>
                            <SkeletonBox width="70%" height="14px" />
                        </div>
                    </div>

                    {/* Chart container */}
                    <div>
                        <div >
                            <SkeletonBox width="100%" height="300px" />
                        </div>
                    </div>
                </>
            )}

            {showNoData && <Nodata />}

            {/* <div className={styles.topsec}>
             <div className={styles.title}>Average Deal Size by Product</div>
             <p>Shows the total revenue generated by each product, helping identify which products contribute most to overall sales.</p>
           </div>
           <div className={styles.section}>
            <div className={styles.contwithimg}><span><img src={trendchartimg}/></span> 25% vs last month</div> 
               <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
               <p>Quick highlights from your product performance data to help identify top-performing products and emerging trends.</p>
           </div>
            <div className={styles.commonbox}>
                           <div className={styles.labelsection}>
                              <div className={styles.labelwithlegend}>
                                <div className={styles.label}>Xaxis:</div>
                                <span>Product</span>
                            </div>
                            <div className={styles.labelwithlegend}>
                                <div className={styles.label}>Yaxis:</div>
                                <span>Product</span>
                            </div>
                           </div>
                            <ResponsiveContainer width="100%" height={220}>
                                <BarChart data={databar} barCategoryGap={20}>
                                    <CartesianGrid
                                        strokeDasharray="3 3"
                                        vertical={false}
                                    />
                                    <XAxis
                                        dataKey="name"
                                        tickLine={false}
                                        axisLine={false}
                                    />
                                    <YAxis
                                        tickLine={false}
                                        axisLine={false}
                                        tickFormatter={() => "value"}
                                    />
                                    <Bar
                                        dataKey="value"
                                        fill="#5B61F6"
                                        radius={[6, 6, 0, 0]}
                                    />
                                </BarChart>
                            </ResponsiveContainer>
                        </div>
                         <div className={styles.topsec}>
             <div className={styles.title}>Recent Quotes</div>
             <p>Displays the ranking of products based on their overall sales performance, including revenue generated and units sold during the selected period.</p>
           </div>
           <div className={styles.section}>
               <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
               <p>Quick highlights from your product performance data to help identify top-performing products and emerging trends.</p>
           </div>
           <div className={styles.leaderboardtable}>
              <div className={styles.tablelikecontainer}>
                 
                   <div className={styles.row}>
                        <div>
                            <div className={styles.item}>
                               <h5>Company name:</h5>
                               <span>Customer name:</span>
                            </div>
                        </div>
                       <div>
                            <div className={styles.item} >
                               <h5>Product name:</h5>
                               <span>ID:</span>
                            </div>
                        </div> 
                        <div>
                            <div className={`${styles.item} ${styles.last}`} >
                              <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                               <span>Total: £1850.83</span>
                            </div>
                        </div>
                  </div>
                    <div className={styles.row}>
                        <div>
                            <div className={styles.item}>
                               <h5>Company name:</h5>
                               <span>Customer name:</span>
                            </div>
                        </div>
                       <div>
                            <div className={styles.item} >
                               <h5>Product name:</h5>
                               <span>ID:</span>
                            </div>
                        </div> 
                        <div>
                            <div className={`${styles.item} ${styles.last}`} >
                              <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                               <span>Total: £1850.83</span>
                            </div>
                        </div>
                  </div>
                       <div className={styles.row}>
                        <div>
                            <div className={styles.item}>
                               <h5>Company name:</h5>
                               <span>Customer name:</span>
                            </div>
                        </div>
                       <div>
                            <div className={styles.item} >
                               <h5>Product name:</h5>
                               <span>ID:</span>
                            </div>
                        </div> 
                        <div>
                            <div className={`${styles.item} ${styles.last}`} >
                              <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                               <span>Total: £1850.83</span>
                            </div>
                        </div>
                  </div>
              </div>
             
                 <div className={styles.viewallbtn}>
                     View all <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
  <path d="M16.3604 4.48975L12.0899 0.21975C11.7969 -0.07325 11.3223 -0.07325 11.0294 0.21975C10.7365 0.51275 10.7364 0.98735 11.0294 1.28025L15.0797 5.33005H0.75C0.3359 5.33005 0 5.66595 0 6.08005C0 6.49415 0.3359 6.83005 0.75 6.83005H15.0791L11.0293 10.8799C10.7363 11.1729 10.7363 11.6475 11.0293 11.9404C11.1758 12.0869 11.3672 12.1601 11.5596 12.1601C11.752 12.1601 11.9434 12.0869 12.0899 11.9404L16.3555 7.67475C16.7832 7.25485 17.0196 6.69335 17.0225 6.09365C17.0254 5.48865 16.7901 4.91945 16.3604 4.48975Z" fill="#23232B"/>
</svg></span>
                 </div>
           </div>
           <div className={styles.topsec}>
             <div className={styles.title}>Recent Invoices</div>
             <p>Displays the ranking of products based on their overall sales performance, including revenue generated and units sold during the selected period.</p>
           </div>
           <div className={styles.section}>
               <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
               <p>Quick highlights from your product performance data to help identify top-performing products and emerging trends.</p>
           </div>
           <div className={styles.leaderboardtable}>
              <div className={styles.tablelikecontainer}>
                 
                   <div className={styles.row}>
                        <div>
                            <div className={styles.item}>
                               <h5>Company name:</h5>
                               <span>Customer name:</span>
                            </div>
                        </div>
                       <div>
                            <div className={styles.item} >
                               <h5>Product name:</h5>
                               <span>ID:</span>
                            </div>
                        </div> 
                        <div>
                            <div className={`${styles.item} ${styles.last}`} >
                              <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                               <span>Total: £1850.83</span>
                            </div>
                        </div>
                  </div>
                    <div className={styles.row}>
                        <div>
                            <div className={styles.item}>
                               <h5>Company name:</h5>
                               <span>Customer name:</span>
                            </div>
                        </div>
                       <div>
                            <div className={styles.item} >
                               <h5>Product name:</h5>
                               <span>ID:</span>
                            </div>
                        </div> 
                        <div>
                            <div className={`${styles.item} ${styles.last}`} >
                              <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                               <span>Total: £1850.83</span>
                            </div>
                        </div>
                  </div>
                       <div className={styles.row}>
                        <div>
                            <div className={styles.item}>
                               <h5>Company name:</h5>
                               <span>Customer name:</span>
                            </div>
                        </div>
                       <div>
                            <div className={styles.item} >
                               <h5>Product name:</h5>
                               <span>ID:</span>
                            </div>
                        </div> 
                        <div>
                            <div className={`${styles.item} ${styles.last}`} >
                              <div className={`${styles.badge} ${styles.blue}`}>smart insight</div>
                               <span>Total: £1850.83</span>
                            </div>
                        </div>
                  </div>
              </div>
             
                 <div className={styles.viewallbtn}>
                     View all <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
  <path d="M16.3604 4.48975L12.0899 0.21975C11.7969 -0.07325 11.3223 -0.07325 11.0294 0.21975C10.7365 0.51275 10.7364 0.98735 11.0294 1.28025L15.0797 5.33005H0.75C0.3359 5.33005 0 5.66595 0 6.08005C0 6.49415 0.3359 6.83005 0.75 6.83005H15.0791L11.0293 10.8799C10.7363 11.1729 10.7363 11.6475 11.0293 11.9404C11.1758 12.0869 11.3672 12.1601 11.5596 12.1601C11.752 12.1601 11.9434 12.0869 12.0899 11.9404L16.3555 7.67475C16.7832 7.25485 17.0196 6.69335 17.0225 6.09365C17.0254 5.48865 16.7901 4.91945 16.3604 4.48975Z" fill="#23232B"/>
</svg></span>
                 </div>
           </div> */}


        </div>
    </>)
} export default Insights
