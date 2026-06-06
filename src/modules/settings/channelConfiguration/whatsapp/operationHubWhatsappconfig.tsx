import React, { useEffect, useRef, useState } from 'react'
import styles from "./scss/whatsapp.module.scss"
import BootstrapTable from 'react-bootstrap-table-next';
import paginationFactory from 'react-bootstrap-table2-paginator';
import { tableOptions } from '../../../../components/custom_components/tableOptions';
import { assets } from '../../../../styles/assets/assets';
import { whatsappService } from '../../../../base/service/apiservice';
import Loaders from '../../../../components/loader/loader';
import WorktualModal from '../../../../components/custom_components/worktualModal';
import { useToast } from '../../../../components/custom_components/useToast';
import BlurToastDemo from '../../../../components/custom_components/blurtast/blurtoast';
import { config } from '../../../../base/constant';
import { GetItemFromStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
import { useNavigate, useSearchParams } from 'react-router-dom';
import SignupStatus from '../../../../components/whatsappconfig/signupStatus';
import Spinner from '../../../../components/custom_components/spinner/spinner';

function OperationHubWhatsappConfig() {
  const navigate = useNavigate();
  const [loader, setLoader] = useState<any>(false);
  const [accountDatas, setAccountDatas] = useState([]);
  const [loading, setLoading] = useState(true);
  const [whatsappDeatils, setWhatsappDetails] = useState<any>(null);
  const [showModal, setShowModal] = useState(false);
  const [signupStatus,setSignUpStatus] = useState<any>(false);
  console.log("signup",signupStatus);
  const { toastOpen, toastMessage, showToast, closeToast } = useToast();
  const loginUserDetails :any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
  
console.log("lg",loginUserDetails)

  
  const wabaIdRef = useRef<string | null>(null);
  const businessIdRef = useRef<string | null>(null);
  const phoneNumberIdRef = useRef<string | null>(null);


  useEffect(() => {
    getWhatsappDetails();
  }, []);

  const getWhatsappDetails = () => {
    try {
      setLoader(true);
      whatsappService.get(`/get_business_account/null`)
        .then((response) => {
          if (response?.data?.statusCode === 200 && response?.data?.getAccountsNumbers?.length > 0) {
            setAccountDatas(response?.data?.getAccountsNumbers);
            setLoader(false);
          } else {
            setAccountDatas([]);
            setLoader(false);
          }
        }).catch((error) => {
          console.log(error)
          setAccountDatas([])
        }).finally(() => {
          setLoading(false);
          setLoader(false);
          const loaders = document.getElementById('contact_center_mainloader');
          if (loaders) {
            loaders.style.display = 'none';
          }
        });
    } catch (error) {
      console.log(error)
    }
  }

  const handleStatusChange = async (data: any) => {
    try {
      const body: any = {
        mobileNumber: data?.mobileNumber
      };
      const response: any = await whatsappService.post(body, `/change_account_status`);
      if (response?.data?.statusCode === 200) {
        getWhatsappDetails();
        showToast("✅ Connected successfully!");
      } else {
        showToast("❌ Failed to update status");
      }
    } catch (error) {
      showToast("❌ Failed to update status");
    }
  };

  const handleBotChange = async (data: any) => {
    try {
      const body: any = {
        mobileNumber: data?.mobileNumber
      };
      const response: any = await whatsappService.post(body, `/update_bot_action`);
      if (response?.data?.statusCode === 200) {
        getWhatsappDetails();
        showToast("✅ Bot Enable successfully!");
      } else {
        showToast("❌ Bot action Failed");
      }
    } catch (error) {
      showToast("❌ Bot action Failed");
    }
    finally{
      setLoader(false);
    }
  };

  const handleRowClick = (row: any) => {
    setWhatsappDetails(row);
    setShowModal(true);
  };

  const columns = [
    {
      dataField: "businessName",
      text: "Business Name",
      sort: true,
      formatter: (item: any, row: any) => (
        <span
          className="clickabletxt"
          style={{ cursor: "pointer" }}
          onClick={() => handleRowClick(row)}
        >
          {item}
        </span>
      )
    },
    // {
    //   dataField: "action",
    //   text: "",
    //   sort: false,
    //   formatter: (cell: any, row: any) => {
    //     return <div style={{ position: "relative" }}>
    //       <span className="tablehideselect" >
    //         <div  style={{color:"#00b885"}} className="selecttxtcontainer">
    //           Actions{" "}
    //           <span
    //             style={{ cursor: "pointer" }}

    //           >
    //             <svg
    //               xmlns="http://www.w3.org/2000/svg"
    //               width="12"
    //               height="6"
    //               viewBox="0 0 12 6"
    //               fill="none"
    //             >
    //               <path
    //                 d="M5.9996 5.90231C5.3971 5.90231 4.8316 5.66791 4.4063 5.24121L0.552798 1.37691C0.260298 1.08391 0.261298 0.609307 0.554298 0.316407C0.848198 0.024407 1.3233 0.0244068 1.6153 0.318407L5.4683 4.18271C5.752 4.46591 6.2476 4.46591 6.5303 4.18271L10.3838 0.318407C10.6758 0.0245068 11.1509 0.024507 11.4448 0.316407C11.7378 0.609407 11.7387 1.08401 11.4463 1.37691L7.5928 5.24121C7.1675 5.66801 6.6017 5.90231 5.9996 5.90231Z"
    //                 fill="#00B885"
    //               />
    //             </svg>
    //           </span>
    //         </div>
    //         <div
    //           style={{
    //             position: "absolute",
    //             top: "100%",
    //            left:52,
    //             background: "white",
    //             boxShadow: "0 2px 6px rgba(0,0,0,0.15)",
    //             borderRadius: "4px",
    //             zIndex: 10,
    //             width: "120px"
    //           }}
    //         >
    //           <div className="dropdown-item" onClick={() => handleStatusChange(row)}>{row?.isactive === 0 ? "Connect Number" : "Not Connect Number"}</div>
    //           <div className="dropdown-item" onClick={() => handleBotChange(row)}>{row?.isBot === 0 ? "Enable Bot" : "Disable Bot"}</div>
    //         </div>
    //       </span>
    //     </div>
    //   }
    // },

    {
      dataField: "mobileNumber",
      text: "Sender Number",
      formatter: (item: string) => {
        return <>
          <span >{
            item
          }</span>
        </>
      }
    },
    {
      dataField: "isactive",
      text: "Status",
      sort: true,
      formatter: (item: number) =>
        item === 1 ? (
          <span className={`${styles.statuselem} ${styles.pending}`}>Connected</span>
        ) : (
          <span className={`${styles.statuselem} ${styles.formaterr}`}>Not Connected</span>
        )
    },
    {
      dataField: "isBot",
      text: "Bot Enable/Disable",
      sort: true,
      formatter: (item: number) =>
        item === 1 ? (
          <span className={`${styles.statuselem} ${styles.pending}`}>Enabled</span>
        ) : (
          <span className={`${styles.statuselem} ${styles.formaterr}`}>Disabled</span>
        )
    },
  ];

  const data = [
    { business_name: 'BrightWave Solutions', sender_number: '+44 7700 900123', status: 'Active' },
  ]

  const tableOpt = {
    ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
      text: "All",
      value: 20,
    }]
  }

  const launchWhatsAppSignup = () => {
    // console.log("here comes");
    try {
      setLoader(true);
    
       (window as any).FB.login(
      (response: any) => {
        // setLoader(true);
        if (response.authResponse) {
          const code = response.authResponse.code;
          window.location.href = `${config().BUSINESS_WHATSAPP_CONF_URL}?code=${code}&domainId=${loginUserDetails?.domainId}` +
            `${wabaIdRef.current ? `&wabaId=${wabaIdRef.current}` : ''}` +
            `${businessIdRef.current ? `&businessId=${businessIdRef.current}` : ''}` +
            `${phoneNumberIdRef.current ? `&phoneNumberId=${phoneNumberIdRef.current}` : ''}`;
        }
      },
      {
        config_id: "1109736967317755",
        response_type: "code",
        override_default_response_type: true,
        extras: {
          setup: {},
          featureType: "whatsapp_business_app_onboarding",
          sessionInfoVersion: "3",
        },
      }
    );
    } catch (error :any) {
      setLoader(false);
      console.log("err",error)
    }
    finally{
      setLoader(false);
    }
   
  };

    useEffect(() => {
    // Load the Facebook SDK
    const loadFbSdk = () => {
      (window as any).fbAsyncInit = function () {
        (window as any).FB.init({
          appId: "2792648964217350",
          autoLogAppEvents: true,
          xfbml: true,
          version: "v22.0",
        });
      };

      const script = document.createElement("script");
      script.src = "https://connect.facebook.net/en_US/sdk.js";
      script.async = true;
      script.defer = true;
      script.crossOrigin = "anonymous";
      document.body.appendChild(script);
    };

    loadFbSdk();

    const handleReceiveEmbeddedSignupData = ({ waba_id, business_id, phone_number_id }: { waba_id: string, business_id: string, phone_number_id: string }) => {
      wabaIdRef.current = waba_id;
      businessIdRef.current = business_id;
      phoneNumberIdRef.current = phone_number_id;
    };

    // Handle messages from Facebook
    const handleMessage = (event: MessageEvent) => {
      if (
        event.origin !== "https://www.facebook.com" &&
        event.origin !== "https://web.facebook.com"
      ) {
        return;
      }

      try {
        const data = JSON.parse(event.data);
        if (data.type === "WA_EMBEDDED_SIGNUP") {
          if (data.event === "FINISH_WHATSAPP_BUSINESS_APP_ONBOARDING") {
            console.log(
              "Phone number ID:",
              data.data.phone_number_id,
              "WhatsApp business account ID:",
              data.data.waba_id,
              "Business ID:",
              data.data.business_id
            );
            const waba_id = data.data.waba_id;
            const business_id = data.data.business_id;
            const phone_number_id = data.data.phone_number_id;
            handleReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
          } else if (data.event === "FINISH") {
            console.log(
              "Phone number ID:",
              data.data.phone_number_id,
              "WhatsApp business account ID:",
              data.data.waba_id,
              "Business ID:",
              data.data.business_id
            );
            const waba_id = data.data.waba_id;
            const business_id = data.data.business_id;
            const phone_number_id = data.data.phone_number_id;
            handleReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
          } else if (data.event === "CANCEL") {
            console.warn("Cancel at:", data.data.current_step);
          } else if (data.event === "ERROR") {
            console.error("Error:", data.data.error_message);
          }
        }
      } catch {
        console.log("Non JSON Responses", event.data);
      }
      
    };

    window.addEventListener("message", handleMessage);

    return () => {
      window.removeEventListener("message", handleMessage);
    };
  }, []);


    useEffect(()=>{
      if(signupStatus){

      }
    },[signupStatus])



  return (
    <>{signupStatus?
    //    <SignupStatus status={signUpStatus} onGoBack={() => {navigate('/settings');handleSignupStatus();}} />
        <></>
       :<>

    
      {loader ? (
         <div className='loadersinner'><Spinner /></div>
        //  <div className='loadersinner'><Loaders /></div>
        // <Loaders />
      ) : (
        <div className={styles.overallbody}>
          <div className={styles.innerbody}>
            {/* <div className={styles.headerarea}>
              <div className={styles.lefts}>
                <h5>Whatsapp configuration</h5>
                <p>Microcopy for Whatsapp configuration</p>
              </div>
              <div className={styles.rights}>
                <button className='primarybtn' onClick={launchWhatsAppSignup} >Add account</button>
              </div>
            </div> */}
             <div className={`${styles.topheading} ${styles.header}`}>
                <div className={styles.tophdlefts}>
                    <h2>Whatsapp configuration</h2>
                    <p className={styles.headerdesctxt}>Set up WhatsApp to manage and send campaigns effectively.</p>
                </div>
                {/* <div className='formbtns'>
                        { accountDatas?.length > 0 && <button className='primarybtn' onClick={launchWhatsAppSignup} >Add account</button>}
                </div> */}
            </div>
            {!loading && accountDatas?.length > 0 && (
              <div className='table-wrapper'>

              <div className='desktoptable desktoptablescroll'>
                <BootstrapTable
                  keyField="id"
                  data={accountDatas}
                  columns={columns}
                  pagination={paginationFactory(tableOpt)}
                />
              </div>
              </div>
            )}
            {!loading && accountDatas?.length === 0 && (
              <div style={{ display: "grid", placeItems: "center", }}>
                <div className='pannelcenter'>
                  <div className='imgcon'>
                    <img src={assets.emptywapp} />
                    


                  </div>
                  <div className='info'>
                    <h5>Configure WhatsApp Channel</h5>
                    <p>You need to connect with your WhatsApp business account to proceed with this integration.</p>
                  </div>
                  {/* <button className='primarybtn' onClick={launchWhatsAppSignup}>Add Account</button> */}
                </div>
              </div>
            )}
          </div>
          {/* ✅ Toast Component */}
          <BlurToastDemo
            open={toastOpen}
            message={
              <div style={{ display: "flex", alignItems: "center", gap: "8px" }}>
                <span>{toastMessage}</span>
                <svg
                  style={{ cursor: "pointer" }}
                  onClick={closeToast}
                  xmlns="http://www.w3.org/2000/svg"
                  width="11"
                  height="10"
                  viewBox="0 0 11 10"
                  fill="none"
                >

                  <path
                    d="M6.29984 4.99937L10.2032 1.09571C10.4245 0.874973 10.4245 0.51564 10.2032 0.294906C9.98184 0.0735729 9.62244 0.0735729 9.40237 0.294906L5.49904 4.19851L1.59571 0.294906C1.37564 0.0735729 1.01497 0.0735729 0.794906 0.294906C0.573573 0.51564 0.573573 0.874973 0.794906 1.09571L4.69824 4.99937L0.794906 8.90304C0.573573 9.12377 0.573573 9.48311 0.794906 9.70384C0.905573 9.81451 1.05011 9.86984 1.19531 9.86984C1.34051 9.86984 1.48564 9.81451 1.59571 9.70384L5.49904 5.80024L9.40237 9.70384C9.51237 9.81451 9.65757 9.86984 9.80277 9.86984C9.94797 9.86984 10.0925 9.81451 10.2032 9.70384C10.4245 9.48311 10.4245 9.12377 10.2032 8.90304L6.29984 4.99937Z"
                    fill="white"
                  />
                </svg>
              </div>
            }
          />
        </div>
      )
      }
      </>
    }
      {showModal && (
        <WorktualModal
          show={showModal}
          title={whatsappDeatils?.businessName || "WhatsApp Details"}
          onClose={() => setShowModal(false)}
          footer={
            <div className={styles.footerbtns}>
              <button className='cancelbtn' onClick={() => setShowModal(false)}>Go Back</button>
            </div>
          }
        >
          <>
            <div className={styles.viewmodalbody}>
              <div className={styles.row}>
                <div className={styles.view}>WhatsApp Number</div>
                <div className={styles.data}>{whatsappDeatils?.mobileNumber}</div>
              </div>
              <div className={styles.row}>
                <div className={styles.view}>WhatsApp Business Name</div>
                <div className={styles.data}>{whatsappDeatils?.businessName}</div>
              </div>
              <div className={styles.row}>
                <div className={styles.view}>Message Limit</div>
                <div className={styles.data}>250</div>
              </div>
              <div className={styles.row}>
                <div className={styles.view}>Phone Number Status</div>
                <div className={styles.data}>{whatsappDeatils?.status}</div>
              </div>
            </div>
          </>
        </WorktualModal>
      )}
    </>
  );
}

export default OperationHubWhatsappConfig