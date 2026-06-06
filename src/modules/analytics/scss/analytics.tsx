import React, { useEffect, useState,useRef,useLayoutEffect } from 'react'
import Analticsinput from '../../../components/custom_components/analticseditor'
import styles from "./analytics.module.scss"
import Slider from '../../../components/custom_components/slider/slider'
import {
  BarChart,
  Bar,
  XAxis,
  YAxis,
  Tooltip,
  ResponsiveContainer,
} from "recharts";
import SocketService from '../../../base/service/socketService';
import { v4 as uuidv4 } from "uuid";
import { useDispatcher } from '../../../store/redux-store/dispatch';
import { analyticsSliceActions } from '../../../store/slice/analytics';
import { useSelector } from 'react-redux';
import { GetItemFromStorage, SetItemInStorage } from '../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../base/customhooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
import { whatsappService } from '../../../base/service/apiservice';
import axios from 'axios';
import { config } from '../../../base/constant';
import { useNavigate, useSearchParams } from 'react-router-dom';
import { assets } from '../../../styles/assets/assets';
import { decryptAESCVM } from '../../../base/utils';
import Cookies from '../../../utils/cookies';
import { getToken } from 'firebase/messaging';
import { messaging } from '../../../firebase';

const AI_CONTEXT_TYPES = {
  CAMPAIGN_RANKING: -1,
  WEEKLY_SUMMARY: 0,
  CHANNEL_METRICS: 1,
  TOP_CAMPAIGNS: 2,
  PERFORMANCE_BENCHMARK: 3,
}
const Analytics = () => {
  const dispatch = useDispatcher();
  const [searchParams] = useSearchParams();
  const analyticsData = useSelector((state: any) => state.analyticsData);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN??"");
  const userData: any = userDetails?jwtDecode(userDetails):{};
  const scrollerRef = useRef<HTMLDivElement>(null);
  const [isChat, setIsChat] = React.useState(false);
  const [message, setMessage] = React.useState("");
  const [performanceRankingData, setPerformanceRankingData] = React.useState<any>([]);
  const [weeklyCampaignData, setWeeklyCampaignData] = React.useState<any>([]);
  const [topPerformingData, setTopPerformingData] = React.useState<any>([]);
  const [benchmarkData, setBenchmarkData] = React.useState<any>([]);
  const [channelPerformanceData, setChannelPerformanceData] = React.useState<any>([]);
  const [currentSlide, setCurrentSlide] = React.useState<number>(0);
  const [scheduledCampaignData, setScheduledCampaignData] = useState<any>([])
  const [pendingReverts, setPendingReverts] = useState<{ [key: string]: NodeJS.Timeout }>({});
  const [aiContextData, setAiContextData] = React.useState<any>({
    weeklySummary: "",
    campaignRanking: "",
    topCampaigns: "",
    performanceBenchmark: "",
    channelMetrics: "",
    actionPreview: ''
  })
  const navigate = useNavigate();
  const [loader,setLoader] = useState<any>(true);
  const [cvm,setCVM] = useState<any>(false);
  

   useEffect(()=>{
      const getBrowserName = () => {
          const ua = navigator.userAgent;

          if (ua.includes("Chrome") && !ua.includes("Edg") && !ua.includes("OPR")) return "Chrome";
          if (ua.includes("Edg")) return "Edge";
          if (ua.includes("Firefox")) return "Firefox";
          if (ua.includes("Safari") && !ua.includes("Chrome")) return "Safari";
          if (ua.includes("OPR") || ua.includes("Opera")) return "Opera";
          return "Unknown";
        };

        const browserType = getBrowserName();

        const insertFcmTokenKey = async (token: any) => {
          try {
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
  
      const init = async () => {
        try {
          const queryparams: any = searchParams.get('cvm_data');
          if(queryparams?.length>0){  
              setCVM(true);          
              setLoader(true);
              const decodedQueryparams: any = await decryptAESCVM(queryparams.replaceAll(" ", "+"));
              const params =  JSON?.parse(decodedQueryparams);                      
              const token = params?.token ;
              console.log("token",token)
              // const token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1dWlkIjoiNjkzMDNjYjEtOWU0OC00YzQwLWIxYTYtMjQzZTkwMmQ0NzZlIiwidXNlcm5hbWUiOiJtZWRld2F4NTk4QGt5dHN0b3JlLmNvbSIsImRldmljZUlkIjoiMWViODgwNTItMzZlYi00NDdlLTljZWQtOWM4MzU3MjM3MjM4Iiwic2lwTG9naW5JZCI6IjI5NzkwIiwicm9sZUlkIjoiMyIsImRvbWFpbklkIjoxODY3NywiZXh0IjoiMjAwIiwiY29tcGFueUlkIjoxODc1MCwiZW5ldGVwcmlzZWlkIjoyMzQ4MSwiaG9zdEFkZHJlc3MiOiIyMzQ4MS51cmNoYXQud29ya3R1YWwuY28udWsiLCJvcmRlcklkIjoyNjQxOSwiZGlyVXNlcklkIjoxNTI0MTAsInByb2ZpbGVOYW1lIjoibWVkZXdheCBreXRzdG9yZSIsInByb2ZpbGVJbWciOiJodHRwczovL3Vyc3RvcmFnZS53b3JrdHVhbC5jby51ay9maWxlcy91c2VyLzE4NzUwLzI5NzkwXzIwMC9ib29rbmcuanBlZ18xNzUxOTg1NjA5OTUyLnBuZyIsInNvdXJjZSI6IldlYiIsImlwYWRkcmVzcyI6IjEzNi4xNDQuNTYuMjU1OjQ0MyIsImNsaSI6IiIsInNpcF9wYXNzd29yZCI6IjlUM2lkYnBEIiwiY3VzdG9tZXJfaWQiOjE4ODUxLCJjb3VudHJ5X2NvZGUiOiIwIiwibG9jYWxfbnVtYmVyIjoiMjE4NDU0OTg0MSIsImFwcF9sb2dfaWQiOjI4NDkyLCJvcmRlcl9pZCI6MjY0MTksImRwX3Bhc3N3b3JkIjoiZDg3YzQzYWI2NTY1YjQzIiwicGxhdGZvcm0iOjIzNDgxLCJ1c2VyX3N0YXR1cyI6IkF2YWlsYWJsZSIsInNpdGVfaWQiOjE3Mjg0LCJkb21haW5fbmFtZSI6IjIzNDgxLmNhbGwud29ya3R1YWwuY28udWsiLCJtZWV0aW5nX2lkIjoiNjY5LTYzMS0wNjI2IiwidmlkZW9fbWVldF9kb21haW4iOiIyMzQ4MS51cm1lZXQud29ya3R1YWwuY28udWsiLCJwcm9maWxlX2ltYWdlX3RodW1ibmFpbCI6Imh0dHBzOi8vdXJzdG9yYWdlLndvcmt0dWFsLmNvLnVrL2ZpbGVzL3VzZXIvMTg3NTAvMjk3OTBfMjAwL2Jvb2tuZy5qcGVnXzE3NTE5ODU2MDk5NTIucG5nIiwiZGlyX3VzZXJfaWQiOjE1MjQxMCwiZG9tYWluX2lkIjoxODY3Nywic2lwX2xvZ2luX2lkIjoiMjk3OTAiLCJjYWxsZXJfaWQiOiJtZWRld2F4IGt5dHN0b3JlIiwiY29tcGFueV9pZCI6MTg3NTAsImVudGVycHJpc2VpZCI6MjM0ODEsInJvbGVfaWQiOiIzIiwicHJvZmlsZV91cmwiOiJodHRwczovL3Vyc3RvcmFnZS53b3JrdHVhbC5jby51ay9maWxlcy91c2VyLzE4NzUwLzI5NzkwXzIwMC9ib29rbmcuanBlZ18xNzUxOTg1NjA5OTUyLnBuZyIsIkVtYWlsIjoibWVkZXdheDU5OEBreXRzdG9yZS5jb20iLCJob3N0X2FkZHJlc3MiOiIyMzQ4MS51cmNoYXQud29ya3R1YWwuY28udWsiLCJmaXJzdF9uYW1lIjoibWVkZXdheHMiLCJsYXN0X25hbWUiOiJreXRzdG9yZSIsImNsaWVudF9kb21haW4iOnsiaXZyIjoiaHR0cHM6Ly9pdnIud29ya3R1YWwuY28udWsiLCJ1cmFwaSI6Imh0dHBzOi8vdXJhcGkud29ya3R1YWwuY28udWsiLCJ1cnNpcCI6Imh0dHA6Ly91cnNpcC1hcGkud29ya3R1YWwuY28udWsiLCJzdG9yYWdlIjoiaHR0cHM6Ly91cnN0b3JhZ2Uud29ya3R1YWwuY28udWsiLCJhdWRpb3NndyI6ImF1ZGlvc2d3dGgud29ya3R1YWwuY28udWsiLCJ2aWRlb3NndyI6InZpZGVvc2d3dGgud29ya3R1YWwuY28udWsiLCJyZWNvcmRpbmciOiJodHRwczovL3JlY29yZGluZy53b3JrdHVhbC5jby51ayIsInZpZGVvbWVldCI6Imh0dHBzOi8vbWVldC53b3JrdHVhbC5jby51ayIsInZvaWNlbWFpbCI6Imh0dHBzOi8vdm9pY2VtYWlsLndvcmt0dWFsLmNvLnVrIiwiY2hhdHNvY2tldCI6Imh0dHBzOi8vdXJjaGF0LWlvLndvcmt0dWFsLmNvLnVrIiwibWVldHNvY2tldCI6Imh0dHBzOi8vdXJtZWV0LWlvLndvcmt0dWFsLmNvLnVrIiwiYXVkaW90dXJudjQiOiJ0dXJuOmF1ZGlvdHVybnY0dGgud29ya3R1YWwuY28udWs6MzQ3OCIsImF1ZGlvdHVybnY2IjoidHVybjphdWRpb3R1cm52NnRoLndvcmt0dWFsLmNvLnVrOjM0NzgiLCJ2aWRlb3R1cm52NCI6InR1cm46dmlkZW90dXJudjR0aC53b3JrdHVhbC5jby51azozNDc4IiwidmlkZW90dXJudjYiOiJ0dXJuOnZpZGVvdHVybnY2dGgud29ya3R1YWwuY28udWs6MzQ3OCJ9LCJpc190b3VyX2ZsYWciOjEsImRhc2hib2FyZF90b3VyIjoxLCJjaGF0Ym90X3RvdXIiOjEsImNoYXRmbG93X3RvdXIiOjAsInBsYW5faWQiOiIxNjAiLCJpc19jYXNfdXNlciI6MSwiY2Nhc19yb2xlX2luZm8iOiIzIiwiY2FzX2RvbWFpbiI6IiIsImlzX3VjYXNfdXNlciI6MCwidWNhc19yb2xlX2luZm8iOiIwIiwidXNyX2lkIjoyNTI4OSwidXNlcklkIjoyNzc3NiwiaXNfY3JtX3VzZXIiOjAsImNybV9yb2xlX2luZm8iOiIiLCJjcm1fZG9tYWluIjoiIiwiY29tcGFueU5hbWUiOiJ3c3Rlc3RtZWRld2F4IiwicHJvZHVjdF9pZCI6IjE1IiwicGFyZW50X2lkIjo1NiwiaWF0IjoxNzU5NDA2NDIxLCJleHAiOjE3NjQ5MzYwMjF9.pg6Xj9x7zUZIGVG0rMaMBTSkGQeRHqbrElihzDfCreg" ;
             
              try {                
                  SetItemInStorage(LocalStorage.ACCESS_TOKEN, token);
                  let tokenData: any = Cookies.get();
                  if (!tokenData['ssoToken']) {
                    Cookies.set("ssoToken", token, 365);
                  } 
                  await webpushNotification();                                              
                  new SocketService(token, dispatch);                
              
              } catch (e: any) {
               console.log("error",e)
              } 
          }
        } catch (error) {
          console.log("API ERROR",error)
        }finally{        
         setLoader(false);
          const loaders = document.getElementById('contact_center_mainloader');
          if (loaders) {
            loaders.style.display = 'none';
          }
        }  
      }
      init();
   },[])

    useLayoutEffect(() => {
        if (!scrollerRef.current) return;
        scrollerRef.current?.scrollIntoView({ behavior: 'smooth', block: "end" });
    
      }, [analyticsData.analyticsMessage?.length]);
  
  const sessionId = uuidv4();

  const sendMessage = (msg: string) => {
    try {
      if (!msg || analyticsData.isTyping) return;
      setMessage(msg);
      if (!isChat) {
        setIsChat(true);
      }
      const messageId = uuidv4();
      const data = {
        sessionId,
        message: msg.trim(),
        messageId,
        domainId: userData?.domainId ?? null,
        ext: Number(userData?.ext) || null,
        chatType: "OVERALL ANALYTICS",
        sender: "userText",
        date: new Date().toISOString(),
      };
      dispatch(analyticsSliceActions.setIsTyping(true));
      const temp_data = !isChat ? [] : [...analyticsData.analyticsMessage];
      temp_data.push(data);
      dispatch(analyticsSliceActions.setAnalyticsMessage(temp_data));
      SocketService.sentAnalyticsMessage(data);
    } catch (error) {
      console.error("sendMessage error:", error);
    }
  };

  const getRecommendedCampaign=()=>{
    try {
      whatsappService.get('/get_recommended_campaign').then((res: any) => {
        if (res?.data?.statusCode === 200 && res?.data?.finalResponse?.length > 0) {         
           setAiContextData((prev: any) => ({ ...prev, actionPreview: res?.data?.finalResponse?.[0]?.recommendation }))
        } else {            
            getActionPreview() ; 
        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  useEffect(() => {
    const fetchAllData = () => {
      getCampaignPerformanceRanking()
      getWeeklyCampaignSummary()
      getTopPerformingCampaigns()
      getCampaignPerformanceBenchmark()
      getChannelPerformanceMetrics()
      getScheduledCampaign()
      // getActionPreview()
      getRecommendedCampaign();
     
    }
    fetchAllData() 
    let interval: any;
    if (!isChat) {
      interval = setInterval(fetchAllData, 30000);
    }
    return () => {
      if (interval) clearInterval(interval);
    };
  }, [isChat])

  const getCampaignPerformanceRanking = () => {
    try {
      whatsappService.get('/get_performance_ranking').then((res: any) => {
        if (res?.data?.statusCode === 200 && res?.data?.getPerformanceRankingRes?.length > 0) {
          setPerformanceRankingData(res?.data?.getPerformanceRankingRes)
        } else {
          setPerformanceRankingData([])
        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  const getWeeklyCampaignSummary = () => {
    try {
      whatsappService.get('/get_weekly_campaign_summary').then((res: any) => {
        if (res?.data?.statusCode === 200 && res?.data?.getWeeklyCampaignRes?.length > 0) {
          setWeeklyCampaignData(res?.data?.getWeeklyCampaignRes.map((row: any) => ({ name: row.Name, value: row.Total })))
        } else {
          setWeeklyCampaignData([])
        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  const getTopPerformingCampaigns = () => {
    try {
      whatsappService.get('/get_top_performing_campaigns').then((res: any) => {
        if (res?.data?.statusCode === 200 && res?.data?.getTopPerformingRes?.length > 0) {
          setTopPerformingData(res?.data?.getTopPerformingRes.map((row: any) => ({ name: row.Name, value: row.Total })))
        } else {
          setTopPerformingData([])
        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }
  
  const getCampaignPerformanceBenchmark = () => {
    try {
      whatsappService.get('/get_campaign_performance_benchmark').then((res: any) => {
        if (res?.data?.statusCode === 200 && res?.data?.getBenchmarkRes?.length > 0) {
          const data = res?.data?.getBenchmarkRes[0]
          const tempData = []
          tempData.push({ name: "Current Week", value: data?.CurrentCount })
          tempData.push({ name: "Past Week", value: data?.PastCount })
          setBenchmarkData(tempData)
        } else {
          setBenchmarkData([])
        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  const getChannelPerformanceMetrics = () => {
    try {
      whatsappService.get('/get_channel_performance_metrics').then((res: any) => {
        if (res?.data?.statusCode === 200 && res?.data?.getChannelPerformanceRes?.length > 0) {
          setChannelPerformanceData(res?.data?.getChannelPerformanceRes.map((row: any) => ({ name: row.Channel, value: row.TotalCount })))
        } else {
          setChannelPerformanceData([])
        }
      })
    } catch (error) {
      console.log("error", error);
    }
  }

  useEffect(() => {
    if (performanceRankingData.length > 0)
      getAiContext(AI_CONTEXT_TYPES.CAMPAIGN_RANKING)
  }, [performanceRankingData])

  useEffect(() => {
    if (currentSlide === AI_CONTEXT_TYPES.WEEKLY_SUMMARY && weeklyCampaignData.length > 0) {
      getAiContext(AI_CONTEXT_TYPES.WEEKLY_SUMMARY)
    } else if (currentSlide === AI_CONTEXT_TYPES.TOP_CAMPAIGNS && topPerformingData.length > 0) {
      getAiContext(AI_CONTEXT_TYPES.TOP_CAMPAIGNS)
    } else if (currentSlide === AI_CONTEXT_TYPES.PERFORMANCE_BENCHMARK && benchmarkData.length > 0) {
      getAiContext(AI_CONTEXT_TYPES.PERFORMANCE_BENCHMARK)
    } else if (currentSlide === AI_CONTEXT_TYPES.CHANNEL_METRICS && channelPerformanceData.length > 0) {
      getAiContext(AI_CONTEXT_TYPES.CHANNEL_METRICS)
    }
  }, [currentSlide, weeklyCampaignData, topPerformingData, benchmarkData, channelPerformanceData])

  const getAiContext = async (index: number) => {
    try {
      let data = null
      if (index !== -1)
        setCurrentSlide(index)
      if (index === AI_CONTEXT_TYPES.WEEKLY_SUMMARY) {
        data = weeklyCampaignData
      } else if (index === AI_CONTEXT_TYPES.TOP_CAMPAIGNS) {
        data = topPerformingData
      } else if (index === AI_CONTEXT_TYPES.PERFORMANCE_BENCHMARK) {
        data = benchmarkData
      } else if (index === AI_CONTEXT_TYPES.CHANNEL_METRICS) {
        data = channelPerformanceData
      } else if (index === AI_CONTEXT_TYPES.CAMPAIGN_RANKING) {
        data = performanceRankingData
      }
      const type = index === -1 ? null : index === 0 ? 'Weekly Summary' : index === 1 ? 'Channel Metrics' : index === 2 ? 'Top Campaigns' : index === 3 ? 'Performance Benchmark' : null
      const url: string = `${config().AI_INSIGHTS_URL}`;
      const response = await axios.post(
        url,
        { type, data },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        if (index === AI_CONTEXT_TYPES.WEEKLY_SUMMARY) {
          setAiContextData((prev: any) => ({ ...prev, weeklySummary: response?.data?.response }))
        } else if (index === AI_CONTEXT_TYPES.TOP_CAMPAIGNS) {
          setAiContextData((prev: any) => ({ ...prev, topCampaigns: response?.data?.response }))
        } else if (index === AI_CONTEXT_TYPES.PERFORMANCE_BENCHMARK) {
          setAiContextData((prev: any) => ({ ...prev, performanceBenchmark: response?.data?.response }))
        } else if (index === AI_CONTEXT_TYPES.CHANNEL_METRICS) {
          setAiContextData((prev: any) => ({ ...prev, channelMetrics: response?.data?.response }))
        } else if (index === AI_CONTEXT_TYPES.CAMPAIGN_RANKING) {
          setAiContextData((prev: any) => ({ ...prev, campaignRanking: response?.data?.response }))
        }
      }
    } catch (error) {
      console.log("error", error);
    }
  }

  const getScheduledCampaign = () => {
    try {
      whatsappService.get('/get_campaign_schedule_details').then((res: any) => {
        if (res?.data?.statusCode === 200 && res?.data?.getCampaignSchedulDetailsRes?.length > 0) {
          setScheduledCampaignData(res?.data?.getCampaignSchedulDetailsRes)
        } else {
          setScheduledCampaignData([])
        }
      })
    } catch (error) {
      console.log(error)
    }
  }

  const revertCampaign = (campaignChatId: string) => {
    try {
      whatsappService.get(`/insert_revert_status/${campaignChatId}`).then((res: any) => {
        if (res?.data?.statusCode === 200) {
          getScheduledCampaign()
        } else {

        }
      })
    } catch (error) {
      console.log(error)
    }
  }

  const handleRevertClick = (campaignChatId: string) => {
    try {
      if (pendingReverts[campaignChatId]) return;
      const timeoutId = setTimeout(() => {
        revertCampaign(campaignChatId);
        setPendingReverts(prev => {
          const updated = { ...prev };
          delete updated[campaignChatId];
          return updated;
        });
      }, 5000);

      setPendingReverts(prev => ({ ...prev, [campaignChatId]: timeoutId }));
    } catch (error) {
      console.log(error)
    }
  };

  const handleUndoClick = (campaignChatId: string) => {
    try {
      const timeoutId = pendingReverts[campaignChatId];
      if (timeoutId) {
        clearTimeout(timeoutId);
        setPendingReverts(prev => {
          const updated = { ...prev };
          delete updated[campaignChatId];
          return updated;
        });
      }
    } catch (error) {
      console.log(error)
    }
  };

  const insertRecommendedCampaign=async(data :any)=>{
    try {
      let body :any={
        recommendationCampaign:data
      }
       await whatsappService.post(body, "/insert_recommended_campaign").then((response: any) => {                  
                              console.log("res",response);   
                              getRecommendedCampaign();                        
                             
                          }).catch((error) => {                   
                            console.log(error);
                          })
    } catch (error) {
      console.log("err",error);
    }
      
  }
  const getActionPreview=async()=>{
    try {
       const url: string = `${config().ANALYTICS_ACTION_PREVIEW}`;
      const response = await axios.post(
        url,
        { domainId:userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        setAiContextData((prev: any) => ({ ...prev, actionPreview: response?.data?.message }))
        await insertRecommendedCampaign(response?.data?.message);
      }
    } catch (error) {
      console.log(error)
    }
  }

  const SkeletonBox = ({ width = "100%", height = "16px", borderRadius = "6px" }) => (
  <div
    className={"skeletonBox"}
    style={{ width, height, borderRadius }}
  />
);
  
  return (
    <>
      <div className={`${styles.maincontainer} analtyticsscreen `}>
        {!isChat && <div className={styles.center}>
          <h5>What you want to know?</h5>
          <Analticsinput callBackMsg={sendMessage} />
        </div>}
        {isChat &&
          <div>
            <div className={`${styles.bottomInput} bottominptdown`}>
              <Analticsinput callBackMsg={sendMessage} />
            </div>
            <div className={styles.backnav} onClick={() => setIsChat(false)}>
              <span style={{display:"grid"}}>
                <img src={assets.backicon}></img>
              </span>
              {/* <h5 style={{ fontFamily: "intermedium", fontSize: "16px" }}>{message}</h5> */}
              <h5 style={{ fontFamily: "intermedium", fontSize: "16px" }}>Back</h5>
            </div>
          </div>
        }
      </div>
      {isChat && <div className={styles.msgbody}>
        <div className={`${styles.maincontainer} ${styles.analtcsmaincontainer}`}>
          {Array.isArray(analyticsData?.analyticsMessage) &&
            analyticsData.analyticsMessage.map((msg: any) => {
              const key = msg?.messageId
              const sender = msg?.sender;
              if (sender === "userText") {
                return (
                  <div key={key} className={`${styles.msgbubble} ${styles.right}`}>
                    <div
                      className="chatContent"
                      dangerouslySetInnerHTML={{ __html: msg?.message || "" }}
                    />
                  </div>
                );
              }
              if (sender === "botText") {
                return (
                  <div key={key} className={`${styles.msgbubble} ${styles.left}`}>
                    <div
                      className="chatContent"
                      dangerouslySetInnerHTML={{ __html: msg?.bot_response || "" }}
                    />
                  </div>
                );
              }
              return null;
            })
          }
          {analyticsData?.isTyping && (
            <p className="generatings">
              {/* AI is Thinking */}
              Agent is Analysing
              <div className="typingdot">
                <span></span>
                <span></span>
                <span></span>
              </div>
            </p>
          )}
          <div ref={scrollerRef} />
        </div>
      </div>}

      {!isChat && <div className={styles.bigmaincontainer}>
        <div className={styles.dashboard}>
          <div className={styles.content}>
            <div className={`${styles.card} ${styles.narrative}`}>
              <h3>AI Narrative - Weekly Summary</h3>
              <span className={styles.tag}>Why did it happen?</span>
              {AI_CONTEXT_TYPES.WEEKLY_SUMMARY === currentSlide && aiContextData?.weeklySummary ?
                <div dangerouslySetInnerHTML={{ __html: aiContextData?.weeklySummary }} /> : ""}
              {AI_CONTEXT_TYPES.TOP_CAMPAIGNS === currentSlide && aiContextData?.topCampaigns ? <div dangerouslySetInnerHTML={{ __html: aiContextData?.topCampaigns }} /> : ""}
              {AI_CONTEXT_TYPES.PERFORMANCE_BENCHMARK === currentSlide && aiContextData?.performanceBenchmark ? <div dangerouslySetInnerHTML={{ __html: aiContextData?.performanceBenchmark }} /> : ""}
              {AI_CONTEXT_TYPES.CHANNEL_METRICS === currentSlide && aiContextData?.channelMetrics ? <div dangerouslySetInnerHTML={{ __html: aiContextData?.channelMetrics }} /> : ""}

              {Object.values(aiContextData).filter((row: any) => row !== "").length === 0 &&
                <div className={"skeletonTextGroup"}>
                  <SkeletonBox width="80%" height="14px" />
                  <SkeletonBox width="60%" height="14px" />
                  <SkeletonBox width="90%" height="14px" />
                </div>
              }

              <div style={{ margin: "32px 0" }}>
                <Slider currentSlide={(e: number) => getAiContext(e)}>
                  <div className={styles.slide}>
                    <h3>Weekly Campaign Summary</h3>
                    <div className={styles.chartcontainer}>
                      <ResponsiveContainer width="100%" height={200}>
                        <BarChart data={weeklyCampaignData} margin={{ top: 0, right: 0, left: -20, bottom: 0 }}>

                          <XAxis dataKey="name" hide />
                          <YAxis axisLine={false} tickLine={false} tick={{ fontSize: 14, fill: "#757676" }} />
                          <Tooltip
                            cursor={{ fill: "transparent" }}
                            contentStyle={{
                              backgroundColor: "#000",
                              border: "none",
                              borderRadius: "6px",
                              color: "#fff"
                            }}
                            itemStyle={{ color: "#fff" }}
                            labelStyle={{ color: "#fff" }}
                          />

                          <Bar dataKey="value" fill="#4F46E5" radius={[6, 6, 6, 6]} background={{ fill: "#F3F4F6", radius: [6, 6, 6, 6] } as any}
                            isAnimationActive={false} />
                        </BarChart>
                      </ResponsiveContainer>
                    </div>

                    <div className={styles.legend}>
                      <div className={styles.row}><span>Xaxis</span> <span>Campaign name</span></div>
                      <div className={styles.row}><span>Yaxis</span> <span>Engagement score</span></div>
                    </div>
                  </div>

                  <div className={styles.slide}>
                    <h3>Channel Performance Metrics</h3>
                    <div className={styles.chartcontainer}>
                      <ResponsiveContainer width="100%" height={200}>
                        <BarChart data={channelPerformanceData} margin={{ top: 0, right: 0, left: -20, bottom: 0 }}>
                          <XAxis dataKey="name" hide />
                          <YAxis axisLine={false} tickLine={false} tick={{ fontSize: 14, fill: "#757676" }} />
                          <Tooltip
                            cursor={{ fill: "transparent" }}
                            contentStyle={{
                              backgroundColor: "#000",
                              border: "none",
                              borderRadius: "6px",
                              color: "#fff"
                            }}
                            itemStyle={{ color: "#fff" }}
                            labelStyle={{ color: "#fff" }}
                          />
                          <Bar dataKey="value" fill="#00E2A4" radius={[6, 6, 6, 6]} background={{ fill: "#F3F4F6", radius: [6, 6, 6, 6] } as any} />
                        </BarChart>
                      </ResponsiveContainer>
                    </div>
                    <div className={styles.legend}>
                      <div className={styles.row}><span>Xaxis</span> <span>Channels</span></div>
                      <div className={styles.row}><span>Yaxis</span> <span>Engagement score</span></div>
                    </div>
                  </div>

                  <div className={styles.slide}>
                    <h3>Top Performing Campaigns</h3>
                    <div className={styles.chartcontainer}>
                      <ResponsiveContainer width="100%" height={200}>
                        <BarChart data={topPerformingData} margin={{ top: 0, right: 0, left: -20, bottom: 0 }}>
                          <XAxis dataKey="name" hide />
                          <YAxis axisLine={false} tickLine={false} tick={{ fontSize: 14, fill: "#757676" }} />
                          <Tooltip
                            cursor={{ fill: "transparent" }}
                            contentStyle={{
                              backgroundColor: "#000",
                              border: "none",
                              borderRadius: "6px",
                              color: "#fff"
                            }}
                            itemStyle={{ color: "#fff" }}
                            labelStyle={{ color: "#fff" }}
                          />
                          <Bar dataKey="value" fill="#FF793D" radius={[6, 6, 6, 6]} background={{ fill: "#F3F4F6", radius: [6, 6, 6, 6] } as any} />
                        </BarChart>
                      </ResponsiveContainer></div>
                    <div className={styles.legend}>
                      <div className={styles.row}><span>Xaxis</span> <span>Campaign name</span></div>
                      <div className={styles.row}><span>Yaxis</span> <span>Engagement score</span></div>
                    </div>
                  </div>

                  <div className={styles.slide}>
                    <h3>Campaign Performance Benchmark</h3>
                    <div className={styles.chartcontainer}>
                      <ResponsiveContainer width="100%" height={200}>
                        <BarChart data={benchmarkData} margin={{ top: 0, right: 0, left: -20, bottom: 0 }}>
                          <XAxis dataKey="name" hide />
                          <YAxis axisLine={false} tickLine={false} tick={{ fontSize: 14, fill: "#757676" }} />
                          <Tooltip
                            cursor={{ fill: "transparent" }}
                            contentStyle={{
                              backgroundColor: "#000",
                              border: "none",
                              borderRadius: "6px",
                              color: "#fff"
                            }}
                            itemStyle={{ color: "#fff" }}
                            labelStyle={{ color: "#fff" }}
                          />
                          <Bar dataKey="value" fill="#439CF3" radius={[6, 6, 6, 6]} background={{ fill: "#F3F4F6", radius: [6, 6, 6, 6] } as any} />
                        </BarChart>
                      </ResponsiveContainer></div>
                    <div className={styles.legend}>
                      <div className={styles.row}><span>Xaxis</span> <span>Current Week vs Past Week</span></div>
                      <div className={styles.row}><span>Yaxis</span> <span>Engagement score</span></div>
                    </div>
                  </div>
                </Slider>
              </div>
               {/* <SkeletonBox height="200px" /> */}
            </div>
            <div className={`${styles.card} ${styles.ranking}`}>
              <h3>Campaign Performance ranking</h3>
              <span className={styles.tag}>What happened?</span>
              <div className={styles.rankingGrid}>
                <div className={styles.score}>
                  <strong>Email</strong> <span>{performanceRankingData[0]?.Email} <span className={styles.scoretxt}>score</span></span>
                </div>
                <div className={styles.score}>
                  <strong>SMS</strong> <span>{performanceRankingData[0]?.SMS}<span className={styles.scoretxt}>score</span></span>
                </div>
                <div className={styles.score}>
                  <strong>WhatsApp</strong> <span>{performanceRankingData[0]?.Whatsapp} <span className={styles.scoretxt}>score</span></span>
                </div>
                <div className={styles.score}>
                  <strong>Web Push</strong>  <span>{performanceRankingData[0]?.WebPush} <span className={styles.scoretxt}>score</span></span>
                </div>
                <div className={styles.score}>
                  <strong>In-App</strong> <span>{performanceRankingData[0]?.inApp} <span className={styles.scoretxt}>score</span></span>
                </div>
                <div className={styles.score}>
                  <strong>App Push</strong> <span>{performanceRankingData[0]?.appPush} <span className={styles.scoretxt}>score</span></span>
                </div>
              </div>

              <p className={styles.rankingInfo}>
                {aiContextData?.campaignRanking ? <div dangerouslySetInnerHTML={{ __html: aiContextData?.campaignRanking }} /> : <div className={"skeletonTextGroup"}>
                  <SkeletonBox width="80%" height="14px" />
                  <SkeletonBox width="60%" height="14px" />
                  <SkeletonBox width="90%" height="14px" />
                </div>}
              </p>

              {/* <button className={"cancelbtn"}>View Details</button> */}           
              {/* <SkeletonBox width='120px' height="28px" /> */}
            </div>


            <div className={`${styles.card} ${styles.actions}`}>
              <h3>Recent actions</h3>
              <span className={styles.tag}>What should we do?</span>
              <ul className={styles.actionsList}>
                {scheduledCampaignData.map((row: any) => {
                  if(row?.campaignChatId===null)
                      return null
                  const isPending = !!pendingReverts[row.campaignChatId];
                  return (
                    <li key={row.campaignId}>
                      <p>{row?.campaignName}</p>
                      {isPending ? (
                        <button
                          className="cancelbtn"
                          onClick={() => handleUndoClick(row.campaignChatId)}
                        >
                          Undo
                        </button>
                      ) : (
                        <button
                          className="cancelbtn"
                          onClick={() => handleRevertClick(row.campaignChatId)}
                        >
                          Revert
                        </button>
                      )}
                    </li>
                  );
                })}
                 {/* <SkeletonBox width='100%' height="110px" /> */}
              </ul>
            </div>
            <div className={`${styles.card} ${styles.preview}`}>
              <h3>Action Preview</h3>
              <span className={styles.tag}>What might happen?</span>
              <div className={`${styles.previewImpact} ${styles.block}`}>                
                {aiContextData?.actionPreview ? <div dangerouslySetInnerHTML={{ __html: aiContextData?.actionPreview }} />:
                <SkeletonBox width='100%' height="140px" />
                }
                {cvm && <button style={{ width: "200px" }} className={"primarybtn"} onClick={() => window.parent.postMessage(
                      {
                        type: "CVM_IFRAME",
                        message: {
                          data: aiContextData?.actionPreview
                        },
                      },
                      "*"
                    )
                }>Create</button>}
                {!cvm &&<button style={{width:"200px"}} className={"primarybtn"} onClick={()=>navigate('/home',{ state: { data:aiContextData?.actionPreview } })}>Create</button>}
              </div>
            </div>
          </div>
        </div>
      </div>
      }
    </>

  )
}

export default Analytics

