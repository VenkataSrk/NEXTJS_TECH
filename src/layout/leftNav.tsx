import React, { useEffect, useState } from 'react'
import styles from './scss/leftnav.module.scss';
import { leftnavassets, assets } from '../styles/assets/assets';
import { NavLink, useLocation, useNavigate } from 'react-router-dom';
import { contactsSliceActions } from '../store/slice/contacts';
import { useDispatcher } from '../store/redux-store/dispatch';
import { commonDataSliceActions } from '../store/slice/commonSlice/commonSlice';
import { useSelector } from 'react-redux';
import { whatsappService } from '../base/service/apiservice';
import WorktualDropdown from '../components/custom_components/WroktualDropdown';
import Search from '../components/search/search';
import { campaignSliceActions } from '../store/slice/campaign';
// import Search from '../components/search/search';




const LeftNav = () => {

  // const [activeState, setActiveState] = useState('dash');
  // const handleNav = (menu: string) => {
  //   console.log("menu", menu);
  //   setActiveState(menu)
  // }
  const dispatch = useDispatcher();
  const navigate = useNavigate();
  const deActiveHome = useSelector((state: any) => state.commonData.disableHomeActive)
  let callOldCampaigns = useSelector((state :any)=>state.commonData.callOldCampaigns);
  console.log("callOldCampaigns",callOldCampaigns)
  const [settings, setSettings] = useState<any>(false);
  const [deactiveHome, setDeactiveHome] = useState<any>(false);
  const [getPastCampaign, setPastCampaign] = useState<any>(null);
  const [pastCampaignCopy, setPastCampaignCopy] = useState<any>([]);
  const [getLiveCampaign, setLiveCampaign] = useState<any>(null);
  const [liveCampaignCopy, setLiveCampaignCopy] = useState<any>([]);
  const [getFutureCampaign, setFutureCampaign] = useState<any>(null);
  const [futureCampaignCopy, setFutureCampaignCopy] = useState<any>([]);
  const [activePastMenu, setActivePastMenu] = useState<any>(-1);
  const [activeFutureMenu, setActiveFutureMenu] = useState<any>(-1);
  const [activeDraft, setActiveDraft] = useState<any>(-1);
  console.log("actddraft",activeDraft)
  const [allDraftData, setAllDraftData] = useState<any>([]);
  console.log("alldraft",allDraftData)

  const [activeRecommendation,setActiveRecommendation] = useState<any>(-1);
  const [allRecommendation,setAllRecommendation]= useState<any>([]);
  const [conversationData, setConversationData] = useState<any>([]);
  const [selectedDraft, setSelectedDraft] = useState<any>([]);
  const [open,setOpen] = useState<any>(false);
  const updateHomeAPI :any=useSelector((state :any)=>state.commonData.updateHomeAPI)
  console.log("update>>>",updateHomeAPI);  
  const selectCampaign = useSelector((state :any)=>state.commonData.multiSearchData)
    console.log("selectcamp",selectCampaign)
  const [showActions,setShowActions]=useState<any>(-1);
  const initialdropdownleftmenu = {
    draft:false,
    livecamp:false,
    schedulecamp:false,
    recommendation:false
  }
  const [leftMenuDropDown,setLeftMenuDropDown] = useState<any>(initialdropdownleftmenu);
  console.log("menudropDwon",leftMenuDropDown)
  const homeClick =()=>{
    //  const baseUrl =
    //   import.meta.env.VITE_APP_ENV === 'development'
    //     ? 'http://localhost:7000'
    //     // ? 'https://qacampaign.worktual.co.uk'
    //     : 'https://campaign.worktual.co.uk';
    // window.location.href = `${baseUrl}/home`;
    setActivePastMenu(-1);
    setActiveFutureMenu(-1);
    setActiveDraft(-1);
    setActiveRecommendation(-1);
    setDeactiveHome(false)
    dispatch(commonDataSliceActions.setShowBuildCampaign(false));
    dispatch(commonDataSliceActions.setDisableHomeActive(false));
    dispatch(commonDataSliceActions.setCreateNewCampaign(true));
    dispatch(commonDataSliceActions.setShowMainChat(true));
    dispatch(commonDataSliceActions.setClearFiles(true));
  }

  // const createCampaign =()=>{
  //   dispatch(commonDataSliceActions.setIsNewCampaign(true));
  //   dispatch(commonDataSliceActions.setShowBuildCampaign(false));
  //   // dispatch(commonDataSliceActions.setShowBuildCampaign(true));
  //   dispatch(commonDataSliceActions.setCreateNewCampaign(true));
  //   setDeactiveHome(true);
  //   dispatch(commonDataSliceActions.setDisableHomeActive(true));
  //   dispatch(commonDataSliceActions.setShowMainChat(true));
  //   navigate('/home');
  // }

    const createCampaign=()=>{
      dispatch(campaignSliceActions.reset());
      dispatch(commonDataSliceActions.setInitialChatMsg("create campaign"));
      dispatch(commonDataSliceActions.setShowBuildCampaign(false));
      dispatch(commonDataSliceActions.setDisableHomeActive(true));
      dispatch(commonDataSliceActions.setIsNewCampaign(true));
      dispatch(commonDataSliceActions.setCreateNewCampaign(true));
      dispatch(commonDataSliceActions.setShowMainChat(true));
      dispatch(commonDataSliceActions.setClearFiles(true));
      setActivePastMenu(-1);
      navigate('/home');
  }

  const handleGetPastCampaign = async () => {
      try {
        await whatsappService.get(`/get_past_campaign`)
          .then((response) => {
            if (response?.data?.statusCode === 200 && response?.data?.getPastCampaignListResponse?.length > 0) {
              setPastCampaign(response?.data?.getPastCampaignListResponse);
              setPastCampaignCopy(response?.data?.getPastCampaignListResponse);
            }
          }).catch((error) => {
            console.log(error)
          })
      } catch (error) {
        console.log("error", error);
      }
    }

  const onclickCampaign=(item:any,index:number,campaignState :any)=>{
    try {
      console.log("onclicksdfsfsf")
      if (campaignState == "past") {
        if(index==activePastMenu){
           console.log("ret1")
           return ;
        }
        setActivePastMenu(index);
        setActiveFutureMenu(-1);
      } else {
        if(index==activeFutureMenu){
           console.log("ret2")
          return;
        }
        setActivePastMenu(-1);
        setActiveFutureMenu(index);

      }
      // dispatch(commonDataSliceActions.setPastCampaign({"item":item},{"index":index}))
      dispatch(commonDataSliceActions.setLiveAnalyticsCampaign({"item":item},{"index":index}))
      // dispatch(commonDataSliceActions.setDisableHomeActive(true));
      // dispatch(commonDataSliceActions.setShowMainChat(true));
      // navigate('/home')
      navigate('/analyticssummary')
      
    } catch (error) {
      console.log(error)
    }
  }

  const handleGetLiveCampaign = async () => {
      try {
        await whatsappService.get(`/get_live_campaign_list`)
          .then((response) => {
            if (response?.data?.statusCode === 200 && response?.data?.getLiveCampaignListResponse?.length > 0) {
              console.log("res???:",response?.data?.getLiveCampaignListResponse)
              setLiveCampaign(response?.data?.getLiveCampaignListResponse);
              setLiveCampaignCopy(response?.data?.getLiveCampaignListResponse);
            }
          }).catch((error) => {
            console.log(error)
          })
      } catch (error) {
        console.log("error", error);
      }
    }

  const handleGetFutureCampaign = async () => {
    try {
      await whatsappService.get(`/get_Future_campaign_list`)
        .then((response) => {
          if (response?.data?.statusCode === 200 && response?.data?.getFutureCampaignListResponse?.length > 0) {
            console.log("res???:", response?.data?.getFutureCampaignListResponse)
            setFutureCampaign(response?.data?.getFutureCampaignListResponse);
            setFutureCampaignCopy(response?.data?.getFutureCampaignListResponse);
          }
        }).catch((error) => {
          console.log(error)
        })
    } catch (error) {
      console.log("error", error);
    }
  }

      // const getIndividualDraft = async (data: any, index: any) => {
      //     console.log("getIndividualDraftsss", data, index)
      //     try {
      //         let body = {
      //             id: data?.id,
      //             campaignchatid: data?.campaignchatid,
      //             conversationName: data?.conversationName
      //             // conversationName: data?.conversationName??"New Campaign"
      //         }
  
  
      //         await whatsappService.post(body, '/get_draft_details_campaign').then((res: any) => {
      //             console.log("getIndividualDraft", res)
      //             if (res?.data?.statusCode == 200 && res?.data?.getDraftData?.[0]?.conversation?.length > 0) {
      //                 console.log("sfasfdsf", res?.data?.getDraftData?.[0]?.conversation)
      //                 setConversationData(JSON.parse(res?.data?.getDraftData?.[0]?.conversation))
      //                 setSelectedDraft(res?.data?.getDraftData?.[0])
      //                 setActiveDraft(index);
      //                 // setAllDraftData(res?.data?.getAllDraftRes);
      //             }
      //         })
  
      //     } catch (error: any) {
      //         console.log("err", error)
      //     }
      // }

      const getAllDraft = async () => {
              try {
                  whatsappService.get('/get_all_draft_campaign').then((res: any) => {
                      console.log("getAllDraftres", res)
                      if (res?.data?.statusCode == 200) {
      
                          // getIndividualDraft(res?.data?.getAllDraftRes?.[0], 0)
                          setAllDraftData(res?.data?.getAllDraftRes);
                          setAllRecommendation(res?.data?.getAllDraftRes);
                      }
                      else {
                          setAllDraftData([]);
                      }
                  })
      
              } catch (error: any) {
                  console.log("err", error)
              }
          }

        const getAllRecommendation = async () => {
              try {
                  // whatsappService.get('/get_all_draft_campaign').then((res: any) => {
                  //     console.log("getAllDraftres", res)
                  //     if (res?.data?.statusCode == 200) {
      
                  //         // getIndividualDraft(res?.data?.getAllDraftRes?.[0], 0)
                        
                  //         setAllRecommendation(res?.data?.getAllDraftRes);
                  //     }
                  //     else {
                  //         setAllDraftData([]);
                            // setAllRecommendation([]);
                  //     }
                  // })
      
              } catch (error: any) {
                  console.log("err", error)
              }
          }

      const handleArchive =async(item :any)=>{
        try {
          
          let body :any={
            id:item?.id
          }
             await whatsappService.post(body, '/update_archive_status').then((res: any) => {
                            console.log("update_archive_status", res)
                            getAllDraft();
                            // if (res?.data?.statusCode == 200 && res?.data?.getDraftData?.[0]?.conversation?.length > 0) {
                            //     console.log("sfasfdsf", res?.data?.getDraftData?.[0]?.conversation)
                            //     setConversationData(JSON.parse(res?.data?.getDraftData?.[0]?.conversation))
                            //     setSelectedDraft(res?.data?.getDraftData?.[0])
                            //     setActiveDraft(index);
                            //     // setAllDraftData(res?.data?.getAllDraftRes);
                            // }
                        })
        } catch (error) {
          console.log("API Error:",error)
        }
      }

      const handleDelete = (campaignChatId :any) => {
              try {
                
                let body: any = {
                    campaignchatid: campaignChatId
                }
                whatsappService.post(body, '/delete_draft_campaign').then((res: any) => {
                    if (res?.data?.statusCode == 200) {
                        getAllDraft();
                      
                    }
                })
              } catch (error) {
                console.log("API ERROR:",error)
              }
          }

        const handleDeleteRecommendation = (campaignChatId: any) => {
          try {

            let body: any = {
              campaignchatid: campaignChatId
            }
            // whatsappService.post(body, '/delete_draft_campaign').then((res: any) => {
            //   if (res?.data?.statusCode == 200) {
            
            // getAllRecommendation();

            //   }
            // })
          } catch (error) {
            console.log("API ERROR:", error)
          }
        }

useEffect(()=>{
    if(window.location.href=="https://campaign.worktual.co.uk/addWorktualAddress" || window.location.href =="https://campaign.worktual.co.uk/setupSmtpEmail" || window.location.href=="http://localhost:4000/addWorktualAddress"||window.location.href=="http://localhost:4000/setupSmtpEmail"){
      setSettings(true)
    }
    else{
      setSettings(false)
    }
   
  speechSynthesis.cancel();
},[window.location.href])



useEffect(()=>{
  if(location.pathname!=='/home'){
    dispatch(commonDataSliceActions.setShowBuildCampaign(false));
  }
  handleGetPastCampaign();
  handleGetLiveCampaign();
  handleGetFutureCampaign();
  getAllDraft();
  getAllRecommendation();
},[])

useEffect(()=>{
 
 if(location.pathname!=='/drafts'){
      setActiveDraft(-1);
    }
},[window.location.href])

useEffect(()=>{
  if(callOldCampaigns){
    handleGetPastCampaign();
    handleGetLiveCampaign();
    handleGetFutureCampaign();
    getAllDraft();
    getAllRecommendation();
    dispatch(commonDataSliceActions.setCallOldCampaign(false));
  }
},[callOldCampaigns])

useEffect(()=>{
if(selectCampaign?.data){
  if(selectCampaign?.campType=="schedule"){
    onclickCampaign(selectCampaign?.data,selectCampaign?.index,"future")
  }else if(selectCampaign?.campType=="live"){
    onclickCampaign(selectCampaign?.data,selectCampaign?.index,"past")
  }else if(selectCampaign?.campType=="draft"){
    
    setActiveDraft(selectCampaign?.index)
    setActivePastMenu(-1)
    setActiveFutureMenu(-1)
     setActiveRecommendation(-1)
    dispatch(commonDataSliceActions.setDraftData({ "draft": selectCampaign?.data, "index": selectCampaign?.index }))
    navigate("/drafts");
  }
  dispatch(commonDataSliceActions.setMultiSearchData({}));
}

},[selectCampaign?.data?.CampaignChatId,selectCampaign?.data?.campaignchatid])

  useEffect(() => {
    setShowActions(-1);
  }, [activeDraft])

  useEffect(()=>{
    if(updateHomeAPI){

      handleGetLiveCampaign();
      handleGetFutureCampaign();
      getAllDraft();
      dispatch(commonDataSliceActions.setUpdateHomeAPI(false));
    }
  },[updateHomeAPI])


  return (
    <div className={styles.leftnav}>
      <div className={styles.createcamleftnav}>
           <span style={{display:"grid"}}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
  <path d="M10 0C4.5 0 0 4.5 0 10C0 15.5 4.5 20 10 20C15.5 20 20 15.5 20 10C20 4.5 15.5 0 10 0ZM10 18.5C5.3 18.5 1.5 14.7 1.5 10C1.5 5.3 5.3 1.5 10 1.5C14.7 1.5 18.5 5.3 18.5 10C18.5 14.7 14.7 18.5 10 18.5Z" fill="#7423D7"/>
  <path d="M14.3 9.3H10.8V5.7C10.8 5.3 10.4 5 10 5C9.6 5 9.2 5.3 9.2 5.8V9.3H5.7C5.3 9.3 5 9.6 5 10C5 10.4 5.3 10.8 5.8 10.8H9.3V14.3C9.3 14.7 9.6 15.1 10.1 15.1C10.6 15.1 10.9 14.8 10.9 14.3V10.8H14.4C14.8 10.8 15.2 10.5 15.2 10C15.2 9.5 14.7 9.3 14.3 9.3Z" fill="#7423D7"/>
</svg></span>
<p
  onClick={() => {
    // const baseUrl =
    //   import.meta.env.VITE_APP_ENV === 'development'
    //     ? 'http://localhost:7000'
    //     // ? 'https://qacampaign.worktual.co.uk'
    //     : 'https://campaign.worktual.co.uk';
    // window.location.href = `${baseUrl}/home`;
   
   createCampaign();
    
  }}
>
  Create Campaign
</p>
      </div>
      <ul className={styles.ltnverticallists}>
        <li className={styles.ltnlists}>
          <NavLink to={'/home'} className={styles.ltnnavlink} viewTransition>
            {
              ({ isActive }) => (
                <div className={`${styles.ltnlistitem} ${(isActive && !deActiveHome)? styles.ltnactiveitem : ''}`} onClick={()=>homeClick()}>
                  <img src={(isActive && !deActiveHome )? leftnavassets.home_active : leftnavassets.home_inactive} alt="customerprofile_inactive" /><span>Home</span>
                </div>
              )
            }
          </NavLink>
        </li>
        {/* <li className={styles.ltnlists}>
          <NavLink to={'/contacts'} className={styles.ltnnavlink} viewTransition>
            {
              ({ isActive }) => (
                <div className={`${styles.ltnlistitem} ${isActive ? styles.ltnactiveitem : ''}`}>
                  <img src={isActive ? leftnavassets.customerprofile_active : leftnavassets.customerprofile_inactive} alt="customerprofile_inactive" /><span>Contacts</span>
                </div>
              )
            }
          </NavLink>
        </li>
        <li className={styles.ltnlists}>
          <NavLink to={'/reports'} className={styles.ltnnavlink} viewTransition>
            {
              ({ isActive }) => (
                <div className={`${styles.ltnlistitem} ${isActive ? styles.ltnactiveitem : ''}`}>
                  <img src={isActive ? leftnavassets.analtcs_active : leftnavassets.analtcs_inactive} alt="reports_inactive" /><span>Reports</span>
                </div>
              )
            }
          </NavLink>
        </li>
        <li className={styles.ltnlists}>
          <NavLink to={'/analytics'} className={styles.ltnnavlink} viewTransition>
            {
              ({ isActive }) => (
                <div className={`${styles.ltnlistitem} ${isActive ? styles.ltnactiveitem : ''}`}>
                  <img src={isActive ? leftnavassets.reports_active : leftnavassets.reports_inactive} alt="reports_inactive" /><span>Analytics</span>
                </div>
              )
            }

          </NavLink>
        </li>
        {/* <li className={styles.ltnlists}>
          <NavLink to={'/knowledgebase'} className={styles.ltnnavlink} viewTransition>
            {
              ({ isActive }) => (
                <div className={`${styles.ltnlistitem} ${isActive ? styles.ltnactiveitem : ''}`}>
                  <img src={ isActive ? leftnavassets.knowledgebaseactive  :  leftnavassets.knowledgebaseinactive } alt="reports_inactive" /><span>Knowledge Base</span>
                </div>
              )
            }

          </NavLink>
        </li> */}
        {/* <li className={styles.ltnlists}>
          <NavLink to={'/drafts'} className={styles.ltnnavlink} viewTransition>
            {
              ({ isActive }) => (
                <div className={`${styles.ltnlistitem} ${isActive ? styles.ltnactiveitem : ''}`}>
                  <img src={leftnavassets.reports_inactive} alt="reports_inactive" /><span>Drafts</span>
                </div>
              )
            }

          </NavLink>
        </li> */}
        {/* <li className={styles.ltnlists}>
          <NavLink to={'/settings'} className={styles.ltnnavlink} viewTransition>
            {
              ({ isActive }) => (
                <div className={`${styles.ltnlistitem} ${(isActive ||settings) ? styles.ltnactiveitem : ''}`}>
                  <img src={(isActive ||settings)? leftnavassets.setting_active : leftnavassets.setting_inactive} alt="setting" /><span>Settings</span>
                </div>
              )
            }

          </NavLink>
        </li> */}
      </ul>
      <ul className={`${styles.ltnverticallists} ${styles.searchlftnav}`}>
          <li className={styles.ltnlists} onClick={()=>setOpen(true)}>
               <div className={`${styles.ltnlistitem} `}>
                  <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
  <path d="M18.8569 17.7998L12.6069 11.5498C12.5938 11.5367 12.5763 11.5322 12.5625 11.5203C12.7922 11.236 13.0081 10.9364 13.1965 10.61C15.1465 7.23 13.9865 2.9 10.6065 0.95C9.49646 0.31 8.27646 0 7.07646 0C4.63646 0 2.25646 1.27 0.946456 3.54C-1.00354 6.92 0.156455 11.25 3.53646 13.2C4.64646 13.84 5.86646 14.15 7.06646 14.15C8.67395 14.15 10.2486 13.5887 11.5152 12.5635C11.5277 12.5781 11.5325 12.5965 11.5463 12.6103L17.7963 18.8603C17.9428 19.0068 18.1342 19.08 18.3266 19.08C18.519 19.08 18.7104 19.0068 18.8569 18.8603C19.1499 18.5673 19.1499 18.0927 18.8569 17.7998ZM7.06656 12.65C6.09656 12.65 5.13656 12.39 4.28656 11.9C2.99656 11.1599 2.07656 9.95 1.68656 8.52C1.29656 7.08 1.49656 5.58 2.24656 4.29C3.23656 2.57 5.09656 1.5 7.07656 1.5C8.04656 1.5 9.00655 1.76 9.85655 2.25C11.1466 2.99 12.0666 4.2 12.4566 5.63C12.8466 7.07 12.6466 8.57 11.8966 9.86C10.9065 11.58 9.04656 12.65 7.06656 12.65Z" fill="black"/>
</svg><span>Search</span>
                </div>
          </li>
      </ul>

{/* {getPastCampaign?.length>0 ?          
          getPastCampaign.map((item :any,ind :any)=>{
            return(
            <ul>
              <li
                className={`${ind===activePastMenu?styles.activemenuitem:''}`}
                onClick={()=>onclickCampaign(item,ind)}
              >{JSON.parse(item?.campaign_Details)?.[0]?.CampaignName}
               
              </li>
            </ul>
            )
          }): */}
          <div className={styles.historyscroller}>

          
      {getLiveCampaign?.length>0 &&    
       <ul className={`${styles.ltnverticallists} ${styles.campaignhistory}`}>
          <li className={styles.ltnlists}>
               <div className={`${styles.ltnlistitem} ${styles.header}`} onClick ={()=>setLeftMenuDropDown((prev :any)=>({...prev,"livecamp":!prev?.livecamp}))}>
                   Live Campaigns
                   <span className={styles.chevron} ><svg xmlns="http://www.w3.org/2000/svg" style={{rotate:` ${leftMenuDropDown?.livecamp ? "180deg" :""}`}} width="12" height="6" viewBox="0 0 12 6" fill="none">
                  <path d="M10.5809 5.8065C10.3885 5.8065 10.1961 5.7333 10.0497 5.5858L6.19621 1.7206C6.05461 1.579 5.86611 1.5009 5.66541 1.4999C5.46471 1.4999 5.27581 1.579 5.13371 1.7206L1.28121 5.5858C0.989214 5.8797 0.513614 5.8797 0.220714 5.5878C-0.0727862 5.2948 -0.0736861 4.8202 0.218714 4.5273L4.07171 0.662101C4.49751 0.235301 5.06341 0 5.66551 0C6.26761 0.001 6.83351 0.235401 7.25881 0.662101L11.1123 4.5273C11.4048 4.8203 11.4038 5.2949 11.1103 5.5878C10.9643 5.7333 10.7723 5.8065 10.5809 5.8065Z" fill="#404040" />
                </svg></span >
                </div>
                 {
                leftMenuDropDown?.livecamp &&
                 <div className={styles.prevcamcontainer}>
                  {getLiveCampaign?.length>0 ?          
                    getLiveCampaign.map((item :any,ind :any)=>{
                      return(
                              <li className={ind==activePastMenu?styles.active:''} key={ind} onClick={()=>onclickCampaign(item,ind,"past")}>{JSON.parse(item?.campaign_Details)?.[0]?.CampaignName}</li>
                              // <li className={ind==activePastMenu?styles.active:''} key={ind} onClick={()=>onclickCampaign(item,ind,"past")}>{JSON.parse(item?.campaign_Details)?.[0]?.CampaignName}</li>
                                )
                    }):<></>}
                      
                 </div>
                }
          </li>
      </ul>
      }

       {getFutureCampaign?.length>0 &&  <ul className={`${styles.ltnverticallists} ${styles.campaignhistory}`}>
          <li className={styles.ltnlists}>
               <div className={`${styles.ltnlistitem} ${styles.header}`} onClick ={()=>setLeftMenuDropDown((prev :any)=>({...prev,"schedulecamp":!prev?.schedulecamp}))}>
                   Scheduled Campaigns
                   <span className={styles.chevron} ><svg xmlns="http://www.w3.org/2000/svg" style={{rotate:` ${leftMenuDropDown?.schedulecamp ? "180deg" :""}`}} width="12" height="6" viewBox="0 0 12 6" fill="none">
                  <path d="M10.5809 5.8065C10.3885 5.8065 10.1961 5.7333 10.0497 5.5858L6.19621 1.7206C6.05461 1.579 5.86611 1.5009 5.66541 1.4999C5.46471 1.4999 5.27581 1.579 5.13371 1.7206L1.28121 5.5858C0.989214 5.8797 0.513614 5.8797 0.220714 5.5878C-0.0727862 5.2948 -0.0736861 4.8202 0.218714 4.5273L4.07171 0.662101C4.49751 0.235301 5.06341 0 5.66551 0C6.26761 0.001 6.83351 0.235401 7.25881 0.662101L11.1123 4.5273C11.4048 4.8203 11.4038 5.2949 11.1103 5.5878C10.9643 5.7333 10.7723 5.8065 10.5809 5.8065Z" fill="#404040" />
                </svg></span >
                </div>
                 {
                leftMenuDropDown?.schedulecamp &&
                 <div className={styles.prevcamcontainer}>
                  {getFutureCampaign?.length>0 ?          
                    getFutureCampaign.map((item :any,ind :any)=>{
                      return(
                              <li className={ind==activeFutureMenu?styles.active:''} key={ind} onClick={()=>onclickCampaign(item,ind,"future")}>{JSON.parse(item?.campaign_Details)?.[0]?.CampaignName}</li>
                                )
                    }):<></>}
                      
                 </div>
                }
          </li>
      </ul>}

            {/* {allRecommendation.length>0 &&
<ul className={`${styles.ltnverticallists} ${styles.drafthistory}`}>
          <li className={styles.ltnlists}>
               <div className={`${styles.ltnlistitem} ${styles.header}`}>
                  Recommendation Campaign
                </div>
                 
                 <div className={styles.prevcamcontainer}>
                  {allRecommendation.map((item: any, index: any) => {
                            return (
                              

                              <li
                                className={activeRecommendation === index ? styles.active : ""}
                                onClick={() => {
                                  setActiveRecommendation(index)
                                  setActivePastMenu(-1)
                                  setActiveFutureMenu(-1)
                                  setActiveRecommendation(-1)
                                  dispatch(commonDataSliceActions.setDraftData({ "draft": item, "index": index }))
                                  navigate("/drafts");
                                }

                                }
                              > {item?.conversationName ?? "New Campaign"} <span style={{ display: "none", position: "relative" }}>
                                <svg xmlns="http://www.w3.org/2000/svg" width="18" height="5" viewBox="0 0 18 5" fill="none" onClick={()=>setShowActions(showActions==-1?index:-1)}>
                                <path fill-rule="evenodd" clip-rule="evenodd" d="M13.461 2.2435C13.461 3.4774 14.4706 4.487 15.7045 4.487C16.9384 4.487 17.948 3.4774 17.948 2.2435C17.948 1.0096 16.9384 0 15.7045 0C14.4706 0 13.461 1.0096 13.461 2.2435ZM11.2175 2.2435C11.2175 1.0096 10.2079 0 8.974 0C7.7401 0 6.7305 1.0096 6.7305 2.2435C6.7305 3.4774 7.7401 4.487 8.974 4.487C10.2079 4.487 11.2175 3.4774 11.2175 2.2435ZM2.2435 0C3.4774 0 4.487 1.0096 4.487 2.2435C4.487 3.4774 3.4774 4.487 2.2435 4.487C1.0096 4.487 0 3.4774 0 2.2435C0 1.0096 1.0096 0 2.2435 0Z" fill="#404040" />
                              </svg>  
                              {(showActions==index) &&
                                <div
                                style={{
                                  position: "absolute",
                                  top: 16,
                                  left: -99,
                                  background: "white",
                                  boxShadow: "0 2px 6px rgba(0,0,0,0.15)",
                                  borderRadius: "6px",
                                  zIndex: 9999999999,
                                  width: "120px",
                                  padding: "4px 0",
                                }}
                              >
                                    <div className="dropdown-item"  onClick={() => handleDeleteRecommendation(item?.campaignchatid)}>
                                      Delete
                                    </div>
                                  </div> } </span></li> 
                            )
                        })}
                 </div>
          </li>
      </ul>
      } */}

       {allDraftData.length>0 &&
      <ul className={`${styles.ltnverticallists} ${styles.drafthistory}`}>
          <li className={styles.ltnlists}>
               <div className={`${styles.ltnlistitem} ${styles.header}`} onClick ={()=>setLeftMenuDropDown((prev :any)=>({...prev,"draft":!prev?.draft}))}>
                  Drafts
                <span className={styles.chevron} ><svg xmlns="http://www.w3.org/2000/svg" style={{rotate:` ${leftMenuDropDown?.draft ? "180deg" :""}`}} width="12" height="6" viewBox="0 0 12 6" fill="none">
                  <path d="M10.5809 5.8065C10.3885 5.8065 10.1961 5.7333 10.0497 5.5858L6.19621 1.7206C6.05461 1.579 5.86611 1.5009 5.66541 1.4999C5.46471 1.4999 5.27581 1.579 5.13371 1.7206L1.28121 5.5858C0.989214 5.8797 0.513614 5.8797 0.220714 5.5878C-0.0727862 5.2948 -0.0736861 4.8202 0.218714 4.5273L4.07171 0.662101C4.49751 0.235301 5.06341 0 5.66551 0C6.26761 0.001 6.83351 0.235401 7.25881 0.662101L11.1123 4.5273C11.4048 4.8203 11.4038 5.2949 11.1103 5.5878C10.9643 5.7333 10.7723 5.8065 10.5809 5.8065Z" fill="#404040" />
                </svg></span >
                </div>
               {
                leftMenuDropDown?.draft && 
                 <div className={styles.prevcamcontainer}>
                  {allDraftData.map((item: any, index: any) => {
                            return (
                              

                              <li
                                className={activeDraft === index ? styles.active : ""}
                                onClick={() => {
                                  setActiveDraft(index)
                                  setActivePastMenu(-1)
                                  setActiveFutureMenu(-1)
                                  setActiveRecommendation(-1)
                                  dispatch(commonDataSliceActions.setDraftData({ "draft": item, "index": index }))
                                  navigate("/drafts");
                                }

                                }
                              > <span className={styles.prevcamlicontent}>{item?.conversationName ?? "New Campaign"} </span><span style={{ display: "none", position: "relative" }}>
                                <svg xmlns="http://www.w3.org/2000/svg" width="18" height="5" viewBox="0 0 18 5" fill="none" onClick={()=>setShowActions(showActions==-1?index:-1)}>
                                <path fill-rule="evenodd" clip-rule="evenodd" d="M13.461 2.2435C13.461 3.4774 14.4706 4.487 15.7045 4.487C16.9384 4.487 17.948 3.4774 17.948 2.2435C17.948 1.0096 16.9384 0 15.7045 0C14.4706 0 13.461 1.0096 13.461 2.2435ZM11.2175 2.2435C11.2175 1.0096 10.2079 0 8.974 0C7.7401 0 6.7305 1.0096 6.7305 2.2435C6.7305 3.4774 7.7401 4.487 8.974 4.487C10.2079 4.487 11.2175 3.4774 11.2175 2.2435ZM2.2435 0C3.4774 0 4.487 1.0096 4.487 2.2435C4.487 3.4774 3.4774 4.487 2.2435 4.487C1.0096 4.487 0 3.4774 0 2.2435C0 1.0096 1.0096 0 2.2435 0Z" fill="#404040" />
                              </svg>  
                              {(showActions==index) &&
                                <div
                                style={{
                                  position: "absolute",
                                  top: 16,
                                  left: -99,
                                  background: "white",
                                  boxShadow: "0 2px 6px rgba(0,0,0,0.15)",
                                  borderRadius: "6px",
                                  zIndex: 9999999999,
                                  width: "120px",
                                  padding: "4px 0",
                                }}
                              >
                                    <div className="dropdown-item" onClick={()=>handleArchive(item)}>
                                      Archive
                                    </div>
                                    <div className="dropdown-item"  onClick={() => handleDelete(item?.campaignchatid)}>
                                      Delete
                                    </div>
                                  </div> } </span></li> 
                            )
                        })}
                 </div>
               }  
                
          </li>
      </ul>
      }


      </div>
  
  {open &&  <Search open={open} setOpen={()=>setOpen(false)}/>}

    </div>
  )
}

export default LeftNav;