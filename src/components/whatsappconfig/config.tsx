// import React, { useState, useEffect, useRef } from "react";
// import styles from "./scss/config.module.scss";
// import { assets } from "../../styles/images/assets";
// import Signup from "./signup";
// import SignupStatus from "./signupStatus";
// import UserTable from "./userTable";
// import { config } from "../../base/constants";
// import { useSearchParams, useNavigate } from "react-router-dom";
// import UserForm from "./userForm";
// import { whatsappService } from "../../base/services/apiservices";
// import Loaders from "../../loader/loader";
// import ContentLayout from "../../layout/contentLayout";
// import { Link } from "react-router-dom";
// import { GetItemFromStorage } from "../../base/customhooks/useStorage";
// import { LocalStorage } from '../../base/customhooks/localStorageKeys';
// import { jwtDecode } from 'jwt-decode';
// import { contactDataSliceActions } from "../../store/slice/commonSlice/contactSlice";
// import { useDispatch } from "react-redux";
// interface AccountData {
//   businessName: string;
//   createdDate: string;
//   domainId: number;
//   id: number;
//   isactive: number;
//   messageLimit: number;
//   mobileNumber: string;
//   status: string;
// }

// const WhatsappConfig: React.FC = () => {
//   const [searchParams] = useSearchParams();
//   const [signUpStatus, setSignUpStatus] = useState<string | null>(null);
//   const [accountDatas, setAccountDatas] = useState<AccountData[]>([]);
//   const [selectedAccount, setSelectedAccount] = useState<AccountData | null>(null);
//   const [loader, setLoader] = useState<boolean>(false);
//   const wabaIdRef = useRef<string | null>(null);
//   const [wabaId, setWabaId] = useState<string | null>(null);
//   const businessIdRef = useRef<string | null>(null);
//   const [businessId, setBusinessId] = useState<string | null>(null);
//   const phoneNumberIdRef = useRef<string | null>(null);
//   const [phoneNumberId, setPhoneNumberId] = useState<string | null>(null);

//   const dispatch = useDispatch();
//   const handleReceiveEmbeddedSignupData = ({ waba_id, business_id, phone_number_id }: { waba_id: string, business_id: string, phone_number_id: string }) => {
//     setWabaId(waba_id);
//     wabaIdRef.current = waba_id;

//     setBusinessId(business_id);
//     businessIdRef.current = business_id;

//     setPhoneNumberId(phone_number_id);
//     phoneNumberIdRef.current = phone_number_id;
//   };

//   console.log("signUpStatus:::", signUpStatus);
//   console.log("accountDatas:::", accountDatas);
//   const navigate = useNavigate();

//   useEffect(() => {
//     setSignUpStatus(searchParams.get("status"));
//   }, [searchParams]);

//   useEffect(() => {
//     try {
//       setLoader(true);
//       whatsappService.get(`/get_business_account/null`)
//         .then((response :any) => {
//           if (response.data.statusCode === 200 && response.data.getAccountsNumbers?.length > 0) {
//             setAccountDatas(response.data.getAccountsNumbers);
//             setLoader(false);
//           } else {
//             setAccountDatas([]);
//             setLoader(false);
//           }
//         })
//         .catch((error :any) => {
//           console.log(error)
//           setAccountDatas([])
//         })
//     } catch (error) {
//       console.log(error)
//     }
//   }, []);

//   const accessToken = GetItemFromStorage(LocalStorage.ACCESS_TOKEN);
//   const loginUserDetails: any = accessToken ? jwtDecode(accessToken) : null;
//   // dispatch(contactDataSliceActions.setLoginDetails(loginUserDetails));
//   console.log("loginUserDetails:::", loginUserDetails);

//   const launchWhatsAppSignup = () => {
//     (window as any).FB.login(
//       (response: any) => {
//         if (response.authResponse) {
//           const code = response.authResponse.code;
//           window.location.href = `${config().BUSINESS_WHATSAPP_CONF_URL}?code=${code}&domainId=${loginUserDetails?.domainId}` +
//             `${wabaIdRef.current ? `&wabaId=${wabaIdRef.current}` : ''}` +
//             `${businessIdRef.current ? `&businessId=${businessIdRef.current}` : ''}` +
//             `${phoneNumberIdRef.current ? `&phoneNumberId=${phoneNumberIdRef.current}` : ''}`;
//         }
//       },
//       {
//         config_id: "1109736967317755",
//         response_type: "code",
//         override_default_response_type: true,
//         extras: {
//           setup: {},
//           featureType: "whatsapp_business_app_onboarding",
//           sessionInfoVersion: "3",
//         },
//       }
//     );
//   };


//   return (
//     <ContentLayout title={
//       <div className="titleArea">
//         <Link to='/settings'><img src={assets.navarrow_icon} alt="backarrow" /></Link>        <div className='titleright'>
//           <h4>WhatsApp</h4>
//           <p className="witharrow">Set up integration and settings for communicating with customers through WhatsApp, including message templates and automated responses.</p>
//         </div>
//       </div>
//     }>
//       <div className={styles.whatsappmain}>
//         {signUpStatus ? (
//           <SignupStatus status={signUpStatus} onGoBack={() => navigate("/whatsappconfig")} />
//         ) : selectedAccount ? (
//           <UserForm account={selectedAccount} onGoBack={() => setSelectedAccount(null)} />
//         )
//           : loader ? (<Loaders />)
//             : accountDatas?.length > 0 ? (
//               <UserTable accDatas={accountDatas} onLaunchSignup={launchWhatsAppSignup} onSelectAccount={setSelectedAccount} />
//             ) : (
//               <Signup onLaunchSignup={launchWhatsAppSignup} onReceiveEmbeddedSignupData={handleReceiveEmbeddedSignupData} />
//             )}
//       </div>

//     </ContentLayout>
//   );
// }

// export default WhatsappConfig;
