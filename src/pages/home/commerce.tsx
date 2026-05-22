import React from "react";
import Styles from './scss/commerce.module.scss';
import Trade from '../../assets/images/home/Icon.svg'
import DonutChart from "./donutchart";
const Commerce = () => {
     const chartData = [
        { name: "A", value: 400 },
        { name: "B", value: 300 },
        { name: "C", value: 200 }
    ];

    const COLORS = ["#DB4437", "#F4B400", "#1A73E8"];
    return (
        <>
            <div className={Styles.salecontainer}>
                <div className={Styles.saleblockcontainer}>
                    <div className={Styles.insightscard}>
                        <div className={Styles.insightscardtitle}>AI Insights</div>
                        <div className={Styles.insightscardsubtitle}>Generated 3 minutes ago</div>
                    </div>
                    <div className={Styles.ratetitle}>Your sales conversion rate has <span>increased by 15%</span>this week. Focus efforts on <span>high-value leads</span>to maximize revenue potential.</div>
                </div>
                <div className={Styles.totalsaleblock}>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Total Sales / Revenue</div>
                        <div className={Styles.totalsalesubtitle}>£15,000</div>
                        <div className={Styles.totalsaledesc}>Revenue generated today reflects all completed sales so far</div>
                        <div className={Styles.tradelist}><img src={Trade} alt="Trade" />+10% higher than yesterday</div>
                    </div>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Total Sales / Revenue</div>
                        <div className={Styles.totalsalesubtitle}>£15,000</div>
                        <div className={Styles.totalsaledesc}>Revenue generated today reflects all completed sales so far</div>
                        <div className={Styles.tradelist}><img src={Trade} alt="Trade" />+10% higher than yesterday</div>
                    </div>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Total Sales / Revenue</div>
                        <div className={Styles.totalsalesubtitle}>£15,000</div>
                        <div className={Styles.totalsaledesc}>Revenue generated today reflects all completed sales so far</div>
                        <div className={Styles.tradelist}><img src={Trade} alt="Trade" />+10% higher than yesterday</div>
                    </div>
                </div>
                 <div className={`${Styles.totalsaleblock} ${Styles.totalsalelist}`}>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Total Sales / Revenue</div>
                        <div className={Styles.totalsalesubtitle}>£15,000</div>
                        <div className={Styles.totalsaledesc}>Revenue generated today reflects all completed sales so far</div>
                    </div>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Total Sales / Revenue</div>
                        <div className={Styles.totalsalesubtitle}>£15,000</div>
                        <div className={Styles.totalsaledesc}>Revenue generated today reflects all completed sales so far</div>
                    </div>
                </div>
                 <div className={Styles.funnelcontainer}>
                    <div className={Styles.leadconversation}>Lead-to-Customer Conversion</div>
                    <div className={Styles.convolist}>
                        <DonutChart
                            data={chartData}
                            colors={COLORS}
                            centerText="Total"
                        />
                        <div className={Styles.funneldatalist}>
                            <div className={Styles.datahistory}><div className={Styles.paidads}></div>Paid Ads: <span>100%</span></div>
                            <div className={Styles.datahistory}><div className={`${Styles.paidads} ${Styles.organicsearch}`}></div>Organic Search:<span>75%</span></div>
                            <div className={Styles.datahistory}><div className={`${Styles.paidads} ${Styles.referrals}`}></div>Referrals:<span>50%</span></div>
                        </div>
                    </div>

                </div>
            </div>
        </>
    )
}
export default Commerce