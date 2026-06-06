// import React, { useEffect, useState } from "react";
// import styles from "../../../../assets/styles/autoreneval.module.scss";
// import addon1 from "../../../../assets/images/myplan/addon1.svg";
// import contactcentericonrec from '../../../../assets/images/agentworkspace/agentrecord.svg'
// import { jwtDecode } from "jwt-decode";
// import moment from "moment";
// import { useNavigate } from "react-router-dom";
// import { GetItemFromStorage } from "../../../../../base/customhooks/useStorage";
// import { LocalStorage } from "../../../../../base/customhooks/localStorageKeys";
// import { AICoreServices, CoreServices } from "../../../../../base/service/apiservice";
// import Accountsuspended from "./accountsuspended";

// export default function ContactCenterSummary() {
//   const accessToken = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
//   const loginUserDetail = jwtDecode(accessToken);
//   const [open, setOpen] = useState(true);
//   const [subscriptionList, setSubscriptionList] = useState<any>();
//   const [addOnList, setAddonList] = useState<any>([]);

//   const navigate = useNavigate()

//   const getSubscriptionManagement = async () => {
//     const accessToken = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
//     const loginUserDetail = jwtDecode(accessToken);

//     try {
//       let payload = {
//         company_id: loginUserDetail?.company_id
//       }
//       console.log("payloadpayload", payload)
//       AICoreServices.post(payload, `/get_my_plans_subscription_management`)
//         .then((response: any) => {
//           // console.log("inside response", response)
//           if (response && response.data.statusCode === 0) {
//             // console.log("getUserManagementInfo", response)
//             setSubscriptionList(response.data.result[0]);
//           }
//         })

//     } catch (err) {
//       console.log("getUserExtensionInfo err:", err);
//     }

//   };

//   const getAddonList = () => {
//     try {
//       // CoreServices.post(payload, `/get_my_plans_addon_list`)
//       AICoreServices.get(`/get_addOn_purchase_list`)
//         .then((response: any) => {
//           console.log("inside_response", response)
//           if (response && response.data.statusCode === 200) {
//             setAddonList(response.data.result[0])
//           }
//         })

//     } catch (err) {
//       console.log("getUserExtensionInfo err:", err);
//     }

//   };
//   useEffect(() => {
//     const init = () => {
//       getSubscriptionManagement();
//       getAddonList();
//     }
//     init()
//   }, [])

//   const addons = [
//     {
//       id: 1,
//       icon: addon1,
//       title: "Single Level - IVR",
//       desc: "Handle audio responses/call routing using a single-level IVR menu with up to 5 DTMF keys.",
//       price: "£14",
//     },
//     {
//       id: 2,
//       icon: addon1,
//       title: "Multi Level - IVR",
//       desc: "Handle audio responses/call routing via multiple IVR menus at different levels.",
//       price: "£14",
//     },
//     {
//       id: 3,
//       icon: addon1,
//       title: "Hunt Group",
//       desc: "Route incoming calls automatically to particular groups.",
//       price: "£14",
//     },
//   ];

//   return (
//     <>
//       {/* <div className={styles.createmaincontainer}>
//         <div className={styles.headerole}>
//           <h3>Subscription Management</h3>
//           <p>Manage all your product's subscription and billing details</p>
//         </div>
//         <div className={styles.cardblockeditor}>Products</div>
//         <div className={styles.subscribe}>
//           <div className={styles.cardblock}>
//             <div className={styles.cardblocktop}>
//               <div className={styles.cardblocktopleft}>
//                 <span><img src={contactcentericonrec}></img></span>
//                 <div className={styles.titlee}>
//                   <div className={styles.title}>{subscriptionList?.prd_name}</div><span className={styles.role}>imac</span>
//                 </div>

