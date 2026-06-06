import React, { useEffect, useState } from 'react'
import styles from "./scss/archive.module.scss"
import { assets } from '../../../src/styles/assets/assets';
import { whatsappService } from '../../base/service/apiservice';
import moment from 'moment';
import { commonDataSliceActions } from '../../store/slice/commonSlice/commonSlice';
import { useNavigate } from 'react-router-dom';
import { useDispatch, useSelector } from 'react-redux';

function Archive() {
 const navigate = useNavigate();
 const dispatch = useDispatch();
const [allArchive,setAllArchive] = useState<any>([]);
const archiveRefresh =useSelector((state :any)=>state.commonData.archiveRefresh);
console.log("allarchive",allArchive);
const campaigns = [
{
title: "TrialNudge",
date: "06 Nov, 2025",
desc: "Remind trial users to explore key features before expiry.",
},
{
title: "PlanBoost",
date: "06 Nov, 2025",
desc: "Encourage users to upgrade to higher-tier plans.",
},
{
title: "SyncAlert",
date: "05 Nov, 2025",
desc: "Notify users about integration sync issues or fixes.",
},
{
title: "ProLaunch",
date: "05 Nov, 2025",
desc: "Promote a new module, tool, or product within your suite.",
},
];

  
  const getAllArchive = async () => {
    try {
      whatsappService.get('/get_archieve_details_list').then((res: any) => {

        if (res?.data?.statusCode == 200) {
          console.log("res", res?.data?.getAllArchiveRes)
          setAllArchive(res?.data?.getAllArchiveRes);
        }
        else {
          setAllArchive([]);
        }
      })

    } catch (error: any) {
      console.log("err", error)
    }
  }

  const handleArchive = async (item: any) => {
    try {
      
      let body: any = {
        id: item?.id
      }
      await whatsappService.post(body, '/update_archive_status').then((res: any) => {
        console.log("update_archive_status", res)
        getAllArchive();
        // if (res?.data?.statusCode == 200 && res?.data?.getDraftData?.[0]?.conversation?.length > 0) {
        //     console.log("sfasfdsf", res?.data?.getDraftData?.[0]?.conversation)
        //     setConversationData(JSON.parse(res?.data?.getDraftData?.[0]?.conversation))
        //     setSelectedDraft(res?.data?.getDraftData?.[0])
        //     setActiveDraft(index);
        //     // setAllDraftData(res?.data?.getAllDraftRes);
        // }
      })
    } catch (error) {
      console.log("API ERROR:",error)
    }
  }

  const handleDelete = (campaignChatId: any) => {
    try {
      
      let body: any = {
        campaignchatid: campaignChatId
      }
      whatsappService.post(body, '/delete_draft_campaign').then((res: any) => {
        if (res?.data?.statusCode == 200) {
          getAllArchive();
          navigate('/archive')
        }
      })
    } catch (error) {
      console.log("API ERROR:",error)
    }
  }

  const handleArchiveConv=(item :any,index :any)=>{
    console.log("arch--")
    dispatch(commonDataSliceActions.setArchiveData({ "draft": item, "index": index }))
    navigate('/archiveconv')
  }

  useEffect(() => {
    getAllArchive();
  }, [])

  useEffect(()=>{
    if(archiveRefresh){
       getAllArchive();
       dispatch(commonDataSliceActions.setArchiveRefresh(false));
    }
  },[archiveRefresh])
  return (
    <>
 <div className={styles.ticketright}>

<div className={styles.maincontainer}>
  <div className={styles.header}>
       <div className={styles.lefts}>
                <h3>Archive</h3>
                <p>Keep things organised by archiving what’s no longer active.</p>
              </div>
              
  </div>
<div className={styles.wrapper}>
  {allArchive.length==0 && 
  
  <div className={"nodataclass"}><img src={assets.nodataimg}/>No Data found</div>
  }
{allArchive.map((item :any, i :any) => (
<div key={i} className={styles.card} >
<div className={styles.row} >
  <div onClick={()=>{handleArchiveConv(item,i)}} className={styles.campaignlist}>

<h3 className={styles.title}>{item.conversationName??"New Campaign"}</h3>
<span className={styles.badge}>{moment(item.updatedOn).format("DD MMM, YYYY")}</span>
  </div>
<div className={styles.actions}>
  <span className={styles.icon} onClick={()=>handleArchive(item)}><img src={assets.archiveicon}></img></span>
<span className={styles.icon} onClick={() => handleDelete(item?.campaignchatid)}> <img src={assets.delicon}></img></span>

</div>
</div>
{/* <p className={styles.desc}>{item.desc}</p> */}
<p className={styles.desc}>Keep things organised by archiving what’s no longer active.</p>
</div>
))}
</div>
</div></div>
 {/* // delete modal */}
 {/* <div className={styles.modaloverlay} >
                            <div
                                className={styles.modalcontent}
                                onClick={(e) => e.stopPropagation()} 
                            >
                                <div className={styles.modheader}>
                                    <h3>Delete</h3>
                                    
                                </div>
                                <div className={styles.containerblockindel}>
                                    <p>Are you sure to delete the role 'Test 01'?</p>
                                </div>
                                <div className={styles.btn}>
                                    <button className={styles.canelbtn}>Cancel</button>
                                    <button className={styles.redbtn}>Delete</button>
                                </div>
                            </div>
                        </div> */}


    </>
  )
}

export default Archive