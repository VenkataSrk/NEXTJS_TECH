import React, { useEffect, useState } from "react";
import styles from "./scss/tracker.module.scss"
import { useSearchParams } from "react-router-dom";
import { getIndividualProductDetailsApi } from "../../base/service/core_serviceApi";
import SkeletonBox from "../marketing/skeletonbox";
import Dynamicrechart from "./dynamicChart";


type ProductTypeProps={
pID?: string;
showProductName?: (pid :any)=>void;
}
function Productname({pID='',showProductName}:ProductTypeProps) {
    const [params] = useSearchParams();
    const productId = params.get("productId");
    const [product, setProduct] = useState<any>([]);
    const [loading, setLoading] = useState(false);
    const [chartData, setChartData] = useState<any>(null);

    useEffect(() => {
        fetchProduct();
    }, [productId]);

    const fetchProduct = async () => {
        setLoading(true);
        try {
            const body = {
                producId:pID?pID: Number(productId),
            };

            const res = await getIndividualProductDetailsApi(body);
            const responseData = res?.response?.[0];
            setProduct(responseData?.product_details || []);
            const chart = responseData?.total_revenue_growth?.revenue_chart;

            if (chart) {
                setChartData({
                    chartname: chart.chartname,
                    data: chart.data,
                    title: chart.title || "Total Revenue Growth",
                });
            }

        } catch (error) {
            console.log("getProductDetails Error", error);
            setProduct(null);
        } finally {
            setLoading(false);
        }
    };


    return (<>
        <div className={styles.trackerbodyinner}>
                
                {pID &&<div className={`${styles.campaigndetail} chatmaincontainer campaigndetailincontacts  ${styles.stickydetails} ${styles.insidecontact}`} onClick={()=>showProductName?.('')}>

                    <div className={styles.campaigndetailleft}>

                        <div style={{ display: "flex", gap: "12px", alignItems: "baseline" } }>
                            <span style={{ display: "grid" , position:"relative", top:"4px"}}>
                                <svg style={{ position: "relative", cursor: "pointer" }} xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none"><path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#404040"></path></svg>

                            </span>

                            <div className={styles.title}>{product?.display?.[1]?.value}<span>{product?.display?.[4]?.value}</span></div>
                        </div>
                     
                    </div>
                </div>
                }
            {!loading && product && (
                <>
                    <div className={`${styles.overalltrackercard} ${styles.productindividul}`}>
                    <div className={styles.lefts}>
                            <img src={product?.prod_image} />
                        </div>
                        <div className={styles.rights}>
                            {product?.display?.map((row: any, id: any) => (
                                <div className={styles.block} key={id}>
                                    <div className={styles.column}>{row.key} :</div>
                                    <div className={styles.column}>
                                        {row.value ?? "-"}
                                    </div>
                                </div>
                            ))}
                        </div>
                    </div>
                    {chartData && (
                        <div className={styles.gradientcard}>
                            <div className="title">
                                {chartData.title}
                            </div>
                            <div style={{ width: "100%", height: 300, marginTop: "32px" }}>
                                <Dynamicrechart dynamicrechartdata={chartData} />
                            </div>
                        </div>
                    )}
                </>
            )}
        </div>

        {loading && (
            <div style={{ padding: "16px 0", display: "flex", flexDirection: "column", gap: "8px" }} className="maincontainer">
                {Array.from({ length: 6 }, (_, index) => (
                    <div key={index} style={{ display: "flex", justifyContent: "center" }}>
                        <SkeletonBox width="100%" height="120px" />
                    </div>
                ))}
            </div>
        )}

    </>)
} export default Productname