//                 <span className={styles.status}>Active</span>
//               </div>
//               <div className={styles.cardblocktopright}>
//                 <button>Deactivate</button>
//               </div>
//             </div>
//             <div className={styles.cardblockbottom}>
//               <div className={styles.cardblockbottomleft}>
//                 <div className={styles.fieldmain}>
//                   <div className={styles.label}>Users</div>
//                   <div className={styles.value}>{subscriptionList?.user_count}</div>
//                 </div>

//                 <div className={styles.fieldmain}>
//                   <div className={styles.label}>Contract type</div>
//                   <div className={styles.value}>Monthly</div>
//                 </div>

//                 <div className={styles.fieldmain}>
//                   <div className={styles.label}>Next billing date</div>
//                   <div className={styles.value}>{subscriptionList?.bill_date}</div></div>

//                 <div className={styles.fieldmain}>
//                   <div className={styles.label}>Contact end rate</div>
//                   <div className={styles.value}>{moment(subscriptionList?.contract_end_date).format('YYYY-MM-DD')}</div>
//                 </div>
//               </div>

//             </div>

//           </div>

//         </div>

//         <div className={styles.addonsSection}>

//           {addOnList?.length > 0 && <span className={styles.title}> Add ons</span>}
//           <div className={styles.addonList}>
//             {addons.map((addon) => (
//             {addOnList.map((addon: any, ind: any) => (<>{addon?.isPurchased == 1 &&

//               <div key={ind} className={styles.addonCard}>
//                 <div className={styles.addonCardleft}>
//                   <div className={styles.icon}>
//                     <img src={addon.image_name} alt="" />
//                   </div>
//                   <div className={styles.info}>
//                     <div className={styles.title}>{addon.addon_feature_name}</div>
//                     <div className={styles.desc}>{addon.addon_feature_description}</div>
//                   </div>
//                 </div>

//               </div>}</>
//             ))}
//           </div>

//         </div>

//         <div className={styles.addonsSection}>
//           {addOnList?.length > 0 && <span className={styles.title}> Recommendation</span>}
//           <div className={styles.addonList}>
//             {addOnList.map((addon: any, ind: any) => (<>{addon?.isPurchased == 0 &&
//               <div key={ind} className={styles.addonCard}>
//                 <div className={styles.addonCardleft}>
//                   <div className={styles.icon}>
//                     <img
//                       src={
//                         addon?.image_name &&
//                           addon.image_name !== "-" &&
//                           addon.image_name.trim() !== ""
//                           ? addon.image_name
//                           : "https://qaurstorage.worktual.co.uk/userdetails_myaccount/ddd.png"
//                       }

//                       onError={(e) => {
//                         e.target.onerror = null;
//                         e.target.src = "https://qaurstorage.worktual.co.uk/userdetails_myaccount/8da49aac-1e28-4919-9bd9-a879a51b8360-Rulebased_FacebookBot.png";
//                       }}
//                       alt="" />
//                   </div>
//                   <div className={styles.info}>
//                     <div className={styles.title}>{addon.addon_feature_name}</div>
//                     <div className={styles.desc}>{addon.addon_feature_description}</div>
//                   </div>
//                 </div>
//                 <div className={styles.addonCardright}>

//                   <button onClick={() => navigate('/home')}>Buy Now</button>
//                 </div>
//               </div>}</>
//             ))}
//           </div>

