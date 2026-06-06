import React, { useEffect, useState } from "react";
import styles from "./scss/pastcampaign.module.scss"
import { assets, leftnavassets } from '../../../src/styles/assets/assets'
import ProgressBar from "../../components/custom_components/progressbar/progressbar";
import { jwtDecode } from "jwt-decode";
import { GetItemFromStorage } from "../../base/customhooks/useStorage";
import { LocalStorage } from "../../base/customhooks/localStorageKeys";
import { config } from "../../base/constant";
import axios from "axios";

const PastCampaignStatus = ({disableCampaign,selectedCampaign} :any) => {
    const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
       const userData: any = jwtDecode(userDetails);
       const [summary,setSummary] = useState<any>('');
       const [summaryone,setSummaryOne] = useState<any>('');
    const handleChange=()=>{
        disableCampaign();
    }

    const handlesummary=async()=>{
        try {
        
                const url: string = `${config().NLP_CAMPAIGN_URL}/${import.meta.env.VITE_APP_ENV=="development"?'past_campaign_summry_from_db':'past_campaign_summry_from_db'}`;
                const response = await axios.post(
                    url,
                    { domainId: userData?.domainId,
                      chatId:selectedCampaign?.CampaignChatId
                    },

                    {
                    headers: {
                        "Content-Type": "application/json",
                    },
                    }
                )

                if (response?.status === 200) {
                    console.log("res?sum??",response)
                    setSummaryOne(response?.data?.Message??"");
                }

            } catch (error) {
                console.log("API Error:",error)
            }
    }

    const summaryTwo=async()=>{
        try {
                const url: string = `${config().NLP_CAMPAIGN_URL}/${import.meta.env.VITE_APP_ENV=="development"?'All_campaign_summaries':'All_campaign_summaries_live'}`;
                const response = await axios.post(
                    url,
                    { domainId: userData?.domainId,
                    chatId:selectedCampaign?.CampaignChatId
                    },

                    {
                    headers: {
                        "Content-Type": "application/json",
                    },
                    }
                )

                if (response?.status === 200) {
                    console.log("res???",response)
                    setSummary(response?.data?.Message??"");
                }

            } catch (error) {
                console.log("API Error:",error)
            }
    }
    useEffect(()=>{
        const init=async()=>{
            console.log("res---",selectedCampaign);
            await handlesummary();
            await summaryTwo();
            
        }
        init();
    },[])
return (
<div className={styles.wrapper}>
    <div style={{cursor:"pointer"}} className='breadcrumbs' onClick={()=>handleChange()}>
                        <ol>
                            <li>
                                {/* <Link to=''>Connectors</Link> */}
                            </li>
                            {/* <li>Past Campaigns</li> */}
                            <li>{selectedCampaign?.campaign_Details?JSON?.parse(selectedCampaign?.campaign_Details)?.[0]?.CampaignName??"-":""}</li>
                        </ol>
                    </div>
{/* <div>
    <p onClick={()=>handleChange()}>Past Campaigns</p>
</div> */}


        {/* <div className={styles.progressSection}>
            <h4 className={styles.progresstitle}>Campaign progress</h4>
            <div className={styles.progressHeader}>
                <span>100%</span>
            </div>
            <ProgressBar progress={50} />
            <p className={styles.subtext}>Goal achieved so far</p>
        </div>


        <div className={styles.summary}>
            <h3>📊 Campaign Performance Summary:</h3>
            <p>Your multi-channel campaign reached audiences across Email, SMS, and WhatsApp, achieving a strong overall performance this cycle.</p>
            <ul>
                <li>Email delivered to 120,000 recipients, with an open rate of 42% and click-through rate of 7.5%.</li>
                <li>SMS engaged 85,000 users, generating 6,200 link clicks and 1,480 conversions.</li>
                <li>WhatsApp saw 56,000 active conversations, with response engagement at 63% and 2,300 direct inquiries logged.</li>
            </ul>
        </div>


        <div className={styles.resultBox}>
            <div className={styles.stackingicons}>
                <span className={styles.icon}> <img src={assets.emailicncard}></img></span>
                <span className={styles.icon}><img src={assets.smsicncard}></img> </span>
                <span className={styles.icon}> <img src={assets.whtsappicncard}></img></span>
            </div>
            <p>
                Across all channels, your campaign drove <strong>4,800 total conversions</strong> and sustained an
                average engagement rate of <strong>58%</strong>.
            </p>
            <p className={styles.bottomNote}>
                Your campaign has officially completed and all engagement data has been synced and finalized — ready for
                analysis and optimization of your next cycle.
            </p>
        </div> */}

        <div
            className="chatContent"
            dangerouslySetInnerHTML={{
                __html: summaryone || "",
            }}
        />
        <div
            className="chatContent"
            dangerouslySetInnerHTML={{
                __html: summary || "",
            }}
        />
</div>
);
};


export default PastCampaignStatus;