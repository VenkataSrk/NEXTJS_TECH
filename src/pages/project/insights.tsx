"use-client"
import styles from './scss/projectinsights.module.scss'
import Predictive from '../../assets/images/projectinsights/predictiveicon.svg'
import Insightsicon from '../../assets/images/projectinsights/insightsicon.svg'
import Client from '../../assets/images/projectinsights/clienttotal.svg'
import Signals from '../../assets/images/projectinsights/signals.svg'
import Pdfile from '../../assets/images/projectinsights/filepdf.svg'
import System from '../../assets/images/projectinsights/system.svg'
import Resource from '../../assets/images/projectinsights/resourcelist.svg'
import Workload from '../../assets/images/projectinsights/workload.svg'
import Aicon from '../../assets/images/projectinsights/aicon.svg'
import Arrow from '../../assets/images/projectinsights/arrownear.svg'
import Arrownear from '../../assets/images/project/arrow.svg'
import Nodata from '../component/nodata'
import { useSelector } from 'react-redux'
import { useMemo } from 'react'
const Projectsinsights = () => {

    const { projectInsight, projectInsightLoader } = useSelector((state: any) => state.projectStore);
    const insight = useMemo(() => projectInsight?.[0], [projectInsight]);
    const hasInsight = !!insight;

    return (
        <>
            <div className={styles.projectinsightsmaincontainer}>

                <div className={styles.projectblock}>
                    {insight?.predictiveSummary?.overAllOutlook?.length > 0 &&
                        <div className={styles.predictionsummary}>
                            <div className={styles.predictivetitle}><img src={Predictive} alt="Predictive" />Predictive Project Summary</div>
                            <div className={styles.completionblock}>
                                {insight?.predictiveSummary?.overAllOutlook?.length > 0 &&
                                    <div className={styles.completionlist}>
                                        <div className={styles.completionblocktitle}>Overall Completion Outlook</div>
                                        <div className={styles.completionblocksubtitle} dangerouslySetInnerHTML={{ __html: insight?.predictiveSummary?.overAllOutlook || '' }} />
                                        {/* <div className={styles.completionblocktitlebtn}><img src={Insightsicon} alt="Insightsicon" />Displays last week’s performance percentage.</div> */}
                                    </div>
                                }
                                {insight?.predictiveSummary?.onTimeProbability?.length > 0 &&

                                    <div className={styles.completionlist}>
                                        <div className={styles.completionblocktitle}>On-Time Probability Assessment</div>
                                        <div className={styles.completionblocksubtitle} dangerouslySetInnerHTML={{ __html: insight?.predictiveSummary?.onTimeProbability || '' }} />
                                    </div>
                                }
                                {insight?.predictiveSummary?.majorrisk?.length > 0 &&
                                    <div className={styles.majorpredicted}>
                                        <div className={styles.majorpredictedtitle}>Major Predicted Risks</div>
                                        {insight?.predictiveSummary?.majorRisk?.map((item: any) => {
                                            return (
                                                <div className={styles.flowcontainer}>
                                                    <div className={styles.containermain}>
                                                        <div className={styles.dot}></div>
                                                        <div className={styles.subtitleline} dangerouslySetInnerHTML={{ __html: item || '' }} />
                                                    </div>
                                                </div>
                                            )
                                        })

                                        }
                                    </div>
                                }
                                {insight?.predictiveSummary?.aiRecommendation?.length > 0 &&

                                    <div className={styles.recommendation}>
                                        <div className={styles.recommendetails}>
                                            <div className={styles.recommendtitle}><img src={Aicon} alt="Aicon" />AI Recommendations</div>
                                            <div className={styles.insightsdetails} dangerouslySetInnerHTML={{ __html: insight?.predictiveSummary?.aiRecommendation || '' }} />
                                        </div>
                                        {insight?.predictiveSummary?.projectAssessment?.length > 0 &&
                                            <div className={styles.stablemaincontainer}>
                                                {insight?.predictiveSummary?.projectAssessment.map((item, index) => (
                                                    <div className={styles.stablecontainer} key={index}>
                                                        <div className={styles.stabletitle}>{item.title}</div>
                                                        <div className={styles.stablesubtitle}>{item.value}</div>
                                                    </div>
                                                ))}
                                            </div>
                                        }
                                    </div>
                                }
                            </div>
                        </div>
                    }
                    {insight?.recentActivity?.length > 0 &&

                        <div className={styles.recentactivity}>
                            <div className={styles.requirementlistdata}>

                                <div className={styles.listdata}>
                                    <div className={styles.listdatatitle}>Recent activity timeline</div>
                                </div>

                                {insight?.recentActivity?.map((item, index) => (
                                    <div className={styles.interestedblock} key={index}>
                                        <div className={styles.interestlist}>
                                            <div className={styles.interestlistdot}></div>

                                            <div className={styles.interestedheader}>
                                                <div className={styles.interestheader}>{item.title}</div>
                                                <div className={styles.interestsubheader}>{item.subtitle}</div>
                                            </div>
                                        </div>

                                        <div className={styles.interestsubslot}>{item.time}</div>
                                    </div>
                                ))}

                                {/* <div className={styles.requirebtn}>
                                    View all activity <img src={Arrownear} alt="Arrow" />
                                </div> */}

                            </div>
                        </div>
                    }
                    {insight?.requirementList?.length > 0 &&
                        <div className={styles.requirementlistdata}>

                            <div className={styles.listdata}>
                                <div className={styles.listdatatitle}>Requirements List</div>
                                <div className={styles.listdatasubtitle}>Requirements count here.</div>
                            </div>
                            {insight?.requirementList?.map((item, index) => (
                                <div className={styles.blockcontainername} key={index}>
                                    {/* Top section */}
                                    <div className={styles.listdatamain}>
                                        <div className={styles.customername}>
                                            <div className={styles.companylist}>{item.customerLabel}</div>
                                            <div className={styles.companytitle}>{item.customerValue}</div>
                                        </div>

                                        <div className={styles.requiredtag}>{item.priority}</div>
                                    </div>

                                    {/* Requirement section */}
                                    <div className={styles.customername}>
                                        <div className={styles.companylist}>{item.requirementLabel}</div>
                                        <div className={styles.companytitle}>
                                            {item.requirementDescription}
                                        </div>
                                    </div>

                                    {/* Progress / team section */}
                                    <div className={styles.progressbarteam}>
                                        <div className={`${styles.teamnametag} ${styles.completed}`}>{item.status}</div>

                                        <div className={styles.handlingteammain}>
                                            <div className={styles.handlingteamtitle}>{item.teamTitle}</div>
                                            <div className={styles.handlingteamsubtitle}>
                                                {item.teamSubtitle}
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            ))}

                            <div className={styles.requirebtn}>
                                View all Requirements <img src={Arrownear} alt="Arrow" />
                            </div>

                        </div>
                    }

                    {insight?.clientDeliverables?.length > 0 &&
                        <div className={styles.requirementlistdata}>
                            <div className={styles.deliverable}>
                                <img src={Client} alt="Client" />
                                <div className={styles.listdata}>
                                    <div className={styles.listdatatitle}>Client Deliverables</div>
                                    <div className={styles.listdatasubtitle}>SLA commitment tracker</div>
                                </div>
                            </div>
                            <div className={styles.deliverableblockmain}>
                                {insight?.clientDeliverables?.map((item, index) => (
                                    <div className={styles.slacontentleft} key={index}>
                                        <div className={styles.inprogresetting}>
                                            <div className={styles.slacontainer}>
                                                <div className={styles.slatitle}>{item.company}</div>
                                                <div className={styles.slasubtitle}>{item.requirement}</div>
                                            </div>

                                            <div className={styles.progresstag}>{item.status}</div>
                                        </div>

                                        <div className={styles.slaslots}>
                                            <div className={styles.titleslots}>{item.due}</div>
                                            <div className={styles.subtitleslots}>{item.remaining}</div>
                                        </div>
                                    </div>
                                ))}
                            </div>

                            {/* <div className={styles.attendbtn}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16" fill="none">
                                <g clip-path="url(#clip0_1430_25690)">
                                    <path d="M8 14.6666C11.6819 14.6666 14.6667 11.6818 14.6667 7.99992C14.6667 4.31802 11.6819 1.33325 8 1.33325C4.3181 1.33325 1.33333 4.31802 1.33333 7.99992C1.33333 11.6818 4.3181 14.6666 8 14.6666Z" stroke="#EF4444" stroke-width="1.33333" stroke-linecap="round" stroke-linejoin="round" />
                                    <path d="M8 5.33325V7.99992" stroke="#EF4444" stroke-width="1.33333" stroke-linecap="round" stroke-linejoin="round" />
                                    <path d="M8 10.6667H8.00667" stroke="#EF4444" stroke-width="1.33333" stroke-linecap="round" stroke-linejoin="round" />
                                </g>
                                <defs>
                                    <clipPath id="clip0_1430_25690">
                                        <rect width="16" height="16" fill="white" />
                                    </clipPath>
                                </defs>
                            </svg>1 deliverable requires immediate attention
                        </div> */}

                        </div>
                    }

                    {insight?.delayRiskWarn?.length > 0 &&
                        <div className={styles.requirementlistdata}>
                            <div className={styles.deliverable}>
                                <img src={System} alt="System" />
                                <div className={styles.listdata}>
                                    <div className={styles.listdatatitle}>Upcoming Delays & Risk Warnings</div>
                                </div>
                            </div>
                            <div className={styles.sentenceblockmain}>
                                {insight?.delayRiskWarn?.map((item, index) => (
                                    <div className={styles.sentenceblock} key={index}>
                                        <div className={styles.sentencerequiredtag}>
                                            <div className={styles.sentencetitlename}>{item.requirement}</div>
                                            <div className={styles.sentencesubtitlename}>{item.priority}</div>
                                        </div>

                                        <div className={styles.predicted}>
                                            Predicted Delay : <span>{item.predictedDelay}</span>
                                        </div>

                                        <div className={styles.predicted}>
                                            Risk Type : <span>{item.riskType}</span>
                                        </div>

                                        <div className={styles.predictedsubtitle}>
                                            {item.description}
                                        </div>
                                    </div>
                                ))}

                            </div>
                        </div>
                    }

                    {insight?.workloadCapacity?.teamCapacity?.length > 0 &&
                        <div className={styles.requirementlistdata}>
                            <div className={styles.deliverable}>
                                <img src={Workload} alt="Workload" />
                                <div className={styles.listdata}>
                                    <div className={styles.listdatatitle}>Workload & Capacity Predictions</div>
                                </div>
                            </div>
                            <div className={styles.overviewtitle}>Team Capacity Overview</div>
                            <div className={`${styles.sentenceblockmain} ${styles.sentenceblockmainset}`}>
                                {insight?.workloadCapacity?.teamCapacity?.map((item, index) => (
                                    <div className={styles.sentenceblock} key={index}>
                                        <div className={styles.sentencerequiredtag}>
                                            <div className={styles.sentencetitlename}>{item.requirement}</div>
                                            <div className={styles.sentencesubtitlename}>{item.priority}</div>
                                        </div>

                                        <div className={styles.predictedsubtitle}>
                                            {item.description}
                                        </div>
                                    </div>
                                ))}

                            </div>
                            {insight?.workloadCapacity?.predictions?.length > 0 &&
                                <div className={styles.majorpredicted}>
                                    <div className={styles.majorpredictedtitle}>Predictions</div>
                                    <div className={styles.flowcontainer}>
                                        {insight?.workloadCapacity?.predictions?.map((item: any, index: any) => {
                                            return (
                                                <div className={styles.containermain} key={index}>
                                                    <div className={styles.dot}></div>
                                                    <div className={styles.subtitleline} dangerouslySetInnerHTML={{ __html: item || '' }} />
                                                </div>
                                            )

                                        })}
                                        {/* <div className={styles.containermain}>
                                        <div className={styles.dot}></div>
                                        <div className={styles.subtitleline}>Highlights expected future impacts based on ongoing workload and performance trends.</div>
                                    </div>
                                    <div className={styles.containermain}>
                                        <div className={styles.dot}></div>
                                        <div className={styles.subtitleline}>Highlights expected future impacts based on ongoing workload and performance trends.</div>
                                    </div>
                                    <div className={styles.containermain}>
                                        <div className={styles.dot}></div>
                                        <div className={styles.subtitleline}>Highlights expected future impacts based on ongoing workload and performance trends.</div>
                                    </div> */}
                                    </div>
                                </div>
                            }

                            {insight?.workloadCapacity?.aiRecommendation?.length > 0 &&
                                <div className={styles.recommendation}>
                                    <div className={styles.recommendetails}>
                                        <div className={styles.recommendtitle}><img src={Aicon} alt="Aicon" />AI Recommendations</div>
                                        <div className={styles.insightsdetails} dangerouslySetInnerHTML={{ __html: insight?.workloadCapacity?.aiRecommendation || '' }} />
                                    </div>
                                </div>}
                        </div>
                    }

                    {insight?.resourceAllocation?.shortageForecast?.length > 0 &&

                        <div className={styles.requirementlistdata}>
                            <div className={styles.deliverable}>
                                <img src={Resource} alt="Resource" />
                                <div className={styles.listdata}>
                                    <div className={styles.listdatatitle}>Resource Allocation Insights</div>
                                </div>
                            </div>

                            {
                                insight?.resourceAllocation?.shortageForecast?.length > 0 &&
                                <>
                                    <div className={styles.overviewtitle}>Resource Shortage Forecast</div>
                                    <div className={`${styles.sentenceblockmain} ${styles.sentenceblockmainset}`}>
                                        {insight?.resourceAllocation?.shortageForecast?.map((item, index) => (
                                            <div className={styles.sentenceblock} key={index}>
                                                <div className={styles.sentencerequiredtag}>
                                                    <div className={styles.sentencetitlename}>{item.requirement}</div>
                                                    <div className={styles.sentencesubtitlename}>{item.priority}</div>
                                                </div>

                                                <div className={styles.predictedsubtitle}>
                                                    {item.description}
                                                </div>
                                                <div className={styles.predictedsubtitlemenu}>
                                                    {item.cardtitle}
                                                </div>
                                            </div>
                                        ))}

                                    </div>
                                </>
                            }
                            {/* {insight?.resourceAllocation?.recommendedHiring?.length > 0 &&
                                <div className={styles.majorpredicted}>
                                    <div className={styles.majorpredictedtitle}>Hiring & Reassignment Recommendations</div>
                                    <div className={`${styles.flowcontainer} ${styles.flowcontainermenu}`}>

                                        {insight?.resourceAllocation?.recommendedHiring?.map((item: any, index: any) => {
                                            return (
                                                <div className={styles.containermain}>
                                                    <div className={styles.count}>{index + 1}</div>
                                                    <div className={styles.descriptive}>
                                                        <div className={styles.insightsdetails} dangerouslySetInnerHTML={{ __html: item || '' }} />

                                                    //     <div className={styles.subtitleline}>Short descriptive sentence explaining recommended hiring</div>
                                                    // <div className={styles.priorityhigh}>Priority: High • Timeline: Within 2 weeks • Cost: Moderate</div> 
                                                    </div>
                                                </div>
                                            )
                                        })
                                        }
                                    </div>
                                </div>
                            } */}
                            {insight?.resourceAllocation?.aiRecommendation?.length > 0 &&

                                <div className={styles.recommendation}>
                                    <div className={styles.recommendetails}>
                                        <div className={styles.recommendtitle}><img src={Aicon} alt="Aicon" />AI Recommendations</div>
                                        <div className={styles.insightsdetails} dangerouslySetInnerHTML={{ __html: insight?.resourceAllocation?.aiRecommendation || '' }} />
                                    </div>
                                </div>
                            }
                        </div>
                    }

                    {insight?.reworkPrediction?.highRiskRework?.length > 0 &&
                        <div className={styles.requirementlistdata}>
                            <div className={styles.deliverable}>
                                <img src={Pdfile} alt="Pdfile" />
                                <div className={styles.listdata}>
                                    <div className={styles.listdatatitle}>Requirement Stability & Rework Prediction</div>
                                </div>
                            </div>
                            <div className={styles.overviewtitle}>High-Risk Requirements</div>
                            <div className={`${styles.sentenceblockmain} ${styles.sentenceblockmainset}`}>
                                {insight?.reworkPrediction?.highRiskRework.map((item, index) => (
                                    <div className={styles.sentenceblock} key={index}>
                                        <div className={styles.sentencerequiredtag}>
                                            <div className={styles.sentencetitlename}>{item.requirement}</div>
                                            <div className={styles.sentencesubtitlename}>{item.priority}</div>
                                        </div>

                                        <div className={styles.predictedsubtitle}>
                                            {item.description}
                                        </div>
                                        <div className={styles.predictedsubtitlemenu}>
                                            {item.cardtitle}
                                        </div>
                                    </div>
                                ))}

                            </div>
                            {/* {
                                insight?.reworkPrediction?.reworkImpactAnalysis?.length > 0 &&

                                <div className={styles.majorpredicted}>
                                    <div className={styles.majorpredictedtitle}>Rework Impact Analysis</div>
                                    <div className={styles.systemhightlight} dangerouslySetInnerHTML={{ __html: insight?.reworkPrediction?.reworkImpactAnalysis || '' }} />
                                </div>
                            }
                            {insight?.reworkPrediction?.aiRecommendation?.length > 0 &&
                                <div className={styles.recommendation}>
                                    <div className={styles.recommendetails}>
                                        <div className={styles.recommendtitle}><img src={System} alt="Aicon" />AI Recommendations</div>
                                        <div className={styles.insightsdetails} dangerouslySetInnerHTML={{ __html: insight?.reworkPrediction?.aiRecommendation || '' }} />
                                    </div>
                                </div>
                            } */}
                        </div>
                    }
                    {insight?.operationalEfficiency?.predictedTeamOperation?.length > 0 &&

                        <div className={styles.requirementlistdata}>
                            <div className={styles.deliverable}>
                                <img src={Signals} alt="Pdfile" />
                                <div className={styles.listdata}>
                                    <div className={styles.listdatatitle}>Operational Efficiency Signals</div>
                                </div>
                            </div>
                            <div className={styles.overviewtitle}>Predicted Time Drift Across Teams</div>
                            <div className={`${styles.sentenceblockmain} ${styles.sentenceblockmainset}`}>
                                {insight?.operationalEfficiency?.predictedTeamOperation?.map((item, index) => (
                                    <div className={styles.sentenceblock} key={index}>
                                        <div className={styles.sentencerequiredtag}>
                                            <div className={styles.sentencetitlename}>{item.requirement}</div>
                                            <div className={styles.sentencesubtitlename}>{item.priority}</div>
                                        </div>

                                        <div className={styles.predictedsubtitle}>
                                            {item.description}
                                        </div>
                                    </div>
                                ))}

                            </div>
                            {insight?.operationalEfficiency?.aiRecommendation?.length > 0 &&

                                <div className={styles.recommendation}>
                                    <div className={styles.recommendetails}>
                                        <div className={styles.recommendtitle}><img src={Aicon} alt="Aicon" />AI Recommendations</div>
                                        <div className={styles.insightsdetails} dangerouslySetInnerHTML={{ __html: insight?.operationalEfficiency?.aiRecommendation || '' }} />
                                    </div>
                                </div>
                            }
                        </div>
                    }
                    {!hasInsight && projectInsightLoader && (
                        <div className={styles.projectblock}>Loading insights…</div>
                    )}
                    {!hasInsight && !projectInsightLoader && <Nodata />}
                </div>
            </div>
        </>
    )
}
export default Projectsinsights;