//         </div>
//       </div> */}
//        <div className={styles.billingndsubscriptionbody}>
//                 <div className={styles.billingsubscriptioning}>
//                     <div className={styles.headerole}>
//                         <h3>Billing & subscription</h3>
//                         <p>About billing & subscription</p>
//                     </div>
//                     <div className={styles.companydetails}>
//                         <div className={styles.compname}>
//                             Company Name
//                         </div>
//                         <div className={styles.detailswithchevron}>Details <span><svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
//                             <path d="M5.66561 5.80465C5.06311 5.80465 4.49761 5.57025 4.07231 5.14355L0.218813 1.27925C-0.0736866 0.98625 -0.0726864 0.511651 0.220314 0.218751C0.514214 -0.0732493 0.989313 -0.0732494 1.28131 0.220751L5.13431 4.08505C5.41801 4.36825 5.91361 4.36825 6.19631 4.08505L10.0498 0.220751C10.3418 -0.0731494 10.8169 -0.0731493 11.1108 0.218751C11.4038 0.511751 11.4047 0.98635 11.1123 1.27925L7.25881 5.14355C6.83351 5.57035 6.26771 5.80465 5.66561 5.80465Z" fill="#23232B" />
//                         </svg></span></div>
//                     </div>
//                     <div className={styles.redtoast}>
//                         <div className={styles.inner}>
//                             Termination event intimation alert  <span>intimation CTA</span>
//                             <span className={styles.cancelbtn}>
//                                 <svg xmlns="http://www.w3.org/2000/svg" width="10" height="10" viewBox="0 0 10 10" fill="none">
//                                     <path d="M5.67093 4.87047L9.57427 0.9668C9.7956 0.746067 9.7956 0.386733 9.57427 0.166C9.35293 -0.0553333 8.99353 -0.0553333 8.77347 0.166L4.87013 4.0696L0.9668 0.166C0.746733 -0.0553333 0.386067 -0.0553333 0.166 0.166C-0.0553333 0.386733 -0.0553333 0.746067 0.166 0.9668L4.06933 4.87047L0.166 8.77413C-0.0553333 8.99487 -0.0553333 9.3542 0.166 9.57493C0.276667 9.6856 0.4212 9.74093 0.5664 9.74093C0.7116 9.74093 0.856733 9.6856 0.9668 9.57493L4.87013 5.67133L8.77347 9.57493C8.88347 9.6856 9.02867 9.74093 9.17387 9.74093C9.31907 9.74093 9.4636 9.6856 9.57427 9.57493C9.7956 9.3542 9.7956 8.99487 9.57427 8.77413L5.67093 4.87047Z" fill="white" />
//                                 </svg>
//                             </span>
//                         </div>
//                     </div>
//                     {
//                         true &&
//                         <>
//                             <div className={styles.blackbanner}>
//                                 <div className={styles.section}>
//                                     <div className={styles.top}>
//                                         <div className={styles.lefts}>
//                                             <div className={styles.label}>
//                                                 Amount due
//                                             </div>
//                                             <div className={styles.amount}>
//                                                 ₹1,080,835.33
//                                             </div>
//                                         </div>
//                                         <div className={styles.rights}>
//                                             <button>Pay now</button>
//                                             <span>View Invoice</span>
//                                         </div>
//                                     </div>
//                                     <div className={styles.overduechip}>
//                                         Due Feb 5, 2026 • Overdue by 3 days
//                                     </div>
//                                     <div className={styles.datetxt}>
//                                         Usage period: Jan 2 – Feb 2, 2026
//                                     </div>
//                                 </div>
//                                 <hr />
//                                 <div className={styles.section}>
//                                     <div className={styles.subhead}>
//                                         Included in this invoice
//                                     </div>
//                                     <div className={styles.txt}>
//                                         AI Contact Center — Enterprise Plan
//                                     </div>
//                                     <div className={styles.txt}>
//                                         Add-ons: Sessions • Minutes • Languages
//                                     </div>
//                                 </div>
//                             </div>
//                             <div className={styles.subscriptions}>
//                                 <div className={styles.header}>
//                                     <h2 className={styles.title}>Subscriptions</h2>
//                                     <p className={styles.subtitle}>About Usage & Limits</p>
//                                 </div>


//                                 <div className={styles.tabs}>
//                                     <div className={`${styles.tab} ${styles.active}`}>
//                                         AI Contact center
//                                     </div>
//                                     <div className={styles.tab}>Campaign management</div>
//                                     <div className={styles.tab}>CRM</div>
//                                 </div>

//                                 <div className={styles.planCard}>
//                                     <div className={styles.planHeader}>
//                                         <div>
//                                             <h3>AI Contact center</h3>
//                                             <p className={styles.planType}>Enterprise plan</p>
//                                         </div>

