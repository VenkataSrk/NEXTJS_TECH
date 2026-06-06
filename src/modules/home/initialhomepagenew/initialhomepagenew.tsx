
import { useEffect, useState } from "react";
import { useDispatcher } from "../../../store/redux-store/dispatch";
import { commonDataSliceActions } from "../../../store/slice/commonSlice/commonSlice"
import styles from "./scss/initalhomepagenew.module.scss"
import { GetItemFromStorage } from "../../../base/customhooks/useStorage";
import { jwtDecode } from "jwt-decode";
import { LocalStorage } from "../../../base/customhooks/localStorageKeys";
import axios from "axios";
import { config } from "../../../base/constant";

function Initialhomepagenew() {
  const dispatch = useDispatcher();
  const handleCreate=(type :any)=>{
      dispatch(commonDataSliceActions.setClearFiles(true));
      if(type=='campaign'){
          dispatch(commonDataSliceActions.setShowBuildCampaign(false));
          dispatch(commonDataSliceActions.setIsNewCampaign(true));
        // dispatch(commonDataSliceActions.setShowBuildCampaign(true));
      }
      else{
         dispatch(commonDataSliceActions.setShowBuildCampaign(false));
      }
      dispatch(commonDataSliceActions.setDisableHomeActive(true));
      dispatch(commonDataSliceActions.setCreateNewCampaign(true));
      dispatch(commonDataSliceActions.setInitialChatMsg(type));
  }

   const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
   const userData: any = jwtDecode(userDetails);
  const [showLiveCampaign,setShowLiveCampaign] = useState<any>(false);
  const [showTopCampaign,setShowTopCampaign] = useState<any>(false);
  const [showAudienceCampaign,setShowAudienceCampaign] = useState<any>(false);
  const [showObservationCampaign,setShowObservationCampaign] = useState<any>(false);
  const [allLoaded,setAllLoaded]=useState<any>(false);
  const initialLiveCampainData= {
    liveCampaignSummary:'',
    activeCampaign:'',
    engagementRate:'',
    totalReach:'',
    totalImpression:'',
    aiRecommendation:''
  }

  const initialTopPerformingData ={
    tcp_heading:'',
    tcp_summary:'',
    tcp_suggestion:[],
    tcp_analysis:'',
    tcp_channelAnalysis:[]
  }

  const initialAudienceFatigue ={
    audienceFatigueSummary:'',
    audienceFatigueSuggestion:'',
    hiddenAudienceSummary:'',
    hiddenAudienceSuggestion:''
  }

  const initialObservationData ={
    ai_observationHeading:'',
    ai_observationSummary:'',
    ai_observationRecommendation:''
  }

  const [liveCampaignData,setLiveCampaignData] = useState<any>(initialLiveCampainData);
  const [topPerformingData,setTopPerformingData] = useState<any>(initialTopPerformingData);
  const [audienceFatigueData,setAudienceFatigueData] = useState<any>(initialAudienceFatigue);
  const [observatonData,setObservationData] = useState<any>(initialObservationData);
  const SkeletonBox = ({ width = "100%", height = "16px", borderRadius = "6px" }) => (
    <div
      className={"skeletonBox"}
      style={{ width, height, borderRadius }}
    />
  );
const HandleLiveCampaign=async()=>{
  try {
    //  const url: string = `${config().WEEKLYCAMPAIGN_PERFORMANCE_REPORT}`;
    
     const url: string = `${config().NLP_CAMPAIGN_URL}/${import.meta.env.VITE_APP_ENV=="development"?'Campaign_live_summary':'Campaign_live_summary_live'}`;
      const response = await axios.post(
        url,
        { domainId: userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        
       
        const data =JSON.parse(response?.data?.Message);

        const liveData = {
          liveCampaignSummary: data?.summary,
          activeCampaign: data?.active_campaigns,
          engagementRate: data?.engagement_rate,
          totalReach: data?.total_reach,
          totalImpression: data?.total_impressions,
          aiRecommendation: data?.ai_recommendation,
          scheduleCampaign:data?.schedule_campaigns
        }
        setLiveCampaignData(liveData);
        setShowLiveCampaign(true);
        return true;
      }else{
        return false;
      }
    
  } catch (error) {
    console.log("API ERROR:",error)
  }
}

const HandleTopPerformingCampaign=async()=>{
  try {
     const url: string = `${config().NLP_CAMPAIGN_URL}/${import.meta.env.VITE_APP_ENV=="development"?'Top_campaign_summary':'Top_campaign_summary_live'}`;
      const response = await axios.post(
        url,
        { domainId: userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        // setPerformanceReport(response?.data?.Message)
         
           const data =JSON.parse(response?.data?.Message);
          const topPerformingData = {
            tcp_heading: data?.tcp_header,
            tcp_summary: data?.tcp_summary,
            tcp_suggestion: data?.tcp_suggestion,
            tcp_analysis: data?.tcp_analysis,
            tcp_channelAnalysis: data?.tpc_channel_analysis
          }
        setTopPerformingData(topPerformingData);
        setShowTopCampaign(true);
        return true;
      }else{
        return false;
      }
    
  } catch (error) {
    console.log("API ERROR:",error)
  }
}

  const HandleAudienceFatigue = async () => {
    try {
      const url: string = `${config().NLP_CAMPAIGN_URL}/${import.meta.env.VITE_APP_ENV=="development"?"audience_segments_summary":"audience_segments_summary_live "}`;
      const response = await axios.post(
        url,
        { domainId: userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {


        const data = JSON.parse(response?.data?.Message)

        const audienceFatigue = {
          audienceFatigueSummary: data?.AFD_summary,
          audienceFatigueSuggestion: data?.AFD_suggestion,
          hiddenAudienceSummary: data?.HASF_summary,
          hiddenAudienceSuggestion: data?.HASF_suggestion
        }
        setAudienceFatigueData(audienceFatigue)
        setShowAudienceCampaign(true);
        return true;
      }
      else{
        return false;
      }
    } catch (error) {
      console.log("API ERROR:", error)
    }
  }

const HandleAIObservation=async()=>{
  try {
     const url: string = `${config().NLP_CAMPAIGN_URL}/${import.meta.env.VITE_APP_ENV=="development"?'past_audience_segments_campaign':"past_audience_segments_campaign_live"}`;
      const response = await axios.post(
        url,
        { domain_id: userData?.domainId },
        {
          headers: {
            "Content-Type": "application/json",
          },
        }
      )
      if (response?.status === 200) {
        // setPerformanceReport(response?.data?.Message);
      
        const data = JSON.parse(response?.data?.Message);
        const observationData = {
          ai_observationHeading: data?.ai_heading,
          ai_observationSummary: data?.summary,
          ai_observationRecommendation: data?.ai_recommendation
        }
        setObservationData(observationData);
        setShowObservationCampaign(true);
        return true;
      }else{
        return false;
      }
  } catch (error) {
    console.log("API ERROR:",error)
  }
}

useEffect(()=>{
const loadAll = async () => {
    await Promise.all([
      HandleLiveCampaign(),
      HandleTopPerformingCampaign(),
      HandleAudienceFatigue(),
      HandleAIObservation()
    ]);

    setAllLoaded(true);
  };

  loadAll();
},[])


    return (
        <>
            {/* <div className={styles.inputcontainer}>
                <div className={styles.searchblockouter}>
                  <div style={{ marginTop: "16px" }} className={`${styles.searchblock} ${styles.newinput}`}>
                    <div className={styles.searchmessage}>
                      <div className={styles.searchmessageText}>
                        {uploadedFiles.length > 0 && <div className="attachment-container">
                          {uploadedFiles.map((file, index) => (
                            <React.Fragment key={index}>
                              {file.mimetype.startsWith("image/") ? (
                                <div className="simpleimgcard">
                                  <img
                                    src={file.uploadUrl}
                                    alt={file.filename}
                                    className="file-preview"
                                    style={{
                                      width: "40px",
                                      height: "40px",
                                      borderRadius: "6px",
                                      objectFit: "cover",
                                    }}
                                  />
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              ) : (
                                <div className="attachment-card">
                                  <div className="thumb">
                                    <div className="file-icon">
                                      <img src={file.mimetype.includes("pdf")?assets.pdficon:file.mimetype.includes("txt")?assets.uploaddoc:assets.uploadxlsx} alt="file-icon" />
                                    </div>
                                  </div>
                                  <div className="file-info">
                                    <p className="filename" title={file.filename}>
                                      {file.filename}
                                    </p>
                                    <p className="details">
                                      {file.mimetype.includes("pdf")
                                        ? "PDF"
                                        : file.mimetype.split("/")[1].toUpperCase()}{" "}
                                      • {formatFileSize(file.size)}
                                    </p>
                                  </div>
                                  <svg
                                    onClick={() => removeFile(index)}
                                    className="removeebtn"
                                    width="15"
                                    height="15"
                                    viewBox="0 0 15 15"
                                    fill="none"
                                    xmlns="http://www.w3.org/2000/svg"
                                  >
                                    <path
                                      d="M7.52143 0.857544C3.8411 0.857544 0.857544 3.8411 0.857544 7.52143C0.857544 11.2018 3.8411 14.1853 7.52143 14.1853C11.2018 14.1853 14.1853 11.2018 14.1853 7.52143C14.1853 3.8411 11.2018 0.857544 7.52143 0.857544ZM10.4422 9.66174C10.6375 9.85699 10.6375 10.1733 10.4422 10.3684C10.3446 10.4661 10.2167 10.5149 10.0888 10.5149C9.96095 10.5149 9.83307 10.4661 9.73545 10.3684L7.5215 8.1545L5.30756 10.3684C5.20993 10.4661 5.08205 10.5149 4.95417 10.5149C4.82629 10.5149 4.69841 10.4661 4.60078 10.3684C4.40553 10.1732 4.40553 9.85693 4.60078 9.66174L6.81473 7.4478L4.60078 5.23385C4.40553 5.0386 4.40553 4.72233 4.60078 4.52715C4.79604 4.33196 5.1123 4.3319 5.30749 4.52715L7.52143 6.74109L9.73538 4.52715C9.93063 4.3319 10.2469 4.3319 10.4421 4.52715C10.6373 4.7224 10.6373 5.03867 10.4421 5.23385L8.22814 7.4478L10.4422 9.66174Z"
                                      fill="#484848"
                                    />
                                  </svg>
                                </div>
                              )}
                            </React.Fragment>
                          ))}
                        </div>}
                        <div className={styles.inputcontainer}>
                          <textarea
                            ref={textareaRef}
                            placeholder="Write your Goal & Objective"
                            className="textareamainsnd textareaneww"
                            onInput={handleInput}
                            value={campaignMessage}
                            onChange={(e) => handelTextArea(e.target.value)}
                            onKeyDown={(e) => {
                              if (!campaignMessageData?.isTyping && !campaignMessageData?.progress && e.key === "Enter" && !e.shiftKey) {
                                e.preventDefault();
                                handleSend();
                              }
                            }}
                            rows={1}
                            style={{ minHeight: "40px", maxHeight: "120px", height: "40px" }}
                          />
                        </div>
                        <div className={`${styles.actionbottom} ${styles.actionbottomtop}`}>
                          <div>
                            {campaignMessageData?.isTyping || campaignMessageData?.progress ? (
                              <img src={assets.passicon} alt="typing" />
                            ) : campaignMessage && !isRecording ? (
                              <img
                                src={assets.sndarrowchat}
                                alt="send"
                                onClick={handleSend}
                                style={{ cursor: "pointer" , height:"38px"}}
                              />
                            ) : (
                              <>
                                {!isRecording ? 
                                <svg
                                  width="38"
                                  height="38"
                                  viewBox="0 0 38 38"
                                  fill="none"
                                  xmlns="http://www.w3.org/2000/svg"
                                  onClick={micToggle}
                                  aria-pressed={isRecording}
                                >
                                  <path
                                    d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z"
                                    fill="#12C3B3"
                                  />
                                  <g filter="url(#filter0_i_273_2583)">
                                    <path
                                      d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z"
                                      fill="url(#paint0_linear_273_2583)"
                                    />
                                  </g>
                                  <path
                                    d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z"
                                    fill="url(#paint1_linear_273_2583)"
                                  />
                                  <defs>
                                    <filter
                                      id="filter0_i_273_2583"
                                      x="12.9941"
                                      y="9.18848"
                                      width="10.4745"
                                      height="16.4436"
                                      filterUnits="userSpaceOnUse"
                                      colorInterpolationFilters="sRGB"
                                    >
                                      <feFlood floodOpacity="0" result="BackgroundImageFix" />
                                      <feBlend
                                        mode="normal"
                                        in="SourceGraphic"
                                        in2="BackgroundImageFix"
                                        result="shape"
                                      />
                                      <feColorMatrix
                                        in="SourceAlpha"
                                        type="matrix"
                                        values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0"
                                        result="hardAlpha"
                                      />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix
                                        type="matrix"
                                        values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0"
                                      />
                                      <feBlend
                                        mode="normal"
                                        in2="shape"
                                        result="effect1_innerShadow_273_2583"
                                      />
                                    </filter>
                                    <linearGradient
                                      id="paint0_linear_273_2583"
                                      x1="13.9726"
                                      y1="9.18848"
                                      x2="36.7322"
                                      y2="27.1426"
                                      gradientUnits="userSpaceOnUse"
                                    >
                                      <stop offset="0.0337" stopColor="#00E2A4" />
                                      <stop offset="0.1157" stopColor="#00E2A4" />
                                      <stop offset="0.5389" stopColor="#575CFF" />
                                      <stop offset="0.8687" stopColor="#FF793D" />
                                      <stop offset="1" stopColor="#00B885" />
                                    </linearGradient>
                                    <linearGradient
                                      id="paint1_linear_273_2583"
                                      x1="18.3027"
                                      y1="12.9068"
                                      x2="27.2121"
                                      y2="9.92262"
                                      gradientUnits="userSpaceOnUse"
                                    >
                                      <stop stopColor="#EA57FF" />
                                      <stop offset="0.945" stopColor="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg> 
                                : <svg
                                  onClick={micToggle}
                                  width="38" height="38" viewBox="0 0 38 38" fill="none" xmlns="http://www.w3.org/2000/svg">
                                  <rect width="38" height="38" rx="19" fill="#00E2A4" />
                                  <path d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z" fill="#F0FAF7" />
                                  <g filter="url(#filter0_i_3036_4344)">
                                    <path d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z" fill="#F0FAF7" />
                                  </g>
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="#757676" />
                                  <path d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z" fill="url(#paint0_linear_3036_4344)" />
                                  <defs>
                                    <filter id="filter0_i_3036_4344" x="12.9941" y="9.18848" width="10.4746" height="16.4436" filterUnits="userSpaceOnUse" color-interpolation-filters="sRGB">
                                      <feFlood flood-opacity="0" result="BackgroundImageFix" />
                                      <feBlend mode="normal" in="SourceGraphic" in2="BackgroundImageFix" result="shape" />
                                      <feColorMatrix in="SourceAlpha" type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hardAlpha" />
                                      <feOffset dy="2" />
                                      <feGaussianBlur stdDeviation="2" />
                                      <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                      <feColorMatrix type="matrix" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0" />
                                      <feBlend mode="normal" in2="shape" result="effect1_innerShadow_3036_4344" />
                                    </filter>
                                    <linearGradient id="paint0_linear_3036_4344" x1="18.3027" y1="12.9068" x2="27.2121" y2="9.92262" gradientUnits="userSpaceOnUse">
                                      <stop stop-color="#EA57FF" />
                                      <stop offset="0.945" stop-color="#FF5300" />
                                    </linearGradient>
                                  </defs>
                                </svg>}
                              </>
                            )}</div>
                          <div>
                            <input
                              type="file"
                              accept="application/pdf, image/*"
                              ref={fileInputRef}
                              style={{ display: "none" }}
                              onChange={uploadWhatsappTemplate}
                            />
                            <svg
                              xmlns="http://www.w3.org/2000/svg"
                              width="16"
                              height="16"
                              viewBox="0 0 16 16"
                              fill="none"
                              onClick={() => fileInputRef.current?.click()}
                              style={{ cursor: "pointer" }}
                            >
                              <path
                                d="M15.2515 7.24538L8.74468 7.24987V0.748017C8.74468 0.333985 8.40902 -0.00167847 7.99499 -0.00167847C7.58096 -0.00167847 7.2453 0.333985 7.2453 0.748017V7.25097L0.747509 7.25547C0.333482 7.25577 -0.00197814 7.59163 -0.00167827 8.00567C-0.00137839 8.4197 0.334482 8.75516 0.748509 8.75486L7.2453 8.75036V15.2522C7.2453 15.6663 7.58096 16.0019 7.99499 16.0019C8.40902 16.0019 8.74468 15.6663 8.74468 15.2522V8.74936L15.2525 8.74487C15.6665 8.74457 16.002 8.4087 16.0017 7.99467C16.0014 7.58064 15.6655 7.24508 15.2515 7.24538Z"
                                fill="#404040"
                              />
                            </svg>
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                </div>




            </div> */}
            <div className={styles.maincontainer}>

         
            <div className={styles.capsulestabwrapper}>
                <div className={styles.capsulestab}   onClick={()=> handleCreate("create campaign")}>
                    <span style={{ display: "grid" }}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="20" height="19" viewBox="0 0 20 19" fill="none">
                            <mask id="path-1-inside-1_3996_2393" fill="white">
                                <path fill-rule="evenodd" clip-rule="evenodd" d="M17.3925 8.08751H19.4153C19.7388 8.08751 20.0006 7.81897 19.9999 7.49059C19.9993 7.16179 19.7355 6.8961 19.4123 6.8961H17.3896C17.066 6.8961 16.8042 7.16463 16.8049 7.49297C16.8055 7.82186 17.0693 8.08751 17.3925 8.08751ZM14.9916 2.67513L16.0029 0.894912C16.1645 0.610503 16.0701 0.245496 15.7902 0.0804786C15.5107 -0.084284 15.1509 0.0118487 14.9892 0.296597L13.9778 2.07681C13.8162 2.36122 13.9107 2.72623 14.1906 2.89125C14.47 3.05601 14.8298 2.95988 14.9916 2.67513ZM17.3318 4.80754L18.9925 3.83318C19.2744 3.66778 19.3709 3.30154 19.2082 3.01501C19.0454 2.72852 18.6851 2.63039 18.4031 2.79579L16.7425 3.77019C16.4606 3.93559 16.364 4.30183 16.5268 4.58836C16.6895 4.87481 17.0499 4.97294 17.3318 4.80754Z" />
                            </mask>
                            <path fill-rule="evenodd" clip-rule="evenodd" d="M17.3925 8.08751H19.4153C19.7388 8.08751 20.0006 7.81897 19.9999 7.49059C19.9993 7.16179 19.7355 6.8961 19.4123 6.8961H17.3896C17.066 6.8961 16.8042 7.16463 16.8049 7.49297C16.8055 7.82186 17.0693 8.08751 17.3925 8.08751ZM14.9916 2.67513L16.0029 0.894912C16.1645 0.610503 16.0701 0.245496 15.7902 0.0804786C15.5107 -0.084284 15.1509 0.0118487 14.9892 0.296597L13.9778 2.07681C13.8162 2.36122 13.9107 2.72623 14.1906 2.89125C14.47 3.05601 14.8298 2.95988 14.9916 2.67513ZM17.3318 4.80754L18.9925 3.83318C19.2744 3.66778 19.3709 3.30154 19.2082 3.01501C19.0454 2.72852 18.6851 2.63039 18.4031 2.79579L16.7425 3.77019C16.4606 3.93559 16.364 4.30183 16.5268 4.58836C16.6895 4.87481 17.0499 4.97294 17.3318 4.80754Z" fill="#757676" />
                            <path d="M19.9999 7.49059L18.4999 7.49364V7.49365L19.9999 7.49059ZM16.8049 7.49297L18.3049 7.49011L18.3049 7.48992L16.8049 7.49297ZM14.9916 2.67513L16.2957 3.41616L16.2958 3.41609L14.9916 2.67513ZM16.0029 0.894912L14.6987 0.153939L14.6987 0.15395L16.0029 0.894912ZM15.7902 0.0804786L16.552 -1.21168L16.5519 -1.21169L15.7902 0.0804786ZM14.9892 0.296597L16.2934 1.03756L16.2934 1.03748L14.9892 0.296597ZM13.9778 2.07681L15.282 2.81779L15.282 2.81778L13.9778 2.07681ZM14.1906 2.89125L13.4288 4.1834L13.4288 4.18342L14.1906 2.89125ZM17.3318 4.80754L18.0909 6.10129L18.0909 6.10129L17.3318 4.80754ZM18.9925 3.83318L18.2334 2.53942L18.2334 2.53943L18.9925 3.83318ZM19.2082 3.01501L20.5125 2.27416L20.5124 2.27408L19.2082 3.01501ZM18.4031 2.79579L17.6441 1.50199L17.644 1.50207L18.4031 2.79579ZM16.7425 3.77019L17.5016 5.06395L17.5016 5.06392L16.7425 3.77019ZM16.5268 4.58836L15.2225 5.32921L15.2226 5.32938L16.5268 4.58836ZM17.3925 8.08751V9.58751H19.4153V8.08751V6.58751H17.3925V8.08751ZM19.4153 8.08751V9.58751C20.5967 9.58751 21.5022 8.61696 21.4999 7.48754L19.9999 7.49059L18.4999 7.49365C18.499 7.02099 18.8809 6.58751 19.4153 6.58751V8.08751ZM19.9999 7.49059L21.4999 7.48755C21.4976 6.34887 20.58 5.3961 19.4123 5.3961V6.8961V8.3961C18.891 8.3961 18.5009 7.97471 18.4999 7.49364L19.9999 7.49059ZM19.4123 6.8961V5.3961H17.3896V6.8961V8.3961H19.4123V6.8961ZM17.3896 6.8961V5.3961C16.2082 5.3961 15.3026 6.36658 15.3049 7.49602L16.8049 7.49297L18.3049 7.48992C18.3059 7.96268 17.9239 8.3961 17.3896 8.3961V6.8961ZM16.8049 7.49297L15.3049 7.49583C15.3071 8.63484 16.2249 9.58751 17.3925 9.58751V8.08751V6.58751C17.9137 6.58751 18.304 7.00888 18.3049 7.49011L16.8049 7.49297ZM14.9916 2.67513L16.2958 3.41609L17.3072 1.63587L16.0029 0.894912L14.6987 0.15395L13.6873 1.93417L14.9916 2.67513ZM16.0029 0.894912L17.3072 1.63588C17.8652 0.653699 17.5516 -0.622355 16.552 -1.21168L15.7902 0.0804786L15.0284 1.37263C14.5886 1.11335 14.4639 0.567307 14.6987 0.153939L16.0029 0.894912ZM15.7902 0.0804786L16.5519 -1.21169C15.5457 -1.8049 14.2576 -1.45253 13.6849 -0.444287L14.9892 0.296597L16.2934 1.03748C16.0442 1.47623 15.4757 1.63633 15.0284 1.37265L15.7902 0.0804786ZM14.9892 0.296597L13.685 -0.444365L12.6736 1.33585L13.9778 2.07681L15.282 2.81778L16.2934 1.03756L14.9892 0.296597ZM13.9778 2.07681L12.6736 1.33584C12.1156 2.31803 12.4292 3.59408 13.4288 4.1834L14.1906 2.89125L14.9524 1.59909C15.3922 1.85838 15.5168 2.40442 15.282 2.81779L13.9778 2.07681ZM14.1906 2.89125L13.4288 4.18342C14.4351 4.77668 15.723 4.42414 16.2957 3.41616L14.9916 2.67513L13.6874 1.9341C13.9365 1.49562 14.505 1.33534 14.9523 1.59908L14.1906 2.89125ZM17.3318 4.80754L18.0909 6.10129L19.7516 5.12693L18.9925 3.83318L18.2334 2.53943L16.5727 3.51379L17.3318 4.80754ZM18.9925 3.83318L19.7516 5.12694C20.7507 4.54074 21.0747 3.26402 20.5125 2.27416L19.2082 3.01501L17.9039 3.75586C17.6672 3.33906 17.7981 2.79481 18.2334 2.53942L18.9925 3.83318ZM19.2082 3.01501L20.5124 2.27408C19.9421 1.27019 18.6575 0.907487 17.6441 1.50199L18.4031 2.79579L19.1621 4.0896C18.7126 4.3533 18.1488 4.18685 17.904 3.75594L19.2082 3.01501ZM18.4031 2.79579L17.644 1.50207L15.9834 2.47647L16.7425 3.77019L17.5016 5.06392L19.1622 4.08952L18.4031 2.79579ZM16.7425 3.77019L15.9834 2.47644C14.9843 3.06263 14.6603 4.33936 15.2225 5.32921L16.5268 4.58836L17.8311 3.84751C18.0678 4.26431 17.9369 4.80856 17.5016 5.06395L16.7425 3.77019ZM16.5268 4.58836L15.2226 5.32938C15.7929 6.3331 17.0775 6.6959 18.0909 6.10129L17.3318 4.80754L16.5728 3.51378C17.0224 3.24997 17.5862 3.41652 17.831 3.84735L16.5268 4.58836Z" fill="#757676" mask="url(#path-1-inside-1_3996_2393)" />
                            <path d="M8.33984 17.7723L7.9209 18.1581L6.35156 16.0956L6.84961 15.8036L8.33984 17.7723ZM5.53223 13.8573L3.92188 14.8026C2.92109 15.3897 1.61944 15.0417 1.03418 14.0116C0.445375 12.9747 0.803892 11.6382 1.81152 11.0467L3.4043 10.1112L5.53223 13.8573Z" fill="#757676" stroke="#757676" stroke-width="1.5" />
                            <path d="M10.0793 1.45518C10.3505 1.29619 10.7102 1.38725 10.8743 1.67589L12.8333 5.12413L13.2629 5.88097L13.9485 5.34288L14.0422 5.2794C14.4332 5.05002 14.9388 5.18065 15.1721 5.59093C15.4093 6.0084 15.2638 6.53691 14.8655 6.77061C14.833 6.78967 14.7994 6.80699 14.7649 6.8214L13.9827 7.14757L14.4006 7.8839L16.3596 11.3321C16.5274 11.6276 16.4212 12.0076 16.1428 12.171C15.8901 12.3193 15.5594 12.2503 15.3831 12.006L15.1995 11.7511L14.8899 11.7032L14.4465 11.6446C12.3615 11.4139 9.83271 11.7488 7.30981 12.6544L5.23462 9.00108C7.42053 7.10865 9.0536 4.88894 9.85083 2.81651L9.96216 2.5255L9.83521 2.24132C9.7065 1.95332 9.81877 1.60808 10.0793 1.45518Z" fill="#757676" stroke="#757676" stroke-width="1.5" />
                            <circle cx="14" cy="6.5" r="1" fill="#757676" />
                        </svg>
                    </span>
                    create campaign
                </div>
                <div className={styles.capsulestab} onClick={()=> handleCreate("Reports")}>
                    <span style={{ display: "grid" }}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                            <path d="M17.4575 9.7106H9.65114L14.8554 14.3805C14.954 14.4686 15.0859 14.5126 15.2177 14.5126C15.3495 14.5126 15.4811 14.4685 15.58 14.3805C16.8647 13.3232 17.7538 11.884 17.9845 10.2393C18.0502 9.975 17.7868 9.7106 17.4575 9.7106Z" fill="#757676" />
                            <path d="M17.2844 7.5503C17.0134 3.4889 13.7983 0.2736 9.73684 0.00289989C9.71954 0.000899885 9.70244 0 9.68534 0C9.40584 0 9.19534 0.2571 9.19534 0.5444V8.0918H16.7427C17.0474 8.0918 17.3181 7.8549 17.2844 7.5503Z" fill="#757676" />
                            <path d="M7.57384 9.7134V1.707C7.57384 1.4203 7.30354 1.1636 7.01764 1.1636C7.00034 1.1636 6.98294 1.1645 6.96564 1.1665C2.91194 1.7408 -0.162262 5.2541 0.00663773 9.4769C0.140438 13.7603 3.78834 17.2815 8.09304 17.2815C8.13384 17.2815 8.17454 17.2812 8.21564 17.2806C9.90484 17.2806 11.4925 16.74 12.7761 15.7941C13.0464 15.6252 13.0464 15.2198 12.81 14.9833L7.57384 9.7134Z" fill="#757676" />
                        </svg>
                    </span>
                    Reports
                </div>
                <div className={styles.capsulestab} onClick={()=> handleCreate("People")}>
                    <span style={{ display: "grid" }}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none">
                            <path d="M17.1588 10.2559C19.0597 10.2559 20.6007 8.71491 20.6007 6.814C20.6007 4.91309 19.0597 3.3721 17.1588 3.3721C15.2579 3.3721 13.7169 4.91309 13.7169 6.814C13.7169 8.71491 15.2579 10.2559 17.1588 10.2559Z" fill="#757676" />
                            <path d="M18.9502 11.2942H15.3652C13.7083 11.2942 12.3652 12.6374 12.3652 14.2942V20.1278C12.3652 20.404 12.5891 20.6278 12.8652 20.6278H21.4502C21.7263 20.6278 21.9502 20.404 21.9502 20.1278V14.2942C21.9502 12.6374 20.6071 11.2942 18.9502 11.2942Z" fill="#757676" />
                            <path d="M5.4714 13.1769C6.66373 13.1769 7.6303 12.2104 7.6303 11.018C7.6303 9.8257 6.66373 8.85913 5.4714 8.85913C4.27907 8.85913 3.3125 9.8257 3.3125 11.018C3.3125 12.2104 4.27907 13.1769 5.4714 13.1769Z" fill="#757676" />
                            <path d="M4.66483 13.87C3.22003 13.87 2.04883 15.0259 2.04883 16.4517V20.1897C2.04883 20.4316 2.24753 20.6277 2.49263 20.6277H5.79883V15.1665C5.79883 14.7218 5.89133 14.2996 6.04763 13.9091C5.91823 13.8896 5.78883 13.8699 5.65393 13.8699L4.66483 13.87Z" fill="#757676" />
                            <path d="M10.8295 11.9292C12.3015 11.9292 13.4948 10.7359 13.4948 9.26393C13.4948 7.79193 12.3016 6.59863 10.8295 6.59863C9.35736 6.59863 8.16406 7.79203 8.16406 9.26393C8.16406 10.7358 9.35746 11.9292 10.8295 11.9292Z" fill="#757676" />
                            <path d="M9.99458 12.7852C8.31808 12.7852 6.95898 14.1266 6.95898 15.7814V20.1194C6.95898 20.4002 7.18968 20.6278 7.47408 20.6278H11.3108V14.29C11.3108 13.7739 11.4182 13.284 11.5995 12.8308C11.4494 12.8083 11.2992 12.7853 11.1426 12.7853L9.99458 12.7852Z" fill="#757676" />
                        </svg>
                    </span>
                    people
                </div>
                {/* <div className={styles.capsulestab} >
                    <span style={{ display: "grid" }}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="20" height="18" viewBox="0 0 20 18" fill="none">
                            <path d="M18.3926 4.0541H2.8127C2.2598 4.0541 1.8125 3.6068 1.8125 3.0539C1.8125 2.501 2.2599 2.0537 2.8127 2.0537H18.3048C19.6299 2.0537 19.697 0.0175999 18.372 0.000299931C18.3588 9.99306e-05 18.3456 0 18.3323 0H3.0005C1.3433 0 0 1.3434 0 3.0005V14.9533C0 16.6105 1.3434 17.9538 3.0005 17.9538H16.8923C18.5495 17.9538 19.8929 16.6104 19.8929 14.9532V5.5542C19.8929 4.7256 19.2212 4.0541 18.3926 4.0541ZM16.087 12.2479C15.2064 12.2479 14.4926 11.5341 14.4926 10.6535C14.4926 9.7729 15.2064 9.0591 16.087 9.0591C16.9676 9.0591 17.6814 9.773 17.6814 10.6535C17.6814 11.534 16.9676 12.2479 16.087 12.2479Z" fill="#757676" />
                        </svg>
                    </span>
                    Budget
                </div> */}
            </div>
            {(showLiveCampaign && allLoaded)?
            <div className={`${styles.basiccard}`}>
                <div className={styles.title}>
                    <span>
                        <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <path d="M0 14C0 6.26801 6.26801 0 14 0H26C33.732 0 40 6.26801 40 14V26C40 33.732 33.732 40 26 40H14C6.26801 40 0 33.732 0 26V14Z" fill="#DCECFE" />
                            <path d="M16.6666 11.6667V15" stroke="black" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            <path d="M23.3334 11.6667V15" stroke="black" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            <path d="M25.8333 13.3333H14.1667C13.2462 13.3333 12.5 14.0795 12.5 15V26.6667C12.5 27.5871 13.2462 28.3333 14.1667 28.3333H25.8333C26.7538 28.3333 27.5 27.5871 27.5 26.6667V15C27.5 14.0795 26.7538 13.3333 25.8333 13.3333Z" stroke="black" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            <path d="M12.5 18.3333H27.5" stroke="black" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                        </svg>
                    </span>
                    <h4>Live Campaign Summary:</h4>
                </div>
                <div className={styles.content}>
                    <div
                      className="chatContent"
                      dangerouslySetInnerHTML={{ __html: liveCampaignData?.liveCampaignSummary|| "" }}
                    />
                    {/* <p>As of now, 20 campaigns are live — 12 campaigns are performing above average engagement benchmarks.<br></br>
                        "Smart Gadget Launch" is the top performer, contributing 27% of total engagement.<br></br>
                        3 campaigns show fatigue — try refreshing creatives or adjusting audience segments.<br></br>
                        Referral-based campaigns show a 15% higher retention impact this week.</p> */}
                </div>
                <div className={styles.stscardwrapper}>
                    <div className={styles.stscard}>
                        <div className={styles.lefts}>
                            <span><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                <path d="M12.8975 10.7416L14.16 17.8466C14.1742 17.9302 14.1624 18.0162 14.1264 18.093C14.0903 18.1698 14.0317 18.2338 13.9583 18.2764C13.8849 18.319 13.8003 18.3382 13.7157 18.3313C13.6311 18.3245 13.5507 18.292 13.485 18.2382L10.5017 15.9991C10.3577 15.8915 10.1827 15.8333 10.003 15.8333C9.82318 15.8333 9.64822 15.8915 9.5042 15.9991L6.51587 18.2374C6.4503 18.2911 6.36992 18.3235 6.28545 18.3304C6.20098 18.3372 6.11643 18.3181 6.04309 18.2756C5.96975 18.2332 5.9111 18.1693 5.87497 18.0927C5.83884 18.016 5.82694 17.9302 5.84087 17.8466L7.10254 10.7416" stroke="#2563EB" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                <path d="M10 11.6667C12.7614 11.6667 15 9.42809 15 6.66666C15 3.90524 12.7614 1.66666 10 1.66666C7.23858 1.66666 5 3.90524 5 6.66666C5 9.42809 7.23858 11.6667 10 11.6667Z" stroke="#2563EB" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            </svg></span>
                        </div>

                        <div className={styles.rights}>
                            <h4>Active Campaigns</h4>
                            <p>{liveCampaignData?.activeCampaign??""}</p>
                            {/* <p>4.2x</p> */}
                        </div>

                    </div>
                    <div className={styles.stscard}>
                        <div className={styles.lefts}>
                            <span><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                <path d="M12.8975 10.7416L14.16 17.8466C14.1742 17.9302 14.1624 18.0162 14.1264 18.093C14.0903 18.1698 14.0317 18.2338 13.9583 18.2764C13.8849 18.319 13.8003 18.3382 13.7157 18.3313C13.6311 18.3245 13.5507 18.292 13.485 18.2382L10.5017 15.9991C10.3577 15.8915 10.1827 15.8333 10.003 15.8333C9.82318 15.8333 9.64822 15.8915 9.5042 15.9991L6.51587 18.2374C6.4503 18.2911 6.36992 18.3235 6.28545 18.3304C6.20098 18.3372 6.11643 18.3181 6.04309 18.2756C5.96975 18.2332 5.9111 18.1693 5.87497 18.0927C5.83884 18.016 5.82694 17.9302 5.84087 17.8466L7.10254 10.7416" stroke="#2563EB" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                <path d="M10 11.6667C12.7614 11.6667 15 9.42809 15 6.66666C15 3.90524 12.7614 1.66666 10 1.66666C7.23858 1.66666 5 3.90524 5 6.66666C5 9.42809 7.23858 11.6667 10 11.6667Z" stroke="#2563EB" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            </svg></span>
                        </div>

                        <div className={styles.rights}>
                            <h4>Schedule Campaigns</h4>
                            <p>{liveCampaignData?.scheduleCampaign??0}</p>
                            {/* <p>4.2x</p> */}
                        </div>

                    </div>
                   
                    <div className={styles.stscard}>
                        <div className={styles.lefts}>
                            <span><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                <path d="M3.33337 11.6666C3.17567 11.6671 3.02106 11.6229 2.8875 11.5391C2.75393 11.4552 2.6469 11.3352 2.57883 11.193C2.51076 11.0507 2.48445 10.8921 2.50295 10.7355C2.52146 10.5789 2.58402 10.4307 2.68337 10.3083L10.9334 1.80827C10.9953 1.73683 11.0796 1.68856 11.1725 1.67138C11.2655 1.65419 11.3615 1.66911 11.4448 1.71369C11.5281 1.75826 11.5939 1.82985 11.6311 1.91669C11.6684 2.00353 11.6751 2.10047 11.65 2.1916L10.05 7.20827C10.0029 7.33454 9.98701 7.47037 10.0039 7.6041C10.0207 7.73784 10.0698 7.8655 10.1468 7.97612C10.2238 8.08674 10.3265 8.17703 10.4461 8.23923C10.5657 8.30144 10.6986 8.3337 10.8334 8.33327H16.6667C16.8244 8.33273 16.979 8.37695 17.1126 8.46078C17.2461 8.54462 17.3532 8.66464 17.4212 8.80688C17.4893 8.94913 17.5156 9.10778 17.4971 9.26438C17.4786 9.42099 17.4161 9.56913 17.3167 9.6916L9.0667 18.1916C9.00482 18.263 8.92048 18.3113 8.82755 18.3285C8.73461 18.3457 8.6386 18.3308 8.55526 18.2862C8.47192 18.2416 8.40621 18.17 8.36892 18.0832C8.33163 17.9963 8.32497 17.8994 8.35003 17.8083L9.95003 12.7916C9.99721 12.6653 10.0131 12.5295 9.99621 12.3958C9.97936 12.262 9.93032 12.1344 9.85329 12.0237C9.77627 11.9131 9.67356 11.8228 9.55397 11.7606C9.43439 11.6984 9.3015 11.6662 9.1667 11.6666H3.33337Z" stroke="#F59E0B" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            </svg></span>
                        </div>


                        <div className={styles.rights}>
                            <h4>Engagement Rate</h4>
                            <p>{liveCampaignData?.engagementRate??""}</p>
                            {/* <p>4.2x</p> */}
                        </div>

                    </div>
                    <div className={styles.stscard}>
                        <div className={styles.lefts}>
                            <span><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                <path d="M13.3334 5.83334H18.3334V10.8333" stroke="#16A34A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                <path d="M18.3333 5.83334L11.25 12.9167L7.08329 8.75L1.66663 14.1667" stroke="#16A34A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            </svg></span>
                        </div>

                        <div className={styles.rights}>
                            <h4>Total Reach</h4>
                            <p>{liveCampaignData?.totalReach}</p>
                            {/* <p>4.2x</p> */}
                        </div>

                    </div>
                    <div className={styles.stscard}>
                        <div className={styles.lefts}>
                            <span><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                <path d="M9.99996 18.3333C14.6023 18.3333 18.3333 14.6024 18.3333 10C18.3333 5.39762 14.6023 1.66666 9.99996 1.66666C5.39759 1.66666 1.66663 5.39762 1.66663 10C1.66663 14.6024 5.39759 18.3333 9.99996 18.3333Z" stroke="#16A34A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                <path d="M10 15C12.7614 15 15 12.7614 15 10C15 7.23858 12.7614 5 10 5C7.23858 5 5 7.23858 5 10C5 12.7614 7.23858 15 10 15Z" stroke="#16A34A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                <path d="M10 11.6667C10.9205 11.6667 11.6667 10.9205 11.6667 10C11.6667 9.07953 10.9205 8.33334 10 8.33334C9.07957 8.33334 8.33337 9.07953 8.33337 10C8.33337 10.9205 9.07957 11.6667 10 11.6667Z" stroke="#16A34A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            </svg></span>
                        </div>

                        <div className={styles.rights}>
                            <h4>Total Impressions</h4>
                            <p>{liveCampaignData?.totalImpression}</p>
                            {/* <p>4.2x</p> */}
                        </div>

                    </div>
                </div>
                 <div className={`${styles.basiccard} ${styles.recomendationcard}`}>
                <div className={styles.title}>
                    <span>

                        <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <path d="M0 14C0 6.26801 6.26801 0 14 0H26C33.732 0 40 6.26801 40 14V26C40 33.732 33.732 40 26 40H14C6.26801 40 0 33.732 0 26V14Z" fill="#DCE2F9" />
                            <g clip-path="url(#clip0_3996_2488)">
                                <path d="M18.4892 26.664C17.652 26.9214 16.7496 27.0133 15.8228 26.9051C12.63 26.5325 10.2489 23.9079 10.0812 20.8037C10.0766 20.7152 10.0731 20.6256 10.0719 20.5359C10.0905 19.6405 10.2535 17.406 11.3341 15.4115C11.1257 16.0833 11.1955 16.8483 11.4563 17.505C11.9372 18.7183 12.8885 19.4717 13.8887 20.1936C14.6619 20.7525 15.4618 21.2927 16.0976 22.0112C17.2631 23.3304 18.1049 24.9442 18.4904 26.664H18.4892Z" fill="url(#paint0_linear_3996_2488)" />
                                <path d="M26.8786 21.1541C27.1604 21.9831 27.278 22.8832 27.1965 23.8124C26.9147 27.0144 24.36 29.4701 21.2628 29.7274C21.1743 29.7356 21.0846 29.7414 20.9962 29.7437C20.0996 29.7507 17.8616 29.6529 15.8368 28.6306C16.5145 28.8192 17.276 28.7284 17.9257 28.4489C19.1238 27.9343 19.8504 26.9608 20.5432 25.9408C21.0788 25.1526 21.597 24.3363 22.2956 23.6808C23.5811 22.478 25.1705 21.5896 26.8774 21.1541H26.8786Z" fill="url(#paint1_linear_3996_2488)" />
                                <path d="M21.484 13.4263C22.32 13.1655 23.2224 13.07 24.1492 13.1748C27.3431 13.5346 29.7347 16.1498 29.9164 19.2528C29.9222 19.3413 29.9257 19.431 29.9257 19.5206C29.9106 20.416 29.7569 22.6516 28.6845 24.6509C28.8894 23.9779 28.8172 23.214 28.5541 22.5573C28.0685 21.3464 27.1137 20.5965 26.1112 19.8781C25.3369 19.3227 24.5335 18.7859 23.8954 18.0698C22.724 16.7552 21.8763 15.1449 21.4828 13.4263H21.484Z" fill="url(#paint2_linear_3996_2488)" />
                                <path d="M20.0154 26.6863C20.7723 27.1276 21.606 27.407 22.5328 27.5107C25.7267 27.8704 28.6412 25.8549 29.5086 22.8706C29.5331 22.7856 29.5575 22.6994 29.5773 22.6121C29.7625 21.7353 30.4355 19.714 29.511 17.334C29.561 18.0361 29.32 18.7638 28.9171 19.346C28.1742 20.4184 27.0762 20.9366 25.9386 21.4128C25.0595 21.7819 24.1571 22.1266 23.3747 22.682C21.9402 23.7031 20.7793 25.0981 20.0143 26.6851L20.0154 26.6863Z" fill="url(#paint3_linear_3996_2488)" />
                                <path d="M13.2284 18.7509C12.9582 17.9172 12.8535 17.016 12.9489 16.088C13.2773 12.8906 15.8669 10.4722 18.9676 10.2591C19.0561 10.2533 19.1458 10.2486 19.2354 10.2463C20.132 10.2521 22.3676 10.3825 24.3785 11.4351C23.7031 11.2372 22.9405 11.3175 22.2873 11.5865C21.081 12.0837 20.3416 13.0466 19.6336 14.0561C19.0864 14.8363 18.5577 15.6455 17.8486 16.2906C16.5457 17.4747 14.9447 18.3399 13.2307 18.7497L13.2284 18.7509Z" fill="url(#paint4_linear_3996_2488)" />
                                <path d="M26.4721 20.1379C26.9052 19.3764 27.2068 18.4868 27.3023 17.5588C27.6295 14.3614 25.5836 11.4679 22.5912 10.6307C22.5062 10.6063 22.4189 10.5842 22.3315 10.5644C21.4524 10.3885 19.4252 9.73532 17.0546 10.6855C17.7555 10.6284 18.4868 10.8613 19.0724 11.2583C20.153 11.9907 20.6816 13.0829 21.1706 14.2158C21.5479 15.0915 21.9019 15.9904 22.4654 16.7658C23.5006 18.1899 24.8769 19.3903 26.4721 20.139V20.1379Z" fill="url(#paint5_linear_3996_2488)" />
                                <path d="M20.0751 13.446C19.3194 13.0024 18.4263 12.6845 17.4995 12.5762C14.3067 12.2025 11.3841 14.2075 10.505 17.1883C10.4794 17.2733 10.4561 17.3595 10.4352 17.4468C10.2465 18.3225 9.56538 20.3415 10.4806 22.725C10.4328 22.0228 10.6774 21.2951 11.0826 20.7164C11.8301 19.6475 12.9293 19.1329 14.0692 18.6613C14.9495 18.2957 15.8542 17.9545 16.6378 17.4026C18.077 16.3884 19.3042 15.0319 20.0774 13.4472L20.0751 13.446Z" fill="url(#paint6_linear_3996_2488)" />
                                <path d="M13.6022 19.9863C13.1795 20.7536 12.9059 21.6222 12.8244 22.5514C12.5426 25.7535 14.6303 28.6167 17.6356 29.4108C17.7206 29.4341 17.8079 29.455 17.8953 29.4737C18.7767 29.6367 20.8132 30.2596 23.1699 29.2769C22.4701 29.3444 21.7354 29.122 21.1439 28.7331C20.0529 28.017 19.5079 26.9318 19.0038 25.807C18.6137 24.9372 18.2469 24.043 17.6717 23.2757C16.6156 21.8668 15.2067 20.7129 13.601 19.9863H13.6022Z" fill="url(#paint7_linear_3996_2488)" />
                            </g>
                            <defs>
                                <linearGradient id="paint0_linear_3996_2488" x1="10.8058" y1="13.1937" x2="14.2819" y2="26.3023" gradientUnits="userSpaceOnUse">
                                    <stop stop-color="#6C00FF" />
                                    <stop offset="0.52" stop-color="#4702D1" />
                                    <stop offset="1" stop-color="#250E83" />
                                </linearGradient>
                                <linearGradient id="paint1_linear_3996_2488" x1="20.9542" y1="24.8568" x2="26.57" y2="26.4869" gradientUnits="userSpaceOnUse">
                                    <stop stop-color="#6C00FF" />
                                    <stop offset="0.52" stop-color="#4400CB" />
                                    <stop offset="1" stop-color="#1E0099" />
                                </linearGradient>
                                <linearGradient id="paint2_linear_3996_2488" x1="27.8217" y1="21.1566" x2="25.6687" y2="15.9844" gradientUnits="userSpaceOnUse">
                                    <stop offset="0.24" stop-color="#1E0099" />
                                    <stop offset="0.81" stop-color="#6C00FF" />
                                </linearGradient>
                                <linearGradient id="paint3_linear_3996_2488" x1="26.8946" y1="29.407" x2="26.8946" y2="15.5641" gradientUnits="userSpaceOnUse">
                                    <stop stop-color="#00F9FF" />
                                    <stop offset="0.5" stop-color="#6C00FF" />
                                </linearGradient>
                                <linearGradient id="paint4_linear_3996_2488" x1="13.6988" y1="14.3263" x2="20.0388" y2="14.7338" gradientUnits="userSpaceOnUse">
                                    <stop stop-color="#6C00FF" />
                                    <stop offset="0.52" stop-color="#4400CB" />
                                    <stop offset="1" stop-color="#1E0099" />
                                </linearGradient>
                                <linearGradient id="paint5_linear_3996_2488" x1="20.6618" y1="10.6726" x2="27.3376" y2="22.6139" gradientUnits="userSpaceOnUse">
                                    <stop offset="0.269069" stop-color="#6C00FF" />
                                    <stop offset="1" stop-color="#00F9FF" />
                                </linearGradient>
                                <linearGradient id="paint6_linear_3996_2488" x1="10.0125" y1="17.6273" x2="21.4467" y2="17.6273" gradientUnits="userSpaceOnUse">
                                    <stop offset="0.06" stop-color="#380085" />
                                    <stop offset="0.5625" stop-color="#4E44FF" />
                                    <stop offset="1" stop-color="#00F9FF" />
                                </linearGradient>
                                <linearGradient id="paint7_linear_3996_2488" x1="12.7988" y1="24.9023" x2="23.1699" y2="24.9023" gradientUnits="userSpaceOnUse">
                                    <stop offset="0.212285" stop-color="#6C00FF" />
                                    <stop offset="1" stop-color="#00F9FF" />
                                </linearGradient>
                                <clipPath id="clip0_3996_2488">
                                    <rect width="19.9761" height="19.6372" fill="white" transform="translate(10.012 10.1814)" />
                                </clipPath>
                            </defs>
                        </svg>
                    </span>
                    <h4>AI Recommendation</h4>
                </div>
                <div className={styles.content}>
                   <p>{liveCampaignData?.aiRecommendation??""}</p>
                   {/* <p>Your WhatsApp campaigns get the best results between 6–9 PM. Try scheduling then — and add first names for ~15% higher engagement.</p> */}
                </div>
              
            </div>
            </div>:<><div>
            
              <SkeletonBox width="70%" height="20px" />
              <SkeletonBox width="80%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="90%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="100%" height="140px" />
            
          </div></>
}
{(showTopCampaign && allLoaded)?
            <div className={`${styles.basiccard} ${styles.recomendationcard} ${styles.topperformingcampaign}`}>
                <div className={styles.title}>
                    <span>


                        <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                            <path d="M0 12C0 5.37258 5.37258 0 12 0H28C34.6274 0 40 5.37258 40 12V28C40 34.6274 34.6274 40 28 40H12C5.37258 40 0 34.6274 0 28V12Z" fill="#F3E8FF" />
                            <path d="M23.3333 15.8333H28.3333V20.8333" stroke="#0A0A0A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                            <path d="M28.3333 15.8333L21.25 22.9166L17.0833 18.75L11.6666 24.1666" stroke="#0A0A0A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                        </svg>

                    </span>
                    <div className={styles.withbadge}>
                        <span className={styles.badge}>Top Performing Campaign</span>
                         <h4>{topPerformingData?.tcp_heading??""}</h4>
                         {/* <h4>"Diwali Delight Sale"</h4> */}
                    </div>
                  
                </div>
                <div className={styles.content}>
                    {/* <ul>
                        <li>WhatsApp is leading in engagement with 78% opens, but clicks are moderate.</li>
                        <li>Email is converting the best at 8.4%, showing that detailed messages still resonate.</li>
                        <li>SMS engagement has dipped, likely due to repetitive content.</li>
                        <li>App Push notifications are steady, but Web Push needs timing adjustments</li>
                    </ul> */}
                    <div
                      className="chatContent"
                      dangerouslySetInnerHTML={{ __html: topPerformingData?.tcp_summary|| "" }}
                    />
                    <div className={styles.diwalistscardwrapper}>
                        <div className={styles.stscard}>
                            <p>{topPerformingData?.tcp_suggestion?.[0]??""}</p>
                            {/* <p>Personalized subject lines drove 42% opens</p> */}

                        </div>
                        <div className={`${styles.stscard} ${styles.lightgreen}`}>
                            <p>{topPerformingData?.tcp_suggestion?.[1]??""}</p>
                            {/* <p>WhatsApp had highest engagement at 82%</p> */}

                        </div>
                        <div className={`${styles.stscard} ${styles.greengray}`}>
                            <p>{topPerformingData?.tcp_suggestion?.[2]??""}</p>
                            {/* <p>Lunchtime (12-2 PM) best for Web Push</p> */}

                        </div>

                    </div>
                </div>
                 <div className={`${styles.innercard} ${styles.blue} ${styles.performanceanalysis}`}>
                           <div className={styles.title}>
                      
                                 < span>


                     
<svg width="32" height="32" viewBox="0 0 32 32" fill="none" xmlns="http://www.w3.org/2000/svg">
<mask id="path-1-inside-1_4035_2545" fill="white">
<path d="M0 10C0 4.47715 4.47715 0 10 0H22C27.5228 0 32 4.47715 32 10V22C32 27.5228 27.5228 32 22 32H10C4.47715 32 0 27.5228 0 22V10Z"/>
</mask>
<path d="M0 10C0 4.47715 4.47715 0 10 0H22C27.5228 0 32 4.47715 32 10V22C32 27.5228 27.5228 32 22 32H10C4.47715 32 0 27.5228 0 22V10Z" fill="white"/>
<path d="M10 0V1H22V0V-1H10V0ZM32 10H31V22H32H33V10H32ZM22 32V31H10V32V33H22V32ZM0 22H1V10H0H-1V22H0ZM10 32V31C5.02944 31 1 26.9706 1 22H0H-1C-1 28.0751 3.92487 33 10 33V32ZM32 22H31C31 26.9706 26.9706 31 22 31V32V33C28.0751 33 33 28.0751 33 22H32ZM22 0V1C26.9706 1 31 5.02944 31 10H32H33C33 3.92487 28.0751 -1 22 -1V0ZM10 0V-1C3.92487 -1 -1 3.92487 -1 10H0H1C1 5.02944 5.02944 1 10 1V0Z" fill="#E5E7EB" mask="url(#path-1-inside-1_4035_2545)"/>
<path d="M19.3333 11.8333H24.3333V16.8333" stroke="#0A0A0A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round"/>
<path d="M24.3333 11.8333L17.25 18.9167L13.0833 14.75L7.66663 20.1667" stroke="#0A0A0A" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round"/>
</svg>


                                  </span>
                           
                     <div className={styles.withbadge}>
                       <h4>Performance Analysis</h4>
                      <p>{topPerformingData?.tcp_analysis??""}</p>
                      {/* <p>Here's how your channels are performing overall this month</p> */}
                     </div>
                 
                            </div>
                            {/* <div className={styles.smallinnerwhitecardwrapper}>
                              <div className={styles.whitecard}>

                                <p>WhatsApp leads with 78% opens, clicks moderate</p>
                              </div>
                              <div className={styles.whitecard}>

                                <p>WhatsApp leads with 78% opens, clicks moderate</p>
                              </div>
                              <div className={styles.whitecard}>

                                <p>WhatsApp leads with 78% opens, clicks moderate</p>
                              </div>
                              <div className={styles.whitecard}>

                                <p>WhatsApp leads with 78% opens, clicks moderate</p>
                              </div>
                            </div> */}

                              <div className={styles.smallinnerwhitecardwrapper}>
                                {topPerformingData?.tcp_channelAnalysis?.map((item :any, index :any) => (
                                  <div className={styles.whitecard} key={index}>
                                    <p>{item ?? ""}</p>
                                  </div>
                                ))}
                              </div>
                           
                </div>

            </div>:<><div>
            <div className="skeletonTextGroup">
              <SkeletonBox width="70%" height="20px" />
              <SkeletonBox width="80%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="90%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="100%" height="140px" />
            </div>
          </div></>
}

          {/* {(showAudienceCampaign && allLoaded) ?
            <div className={styles.halfcardcontainer}>
              <div className={`${styles.basiccard} ${styles.yellowcard}`}>
                <div className={styles.title}>

                  <h4>Audience Fatigue Detected</h4>
                </div>
                <div className={styles.content}>


                  <div
                    className="chatContent"
                    dangerouslySetInnerHTML={{ __html: audienceFatigueData?.audienceFatigueSummary || "" }}
                  />
                </div>
                <div className={`${styles.innercard} ${styles.yellow}`}>
                  <p>{audienceFatigueData?.audienceFatigueSuggestion ?? ''}</p>

                </div>

              </div>
              <div className={`${styles.basiccard} ${styles.bluecard}`}>
                <div className={styles.title}>

                  <h4>Hidden Audience Segment Emerging</h4>
                </div>
                <div className={styles.content}>

                  <div
                    className="chatContent"
                    dangerouslySetInnerHTML={{ __html: audienceFatigueData?.hiddenAudienceSummary || "" }}
                  />
                </div>
                <div className={`${styles.innercard} ${styles.blue}`}>
                  <p>{audienceFatigueData?.hiddenAudienceSuggestion}</p>

                </div>

              </div>
            </div> : <><div>
              <div className="skeletonTextGroup">
                <SkeletonBox width="70%" height="20px" />
                <SkeletonBox width="80%" height="14px" />
                <SkeletonBox width="60%" height="14px" />
                <SkeletonBox width="90%" height="14px" />
                <SkeletonBox width="60%" height="14px" />
                <SkeletonBox width="100%" height="140px" />
              </div>
            </div></>
          } */}
            {(showObservationCampaign && allLoaded)?
            <div className={`${styles.basiccard} ${styles.greycard}`}>
                <div className={styles.title}>
                    <span className={styles.badge}>AI Observation</span>
                    <h4>{observatonData?.ai_observationHeading}</h4>
                    {/* <h4>Your Campaigns Are Talking — But Not Always to the Right Crowd</h4> */}
                </div>
                <div className={styles.content}>
                    <p className={styles.subheading}>Audience Drift in Ongoing Campaigns</p>
                    {/* <ul>
                        <li>Till this moment, your ‘leads’ segment is showing signs of burnout. </li>
                        <li>Engagement dropped by 32%, and unsubscribes rose by 18% in your last four campaigns.</li>
                    </ul> */}
                    <div
                          className="chatContent"
                          dangerouslySetInnerHTML={{ __html: observatonData?.ai_observationSummary || "" }}
                        />
                </div>
                <div className={`${styles.innercard} ${styles.blue}`}>
                    <div className={styles.title}>
                        <span>

                            <svg width="40" height="40" viewBox="0 0 40 40" fill="none" xmlns="http://www.w3.org/2000/svg">
                                <path d="M0 14C0 6.26801 6.26801 0 14 0H26C33.732 0 40 6.26801 40 14V26C40 33.732 33.732 40 26 40H14C6.26801 40 0 33.732 0 26V14Z" fill="#DCE2F9" />
                                <g clip-path="url(#clip0_3996_2488)">
                                    <path d="M18.4892 26.664C17.652 26.9214 16.7496 27.0133 15.8228 26.9051C12.63 26.5325 10.2489 23.9079 10.0812 20.8037C10.0766 20.7152 10.0731 20.6256 10.0719 20.5359C10.0905 19.6405 10.2535 17.406 11.3341 15.4115C11.1257 16.0833 11.1955 16.8483 11.4563 17.505C11.9372 18.7183 12.8885 19.4717 13.8887 20.1936C14.6619 20.7525 15.4618 21.2927 16.0976 22.0112C17.2631 23.3304 18.1049 24.9442 18.4904 26.664H18.4892Z" fill="url(#paint0_linear_3996_2488)" />
                                    <path d="M26.8786 21.1541C27.1604 21.9831 27.278 22.8832 27.1965 23.8124C26.9147 27.0144 24.36 29.4701 21.2628 29.7274C21.1743 29.7356 21.0846 29.7414 20.9962 29.7437C20.0996 29.7507 17.8616 29.6529 15.8368 28.6306C16.5145 28.8192 17.276 28.7284 17.9257 28.4489C19.1238 27.9343 19.8504 26.9608 20.5432 25.9408C21.0788 25.1526 21.597 24.3363 22.2956 23.6808C23.5811 22.478 25.1705 21.5896 26.8774 21.1541H26.8786Z" fill="url(#paint1_linear_3996_2488)" />
                                    <path d="M21.484 13.4263C22.32 13.1655 23.2224 13.07 24.1492 13.1748C27.3431 13.5346 29.7347 16.1498 29.9164 19.2528C29.9222 19.3413 29.9257 19.431 29.9257 19.5206C29.9106 20.416 29.7569 22.6516 28.6845 24.6509C28.8894 23.9779 28.8172 23.214 28.5541 22.5573C28.0685 21.3464 27.1137 20.5965 26.1112 19.8781C25.3369 19.3227 24.5335 18.7859 23.8954 18.0698C22.724 16.7552 21.8763 15.1449 21.4828 13.4263H21.484Z" fill="url(#paint2_linear_3996_2488)" />
                                    <path d="M20.0154 26.6863C20.7723 27.1276 21.606 27.407 22.5328 27.5107C25.7267 27.8704 28.6412 25.8549 29.5086 22.8706C29.5331 22.7856 29.5575 22.6994 29.5773 22.6121C29.7625 21.7353 30.4355 19.714 29.511 17.334C29.561 18.0361 29.32 18.7638 28.9171 19.346C28.1742 20.4184 27.0762 20.9366 25.9386 21.4128C25.0595 21.7819 24.1571 22.1266 23.3747 22.682C21.9402 23.7031 20.7793 25.0981 20.0143 26.6851L20.0154 26.6863Z" fill="url(#paint3_linear_3996_2488)" />
                                    <path d="M13.2284 18.7509C12.9582 17.9172 12.8535 17.016 12.9489 16.088C13.2773 12.8906 15.8669 10.4722 18.9676 10.2591C19.0561 10.2533 19.1458 10.2486 19.2354 10.2463C20.132 10.2521 22.3676 10.3825 24.3785 11.4351C23.7031 11.2372 22.9405 11.3175 22.2873 11.5865C21.081 12.0837 20.3416 13.0466 19.6336 14.0561C19.0864 14.8363 18.5577 15.6455 17.8486 16.2906C16.5457 17.4747 14.9447 18.3399 13.2307 18.7497L13.2284 18.7509Z" fill="url(#paint4_linear_3996_2488)" />
                                    <path d="M26.4721 20.1379C26.9052 19.3764 27.2068 18.4868 27.3023 17.5588C27.6295 14.3614 25.5836 11.4679 22.5912 10.6307C22.5062 10.6063 22.4189 10.5842 22.3315 10.5644C21.4524 10.3885 19.4252 9.73532 17.0546 10.6855C17.7555 10.6284 18.4868 10.8613 19.0724 11.2583C20.153 11.9907 20.6816 13.0829 21.1706 14.2158C21.5479 15.0915 21.9019 15.9904 22.4654 16.7658C23.5006 18.1899 24.8769 19.3903 26.4721 20.139V20.1379Z" fill="url(#paint5_linear_3996_2488)" />
                                    <path d="M20.0751 13.446C19.3194 13.0024 18.4263 12.6845 17.4995 12.5762C14.3067 12.2025 11.3841 14.2075 10.505 17.1883C10.4794 17.2733 10.4561 17.3595 10.4352 17.4468C10.2465 18.3225 9.56538 20.3415 10.4806 22.725C10.4328 22.0228 10.6774 21.2951 11.0826 20.7164C11.8301 19.6475 12.9293 19.1329 14.0692 18.6613C14.9495 18.2957 15.8542 17.9545 16.6378 17.4026C18.077 16.3884 19.3042 15.0319 20.0774 13.4472L20.0751 13.446Z" fill="url(#paint6_linear_3996_2488)" />
                                    <path d="M13.6022 19.9863C13.1795 20.7536 12.9059 21.6222 12.8244 22.5514C12.5426 25.7535 14.6303 28.6167 17.6356 29.4108C17.7206 29.4341 17.8079 29.455 17.8953 29.4737C18.7767 29.6367 20.8132 30.2596 23.1699 29.2769C22.4701 29.3444 21.7354 29.122 21.1439 28.7331C20.0529 28.017 19.5079 26.9318 19.0038 25.807C18.6137 24.9372 18.2469 24.043 17.6717 23.2757C16.6156 21.8668 15.2067 20.7129 13.601 19.9863H13.6022Z" fill="url(#paint7_linear_3996_2488)" />
                                </g>
                                <defs>
                                    <linearGradient id="paint0_linear_3996_2488" x1="10.8058" y1="13.1937" x2="14.2819" y2="26.3023" gradientUnits="userSpaceOnUse">
                                        <stop stop-color="#6C00FF" />
                                        <stop offset="0.52" stop-color="#4702D1" />
                                        <stop offset="1" stop-color="#250E83" />
                                    </linearGradient>
                                    <linearGradient id="paint1_linear_3996_2488" x1="20.9542" y1="24.8568" x2="26.57" y2="26.4869" gradientUnits="userSpaceOnUse">
                                        <stop stop-color="#6C00FF" />
                                        <stop offset="0.52" stop-color="#4400CB" />
                                        <stop offset="1" stop-color="#1E0099" />
                                    </linearGradient>
                                    <linearGradient id="paint2_linear_3996_2488" x1="27.8217" y1="21.1566" x2="25.6687" y2="15.9844" gradientUnits="userSpaceOnUse">
                                        <stop offset="0.24" stop-color="#1E0099" />
                                        <stop offset="0.81" stop-color="#6C00FF" />
                                    </linearGradient>
                                    <linearGradient id="paint3_linear_3996_2488" x1="26.8946" y1="29.407" x2="26.8946" y2="15.5641" gradientUnits="userSpaceOnUse">
                                        <stop stop-color="#00F9FF" />
                                        <stop offset="0.5" stop-color="#6C00FF" />
                                    </linearGradient>
                                    <linearGradient id="paint4_linear_3996_2488" x1="13.6988" y1="14.3263" x2="20.0388" y2="14.7338" gradientUnits="userSpaceOnUse">
                                        <stop stop-color="#6C00FF" />
                                        <stop offset="0.52" stop-color="#4400CB" />
                                        <stop offset="1" stop-color="#1E0099" />
                                    </linearGradient>
                                    <linearGradient id="paint5_linear_3996_2488" x1="20.6618" y1="10.6726" x2="27.3376" y2="22.6139" gradientUnits="userSpaceOnUse">
                                        <stop offset="0.269069" stop-color="#6C00FF" />
                                        <stop offset="1" stop-color="#00F9FF" />
                                    </linearGradient>
                                    <linearGradient id="paint6_linear_3996_2488" x1="10.0125" y1="17.6273" x2="21.4467" y2="17.6273" gradientUnits="userSpaceOnUse">
                                        <stop offset="0.06" stop-color="#380085" />
                                        <stop offset="0.5625" stop-color="#4E44FF" />
                                        <stop offset="1" stop-color="#00F9FF" />
                                    </linearGradient>
                                    <linearGradient id="paint7_linear_3996_2488" x1="12.7988" y1="24.9023" x2="23.1699" y2="24.9023" gradientUnits="userSpaceOnUse">
                                        <stop offset="0.212285" stop-color="#6C00FF" />
                                        <stop offset="1" stop-color="#00F9FF" />
                                    </linearGradient>
                                    <clipPath id="clip0_3996_2488">
                                        <rect width="19.9761" height="19.6372" fill="white" transform="translate(10.012 10.1814)" />
                                    </clipPath>
                                </defs>
                            </svg>
                        </span>
                        <h4>Recommendation</h4>
                    </div>
                    <div className={styles.content}>
                       <div
                          className="chatContent"
                          dangerouslySetInnerHTML={{ __html: observatonData?.ai_observationRecommendation || "" }}
                        />
                        {/* <p>{observatonData?.ai_observationRecommendation}</p> */}
                        {/* <p>Let’s refine your targeting by layering in age group and recent purchase filters before you rerun it.”</p> */}
                    </div>
                </div>

            </div>:<><div>
            <div className="skeletonTextGroup">
              <SkeletonBox width="70%" height="20px" />
              <SkeletonBox width="80%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="90%" height="14px" />
              <SkeletonBox width="60%" height="14px" />
              <SkeletonBox width="100%" height="140px" />
            </div>
          </div></>
}
               </div>


        </>

    )
}

export default Initialhomepagenew