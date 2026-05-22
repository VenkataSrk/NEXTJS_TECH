import React from "react";
import { useSelector } from "react-redux";
import Styles from './scss/project.module.scss';
import Trade from '../../assets/images/home/Icon.svg'
import Linechart from "./charts/linechart";
import Dynamicrechart from "../product/dynamicChart";
import TimeAgo from "./timeAgo";
import SkeletonBox from "../marketing/skeletonbox";
const Project = () => {

    const homeProject = useSelector((state: any) => state.homeStore?.homeProject);
    const loading: boolean = useSelector((state: any) => state.homeStore?.homeProjectLoader);
    const projectData = homeProject ?? {};
    const chartData = projectData?.[4]
        ? {
            chartname: projectData?.[4].chartname,
            data: projectData?.[4].data,
            title: "Active Projects Over Time",
        }
        : {} as any;

    if (loading) {
        return (
            <div className={Styles.salecontainer}>
                {/* AI Insights */}
                <div className={Styles.saleblockcontainer}>
                    <div className={Styles.insightscard}>
                    <SkeletonBox width="100%" height="20px" />
                    <SkeletonBox width="180px" height="14px" />
                    <SkeletonBox width="70%" height="16px" />
                    </div>
                 
                </div>

                {/* Two cards */}
                <div className={`${Styles.totalsaleblock} ${Styles.totalsalelist}`}>
                    {Array.from({ length: 2 }).map((_, i) => (
                        <div key={i} className={Styles.revenuecontainer}>
                            <SkeletonBox width="150px" height="18px" />
                            <SkeletonBox width="80%" height="22px" />
                            <SkeletonBox width="60%" height="14px" />
                        </div>
                    ))}
                </div>

                {/* Upcoming milestones */}
                <div className={Styles.saleblockcontainer}>
                    <SkeletonBox width="220px" height="20px" />
                    <SkeletonBox width="160px" height="18px" />
                    <SkeletonBox width="70%" height="16px" />
                </div>

                {/* Chart */}
                <div className={Styles.chartline}>
                    <SkeletonBox width="220px" height="20px" />
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
                        <div className={Styles.insightscardsubtitle}>
                            {projectData?.[3]?.created_at ? <TimeAgo timestamp={projectData?.[3]?.created_at || ""} /> : '-'}
                        </div>
                    </div>
                    <div className={Styles.ratetitle}>
                        {projectData?.[3]?.Description || '-'}
                    </div>
                </div>
                <div className={`${Styles.totalsaleblock} ${Styles.totalsalelist}`}>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Total Active Projects</div>
                        <div className={Styles.totalsalesubtitle}>{projectData?.[1]?.["Sub-heading"] || '-'}</div>
                        <div className={Styles.totalsaledesc}>{projectData?.[1]?.Description}</div>
                    </div>
                    <div className={Styles.revenuecontainer}>
                        <div className={Styles.totalsaletitle}>Average Project Duration</div>
                        <div className={Styles.totalsalesubtitle}>{projectData?.[0]?.["Sub-heading"] || '-'}</div>
                        <div className={Styles.totalsaledesc}>{projectData?.[0]?.Description}</div>
                    </div>
                </div>
                <div className={Styles.saleblockcontainer}>
                    <div className={Styles.insightscard}>
                        <div className={Styles.insightscardtitle}>Upcoming Deadlines / Milestones</div>
                        <div className={Styles.projectitle}>{projectData?.[2]?.["Sub-heading"] || '-'}</div>
                    </div>
                    <div className={Styles.ratetitle}>{projectData?.[2]?.Description}</div>
                </div>
                {chartData?.data && <div className={Styles.chartline}>
                    <div className={Styles.targetitle}>Active Projects Over Time</div>
                    <div className={Styles.axislist}>
                        <div className={Styles.axislistitle}><span>x axis</span>Month</div>
                        <div className={Styles.axislistitle}><span>y axis</span>Revenue</div>
                    </div>
                    <div style={{ width: "100%", height: 300, marginTop: "32px" }}>
                        <Dynamicrechart dynamicrechartdata={chartData} />
                    </div>
                </div>}
            </div>
        </>
    )
}
export default Project