//                                         <div className={styles.planPrice}>
//                                             <h3>₹20,000 / month</h3>
//                                             <p>15 days left in this billing cycle/Overdue notification</p>
//                                         </div>
//                                     </div>

//                                     <div className={styles.planRow}>
//                                         <div>
//                                             <p className={styles.label}>Contract term</p>
//                                             <p className={styles.value}>Usage cycle on Jan 2 – Feb 2</p>
//                                         </div>

//                                         <div className={styles.rowRight}>
//                                             <span>Monthly</span>
//                                             <button className={styles.editBtn}>Edit</button>
//                                         </div>
//                                     </div>

//                                     <div className={styles.planRow}>
//                                         <div>
//                                             <p className={styles.label}>Billing frequency</p>
//                                             <p className={styles.value}>Next invoice on Feb 2, 2026</p>
//                                         </div>

//                                         <div className={styles.rowRight}>
//                                             <span>Monthly / Quarterly • 2nd of every month</span>
//                                             <button className={styles.editBtn}>Edit</button>
//                                         </div>
//                                     </div>
//                                 </div>
//                                 <div className={styles.planCard}>
//                                     <div className={styles.planHeader}>
//                                         <div>
//                                             <h3>Usage & limits</h3>
//                                             <p className={styles.planType}>About Usage & limits</p>
//                                         </div>
//                                     </div>

//                                     <div className={styles.planRow}>
//                                         <div>
//                                             <p className={styles.label}>Sessions</p>
//                                         </div>

//                                         <div className={styles.rowRight}>
//                                             <div className={styles.availablesession}><span>Available sessions</span> 1000 out of 5000</div>
//                                             <button className={styles.editBtn}>Add</button>
//                                         </div>
//                                     </div>

//                                     <div className={styles.planRow}>
//                                         <div>
//                                             <p className={styles.label}>Minutes</p>
//                                         </div>

//                                         <div className={styles.rowRight}>
//                                             <div className={styles.availableadd}>
//                                                 <span>Monthly / Quarterly • 2nd of every month</span>
//                                                 <span>International minutes available</span>
//                                             </div>
//                                             <button className={styles.editBtn}>Add</button>
//                                         </div>
//                                     </div>

//                                     <div className={styles.planRow}>
//                                         <div>
//                                             <p className={styles.label}>Sessions</p>
//                                         </div>

//                                         <div className={styles.rowRight}>
//                                             <div className={styles.availablesession}><span>Available sessions</span> <svg xmlns="http://www.w3.org/2000/svg" width="14" height="14" viewBox="0 0 14 14" fill="none">
//                                                 <path d="M6.66667 0C2.98667 0 0 2.98667 0 6.66667C0 10.3467 2.98667 13.3333 6.66667 13.3333C10.3467 13.3333 13.3333 10.3467 13.3333 6.66667C13.3333 2.98667 10.3467 0 6.66667 0ZM6.66667 3.94667C7.04667 3.94667 7.36 4.25333 7.36 4.64C7.36 5.02667 7.05333 5.33333 6.66667 5.33333C6.28 5.33333 5.97333 5.02667 5.97333 4.64C5.97333 4.25333 6.28 3.94667 6.66667 3.94667ZM7.8 8.84667C7.72 9.06 7.29333 9.36667 6.8 9.38667C6.30667 9.40667 5.69333 9.19333 5.76 8.62C5.82667 8.04667 6.01333 7.12667 6.04667 6.92C6.08 6.71333 6.19333 6.31333 6 6.26667C5.80667 6.22 5.54667 6.24 5.52 6.06667C5.49333 5.93333 5.62667 5.84 5.90667 5.82667C6.11333 5.81333 6.58667 5.80667 6.92667 5.88667C7.16667 5.94667 7.33333 6.16667 7.32 6.42C7.30667 6.61333 7.26 6.86 7.2 7.14C7.09333 7.62 7.02 8.12 6.98 8.36C6.94 8.6 7.02 8.99333 7.32667 8.84C7.63333 8.68667 7.90667 8.58667 7.81333 8.84L7.8 8.84667Z" fill="#23232B" />
//                                             </svg></div>
//                                             <button className={styles.editBtn}>Add</button>
//                                         </div>
//                                     </div>
//                                 </div>
//                             </div>
//                             {/* <Accountsuspended /> */}
//                         </>
//                     }
//                 </div>
//             </div>
//     </>
//   );
// }


