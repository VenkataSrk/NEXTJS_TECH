import React, { useEffect, useRef, useState } from 'react';
import styles from '../../modules/settings/scss/settings.module.scss';
import ContentLayout from '../../layout/contentLayout';
import Chatcomponent from '../../components/chatcomponent/chatcomponent';
import { assets } from '../../styles/assets/assets'
import { knowledgeDataSliceActions } from '../../store/slice/knowledgeBaseChat';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { useSelector } from 'react-redux';
import { whatsappService } from '../../base/service/apiservice';
import Createcampaigns from './campaign/createcampaign';
import { v4 as uuidv4 } from "uuid";
import campaign, { campaignSliceActions } from '../../store/slice/campaign';
import { useLocation, useNavigate, useSearchParams } from 'react-router-dom';
import { decryptAESCVM, isJSON } from '../../base/utils';
import Loaders from '../../components/loader/loader';
import { LoginService } from '../../base/service/loginService';
import { SetItemInStorage } from '../../base/customhooks/useStorage';
import { LocalStorage } from '../../base/customhooks/localStorageKeys';
import Cookies from '../../utils/cookies';
import SocketService from '../../base/service/socketService';
import { getToken } from 'firebase/messaging';
import { messaging } from '../../firebase';
import CreateTemplates from './campaign/templtespace';
import { commonDataSliceActions } from '../../store/slice/commonSlice/commonSlice';
import Spinner from '../../components/custom_components/spinner/spinner';


