import styles from "./scss/topbar.module.scss";
import { assets, leftnavassets } from "../styles/assets/assets";
import WorktualDropdown from "../components/custom_components/WroktualDropdown";
import {
  ClearStorage,
  GetItemFromStorage,
} from "../base/customhooks/useStorage";
import Cookies from "../utils/cookies";
import Notification from "../modules/notification/notification";
import { useEffect, useRef, useState } from "react";
import { whatsappService } from "../base/service/apiservice";
import { useDispatch, useSelector } from "react-redux";
import operationshub from "../../src/styles/images/icons/operationshubicon.svg";
import { useNavigate } from "react-router-dom";
import { LocalStorage } from "../base/customhooks/localStorageKeys";
import { jwtDecode } from "jwt-decode";
import { commonDataSliceActions } from "../store/slice/commonSlice/commonSlice";
import { LoginOutService } from "../base/service/loginService";
import { getClientData } from "../myplans-new/apiservice/myplan";
import axios from "axios";
const TopBar = () => {
  const navigate = useNavigate();
  const dispatch = useDispatch();
  const onboardRefresh=useSelector((state :any)=>state.commonData.onboardrefresh);
  const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
  const LoginCcaasUserDetails: any = jwtDecode(accessToken);
  console.log("login", LoginCcaasUserDetails);
  const [showNotification, setShowNotification] = useState<any>(false);
  const [viewProfile, setViewProfile] = useState<any>(false);
  console.log("vp", viewProfile);

  const dropdownRef = useRef<HTMLDivElement | null>(null);
  const dropdownRefTwo = useRef<HTMLDivElement | null>(null);

  const notificationData = useSelector(
    (state: any) => state.campaignData.notification,
  );
  const [completeSetup, setCompleteSetup] = useState<any>(-1);
  const forceLogout = useSelector((state :any)=>state.commonData.forceLogout);
  const insertNotification = (notificationData: any) => {
    try {
      let body: any = {
        id: null,
        channelType: notificationData?.channelType,
        title: notificationData?.title,
        message: notificationData?.message,
        isRead: notificationData?.isRead,
        status: notificationData?.status,
      };
      whatsappService
        .post(body, "/insert_bell_notification_campaign")
        .then(async (res: any) => {
          console.log("insert_bell_notification_campaign_res", res);
          getNotification();
        })
        .catch((err: any) => {
          console.log("error", err);
        });
    } catch (error: any) {
      console.log("err", error);
    }
  };
  const [sampleData, setSampleData] = useState<any>([]);
  console.log("sampledata", sampleData);
  const getNotification = async () => {
    try {
      const body: any = {
        limit: 30,
        offset: 0,
      };
      await whatsappService
        .post(body, "/get_bell_notification_campaign")
        .then((res: any) => {
          console.log("getNotifiction", res?.data?.response);
          setSampleData(res?.data?.response);
          // setNotificationLength()
          // setShowNotification(true);
        })
        .catch((err: any) => {
          console.log("err", err);
        });
    } catch (error) {
      console.log("err", error);
    }
  };

  const onboardSetup = () => {
    try {
      whatsappService
        .get("/onboardCheck")
        .then((res: any) => {
          setCompleteSetup(
            res?.data?.deleteDocResponse?.[0]?.onboard_completion_percentage ||
              0,
          );
        })
        .catch((err: any) => {
          console.log("err:", err);
        });
    } catch (error) {
      console.log("err", error);
    }
  };

  const updateHeartBeat=async()=>{
  try {
      const body :any={
        email:LoginCcaasUserDetails?.Email
      }
      await whatsappService.post(body,'/updateHeartBeat').then((res :any)=>{
        console.log("getNotifiction",res?.data?.response);       
        // setNotificationLength()
        // setShowNotification(true);
      }).catch((err :any)=>{
        console.log("err",err)
      })
   } catch (error) {
     console.log("err",error)
   } 
}

const frequentAPICalls=async()=>{
  try {
    
    await getNotification();
    await updateHeartBeat();
  } catch (error) {
    console.log("err:",error)
  }
}

  useEffect(() => {
    let isMounted = true;

    const run = async () => {
      if (!isMounted) return;

      try {
        await frequentAPICalls();
      } catch (error) {
        console.log("err:", error);
      }

      if (isMounted) {
        setTimeout(run, 60000);
      }
    };

    onboardSetup();
    run();

    return () => {
      isMounted = false;
    };
  }, []);


useEffect(()=>{
    if(onboardRefresh){
      onboardSetup();
      dispatch(commonDataSliceActions.setOnboardRefresh(false));
    }
},[onboardRefresh])


  const inActivelogout=async(email :any)=>{
    try {
      const response = await axios.post(
        `${import.meta.env.VITE_APP_ENV === "development" ? "https://qacampaignapi.worktual.co.uk/campaignService/v1/logout" : "https://campaignapi.worktual.co.uk/campaignService/v1/logout"}`,
        {
          email,
        }
      );

      console.log("Success:", response.data);
     
    } catch (error: any) {
      console.error("Error:", error.response?.data || error.message);
    }
  }

  const logout = async () => {
    try {
      const clientData: any = await getClientData();
      const body = {
        username: LoginCcaasUserDetails.username,
        password: "",
        deviceId: LoginCcaasUserDetails.deviceId,
        source: "CAMPAIGN",
        ipAddress: clientData?.ip || LoginCcaasUserDetails.ipaddress,
        deviceType: clientData?.deviceName || clientData?.device || "Web",
        browser: clientData?.browser || "",
        loginType: "",
        productId: 1,
      };
      await inActivelogout(LoginCcaasUserDetails?.Email);
      await LoginOutService.logout(body);
    } catch (error) {
      console.log("err", error);
    } finally {
      signOut();
    }
  };

  const profileOpt = [{ label: "LogOut", value: 1 }];

  const signOut = () => {
    Cookies.clear();
    ClearStorage();
    localStorage.clear();
    sessionStorage.clear();
    // navigate("/login");
    window.location.href = "/login";
  };

  // const signOut = async() => {

  //  try {
  //    const body={
  //     email:LoginCcaasUserDetails?.Email
  //    }
  //    const res= await whatsappService.post(body,'/logout');
  //    if(res?.data?.statusCode==200 && res?.data?.errCode==1){
  //      Cookies.clear();
  //      ClearStorage();
  //      localStorage.clear();
  //      sessionStorage.clear();
  //      window.location.href = '/login';
  //    }else{
  //       alert("something went wrong!");
  //    }
  //  } catch (error) {
  //     alert("something went wrong!");
  //     console.log("ERR:",error);
  //  }

  // };

  useEffect(() => {
    if (notificationData) {
      insertNotification(notificationData);
    }
  }, [notificationData]);

  useEffect(() => {
    const handleClickOutside = (event: MouseEvent | TouchEvent) => {
      if (
        dropdownRef.current &&
        !dropdownRef.current.contains(event.target as Node)
      ) {
        setViewProfile(false);
      }
    };

    if (viewProfile) {
      document.addEventListener("mousedown", handleClickOutside);
      document.addEventListener("touchstart", handleClickOutside);
    }

    return () => {
      document.removeEventListener("mousedown", handleClickOutside);
      document.removeEventListener("touchstart", handleClickOutside);
    };
  }, [viewProfile, setViewProfile]);

  useEffect(() => {
    getNotification();
    let path = window.location.pathname.split("/").pop();
    path = path?.split("?")[0]?.split("#")[0]; // remove query/hash
    if (path != "home") {
      dispatch(commonDataSliceActions.setShowMainChat(true));
    }
  }, []);

  useEffect(() => {
    const handleClickOutside = (event: MouseEvent) => {
      if (
        dropdownRefTwo.current &&
        !dropdownRefTwo.current.contains(event.target as Node)
      ) {
        setShowNotification(false);
      }
    };

    document.addEventListener("mousedown", handleClickOutside);

    return () => {
      document.removeEventListener("mousedown", handleClickOutside);
    };
  }, []);

  useEffect(()=>{
      if(forceLogout){
        dispatch(commonDataSliceActions.setForceLogout(false));
        navigate('/login');
      }
  },[forceLogout])

  return (
    <>
      <div className={`${styles.topbar} myplansclass`}>
        <div className={`${styles.tblefts} lefts`}>
          <img
            className={`${styles.logo} logo`}
            src={assets.worktual_logo}
            alt="logo"
          />
        </div>
        <div className={styles.tbrights}>
          <ul className={styles.tbrlistshorizontal}>
            {/* <li className={styles.tbrlist}>
              <span className={styles.tbriconwrapper}><img src={assets.help_icon} alt="help_icon" /></span>
            </li> */}
            {completeSetup != 100 && completeSetup != -1 && (
              <li>
                <div
                  className={styles.completevalue}
                  onClick={() => navigate("/onboard")}
                >
                  <div className={styles.setitle}>Complete setup</div>
                  <CircularProgress value={completeSetup} />
                </div>
              </li>
            )}
            <li>
              <button
                className="primarybtn violet"
                onClick={() => navigate("/knowledgebaseOperationsHub")}
              >
                {" "}
                <span style={{ display: "grid" }}>
                  <img src={operationshub}></img>
                </span>
                Operations Hub
              </button>
            </li>
            <li style={{ position: "relative" }} className={styles.tbrlist}  >
              <span className={styles.tbriconwrapper}>
                <img
                  style={{ position: "relative" }}
                  src={assets.notify_icon}
                  alt="notify_icon"
                 onClick={() => {
                    setShowNotification(!showNotification);
                    getNotification();
                  }}
                />
                {sampleData?.some((n: any) => n?.isRead === 0) && (
                  <span
                    style={{ position: "absolute" }}
                    className={styles.counterspan}
                  >
                    {sampleData.filter((n: any) => n?.isRead === 0)?.length}
                  </span>
                )}
              </span>
              {showNotification && (
                <div ref={dropdownRefTwo}>
                  <Notification
                    sampleDatas={sampleData}
                    getNotification={getNotification}
                  />
                </div>
              )}
            </li>
            <li className={styles.tbrlist}>
              <div className="dropdownprof">
                {viewProfile && (
                  <div className={styles.profiledropdown} ref={dropdownRef}>
                    <div className={styles.profileMenu}>
                      <div className={styles.avatarSection}>
                        <div className={styles.avatar}>
                          {LoginCcaasUserDetails?.first_name
                            ?.charAt(0)
                            ?.toUpperCase()}
                        </div>
                        <h3>{LoginCcaasUserDetails?.first_name}</h3>
                      </div>

                      <div className={styles.menu}>
                        <div
                          className={styles.menuItem}
                          onClick={() => navigate("/yourinfo")}
                        >
                          <div className={styles.lefts}>
                            <svg
                              xmlns="http://www.w3.org/2000/svg"
                              width="21"
                              height="20"
                              viewBox="0 0 21 20"
                              fill="none"
                            >
                              <path
                                d="M19.3178 9.6815C20.0004 9.0038 20.2348 8.0165 19.9286 7.1044C19.6229 6.1923 18.8407 5.5458 17.8881 5.4169L14.4677 4.9521C14.1694 4.9121 13.9101 4.7265 13.7739 4.458L12.2177 1.3779C11.788 0.5263 10.9321 0 9.9799 0C9.9721 0 9.9638 1.65892e-07 9.956 0.00100017C8.9941 0.00980017 8.1372 0.5528 7.7197 1.419L6.2212 4.5274C6.0903 4.7989 5.8345 4.9903 5.5366 5.0362L2.126 5.5645C1.1758 5.711 0.406299 6.3721 0.117699 7.2891C-0.171401 8.2071 0.0810995 9.1905 0.7759 9.8555L3.2447 12.2168C3.4786 12.4404 3.587 12.7637 3.5352 13.083L2.9893 16.4551C2.8355 17.4053 3.2266 18.3418 4.0103 18.8994C4.4463 19.2099 4.9532 19.3682 5.4634 19.3682C5.8696 19.3682 6.2779 19.2676 6.6533 19.0655L9.6929 17.4307C9.9571 17.2891 10.2769 17.2852 10.545 17.4229L13.6148 19C14.4693 19.4385 15.481 19.3574 16.254 18.7852C17.027 18.213 17.4 17.2686 17.2286 16.3223L16.6139 12.9268C16.5602 12.6299 16.6559 12.3262 16.8698 12.1133L19.3178 9.6815Z"
                                fill="#404040"
                              />
                            </svg>
                            My Plans
                          </div>
                          <svg
                            xmlns="http://www.w3.org/2000/svg"
                            width="6"
                            height="12"
                            viewBox="0 0 6 12"
                            fill="none"
                          >
                            <path
                              d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z"
                              fill="black"
                            />
                          </svg>
                        </div>
                        {/* <div className={styles.menuItem}>
                    <div className={styles.lefts}>
                      <svg xmlns="http://www.w3.org/2000/svg" width="21" height="20" viewBox="0 0 21 20" fill="none">
                        <path d="M19.3178 9.6815C20.0004 9.0038 20.2348 8.0165 19.9286 7.1044C19.6229 6.1923 18.8407 5.5458 17.8881 5.4169L14.4677 4.9521C14.1694 4.9121 13.9101 4.7265 13.7739 4.458L12.2177 1.3779C11.788 0.5263 10.9321 0 9.9799 0C9.9721 0 9.9638 1.65892e-07 9.956 0.00100017C8.9941 0.00980017 8.1372 0.5528 7.7197 1.419L6.2212 4.5274C6.0903 4.7989 5.8345 4.9903 5.5366 5.0362L2.126 5.5645C1.1758 5.711 0.406299 6.3721 0.117699 7.2891C-0.171401 8.2071 0.0810995 9.1905 0.7759 9.8555L3.2447 12.2168C3.4786 12.4404 3.587 12.7637 3.5352 13.083L2.9893 16.4551C2.8355 17.4053 3.2266 18.3418 4.0103 18.8994C4.4463 19.2099 4.9532 19.3682 5.4634 19.3682C5.8696 19.3682 6.2779 19.2676 6.6533 19.0655L9.6929 17.4307C9.9571 17.2891 10.2769 17.2852 10.545 17.4229L13.6148 19C14.4693 19.4385 15.481 19.3574 16.254 18.7852C17.027 18.213 17.4 17.2686 17.2286 16.3223L16.6139 12.9268C16.5602 12.6299 16.6559 12.3262 16.8698 12.1133L19.3178 9.6815Z" fill="#404040" />
                      </svg>
                      <span>Widget Configuration</span>
                    </div>
                    <span>
                      <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                        <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                      </svg>

                    </span>

                  </div> */}

                        {/* <div className={styles.menuItem}>
                          <div className={styles.lefts}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                              <path d="M11.5332 10.2058C11.3886 10.1947 11.2453 10.24 11.1236 10.319C10.1578 10.9472 8.9391 11.219 7.6508 10.9342C7.1237 10.8181 6.6355 10.6063 6.1997 10.3225C6.0784 10.2432 5.9352 10.1985 5.7909 10.2106C2.5515 10.476 0 13.1674 0 16.4753V17.0081C0 17.3436 0.2721 17.6157 0.6076 17.6157H16.7499C17.0856 17.6157 17.3575 17.3436 17.3575 17.0081V16.4753C17.3575 13.156 14.7888 10.4553 11.5332 10.2058Z" fill="#404040" />
                              <path d="M7.6781 8.6504C8.0173 8.7254 8.3521 8.761 8.6783 8.761C11.4249 8.761 13.5789 6.2368 12.9483 3.3804C12.5872 1.7447 11.3142 0.4717 9.6785 0.1106C9.3393 0.0356 9.0045 0 8.6783 0C5.9317 0 3.7777 2.5242 4.4084 5.3806C4.7695 7.0163 6.0423 8.2893 7.6781 8.6504Z" fill="black" />
                            </svg>View Profile
                          </div>
                          <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                          </svg>
                        </div> */}

                        {/* <div className={styles.menuItem}>
                          <div className={styles.lefts}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                              <path d="M18.5812 10.9286L17.6964 10.4179V9.12594L18.6354 8.58394C19.1881 8.26464 19.4572 7.61914 19.2892 7.01364C19.0905 6.29484 18.8063 5.59274 18.445 4.92774C18.1408 4.36724 17.4929 4.09574 16.861 4.25784L15.8742 4.52244L14.9606 3.61034L15.2409 2.56444C15.4055 1.95014 15.1398 1.30374 14.5949 0.994142C13.945 0.625042 13.2482 0.327142 12.5221 0.111342C11.9074 -0.0732583 11.2531 0.195342 10.9279 0.757842L10.4162 1.64354H9.12568L8.58318 0.704042C8.26388 0.151342 7.61738 -0.120158 7.01188 0.0507419C6.29358 0.248942 5.59198 0.533142 4.92598 0.895442C4.36398 1.20014 4.08908 1.85154 4.25708 2.47844L4.52168 3.46574L3.60808 4.37884L2.56268 4.09854C1.94798 3.93354 1.30198 4.20014 0.993377 4.74404C0.622277 5.39634 0.325377 6.09364 0.110577 6.81534C-0.072023 7.42964 0.193577 8.08584 0.757077 8.41204L1.64238 8.92374V10.2137L0.703377 10.7567C0.150677 11.076 -0.118423 11.7215 0.0495769 12.327C0.248277 13.0458 0.532477 13.7479 0.893777 14.4129C1.19798 14.9734 1.84688 15.2508 2.47778 15.0828L3.46458 14.8182L4.37818 15.7313L4.09788 16.7762C3.93328 17.3914 4.19898 18.0369 4.74338 18.3465C5.39478 18.7166 6.09108 19.0135 6.81618 19.2303C7.42748 19.41 8.08568 19.1453 8.41038 18.5828L8.92208 17.6971H10.2131L10.7556 18.6366C11.0115 19.079 11.4768 19.3397 11.9626 19.3397C12.0832 19.3397 12.2058 19.3231 12.3264 19.2899C13.0452 19.0917 13.7473 18.8065 14.4128 18.4452C14.9753 18.1395 15.2502 17.4882 15.0813 16.8622L14.8167 15.8749L15.7303 14.9618L16.7762 15.2421C17.3914 15.4032 18.0365 15.1405 18.3455 14.5966C18.7166 13.9443 19.0135 13.247 19.2283 12.5253C19.4104 11.912 19.1452 11.2558 18.5812 10.9286ZM9.66908 12.8241C7.93028 12.8241 6.51578 11.4091 6.51578 9.67084C6.51578 7.93154 7.93038 6.51654 9.66908 6.51654C11.4078 6.51654 12.8229 7.93154 12.8229 9.67084C12.8229 11.4091 11.4083 12.8241 9.66908 12.8241Z" fill="#404040" />
                            </svg>
                            <span>Settings</span>
                          </div>
                          <span>
                            <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                              <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="black" />
                            </svg>

                          </span>
                        </div> */}

                        <div className={styles.menuItem} onClick={logout}>
                          <div className={styles.lefts}>
                            <svg
                              xmlns="http://www.w3.org/2000/svg"
                              width="19"
                              height="17"
                              viewBox="0 0 19 17"
                              fill="none"
                            >
                              <path
                                d="M14.0236 2.74238L18.5807 7.29941C18.8479 7.5666 19 7.93398 19 8.3125C19 8.69102 18.8479 9.0584 18.5807 9.32559L14.0236 13.8826C13.7861 14.1201 13.467 14.25 13.133 14.25C12.4391 14.25 11.875 13.6859 11.875 12.992V10.6875H7.125C6.46816 10.6875 5.9375 10.1568 5.9375 9.5V7.125C5.9375 6.46816 6.46816 5.9375 7.125 5.9375H11.875V3.63301C11.875 2.93906 12.4391 2.375 13.133 2.375C13.467 2.375 13.7861 2.50859 14.0236 2.74238ZM5.9375 2.375H3.5625C2.90566 2.375 2.375 2.90566 2.375 3.5625V13.0625C2.375 13.7193 2.90566 14.25 3.5625 14.25H5.9375C6.59434 14.25 7.125 14.7807 7.125 15.4375C7.125 16.0943 6.59434 16.625 5.9375 16.625H3.5625C1.5957 16.625 0 15.0293 0 13.0625V3.5625C0 1.5957 1.5957 0 3.5625 0H5.9375C6.59434 0 7.125 0.530664 7.125 1.1875C7.125 1.84434 6.59434 2.375 5.9375 2.375Z"
                                fill="#DE003D"
                              />
                            </svg>
                            Logout
                          </div>
                        </div>
                      </div>
                    </div>
                  </div>
                )}
                <div
                  className={styles.tbshowprofile}
                  onClick={() => setViewProfile(true)}
                >
                  <div className={styles.profilenamelist}>
                    {LoginCcaasUserDetails?.first_name
                      ?.charAt(0)
                      ?.toUpperCase()}
                  </div>
                  {/* <img src={`https://plus.unsplash.com/premium_photo-1689568126014-06fea9d5d341?fm=jpg&q=60&w=3000&ixlib=rb-4.1.0&ixid=M3wxMjA3fDB8MHxzZWFyY2h8MXx8cHJvZmlsZXxlbnwwfHwwfHx8MA%3D%3D`} alt="profile" /> */}
                </div>
                {/* <WorktualDropdown options={profileOpt}  onChange={signOut}>
              <div className={styles.tbshowprofile}>
                <img src={`https://plus.unsplash.com/premium_photo-1689568126014-06fea9d5d341?fm=jpg&q=60&w=3000&ixlib=rb-4.1.0&ixid=M3wxMjA3fDB8MHxzZWFyY2h8MXx8cHJvZmlsZXxlbnwwfHwwfHx8MA%3D%3D`} alt="profile" />
                </div>
            
           
              </WorktualDropdown> */}
              </div>
            </li>
          </ul>
        </div>
      </div>
    </>
  );
};

export default TopBar;

export const CircularProgress = ({ value = 0 }: any) => {
  const size = 40;
  const strokeWidth = 3;
  const radius = (size - strokeWidth) / 2;
  const center = size / 2;
  const circumference = 2 * Math.PI * radius;
  const offset = circumference * (1 - value / 100);

  return (
    <div className={styles.progresswrapper}>
      <svg width={size} height={size}>
        <circle
          cx={center}
          cy={center}
          r={radius}
          stroke="#2a2a2a"
          strokeWidth={strokeWidth}
          fill="none"
        />
        <circle
          cx={center}
          cy={center}
          r={radius}
          stroke="#fff"
          strokeWidth={strokeWidth}
          fill="none"
          strokeLinecap="round"
          strokeDasharray={circumference}
          strokeDashoffset={offset}
          transform={`rotate(-90 ${center} ${center})`}
        />
      </svg>

      <div className={styles.progresstext}>
        {value}
        <span>%</span>
      </div>
    </div>
  );
};