import React, { useEffect, useMemo, useState } from "react";
import styles from "../../../../assets/styles/autoreneval.module.scss";
// import styles from "../../assets/styles/billingaddons.module.scss";
import moment from "moment";
import { useNavigate, useSearchParams } from "react-router-dom";
import { getCurrencyByDomain, stripHtmlTags } from "../../../../../base/utils";
import { useSelector } from "react-redux";
import { getBillingAndSubcriptionApi, getBotSessionsDomain_ccaas, getStorageDatas_ccaas } from "../AddOn/API/myplanapi";
import Skeleton from "../AddOn/components/skeleton";


// import aiChatAssistSocketservice from "../../../../services/aiChatService";
// import { MyplanProfileActions } from "../../../../store/slice/my_plans/profile";
// import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
// import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
// import { useDispatcher } from "../../../../store/redux-store/dispatch";
// import { commonDataSliceActions } from "../../../../store/slice/commonSilce/commonSlice";


// import Successicon from '../../../../../../assets/images/agentworkspace/successtick.svg'
import { v4 as uuidv4 } from "uuid";
import aiMyPlanSocketService from "../../../../../base/service/aiMypPanSocketService";
import { GetItemFromStorage } from "../../../../../base/customhooks/useStorage";
import { LocalStorage } from "../../../../../base/customhooks/localStorageKeys";
import { useDispatcher } from "../../../../../store/redux-store/dispatch";
import { jwtDecode } from "jwt-decode";