const Home = () => {
  const navigate = useNavigate();
  const dispatch = useDispatcher()
  const location = useLocation();
  const [searchParams] = useSearchParams();
  const [loader, setLoader] = useState<any>(true);
  console.log("loader", loader);
  const knowledgeMessageData = useSelector((state: any) => state.knowledgeBaseData);
  const campaignData = useSelector((state: any) => state.campaignData.chatType);
  const campaignMessageData = useSelector((state: any) => state.campaignData);
  const createNewCampaign=useSelector((state :any)=>state.commonData.createNewCampaign);
  const setPastCampaignData = useSelector((state :any)=>state.commonData.pastCampaignData);
  const isCVM = useRef<any>(false);
  console.log("campmesdata", campaignMessageData)
  console.log("campdata", campaignData);
  const [getPastCampaign, setPastCampaign] = useState<any>(null);
  const [pastCampaignCopy, setPastCampaignCopy] = useState<any>([]);
  const [searchName, setSearchName] = useState<string>('');
  const [activePastMenu, setActivePastMenu] = useState<any>(-1);
  const setOldCamp = useRef<any>(null);
  const [campaignId, setCampaignId] = useState<any>('')
  console.log("campaignId",campaignId)
  
  const [isNewChat, setNewChat] = useState<boolean>(true)
  const [createCampaign,setCreateCampaign]= useState<any>();
  const mainChat= useSelector((state :any)=>state.commonData.showMainChat);




  useEffect(() => {
    const init = async () => {
      dispatch(campaignSliceActions.reset());
      // await getCheckKnowledgeBaseData();
      await getKnowledgeBaseData()
      await handleGetPastCampaign(false, -1);
    }
    init();
    
  }, [])

    const getCheckKnowledgeBaseData = async () => {
      try {
        try {
          await whatsappService.get(`/get_knowledge_list`)
            .then((response) => {
              if (response?.data?.statusCode === 200 && response?.data?.getKnowledgeResponse?.length > 0) {
                console.log("res>>>",response)
              } else {
                  navigate("/onboard");
              }
            }).catch((error) => {
              console.log(error)
            })
        } catch (error) {
          console.log(error)
        }
      } catch (error) {
        console.log(error)
      }
    }

  const handleGetPastCampaign = async (afterCampaignLaunch: boolean, index: number) => {
    try {
      await whatsappService.get(`/get_past_campaign`)
        .then((response) => {
          if (response?.data?.statusCode === 200 && response?.data?.getPastCampaignListResponse?.length > 0) {
            setPastCampaign(response?.data?.getPastCampaignListResponse);
            setPastCampaignCopy(response?.data?.getPastCampaignListResponse);
            if (afterCampaignLaunch) {
              dispatch(campaignSliceActions.setCampaignChatType('new'));
              onclickCampaign(response?.data?.getPastCampaignListResponse[0], index)
            }
          }
        }).catch((error) => {
          console.log(error)
        })
    } catch (error) {
      console.log("error", error);
    }
  }
    const setSearchCampaignName = (data: string) => {
        setSearchName(data);
        if (data && data.trim() !== '') {
            const lowerCaseData = data.toLowerCase();
            const result = pastCampaignCopy?.filter((list: any) =>
                JSON.parse(list?.campaign_Details)?.[0]?.CampaignName?.toLowerCase().includes(lowerCaseData)
            );
            setPastCampaign(result);
        } else {
            setPastCampaign(pastCampaignCopy);
        }
    };

  const getKnowledgeBaseData = async () => {
    try {
      try {
        await whatsappService.get(`/get_knowledge_list`)
          .then((response) => {
            if (response?.data?.statusCode === 200 && response?.data?.getKnowledgeResponse?.length > 0) {
              dispatch(knowledgeDataSliceActions.isModalOpen(false));
            } else {
              dispatch(knowledgeDataSliceActions.isModalOpen(true));
            }
          }).catch((error) => {
            console.log(error)
          })
      } catch (error) {
        console.log(error)
      }
    } catch (error) {
      console.log(error)
    }
  }

  useEffect(() => {
    let campaignChatId = localStorage.getItem("campaignChatId");
    // if (!campaignChatId) {
    //   campaignChatId = uuidv4();
    //   localStorage.setItem("campaignChatId", campaignChatId);
    // }

     if (campaignChatId) {
      campaignChatId = uuidv4();
      localStorage.setItem("campaignChatId", campaignChatId);
      dispatch(commonDataSliceActions.setCurrentCampaignChatId(campaignChatId));
      console.log("chatid created:>>>",campaignChatId)
      console.warn("chatid 1:",campaignChatId);
    }else{
      campaignChatId = uuidv4();
      localStorage.setItem("campaignChatId", campaignChatId);
      dispatch(commonDataSliceActions.setCurrentCampaignChatId(campaignChatId));
      console.log("chatid created:>>>",campaignChatId)
       console.warn("chatid 2:",campaignChatId);
    }

    if(location?.state?.data=="draft"){
      console.log("draftttt,",location?.state?.selectedDraftData?.conversation)
      setTimeout(()=>{
        const conversation = JSON.parse(
          location?.state?.selectedDraftData?.conversation || "[]"
        );
        localStorage.setItem("campaignChatId", conversation?.[0]?.campaignChatId);
        dispatch(commonDataSliceActions.setCurrentCampaignChatId(conversation?.[0]?.campaignChatId));
  console.warn("chatid draft 3:",campaignChatId);
        dispatch(
          campaignSliceActions.setCampaignMessage(conversation ?? [])
        );
      }
      
      // localStorage.setItem("campaignChatId",JSON.parse(location?.state?.selectedDraftData?.conversation ); dispatch(campaignSliceActions.setCampaignMessage(JSON.parse(location?.state?.selectedDraftData?.conversation)??[])
      
      ,100)
      navigate(location.pathname, { replace: true, state: {} });
    }
  }, []);

  const handleCreateCampaign = async () => {
    console.log("callededededede")
    speechSynthesis.cancel();
    setNewChat(true)
    setCampaignId(null)
    setActivePastMenu(-1)
    dispatch(campaignSliceActions.reset());
    const newcampaignChatId = uuidv4();
    localStorage.setItem("campaignChatId", newcampaignChatId);
    dispatch(commonDataSliceActions.setCurrentCampaignChatId(newcampaignChatId));
    console.warn("handlecreateCampaign:::",newcampaignChatId);

  };
  const handleCVMCreateCampaign = () => {
    const newcampaignChatId = uuidv4();
    localStorage.setItem("campaignChatId", newcampaignChatId);
    dispatch(commonDataSliceActions.setCurrentCampaignChatId(newcampaignChatId));
  }

  const handleSetCampaign = (oldCampId: any, item: any) => {

    // if (oldCampId != setOldCamp.current) {
      setOldCamp.current = oldCampId;
      dispatch(campaignSliceActions.reset());
      // dispatch(campaignSliceActions.setCampaignChatType('old'));
      // dispatch(campaignSliceActions.setPastCampaignDetails(item));
      setCampaignId(item)
      localStorage.setItem("campaignChatId", oldCampId);
       dispatch(commonDataSliceActions.setCurrentCampaignChatId(oldCampId));
      console.log("chat handleset:",oldCampId);
    // }
  }

  useEffect(() => {
    const getOld = async () => {
      if (campaignData == "old") {
        dispatch(campaignSliceActions.setCampaignChatType('new'));
        await handleGetPastCampaign(true, 0);
      }
    }
    getOld();
  }, [campaignData])


  useEffect(() => {
    if (isJSON(window.name)) {
      const payload = JSON.parse(window.name);
      window.name = "";
      console.log("payload",payload);
      init(payload);
    } else {
      init(null)
    }

  }, [])

  const getBrowserName = () => {
    const ua = navigator.userAgent;

    if (ua.includes("Chrome") && !ua.includes("Edg") && !ua.includes("OPR")) return "Chrome";
    if (ua.includes("Edg")) return "Edge";
    if (ua.includes("Firefox")) return "Firefox";
    if (ua.includes("Safari") && !ua.includes("Chrome")) return "Safari";
    if (ua.includes("OPR") || ua.includes("Opera")) return "Opera";
    return "Unknown";
  };



  const insertFcmTokenKey = async (token: any) => {
    try {
      const browserType = getBrowserName();
      const data = {
        TokenId: token,
        browser: browserType
      };
      const response = await whatsappService.post(data, `/insert_fcm_token`);
      if (response?.data?.statusCode === 200) {
        console.log("success")
      }
    } catch (error) {
      console.log(error);
    }
  };

  const webpushNotification = async () => {
    try {
      const permission = await Notification.requestPermission();
      if (permission !== "granted") {
        console.warn("Notification permission denied by user");
        return;
      }
      const token = await getToken(messaging, {
        vapidKey: "BD-Y7Qftct_1IUMGgtN3TgZ80z7PJqNDegqq3RefslFL_3EkDZjbXbKAg_4sveEZY_Fm8EdJMhwT2FTd5semeus",

      });

      if (token) {
        console.log("✅ FCM Token:", token);
        insertFcmTokenKey(token);
        return token;
      } else {
        console.warn("⚠️ No registration token available. Check service worker setup.");
      }
    } catch (error) {
      console.error("❌ Error getting token:", error);
    }
  };

  const init = async (cvmanalyticsData: any) => {
    try {
      console.log("cvm",cvmanalyticsData)
      const queryparams: any = searchParams.get('cvm_data');
      if (queryparams?.length > 0) {
        setLoader(true);
        const decodedQueryparams: any = await decryptAESCVM(queryparams.replaceAll(" ", "+"));
        const params = JSON?.parse(decodedQueryparams);
        console.log("params", params?.token);
        const token = params?.token;
        // const token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1dWlkIjoiNjkzMDNjYjEtOWU0OC00YzQwLWIxYTYtMjQzZTkwMmQ0NzZlIiwidXNlcm5hbWUiOiJtZWRld2F4NTk4QGt5dHN0b3JlLmNvbSIsImRldmljZUlkIjoiMWViODgwNTItMzZlYi00NDdlLTljZWQtOWM4MzU3MjM3MjM4Iiwic2lwTG9naW5JZCI6IjI5NzkwIiwicm9sZUlkIjoiMyIsImRvbWFpbklkIjoxODY3NywiZXh0IjoiMjAwIiwiY29tcGFueUlkIjoxODc1MCwiZW5ldGVwcmlzZWlkIjoyMzQ4MSwiaG9zdEFkZHJlc3MiOiIyMzQ4MS51cmNoYXQud29ya3R1YWwuY28udWsiLCJvcmRlcklkIjoyNjQxOSwiZGlyVXNlcklkIjoxNTI0MTAsInByb2ZpbGVOYW1lIjoibWVkZXdheCBreXRzdG9yZSIsInByb2ZpbGVJbWciOiJodHRwczovL3Vyc3RvcmFnZS53b3JrdHVhbC5jby51ay9maWxlcy91c2VyLzE4NzUwLzI5NzkwXzIwMC9ib29rbmcuanBlZ18xNzUxOTg1NjA5OTUyLnBuZyIsInNvdXJjZSI6IldlYiIsImlwYWRkcmVzcyI6IjEzNi4xNDQuNTYuMjU1OjQ0MyIsImNsaSI6IiIsInNpcF9wYXNzd29yZCI6IjlUM2lkYnBEIiwiY3VzdG9tZXJfaWQiOjE4ODUxLCJjb3VudHJ5X2NvZGUiOiIwIiwibG9jYWxfbnVtYmVyIjoiMjE4NDU0OTg0MSIsImFwcF9sb2dfaWQiOjI4NDkyLCJvcmRlcl9pZCI6MjY0MTksImRwX3Bhc3N3b3JkIjoiZDg3YzQzYWI2NTY1YjQzIiwicGxhdGZvcm0iOjIzNDgxLCJ1c2VyX3N0YXR1cyI6IkF2YWlsYWJsZSIsInNpdGVfaWQiOjE3Mjg0LCJkb21haW5fbmFtZSI6IjIzNDgxLmNhbGwud29ya3R1YWwuY28udWsiLCJtZWV0aW5nX2lkIjoiNjY5LTYzMS0wNjI2IiwidmlkZW9fbWVldF9kb21haW4iOiIyMzQ4MS51cm1lZXQud29ya3R1YWwuY28udWsiLCJwcm9maWxlX2ltYWdlX3RodW1ibmFpbCI6Imh0dHBzOi8vdXJzdG9yYWdlLndvcmt0dWFsLmNvLnVrL2ZpbGVzL3VzZXIvMTg3NTAvMjk3OTBfMjAwL2Jvb2tuZy5qcGVnXzE3NTE5ODU2MDk5NTIucG5nIiwiZGlyX3VzZXJfaWQiOjE1MjQxMCwiZG9tYWluX2lkIjoxODY3Nywic2lwX2xvZ2luX2lkIjoiMjk3OTAiLCJjYWxsZXJfaWQiOiJtZWRld2F4IGt5dHN0b3JlIiwiY29tcGFueV9pZCI6MTg3NTAsImVudGVycHJpc2VpZCI6MjM0ODEsInJvbGVfaWQiOiIzIiwicHJvZmlsZV91cmwiOiJodHRwczovL3Vyc3RvcmFnZS53b3JrdHVhbC5jby51ay9maWxlcy91c2VyLzE4NzUwLzI5NzkwXzIwMC9ib29rbmcuanBlZ18xNzUxOTg1NjA5OTUyLnBuZyIsIkVtYWlsIjoibWVkZXdheDU5OEBreXRzdG9yZS5jb20iLCJob3N0X2FkZHJlc3MiOiIyMzQ4MS51cmNoYXQud29ya3R1YWwuY28udWsiLCJmaXJzdF9uYW1lIjoibWVkZXdheHMiLCJsYXN0X25hbWUiOiJreXRzdG9yZSIsImNsaWVudF9kb21haW4iOnsiaXZyIjoiaHR0cHM6Ly9pdnIud29ya3R1YWwuY28udWsiLCJ1cmFwaSI6Imh0dHBzOi8vdXJhcGkud29ya3R1YWwuY28udWsiLCJ1cnNpcCI6Imh0dHA6Ly91cnNpcC1hcGkud29ya3R1YWwuY28udWsiLCJzdG9yYWdlIjoiaHR0cHM6Ly91cnN0b3JhZ2Uud29ya3R1YWwuY28udWsiLCJhdWRpb3NndyI6ImF1ZGlvc2d3dGgud29ya3R1YWwuY28udWsiLCJ2aWRlb3NndyI6InZpZGVvc2d3dGgud29ya3R1YWwuY28udWsiLCJyZWNvcmRpbmciOiJodHRwczovL3JlY29yZGluZy53b3JrdHVhbC5jby51ayIsInZpZGVvbWVldCI6Imh0dHBzOi8vbWVldC53b3JrdHVhbC5jby51ayIsInZvaWNlbWFpbCI6Imh0dHBzOi8vdm9pY2VtYWlsLndvcmt0dWFsLmNvLnVrIiwiY2hhdHNvY2tldCI6Imh0dHBzOi8vdXJjaGF0LWlvLndvcmt0dWFsLmNvLnVrIiwibWVldHNvY2tldCI6Imh0dHBzOi8vdXJtZWV0LWlvLndvcmt0dWFsLmNvLnVrIiwiYXVkaW90dXJudjQiOiJ0dXJuOmF1ZGlvdHVybnY0dGgud29ya3R1YWwuY28udWs6MzQ3OCIsImF1ZGlvdHVybnY2IjoidHVybjphdWRpb3R1cm52NnRoLndvcmt0dWFsLmNvLnVrOjM0NzgiLCJ2aWRlb3R1cm52NCI6InR1cm46dmlkZW90dXJudjR0aC53b3JrdHVhbC5jby51azozNDc4IiwidmlkZW90dXJudjYiOiJ0dXJuOnZpZGVvdHVybnY2dGgud29ya3R1YWwuY28udWs6MzQ3OCJ9LCJpc190b3VyX2ZsYWciOjEsImRhc2hib2FyZF90b3VyIjoxLCJjaGF0Ym90X3RvdXIiOjEsImNoYXRmbG93X3RvdXIiOjAsInBsYW5faWQiOiIxNjAiLCJpc19jYXNfdXNlciI6MSwiY2Nhc19yb2xlX2luZm8iOiIzIiwiY2FzX2RvbWFpbiI6IiIsImlzX3VjYXNfdXNlciI6MCwidWNhc19yb2xlX2luZm8iOiIwIiwidXNyX2lkIjoyNTI4OSwidXNlcklkIjoyNzc3NiwiaXNfY3JtX3VzZXIiOjAsImNybV9yb2xlX2luZm8iOiIiLCJjcm1fZG9tYWluIjoiIiwiY29tcGFueU5hbWUiOiJ3c3Rlc3RtZWRld2F4IiwicHJvZHVjdF9pZCI6IjE1IiwicGFyZW50X2lkIjo1NiwiaWF0IjoxNzU5NDA2NDIxLCJleHAiOjE3NjQ5MzYwMjF9.pg6Xj9x7zUZIGVG0rMaMBTSkGQeRHqbrElihzDfCreg" ;
        isCVM.current = true;
        console.log("cvmanalyticsData", cvmanalyticsData);
        if (params?.goalkey || cvmanalyticsData) {
          dispatch(campaignSliceActions.setIsTyping(true));
        }

        dispatch(campaignSliceActions.setProduct({ type: "CVM", data: params?.goalkey, query: params?.goalvalue, cvmanalytics: cvmanalyticsData }));
        try {

          SetItemInStorage(LocalStorage.ACCESS_TOKEN, token);
          let tokenData: any = Cookies.get();
          if (!tokenData['ssoToken']) {
            Cookies.set("ssoToken", token, 365);
          }
          await webpushNotification();
          new SocketService(token, dispatch);
          await handleCVMCreateCampaign();

        } catch (e: any) {
          console.log("error", e)
        }
      }
    } catch (error) {
      console.log("API ERROR", error)
    } finally {
      await getKnowledgeBaseData()
      await handleGetPastCampaign(false, -1);
      const loaders = document.getElementById('contact_center_mainloader');
      if (loaders) {
          loaders.style.display = 'none';
      }
      setLoader(false);
    }
  }

  const onclickCampaign=(item:any,index:number)=>{
    try {
      console.log("handleSetCampaign>>>>>>>>>>",item?.CampaignChatId,item)
      handleSetCampaign(item?.CampaignChatId,item);
      setActivePastMenu(index);
      setNewChat(false)
      dispatch(campaignSliceActions.setClearMessage(true));
    } catch (error) {
      console.log(error)
    }
  }

  useEffect(() => {
    if (createNewCampaign) {
      handleCreateCampaign();
      dispatch(commonDataSliceActions.setCreateNewCampaign(false));
    }
  }, [createNewCampaign])

  useEffect(()=>{
    console.log("setPastCampaignData",setPastCampaignData);
    if(setPastCampaignData?.item){
      onclickCampaign(setPastCampaignData?.item,setPastCampaignData?.index);
      dispatch(commonDataSliceActions.setPastCampaign(null));
    }
  },[setPastCampaignData])

  useEffect(()=>{
    if(campaignMessageData?.campaignMessage?.length==0){
        dispatch(commonDataSliceActions.setShowMainChat(true));  
    }
  },[campaignMessageData?.campaignMessage?.length])

 

  return (<>
  {/* {loader ? <div className='loadersinner'><Loaders/></div>: */}
  {loader ? <Spinner/>:

  
   <ContentLayout noPadding={true}>
    {mainChat &&
      <div className={`${styles.settingssec} ${isCVM.current?`settingseciframe`:''}`}>
        
        <div className={`${styles.rightsettingsmenu} rightsettingmenu ${isCVM.current?"rights":""}`}>
          <Createcampaigns isCVM={isCVM.current} campaignId={campaignId} isNewChat={isNewChat} CreateNewCampaign={handleCreateCampaign} />
        </div> 
    
      </div>
}
      
      {!mainChat &&
        <div className={`${styles.settingssec} ${isCVM.current ? `settingseciframe` : ''}`}>
          
        <div className={`${styles.rightsettingsmenu} rightsettingmenu ${isCVM.current ? "rights" : ""}`}>
          <CreateTemplates isCVM={isCVM.current} campaignId={campaignId} isNewChat={isNewChat} />
        </div>
      </div>}
      {/* {knowledgeMessageData?.isModalOpen &&
        <Chatcomponent
          headerTitle={
            <div >
              <h5>Hey there! Just a quick setup before we dive in.</h5>
              <span style={{ fontSize: "13px", color: "#404040", fontFamily: "interregular", fontWeight: "400" }}>
              </span>
            </div>
          }
          rightIcon={assets.threedot}
          isOpen={knowledgeMessageData?.isModalOpen}
        />} */}
    </ContentLayout>
}
  </>

  );
};

export default Home;
