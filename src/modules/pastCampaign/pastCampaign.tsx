import React, { useEffect, useState } from 'react'
import styles from "./scss/pastcampaign.module.scss"
import PastCampaignStatus from './campaignsts'
import { assets, leftnavassets } from '../../../src/styles/assets/assets'
import { whatsappService } from '../../base/service/apiservice'
import moment from 'moment'

const PastCampaign = () => {
  const [pastCampaignList,setPastCampaignList] =  useState<any>([]);
  const [pastCampaignListCopy,setPastCampaignListCopy] =  useState<any>([]);
  const [showCampaign,setShowCampaign] = useState<any>(false);
  const [selectedCampaign,setSelectedCampaign] = useState<any>({});

  const handleGetPastCampaign = async () => {
      try {
        await whatsappService.get(`/get_past_campaign_list`)
          .then((response) => {
            if (response?.data?.statusCode === 200 && response?.data?.getPastCampaignListResponse?.length > 0) {
              console.log("res???:", response?.data?.getPastCampaignListResponse)
              setPastCampaignList(response?.data?.getPastCampaignListResponse);
              setPastCampaignListCopy(response?.data?.getPastCampaignListResponse);
            }
          }).catch((error) => {
            console.log(error)
          })
      } catch (error) {
        console.log("error", error);
      }
    }

   const handleSelect=(item :any)=>{
        setSelectedCampaign(item);
        setShowCampaign(true);
        
   }

  useEffect(() => {
    handleGetPastCampaign()
  }, [])
  
  return (
    <>
        <div className={styles.metacontainer}>

    <div className={styles.maincontainer}>
      {!showCampaign &&<><div className={styles.header}>
          <div className={styles.lefts}>
            <h3>Past Campaigns</h3>
            <p>View details and performance of your completed campaigns.</p>
          </div>

        </div> 
        {
          pastCampaignList.length==0 &&
          <div className={"nodataclass"}><img src={assets.nodataimg}/>No Data found</div>
        }
  {pastCampaignList.map((item :any)=>{
    const campaignDetails = JSON.parse(item?.campaign_Details || "[]");

// Collect all channel names into one lowercase string
const channelNames = campaignDetails
  .map((c :any) => c.ChannelName?.toLowerCase() || "")
  .join(",");

      return(
        <>
         
        <div className={styles.card} onClick={()=>handleSelect(item)}>
          <div className={styles.metaRow}>
            <span>
              <strong>Created date</strong> {moment(item?.mincreated).format("DD/MM/YYYY ")}
            </span>
            <span>
              <strong>Created Time</strong> {moment(item?.mincreated).format(" HH:mm")}
            </span>
            <span>
              {/* <strong>Duration</strong> 20 Nov 2025 – 25 Nov 2025 (5 days) */}
            </span>
            <span>
              <strong>Created by</strong> {item?.createdBy??"-"}
            </span>
          </div>
          <hr></hr>

          <div className={styles.titleRow}>
            <div className={styles.lefts}>
              <h3>{JSON.parse(item?.campaign_Details)?.[0]?.CampaignName}</h3>
              {/* <span className={styles.tag}>Outbound</span> */}
              {/* <span className={styles.codeTag}>RP-2024-087</span> */}
            </div>

            <div className={styles.stackingicons}>
              {channelNames.includes("email") &&<span className={styles.icon}> <img src={assets.emailicncard}></img></span>}
              {channelNames.includes("sms")&&<span className={styles.icon}><img src={assets.smsicncard}></img> </span>}
              {channelNames.includes("whatsapp")&&<span className={styles.icon}> <img src={assets.whtsappicncard}></img></span>}
              {channelNames.includes("webpush")&&<span className={styles.icon}> <img src={assets.webpushicon}></img></span>}
              {channelNames.includes("apppush")&&<span className={styles.icon}> <img src={assets.apppushicon}></img></span>}

             
            </div>
          </div>


          {/* <p className={styles.desc}>High value customers</p> */}
        </div>
        <br />
        </>
      )
    })}
    </>
  }
       
  {showCampaign &&
<PastCampaignStatus disableCampaign={()=>setShowCampaign(false)} selectedCampaign ={selectedCampaign}/>
}
    </div>
     </div>
 
    </>
  )
}

export default PastCampaign