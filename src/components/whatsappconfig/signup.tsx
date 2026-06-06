// import { useEffect } from "react";
// import styles from "./scss/config.module.scss";
// import { assets } from "../../styles/images/assets";
// import AddAccountBtn from "./addAccountBtn";

// interface SignupProps {
//   onLaunchSignup: () => void;
//   onReceiveEmbeddedSignupData: (data: { waba_id: string; business_id: string, phone_number_id: string }) => void;
// }

// const Signup: React.FC<SignupProps> = ({ onLaunchSignup, onReceiveEmbeddedSignupData }) => {
//   useEffect(() => {
//     // Load the Facebook SDK
//     const loadFbSdk = () => {
//       (window as any).fbAsyncInit = function () {
//         (window as any).FB.init({
//           appId: "2792648964217350",
//           autoLogAppEvents: true,
//           xfbml: true,
//           version: "v22.0",
//         });
//       };

//       const script = document.createElement("script");
//       script.src = "https://connect.facebook.net/en_US/sdk.js";
//       script.async = true;
//       script.defer = true;
//       script.crossOrigin = "anonymous";
//       document.body.appendChild(script);
//     };

//     loadFbSdk();

//     // Handle messages from Facebook
//     const handleMessage = (event: MessageEvent) => {
//       if (
//         event.origin !== "https://www.facebook.com" &&
//         event.origin !== "https://web.facebook.com"
//       ) {
//         return;
//       }

//       try {
//         const data = JSON.parse(event.data);
//         if (data.type === "WA_EMBEDDED_SIGNUP") {
//           if (data.event === "FINISH_WHATSAPP_BUSINESS_APP_ONBOARDING") {
//             console.log(
//               "Phone number ID:",
//               data.data.phone_number_id,
//               "WhatsApp business account ID:",
//               data.data.waba_id,
//               "Business ID:",
//               data.data.business_id
//             );
//             const waba_id = data.data.waba_id;
//             const business_id = data.data.business_id;
//             const phone_number_id = data.data.phone_number_id;
//             onReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
//           } else if (data.event === "FINISH") {
//             console.log(
//               "Phone number ID:",
//               data.data.phone_number_id,
//               "WhatsApp business account ID:",
//               data.data.waba_id,
//               "Business ID:",
//               data.data.business_id
//             );
//             const waba_id = data.data.waba_id;
//             const business_id = data.data.business_id;
//             const phone_number_id = data.data.phone_number_id;
//             onReceiveEmbeddedSignupData({ waba_id, business_id, phone_number_id });
//           } else if (data.event === "CANCEL") {
//             console.warn("Cancel at:", data.data.current_step);
//           } else if (data.event === "ERROR") {
//             console.error("Error:", data.data.error_message);
//           }
//         }
//       } catch {
//         console.log("Non JSON Responses", event.data);
//       }
//     };

//     window.addEventListener("message", handleMessage);

//     return () => {
//       window.removeEventListener("message", handleMessage);
//     };
//   }, []);
//   return (

//     <div className={`${styles.pannelinner}`}>
//       <img src={assets.whtsappconfig_img} alt="whtsappconfig" />
//       <div className={styles.innertxtbox}>
//         <h4>Configure WhatsApp Channel</h4>
//         <p>You need to connect with your WhatsApp business account to proceed with this integration. </p>
//         <AddAccountBtn btnClass="primarybtn" onClickSignup={onLaunchSignup} btnLabel="Login with facebook" />
//         {/* <button className="primarybtn" onClick={launchWhatsAppSignup}>Login with facebook</button> */}
//       </div>
//     </div>

//   );
// };

// export default Signup;
