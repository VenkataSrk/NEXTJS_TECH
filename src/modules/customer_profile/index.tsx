import React, { useState } from 'react';
import style from './scss/customer.module.scss';
import ContentLayout from '../../layout/contentLayout';
import { leftnavassets } from '../../styles/assets/assets';
import WorktualTab from '../../components/custom_components/worktualTab';
import Journey from './journey';
import Upcoming from './upcoming';
import DonutChart from '../../components/custom_components/apexChartsTypes/donutChart';
import Speedometer from './speedometer';
const Customer_Profile = () => {
    const [series, setSeries] = useState<number[]>([]);
    const [labels, setLabels] = useState<string[]>([]);

      const tabs = [
    { label: 'Journey', content: <Journey /> },
    { label: 'Upcoming', content: <Upcoming /> }
  ]
    return(
        <>
           
           <ContentLayout noPadding={true}>
            <div className={style.aipilotsec}>
             <h4 className={style.aipheader}> AI Pilot</h4>
             <p className={style.aipdesc}>Turns raw data into data-driven actions.</p>
            </div>
             <div className={style.customercontainer}>
                <div className={style.customerleftpanel}>
                        <div className={style.topbarpanel}>
                          <h4>cw</h4>
                          <h3>Cameron Williamson <span>At risk</span></h3>
                          <p>San Francisco, CA</p>
                       </div>
                       <div className={style.aisummaryincustomerpanel}>
                           <h3>AI Summary</h3>
                           <p>A paragraph is a distinct section of writing that focuses on a single idea or topic. It usually begins with a topic sentence that introduces the main point, followed by supporting sentences that explain, provide evidence, or elaborate on the idea. A good paragraph flows logically, often ending with a concluding sentence that wraps up the thought or transitions to the next idea. While the length can vary, paragraphs are generally long enough to fully develop their point but short enough to keep the reader’s attention. In essence, a paragraph is the basic building block of prose, giving structure and clarity to writing.</p>
                       </div>
                       <div className={style.purchaselist}>
                            <div className={style.pruchase}>
                                <img src={leftnavassets.purchase_list1} alt="" />
                                <p>5 purchase</p>
                            </div>
                            <div className={style.pruchase}>
                                 <img src={leftnavassets.purchase_list2} alt="" />
                                 <p>17 App</p>
                            </div>
                            <div className={style.pruchase}>
                                  <img src={leftnavassets.purchase_list3} alt="" />
                                  <p>7 Social</p>
                            </div>
                            <div className={style.pruchase}>
                                  <img src={leftnavassets.purchase_list4} alt="" />
                                  <p>19 Email</p>
                            </div>
                       </div>
                       <div className={style.generalinformation}>
                            <h3>General Information</h3>
                            <p className={style.createmanage}>Create, manage, and monitor customer outreach campaigns to drive engagement, boost retention, and promote offers across multiple communication channels</p>
                            <div className={style.generaldetails}> 
                                <span>Contact ID</span>
                                <p>001</p>
                            </div>
                            <div className={style.generaldetails}>
                                <span>Gender</span>
                                <p>Male</p>
                            </div>
                            <div className={style.generaldetails}>
                                <span>Contact Number</span>
                                <p>987654310</p>
                            </div>
                            <div className={style.generaldetails}>
                                <span>Email  address</span>
                                <p>cwilliamson@gmail.com</p>
                            </div>
                       </div>
                       <div className={style.retentionoffer}>
                            <h3>Retention Offers</h3>
                            <p>Track what works. Boost what converts.</p>
                            <div className={style.netprices}>
                                <p>10GB data @ £10</p>
                                <p>1000GB on net mins £8</p>
                                <p>1000GB on net mins £8</p>
                                <p>Caller tune service @ £25</p>
                                <p>International roaming @ £25</p>
                            </div>

                       </div>
                       <div className={style.analyticmetric}>
                             <p>Analytics Metrics</p>
                       </div>
                       <div className={style.chartcontainer}>
                            <div className={style.clvwrapbody}>
                                <h3>Engagement score</h3>
                                 <Speedometer value={34} />
                                  <div className={style.labelsgrp}>
                                    <h3>94.65</h3>
                                </div>
                            </div>
                             <div className={style.clvwrapbody}>
                                <h3>CLV</h3>
                                <DonutChart
                                   series={[40, 25, 15]}  
                                   labels={["Apple","Orange", "Mango"]} 
                                />
                                {/* <DonutChart series={series} labels={labels} /> */}
                                <div className={style.labelsgrp}>
                                    <div className={style.indvidualchartlabel}>
                                        <span className={style.dot} style={{ backgroundColor: '#438DBE' }}></span>
                                        <p>Expected CLV</p>
                                    </div>
                                    <div className={style.indvidualchartlabel}>
                                        <span className={style.dot} style={{ backgroundColor: '#2CA02C' }}></span>
                                        <p>Actual CLV</p>
                                    </div>
                                </div>
                            </div>
                       </div>
                </div>
                <div className={style.customerightpanel}>
                      <WorktualTab tabs={tabs} />
                </div>
             </div>
           </ContentLayout>
        </>
    )
}
export default Customer_Profile;