function Billingndsubscription() {

  const [sessionExpired, setSessionExpired] = useState(false)
  const [sessionExpiredData, setSessionExpiredData] = useState<any>({})
  const currency = getCurrencyByDomain()
  // const { loginUserDetails } = useSelector((state: any) => state.loginUser);
  const loginUserDetails=jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
  console.log("login",loginUserDetails)
  const [loading, setLoading] = useState(true)
  const [storage_ccaas, setStorage_ccaas] = useState<any>({});
  const navigate = useNavigate();

  const [searchParams, setSearchParams] = useSearchParams()
  const LoginCcaasUserDetails =jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));


 
  const chatsessionId = searchParams.get("sessionId");
  const dispatch = useDispatcher();

  const [currentProductName, setCurrentProductName] = useState('');
  const [ProductList, setProductList] = useState([]);
  const bilingProducts: any = useMemo(() => {
    return ProductList.filter((item: any) => item?.product_id === currentProductName)[0]
  }, [ProductList, currentProductName]);

  useEffect(() => {
    getBillingAndSubcription();
    getBotSessionsDomainApi()
    getStorageDatas_ccaasHandler()
  }, [])

  const getStorageDatas_ccaasHandler = async () => {
    try {
      const res = await getStorageDatas_ccaas();
      if (res) {
        setStorage_ccaas((prev: any) => ({
          ...prev,
          '1': res[0] ?? {}
        }));
      }
    } catch (error) {
      console.log("getStorageDatas_ccaas error:", error);
    }
  };

  const getBotSessionsDomainApi = async () => {
    try {

      getBotSessionsDomain_ccaas()
        .then((response: any) => {
          const [sessionData] = response;
          setSessionExpiredData(sessionData[0])
          const isSessionExpired = sessionData[0]?.remainingSessions == 0 && sessionData[0]?.isUnilimitedChat == 0;
          setSessionExpired(isSessionExpired)
        })

    } catch (error) {
      console.log("getBotSessionsDomainApi error:", error);
      setSessionExpiredData({});
      setSessionExpired(false);
    }
  }

  const [planNames, setPlanNames] = useState([{
    lable: 'AI Contact center',
    value: '1'
  },
  {
    lable: 'Campaign management',
    value: 'Campaign management'
  },
  {
    lable: 'CRM',
    value: '4'
  }]);

  console.log("[lllll]planNamesplanNames", planNames)
  console.log("[lllll]currentProductName", currentProductName)
  console.log("[lllll]ProductList", ProductList)


  const getBillingAndSubcription = async () => {
    try {
      const response: any = await getBillingAndSubcriptionApi();
      if (response?.statusCode === 200) {
        const products = Array.isArray(response?.result) ? (response?.result).flat() : [];

        const defaultData = {
          '1': 'AI Contact center',
          'Campaign management': "Campaign management",
          '4': 'CRM'
        }

        const productList = products.filter((item: any) => item?.product_id);
        const proname = productList.map((item: any) => ({
          label: defaultData[item?.product_id] || item?.prd_name,
          value: item?.product_id
        }))
        setPlanNames(proname);
        setCurrentProductName(proname[0].value || '');
        setProductList(productList)

      } else {
      }
    } catch (error) {
      console.log("getBillingAndSubcription error:", error);
    } finally {
      setLoading(false);
    }
  }


  const openChat = (value: any, files: any) => {
    const sessionId: any = uuidv4() + "setup";
    const body: any = {
      query: stripHtmlTags(value?.trim()),
      sessionId,
      roleId: `${LoginCcaasUserDetails?.roleId || ""}`,
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: `${LoginCcaasUserDetails?.userId || ""}`,
      domainId: `${LoginCcaasUserDetails?.domainId || ""}`,
      module: "",
      ext: `${LoginCcaasUserDetails?.ext || ""}`,
      agent: "myplans",
      isBot: 0,
      customerId: "",
      files: files ?? [],
    };
    aiMyPlanSocketService.io.emit("agent_chat_request", value)
    // aiChatAssistSocketservice.io.emit("crm_chat", body);
    // dispatch(MyplanProfileActions.setSetupAiAgentChat({ ...body, isBot: 0, query: body?.query }))
    // dispatch(MyplanProfileActions.setSetupAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
    // dispatch(commonDataSliceActions.setTabMenuPush({
    //   path: `/myplans/chat-conversation?sessionId=${sessionId}`,
    //   menuName: "New Conversation",
    //   close: 1,
    //   type: "myplans",
    // })
    // );
    // navigate(`/myplans/chat-conversation?sessionId=${sessionId}`);
  }

  const handleTab = (value: any) => {
    setCurrentProductName(value)
    const x = document.getElementById("header_crm");
    if (x) {
      x.scrollIntoView({ behavior: "smooth", block: "end" });
    }
  }

    const handleBillingProcess=(msg :any)=>{
    console.log("handlebill",msg)
    navigate('/home', { state: { data: "myplan", selectedMyPlanData: msg??"" } })
  }

  return (
    <>
    <div className={styles.maincontainer}>
   <div className={styles.billingndsubscriptionbody}>
        {loading ? (
          <Skeleton count={5} />
        ) : (
          <>
         
  <div className={styles.headerole}>
                    <h3> {loginUserDetails?.companyName || ""}</h3>
                    <p>View all the activities done across Worktual platform</p>
                </div>
            {sessionExpired && (
              <div className={styles.redtoast}>
                <div className={styles.inner}>
                  Bot session limit will reach soon. Add sessions now to avoid interruption.
                  <span onClick={() => setSessionExpired(false)} style={{cursor:"pointer"}}>
                    ✕
                  </span>
                </div>
              </div>
            )}

            <div className={styles.blackbanner}>
              <div className={styles.section}>
                <div className={styles.top}>
                  <div className={styles.lefts}>
                    {bilingProducts?.isPaid === 0 ? (
                      <div className={styles.label}>Amount due</div>
                    ) : (
                      <div className={styles.paid}>
                        {/* <img src={Successicon} alt="" /> Paid */}
                      </div>
                    )}

                    <div className={styles.amount}>
                      {currency}{bilingProducts?.Amount_Due ? bilingProducts?.Amount_Due : 0}
                    </div>

                    {!!(bilingProducts?.originalPrice && bilingProducts?.plan_discount) && (
                     <>
                        <div className={styles.cross}>
                          {currency}{bilingProducts?.originalPrice}
                        </div>
                        <div>
                          Discount {Math.round(bilingProducts?.plan_discount) || 0}% •{" "}
                          {(bilingProducts?.originalPrice || 0) - (bilingProducts?.Amount_Due || 0)} off
                        </div>
                    </>
                    )}
                  </div>

                  <div className={styles.rights}>
                    {bilingProducts?.isPaid === 0 && (
                      <button style={{cursor:"pointer"}} onClick={() => handleBillingProcess("My Plan Pay now")} >
                        Pay now
                      </button>
                    )}
                    <span onClick={() => handleBillingProcess("My Plan View Invoice")} style={{cursor:"pointer"}}>
                      View Invoice
                    </span>
                  </div>
                </div>

                <div className={styles.datetxt}>
                  {bilingProducts?.DueDate ? "Due " + moment(bilingProducts?.DueDate).format("MMM DD YYYY") : ""}
                  {bilingProducts?.days_remaining ? ` • ${Math.abs(Number(bilingProducts?.days_remaining))} days remaining` : ""}
                </div>

                <div className={styles.datetxt}>
                  {bilingProducts?.plan_start_date
                    ? `Usage period ${moment(bilingProducts?.plan_start_date).format("MMM D")} - ${moment(bilingProducts?.plan_end_date).format("MMM D YYYY")}`
                    : ""}
                </div>
              </div>
            </div>

            <div className={styles.subscriptions}>
              <div className={styles.header}>
                <h2 className={styles.title}>Subscriptions</h2>
                <p className={styles.subtitle}>About Usage & Limits</p>
              </div>

              <div className={styles.tabs}>
                {planNames.map((item: any, index: number) => (
                  <div className={`${styles.tab} ${currentProductName === item.value ? styles.active : ""}`} onClick={() => handleTab(item.value)} key={index}> {item.label}</div>
                ))}
              </div>

              {/* PLAN CARD */}
              <div className={styles.planCard}>
                <div className={styles.planHeader}>
                  <div className={styles.planPricecard}>
                    <h3>{bilingProducts?.prd_name}</h3>
                    <p className={styles.planType}>
                      {loginUserDetails?.parent_id == "33" ? "Pro plan" : "Enterprise plan"}
                    </p>
                  </div>

                  <div className={styles.planPrice}>
                    <h3>
                      {currency}
                      {bilingProducts?.purchase_amount || bilingProducts?.Amount_Due || 0} / month
                    </h3>

                    {!!(bilingProducts?.originalPrice && bilingProducts?.plan_discount) ? (
                      <div className={styles.orginalamount}>
                        {/* <div className={styles.cross}>
                          {currency}{bilingProducts?.originalPrice}
                        </div> */}
                        <div className={styles.plangreytxt}>
                           <span style={{ textDecoration: "line-through" }}>
  {`${currency}${bilingProducts?.originalPrice ?? ""}`}
</span>&nbsp;{currency}{(bilingProducts?.originalPrice || 0) - (bilingProducts?.Amount_Due || 0)} • Discount {Math.round(bilingProducts?.plan_discount) || 0}%
                        </div>
                      </div>
                    ) : (
                      <p>{bilingProducts?.days_remaining} days left in this billing cycle/Overdue notification</p>
                    )}
                  </div>
                </div>

                <div className={styles.planRow}>
                  <div className={styles.rowleft}>
                    <p className={styles.label}>Contract term</p>
                    <p className={styles.value}>
                      Usage cycle on {bilingProducts?.contract_period ? bilingProducts?.contract_period : ""}
                    </p>
                  </div>
                  <div className={styles.rowRight}>
                    <span>{bilingProducts?.billing_frequency}</span>
                    {/* <button className={styles.editBtn} onClick={() => openChat("My Plan Edit Contract term", [])}> */}
                    <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Edit Contract term")}>
                      Edit
                    </button>
                  </div>
                </div>

                <div className={styles.planRow}>
                  <div className={styles.rowleft}>
                    <p className={styles.label}>Billing frequency</p>
                    <p className={styles.value}>
                      Next invoice on{" "}
                      {bilingProducts?.DueDate ? moment(bilingProducts?.DueDate).format("MMM DD YYYY") : ""}
                    </p>
                  </div>
                  <div className={styles.rowRight}>
                    <span>{bilingProducts?.billing_frequency ?? 0}</span>
                    {/* <button className={styles.editBtn} onClick={() => openChat("My Plan Edit Billing frequency", [])}> */}
                    <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Edit Billing frequency")}>
                      Edit
                    </button>
                  </div>
                </div>
              </div>

              {/* USAGE CARD */}
              <div className={styles.planCard}>
                <div className={styles.planHeader}>
                  <h3>Usage & limits</h3>
                </div>

                {bilingProducts?.product_id == "1" && (
                  <>
                    <div className={styles.planRow}>
                      <p className={styles.label}>Sessions</p>
                      <div className={styles.rowRight}>
                        {sessionExpiredData?.isUnilimitedChat === 1 ? (
                          <span>Unlimited</span>
                        ) : (
                          <span>
                            Available sessions {sessionExpiredData?.remainingSessions || 0} out of {sessionExpiredData?.TotalSession || 0}
                          </span>
                        )}
                        {/* <button className={styles.editBtn} onClick={() => openChat("My Plan Add Sessions", [])}>Add</button> */}
                        <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Add Sessions")}>Add</button>
                      </div>
                    </div>

                    <div className={styles.planRow}>
                      <p className={styles.label}>Concurrent voices</p>
                      <div className={styles.rowRight}>
                        <span>{bilingProducts?.concurrent_voices || 0}</span>
                        <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Add Concurrent voices")}>Add</button>
                      </div>
                    </div>

                    <div className={styles.planRow}>
                      <p className={styles.label}>Languages</p>
                      <div className={styles.rowRight}>
                        <span>{bilingProducts?.usedlanguages || 0} / {bilingProducts?.totlanguage || 0} languages</span>
                        <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Add Languages")}>Add</button>
                      </div>
                    </div>
                  </>
                )}

                <div className={styles.planRow}>
                  <p className={styles.label}>Channels</p>
                  <div className={styles.rowRight}>
                    <span>{bilingProducts?.usedChannel || 0} / {bilingProducts?.totChannel || 0} channels</span>
                    <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Add Channels")}>Add</button>
                  </div>
                </div>

                <div className={styles.planRow}>
                  <p className={styles.label}>Users</p>
                  <div className={styles.rowRight}>
                    <span>Available user {bilingProducts?.availableUser || 0} out of {bilingProducts?.TotalUser || 0}</span>
                    <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Add Users")}>Add</button>
                  </div>
                </div>

                <div className={styles.planRow}>
                  <p className={styles.label}>Storage</p>
                  <div className={styles.rowRight}>

                    <span>
                      Available Storage {storage_ccaas?.[bilingProducts?.product_id]?.total_size_gb_used ?? 0} GB out of {storage_ccaas?.[bilingProducts?.product_id]?.total_size_gb ?? 0} GB
                    </span>

                    <button className={styles.editBtn} onClick={() => handleBillingProcess("My Plan Add Storage")}>Add</button>
                  </div>
                </div>
              </div>

            </div>
          </>
        )}
      </div>
    </div>
   
    </>
  );
}

export default Billingndsubscription;
