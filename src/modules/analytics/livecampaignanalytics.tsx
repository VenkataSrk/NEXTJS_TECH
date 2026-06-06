import React, {  useEffect, useRef, useState } from 'react';
import styles from '../../modules/settings/scss/settings.module.scss';
import ContentLayout from '../../layout/contentLayout';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { v4 as uuidv4 } from "uuid";
import { campaignSliceActions } from '../../store/slice/campaign';
import {  useNavigate } from 'react-router-dom';
import Spinner from '../../components/custom_components/spinner/spinner';
import CampaignAnalytics from './campaignAnalytics';
import { SetItemInStorage } from '../../base/customhooks/useStorage';
import { LocalStorage } from '../../base/customhooks/localStorageKeys';
import Cookies from '../../utils/cookies';
import SocketService from '../../base/service/socketService';
import { analyticsSliceActions } from '../../store/slice/analytics';
import { useSelector } from 'react-redux';


const LiveCampaignAnalytics = () => {
  const navigate = useNavigate();
  const dispatch = useDispatcher()
  const [loader,setLoader] = useState<any>(false);
  const [isNewChat, setNewChat] = useState<boolean>(true)

 const oldCampaignChatId = useSelector((state :any)=>state.commonData.liveAnalyticsCampaign)
 console.log("old",oldCampaignChatId)
  const [campaignId, setCampaignId] = useState<any>(oldCampaignChatId?.item?.CampaignChatId);
  console.log("campaignId",oldCampaignChatId?.item?.CampaignChatId)
  const isCVM = useRef<any>(false);

   const handleCreateCampaign = async () => {
     console.log("callededededede")
     speechSynthesis.cancel();
     setNewChat(true)
    //  setCampaignId(null)
     dispatch(analyticsSliceActions.reset());
    
  
   };


   useEffect(()=>{
        handleCreateCampaign();
   },[oldCampaignChatId?.item?.CampaignChatId])
   

  return (<>
  {/* {loader ? <div className='loadersinner'><Loaders/></div>: */}
  {loader ? <Spinner/>:

  
   <ContentLayout noPadding={true}>
  
      {/* <div className={`${styles.settingssec} ${isCVM.current?`settingseciframe`:''}`}>
        
        <div className={`${styles.rightsettingsmenu} rightsettingmenu ${isCVM.current?"rights":""}`}>
          <CampaignAnalytics campaignId={campaignId} isNewChat={isNewChat} CreateNewCampaign={handleCreateCampaign} />
        </div> 
    
      </div> */}


     
        <div className={`${styles.settingssec} ${isCVM.current ? `settingseciframe` : ''}`}>
          
        <div className={`${styles.rightsettingsmenu} rightsettingmenu ${isCVM.current ? "rights" : ""}`}>
          <CampaignAnalytics isCVM={isCVM.current} campaignId={oldCampaignChatId?.item?.CampaignChatId} isNewChat={isNewChat} />
        </div>
      </div>
    </ContentLayout>
}
  </>

  );
};

export default LiveCampaignAnalytics;
