import React, { useEffect, useRef, useState } from "react";
import styles from "./loginnew.module.scss";
import googleicn from "../../styles/images/icons/googleicon.svg";
import msicn from "../../styles/images/icons/appsicon.svg";
import appleicn from "../../styles/images/icons/appleicon.svg";
import { useNavigate } from "react-router-dom";
import { LoginAllServices, LoginService } from "../../base/service/loginService";
import { LocalStorage } from "../../base/customhooks/localStorageKeys";
import { SetItemInStorage, setTokenIndexDB } from "../../base/customhooks/useStorage";
import Cookies from "../../utils/cookies";
import { v4 as uuidv4 } from "uuid";
import logoImage from "../../styles/images/login_images/headerlogo.svg";
import emaillogin from "../../styles/images/icons/emaillogin.svg";
import locklogin from "../../styles/images/icons/locklogin.svg";
import eyeopen from "../../styles/images/icons/eyeopen.svg";
import eyeclosed from "../../styles/images/icons/eyeclosed.svg";
import { initializeApp, getApps } from "firebase/app";
import { getMessaging, getToken } from "firebase/messaging";
import { whatsappService, emailOtpVerification, emailotpverification, resendOtp, updatePassword } from "../../base/service/apiservice";
import { AICoreServices } from "../../base/service/apiservice";
import Lottie from 'lottie-react';
import worktualLoader from "../../loader/worktual-loader.json"
import { useDispatcher } from "../../store/redux-store/dispatch";
import SocketService from "../../base/service/socketService";
import { config } from "../../base/constant";
import aiMyPlanSocketService from "../../base/service/aiMypPanSocketService";
import Spinner from "../custom_components/spinner/spinner";
import { getClientData, updateDeviceDetails } from "../../base/utils";
import { commonDataSliceActions } from "../../store/slice/commonSlice/commonSlice";
import { getDeviceDetails } from "../../utils/device";
import type { DeviceDetails } from "../../utils/device";
import {
  LoginSocialApple,
  LoginSocialGoogle,
  LoginSocialMicrosoft,
} from "reactjs-social-login";
import axios from "axios";

type LoginJwtPayload = {
  companyId?: string;
  Email?: string;
  email?: string;
  username?: string;
  deviceId?: string;
  ipaddress?: string;
};
type DeviceInfoPayload = {
  currentDevice?: DeviceDetails;
};


const initialErrors = {
  email: false,
  password: false,
  wholeError: false,
};

const Login = () => {
  const dispatch = useDispatcher();
  const navigate = useNavigate();
  const [error, setErrors] = useState(initialErrors);
  const [errorMsg, setErrorMsg] = useState('');
  const [showPassword, setShowPassword] = useState(false);
  const [loading, setLoading] = useState(false);
  const [countryCode,setCountryCode] = useState('in');
  const [isForgetPassword, setIsForgetPassword] = useState("login");
  
  // Forgot Password States
  const [forgotEmail, setForgotEmail] = useState("");
  const [forgotError, setForgotError] = useState("");
  const [forgotLoading, setForgotLoading] = useState(false);
  const [otp, setOtp] = useState("");
  const [otpError, setOtpError] = useState("");
  const [otpLoading, setOtpLoading] = useState(false);
  const [otpMessage, setOtpMessage] = useState("");
  const [otpResendTimer, setOtpResendTimer] = useState(0);
  const [otpAttemptCount, setOtpAttemptCount] = useState(1);
  const [newPassword, setNewPassword] = useState("");
  const [confirmPassword, setConfirmPassword] = useState("");
  const [showConfirmPassword, setShowConfirmPassword] = useState(false);
  const [passwordLoading, setPasswordLoading] = useState(false);
  const [passwordError, setPasswordError] = useState("");

  const [userDetails, setUserDetails] = useState({
    emailId: '',
    password: '',
  });

  const handleUserDetails = (e: React.ChangeEvent<HTMLInputElement>) => {
    const { name, value } = e.target;
    setUserDetails((prev) => ({ ...prev, [name]: value }));
    setErrors((prev) => ({ ...prev, [name]: false, wholeError: false }));
  };

  const togglePasswordVisibility = () => {
    setShowPassword((prev) => !prev);
  };

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

  const firebaseConfig = {
    apiKey: "AIzaSyCsLt2QXdQOMMn4VUNSWeLdPqU3zgUuK58",
    authDomain: "campaignwebpush.firebaseapp.com",
    projectId: "campaignwebpush",
    storageBucket: "campaignwebpush.appspot.com",
    messagingSenderId: "610883259513",
    appId: "1:610883259513:web:0154759264bfa46275d8a8",
    measurementId: "G-PBRTEGFZ8W"
  };
  const app = !getApps()?.length ? initializeApp(firebaseConfig) : getApps()[0];
  const messaging = getMessaging(app);

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
        setTokenIndexDB("tokenId", token);
        setTokenIndexDB("baseurl", config().BASE_URL);
        return token;
      } else {
        console.warn("⚠️ No registration token available. Check service worker setup.");
      }
    } catch (error) {
      console.error("❌ Error getting token:", error);
    }
  };

  const insertFcmTokenKey = async (token: string) => {
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

    const getKnowledgeBaseData = async () => {
      try {
        try {
          await whatsappService.get(`/get_knowledge_list`)
            .then((response) => {
              if (response?.data?.statusCode === 200 && response?.data?.getKnowledgeResponse?.length > 0) {
                 navigate("/home");
                  dispatch(commonDataSliceActions.setCompanyOnboarded(true));
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
    
  const saveDeviceInfo = async (datanew: DeviceInfoPayload, authToken?: string) => {
    try {
      const data = datanew?.currentDevice;
      const tokenData = Cookies.get();
      const authorization = authToken || tokenData?.ssoToken || "";
      if (!authorization) return null;

      const loginUserDetails = jwtDecode<LoginJwtPayload>(authorization);
      const body = {
        companyId: loginUserDetails?.companyId,
        userEmail: loginUserDetails?.Email || loginUserDetails?.email,
        device: data?.deviceType,
        deviceModal: data?.deviceModel,
        osType: data?.osName,
        browserType: data?.browserName,
        ipAddress: data?.ipAddress,
        location: data?.location?.ipBased?.city,
        isActive: 1,
        productType: 'CAMPAIGN',
        deviceId: data?.deviceSignature
      };

      const res = await AICoreServices.post(body, '/insert_user_devices_dtl').catch((e) => console.log(e));
      return res;
    } catch {
      return null;
    }
  };

  const loginAll = async (payload: { username: string; deviceId: string; ipAddress?: string }) => {
    try {
      const clientData: any = await getClientData();
      const body = {
        username: payload?.username,
        password: "",
        deviceId: payload?.deviceId,
        source: "CAMPAIGN",
        ipAddress: clientData?.ip || payload?.ipAddress || "",
        deviceType: clientData?.deviceName || clientData?.device || "Web",
        browser: clientData?.browser || "",
        loginType: "",
        productId: 1,
      };
      await LoginAllServices.loginall(body);
    } catch (error) {
      console.log("err", error);
    }
  };
  const handleSubmitLogin = async () => {
    try {
      console.log("called")
      const { emailId, password } = userDetails;
      await handleSocailSubmitLogin(emailId,password);
    } catch (error) {
      console.log("err",error);
    }
  };

  const loginCheck=async(email :any)=>{
    try {
      const body ={
        email: email,
        // sessionId: `SessionId${uuidv4()}`
      }
      SetItemInStorage(LocalStorage.LOGIN_SESSIONID,body.sessionId);
    const response = await axios.post(
      `${import.meta.env.VITE_APP_ENV === "development"?"https://qacampaignapi.worktual.co.uk/campaignService/v1/loginCheck":"https://campaignapi.worktual.co.uk/campaignService/v1/loginCheck"}`,
      {
        ...body
      }
    );
    return response.data.loginCheckRes?.[0];
    console.log("Success:", response.data);
  } catch (error: any) {
    console.error("Error:", error.response?.data || error.message);
  }
  }

    const handleSocailSubmitLogin = async (emailId :any,password :any) => {
    const trimmedEmail = emailId.trim();
    const trimmedPassword = password.trim();
    const regex = /^[^\s@]+@[^\s@]+\.[^\s@]+$/;
    const emailError = !trimmedEmail ;
    const passwordError = !trimmedPassword;
    console.log("called212",emailError,passwordError,emailId, password)
    const domain :any= window.location.host.split(".");
    const countryCode = domain[domain.length-1];

    const activeCheck =await loginCheck(trimmedEmail);
    // if(true){
    if((activeCheck?.errmsg=="Data Updated Successfully" || activeCheck?.errmsg=="Data Inserted Successfully")){

      setErrors({
        email: emailError,
        password: passwordError,
        wholeError: false,
      });
  
      if (emailError || passwordError) return;
      const uuidNew = uuidv4();
      const body = {
        username: trimmedEmail,
        password: trimmedPassword,
        // source: 'CAMPAIGN',
        source: 'Web',
        ipAddress: '136.144.56.255:443',
        isForceLogin: true,
        deviceId: uuidNew,
        location:countryCode=='in'?'India':countryCode=='uk'?'United Kingdom':'United States',
        // location:'United Kingdom'
        // location:'India'
      };
  
      try {
        setLoading(true);
        const value = await LoginService.login(body);
        console.log("err--->",value)
        if (value?.data?.statusCode === 200) {
          console.log("err--->",value)
          SetItemInStorage(LocalStorage.ACCESS_TOKEN, value?.data?.token);
          const tokenData = Cookies.get();
           const deviceInfo = await getDeviceDetails();
           console.log("deviceInfo>>>>>>>", deviceInfo);
          await saveDeviceInfo(deviceInfo, value?.data?.token);
          await loginAll({
            username: trimmedEmail,
            deviceId: uuidNew,
            ipAddress: body?.ipAddress
          });
          if (!tokenData['ssoToken']) {
            Cookies.set("ssoToken", value.data.token, 365);
          }
          await webpushNotification();
         
          localStorage.setItem('leftnav_active', 'menu1');
          getKnowledgeBaseData();
        
          new SocketService(value?.data?.token, dispatch);
          new aiMyPlanSocketService(value?.data?.token,navigate, dispatch);
          updateDeviceDetails(body?.username);
        } else {
          console.log("err--->",value)
          setErrorMsg(value?.data?.message)
          setErrors((prev) => ({ ...prev, wholeError: true }));
        }
      } catch {
        setErrors((prev) => ({ ...prev, wholeError: true }));
      } finally {
        setLoading(false);
        //  setUserDetails({ emailId: "", password: "" });
        //  debugger;
      }
    }else{
      setErrors({
        email: false,
        password: false,
        wholeError: true,
      });
      setErrorMsg('Credential is active in another device');
    }
  };

   const isSocialMedia = useRef(false);
   const socialMediaLogin = async (data: any, type: any) => {
    const _body = {
      email: data,
      device_type: "", // You can enhance this with UA-parser
      browser: "",     // Or use navigator.userAgent
      login_type: type
    };

    try {
      const response = await whatsappService.socialMediaLogin(_body);
      console.log("response",response);
      // const response = await LoginService.socialMediaLogin(_body);
      if (response?.data?.results[0].v_errcode == 0) {
        console.log("here comes")
        let email = response?.data?.results[0]?.my_acc_email;
        let password = response?.data?.results[0]?.my_acc_password;
        console.log("here comes 3");
        isSocialMedia.current = true;
         
        // const updatedLogin = {
        //   ...login,
        //   email,
        //   password,
        //   errors: false,
        // };
        console.log("here comes 4");
        // setLogin(updatedLogin);
        setUserDetails({
          emailId: email,
          password: password
        });
         await handleSocailSubmitLogin(email,password);
        // setErrors({ ...error, email: false, password: false, wholeError: false });
      } else if (response?.data?.results[0]?.v_errcode == -1) {
        // setLoader(false);
        alert(response?.data?.results[0]?.v_errmsg);
      }
    } catch (err) {
      console.error("SocialMediaLogin error", err);
      return null;
    }
  };

  // Forgot Password Functions
  const OTP_RESEND_COOLDOWN_SECONDS = 10 * 60;
  const OTP_MAX_ATTEMPTS = 5;

  const handleForgotSubmit = async () => {
    const normalizedEmail = forgotEmail.trim();
    if (!normalizedEmail) {
      setForgotError("Email is required");
      return;
    }

    setForgotLoading(true);
    setForgotError("");
    try {
      const response: any = await emailotpverification({
        email: normalizedEmail,
        emailId: normalizedEmail,
      });

      if (response?.statusCode === 200) {
        setOtp("");
        setOtpAttemptCount(1);
        setOtpResendTimer(OTP_RESEND_COOLDOWN_SECONDS);
        setIsForgetPassword("otp");
      } else {
        setForgotError(response?.message || "Invalid email address");
      }
    } catch (e: any) {
      setForgotError(
        e?.response?.data?.message || "Something went wrong. Please try again.",
      );
    } finally {
      setForgotLoading(false);
    }
  };

  const otpInputsRef = useRef<Array<HTMLInputElement | null>>([]);

  const handleOtpInputChange = (index: number, value: string) => {
    const sanitized = value.replace(/\D/g, "");
    const otpDigits = otp.split("");
    otpDigits[index] = sanitized.slice(-1);
    const nextOtp = otpDigits.join("").slice(0, 6);
    setOtp(nextOtp);
    setOtpError("");
    setOtpMessage("");

    if (sanitized && index < 5) {
      otpInputsRef.current[index + 1]?.focus();
    }
  };

  const handleOtpKeyDown = (event: React.KeyboardEvent<HTMLInputElement>, index: number) => {
    if (event.key === "Backspace" && !otp[index] && index > 0) {
      otpInputsRef.current[index - 1]?.focus();
    }
  };

  const handleOtpPaste = (event: React.ClipboardEvent<HTMLInputElement>) => {
    const pasteData = event.clipboardData.getData("text").replace(/\D/g, "").slice(0, 6);
    if (!pasteData) return;
    event.preventDefault();
    setOtp(pasteData);
    setOtpError("");
    setOtpMessage("");
    const firstEmpty = pasteData.length >= 6 ? 5 : pasteData.length;
    otpInputsRef.current[firstEmpty]?.focus();
  };

  const formatTimer = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${String(mins).padStart(2, "0")}:${String(secs).padStart(2, "0")}`;
  };

  const onVerifyOtp = async () => {
    if (otp.length !== 6) {
      setOtpError("Please enter a valid 6-digit OTP.");
      return;
    }

    setOtpLoading(true);
    setOtpError("");
    try {
      const response: any = await emailOtpVerification({
        email: forgotEmail.trim(),
        verify_code: Number(otp),
      });

      if (response?.statusCode === 200) {
        setIsForgetPassword("update");
      } else {
        setOtpError(response?.message || "OTP verification failed.");
      }
    } catch (e: any) {
      setOtpError(e?.response?.data?.message || "OTP verification failed.");
    } finally {
      setOtpLoading(false);
    }
  };

  const onResendOtp = async () => {
    if (otpResendTimer > 0) {
      setOtpError(
        `Please wait ${formatTimer(otpResendTimer)} before requesting a new OTP.`,
      );
      return;
    }

    if (otpAttemptCount >= OTP_MAX_ATTEMPTS) {
      setOtpError("Maximum OTP resend attempts reached.");
      return;
    }

    const normalizedEmail = forgotEmail.trim();
    if (!normalizedEmail) {
      setOtpError("Email is missing.");
      return;
    }
    setOtpLoading(true);
    setOtpError("");
    try {
      const response: any = await resendOtp({
        email: normalizedEmail,
        emailId: normalizedEmail,
      });
      if (response?.statusCode === 200) {
        setOtpMessage(response?.message || "OTP sent successfully.");
        setOtpAttemptCount((prev) => prev + 1);
        setOtpResendTimer(OTP_RESEND_COOLDOWN_SECONDS);
      } else {
        setOtpError(response?.message || "Unable to resend OTP.");
      }
    } catch (e: any) {
      setOtpError(e?.response?.data?.message || "Unable to resend OTP.");
    } finally {
      setOtpLoading(false);
    }
  };

  const onUpdatePassword = async () => {
    const pass = newPassword.trim();
    const confirm = confirmPassword.trim();
    const lowerPass = pass.toLowerCase();
    const email = forgotEmail.trim().toLowerCase();
    const emailName = email.split("@")[0] || "";
    const hasNoPersonalInfo =
      !lowerPass.includes(email) &&
      (!emailName || !lowerPass.includes(emailName));
    const hasMinLength = pass.length >= 8;
    const hasNumberOrSymbol = /[\d\W_]/.test(pass);

    if (!pass) {
      setPasswordError("New password is required.");
      return;
    }
    if (!hasNoPersonalInfo) {
      setPasswordError("Password cannot contain your name or e-mail.");
      return;
    }
    if (!hasMinLength) {
      setPasswordError("Password must be at least 8 characters.");
      return;
    }
    if (!hasNumberOrSymbol) {
      setPasswordError("Password must contain a number or symbol.");
      return;
    }
    if (pass !== confirm) {
      setPasswordError("Passwords do not match.");
      return;
    }

    setPasswordLoading(true);
    setPasswordError("");
    try {
      const response: any = await updatePassword({
        email: forgotEmail.trim(),
        new_pwd: pass,
      });
      if (response?.statusCode === 200) {
        backToLogin();
        navigate("/login");
      } else {
        setPasswordError(response?.message || "Password update failed.");
      }
    } catch (e: any) {
      setPasswordError(e?.response?.data?.message || "Password update failed.");
    } finally {
      setPasswordLoading(false);
    }
  };

  const backToLogin = () => {
    setOtp("");
    setNewPassword("");
    setConfirmPassword("");
    setForgotError("");
    setOtpError("");
    setOtpMessage("");
    setOtpResendTimer(0);
    setOtpAttemptCount(1);
    setPasswordError("");
    setIsForgetPassword("login");
  };

  const openForgotFlow = () => {
    setForgotEmail(userDetails.emailId || "");
    setForgotError("");
    setOtpError("");
    setOtpMessage("");
    setPasswordError("");
    setIsForgetPassword("forget");
  };

  // useEffect(() => {
  //   const loaders = document.getElementById('contact_center_mainloader');
  //   if (loaders) {
  //     loaders.style.display = 'none';
  //   }
  // }, [])

  useEffect(() => {
  const timer = setTimeout(() => {
    const loaders = document.getElementById('contact_center_mainloader');
    if (loaders) {
      loaders.style.display = 'none';
    }
  }, 1000); // 1 second

  // cleanup → remove timer
  return () => clearTimeout(timer);
}, []);


  useEffect(()=>{
    const autoLogin=async()=>{
      const tokenData = Cookies.get();
      if (tokenData['ssoToken']) {
      SetItemInStorage(LocalStorage.ACCESS_TOKEN, tokenData['ssoToken']);
      await webpushNotification();
      setUserDetails({ emailId: "", password: "" });
      localStorage.setItem('leftnav_active', 'menu1');
      getKnowledgeBaseData();
      // navigate("/home");
      new SocketService(tokenData['ssoToken'], dispatch);
      }
    }
    // autoLogin();
  },[])

    useEffect(() => {
    const host = window.location.host;
    const parts = host.split('.');
    if (parts.length > 2) {
      setCountryCode(parts[parts.length - 1]);
    }
   
  }, []);

  // OTP Resend Timer Effect
  useEffect(() => {
    if (isForgetPassword !== "otp" || otpResendTimer <= 0) return;

    const interval = window.setInterval(() => {
      setOtpResendTimer((prev) => {
        if (prev <= 1) {
          window.clearInterval(interval);
          return 0;
        }
        return prev - 1;
      });
    }, 1000);

    return () => window.clearInterval(interval);
  }, [isForgetPassword, otpResendTimer]);

  const normalizedForgotEmail = forgotEmail.trim().toLowerCase();
  const forgotEmailName = normalizedForgotEmail.split("@")[0] || "";
  const trimmedNewPassword = newPassword.trim();
  const lowerNewPassword = trimmedNewPassword.toLowerCase();

  const isNoPersonalInfoRule =
    trimmedNewPassword.length > 0 &&
    !lowerNewPassword.includes(normalizedForgotEmail) &&
    (!forgotEmailName || !lowerNewPassword.includes(forgotEmailName));
  const isMinLengthRule = trimmedNewPassword.length >= 8;
  const isNumberOrSymbolRule = /[\d\W_]/.test(trimmedNewPassword);

  let passwordStrengthScore = 0;
  if (trimmedNewPassword.length > 0) passwordStrengthScore = 1;
  if (isMinLengthRule) passwordStrengthScore = 2;
  if (isMinLengthRule && isNumberOrSymbolRule) passwordStrengthScore = 3;
  if (isNoPersonalInfoRule && isMinLengthRule && isNumberOrSymbolRule) {
    passwordStrengthScore = 4;
  }

  const passwordStrengthClass =
    passwordStrengthScore === 4
      ? styles.alertweakstrong
      : passwordStrengthScore === 3
      ? styles.alertweakmoder
      : passwordStrengthScore === 2
      ? styles.alertweakfair
      : styles.alertweak;

  const passwordStrengthLabel =
    passwordStrengthScore >= 4
      ? "Strong"
      : passwordStrengthScore === 3
      ? "Moderate"
      : passwordStrengthScore === 2
      ? "Fair"
      : trimmedNewPassword.length > 0
      ? "Weak"
      : "";

  return (
    <div className="loginwhitepage">
      <div className={styles.loginheader}>
        <div className={styles.headerview}>
          <img src={logoImage} alt="Header Logo" />
        </div>
        <button className={styles.contactsign}>
          <a href="https://worktual.co.uk/contact/" className={styles.forgot}>Contact Us</a>
        </button>
      </div>

      <div className={styles.body}>
        <div className={styles.centeredcontent}>
          {isForgetPassword === "login" ? (
            <div className={styles.lucasmain} onKeyDown={(e :any) => e.key === 'Enter' && handleSubmitLogin()}>
              <h2 style={{ fontFamily: "intermedium" }} className={styles.inputopmanerh}>Log in to Your Account</h2>
              <p className={styles.inputopmaner}>to access Campaign Management</p>

              <div className={styles.inputopmanerprimary}>
                <button className={styles.google}>
                  <img src={googleicn} alt="Google Icon" />
                  Continue with Google
                </button>
                <button className={styles.appleicon}>
                  <img src={msicn} alt="Microsoft Icon" />
                  Continue with Microsoft
                </button>
                <button className={styles.microsofticon}>
                  <img src={appleicn} alt="Apple Icon" />
                  Continue with Apple
                </button>
              </div>

              <div className={styles.loginid}><p>or</p></div>

              <div className={styles.inputdiv}>
                <div className="inputinsideimg">
                  <input
                    type="email"
                    placeholder="Enter your E-mail"
                    maxLength={250}
                    name="emailId"
                    value={userDetails.emailId}
                    onChange={handleUserDetails}
                    autoFocus
                  />
                  <span className="insideimg"><img src={emaillogin} alt="Email Icon" /></span>
                  {error.email && <span className={styles.errtxt}>Please enter a valid email address</span>}
                </div>
              </div>

              <div className={styles.inputdiv}>
                <div className="inputinsideimg">
                  <input
                    type={showPassword ? 'text' : 'password'}
                    placeholder="Enter your Password"
                    maxLength={250}
                    name="password"
                    value={userDetails.password}
                    onChange={handleUserDetails}
                  />
                  <span className="insideimg">
                    <img src={locklogin} alt="Lock Icon" />
                  </span>
                  <img
                    className="eyeclose"
                    src={showPassword ? eyeopen : eyeclosed}
                    onClick={togglePasswordVisibility}
                    alt="Toggle password visibility"
                  />
                  {error.password && <span className={styles.errtxt}>Please enter your password</span>}
                </div>
              </div>

              <div className={styles.keeplog}>
                <span className={styles.chkboxspan}>
                  <input type="checkbox" id="keeplogged" />
                  <label style={{ fontFamily: "interregular", fontSize: "14px" }} htmlFor="keeplogged">Keep me logged in</label>
                </span>
                <p className="clickabletxt" onClick={openForgotFlow} style={{ cursor: "pointer" }}>Forgot password?</p>
              </div>

              {error.wholeError && (
                <span className={`deletetxt ${styles.noacc} ${styles.errorText}`}>{errorMsg}</span>
              )}
              <div className={styles.lucasmaininput}>
                {
                  loading ? <div className={styles.buttonLoader}>
                     <div  style={{width:"24px", height:"24px"}} className="spinner"></div>
                  </div> :
                    <button className={styles.btnSignin} onClick={handleSubmitLogin} disabled={loading}>
                      Login
                    </button>
                }
                <div className="loadersetactive"></div>
              </div>
              <p >
                Don't have an account yet? <span className="clickabletxt">Sign up</span>
              </p>
            </div>
          ) : isForgetPassword === "forget" ? (
            <div className={styles.lucasmain}>
              <h2 style={{ fontFamily: "intermedium" }} className={styles.inputopmanerh}>Forgot password?</h2>
              <p className={styles.inputopmaner}>
                Enter your registered email address and we'll send you a OTP to reset your password securely.
              </p>

              <div className={styles.inputdiv}>
                <div className="inputinsideimg">
                  <input
                    placeholder="Enter your E-mail"
                    maxLength={100}
                    value={forgotEmail}
                    onChange={(e) => {
                      setForgotEmail(e.target.value);
                      setForgotError("");
                    }}
                    autoFocus={true}
                  />
                  <span className="insideimg"><img src={emaillogin} alt="Email Icon" /></span>
                </div>
              </div>

              {forgotError ? (
                <span className={`deletetxt ${styles.noacc} ${styles.errorText}`}>{forgotError}</span>
              ) : null}

              <div className={styles.lucasmaininput}>
                <button className={styles.btnSignin} onClick={handleForgotSubmit} disabled={forgotLoading}>
                  {forgotLoading ? "Please wait..." : "Confirm E-mail"}
                </button>
              </div>

              <p >
                Don't have a Worktual account yet? <span className="clickabletxt">Sign up</span>
              </p>

              <p  onClick={backToLogin}>
                Already have an account? <span className="clickabletxt">Log In</span>
              </p>
            </div>
          ) : isForgetPassword === "otp" ? (
            <div className={styles.lucasmain}>
              <h2 style={{ fontFamily: "intermedium" }} className={styles.inputopmanerh}>E-mail Verification</h2>
              <p className={styles.inputopmaner}>
                A verification code has been sent to you at{' '}
                <span style={{ color: 'green' }}>{forgotEmail}</span>. Enter the code below.
              </p>

              <div className={styles.inputdiv}>
                <div className={styles.otpInputRow} style={{ display: 'flex', gap: '10px', justifyContent: 'center' }}>
                  {Array.from({ length: 6 }).map((_, index) => (
                    <input
                      key={index}
                      ref={(el) => { otpInputsRef.current[index] = el; }}
                      type="text"
                      inputMode="numeric"
                      pattern="[0-9]*"
                      maxLength={1}
                      aria-label={`OTP Digit ${index + 1}`}
                      title={`OTP Digit ${index + 1}`}
                      value={otp[index] || ""}
                      onChange={(e) => handleOtpInputChange(index, e.target.value)}
                      onKeyDown={(e) => handleOtpKeyDown(e, index)}
                      onPaste={handleOtpPaste}
                      style={{
                        width: '48px',
                        height: '56px',
                        textAlign: 'center',
                        fontSize: '20px',
                        borderRadius: '12px',
                        border: '1px solid #D1D5DB',
                        background: '#F9FAFB',
                        outline: 'none',
                      }}
                      autoFocus={index === 0}
                    />
                  ))}
                </div>
              </div>

              {otpError ? (
                <span className={`deletetxt ${styles.noacc} ${styles.errorText}`}>{otpError}</span>
              ) : null}

              {otpMessage ? (
                <span style={{ color: 'green' }} className={styles.dontaccnt}>{otpMessage}</span>
              ) : null}

              <div className={styles.lucasmaininput}>
                <button className={styles.btnSignin} onClick={onVerifyOtp} disabled={otpLoading}>
                  {otpLoading ? "Verifying..." : "Verify"}
                </button>
                <button className={styles.btnSignin} onClick={openForgotFlow} style={{ marginTop: "10px", backgroundColor: "#f0f0f0", color: "#333" }}>
                  Change E-mail
                </button>
              </div>

              <div  style={{ marginTop: "20px" }}>
                {Math.min(otpAttemptCount, 5)} / 5 attempts - {formatTimer(otpResendTimer)} seconds remaining
              </div>

              <p>
                Didn't receive the OTP?{' '}
                <span
                  onClick={otpResendTimer === 0 && otpAttemptCount < OTP_MAX_ATTEMPTS ? onResendOtp : undefined}
                  style={{
                    color: otpResendTimer === 0 && otpAttemptCount < OTP_MAX_ATTEMPTS ? '#0084FF' : '#9CA3AF',
                    cursor: otpResendTimer === 0 && otpAttemptCount < OTP_MAX_ATTEMPTS ? 'pointer' : 'not-allowed',
                    textDecoration: otpResendTimer === 0 && otpAttemptCount < OTP_MAX_ATTEMPTS ? 'underline' : 'none',
                  }}
                  className="clickabletxt"
                >
                  Resend
                </span>
              </p>
            </div>
          ) : isForgetPassword === "update" ? (
             <div className={styles.lucasmain} style={{position:"relative"}}>
              <h2 style={{ fontFamily: "intermedium" , marginBottom:"16px" }} className={styles.inputopmanerh}>Create New Password</h2>

              <div className={styles.inputdiv}>
                <div className="inputinsideimg">
                  <input
                    placeholder="Enter your new password"
                    type="password"
                    value={newPassword}
                    onChange={(e) => {
                      setNewPassword(e.target.value);
                      setPasswordError("");
                    }}
                    autoFocus
                  />
                  <span className="insideimg">
                    <img src={locklogin} alt="Lock Icon" />
                  </span>
                </div>
              </div>

              {trimmedNewPassword.length > 0 && (
                <div className={styles.inputdiv} style={{ flexDirection: 'column', gap: '10px' }}>
                  <div style={{ display: 'flex', gap: '8px', justifyContent: 'space-between', marginBottom:"12px" }}>
                    {Array.from({ length: 4 }).map((_, index) => (
                      <span
                        key={index}
                        style={{
                          flex: 1,
                          height: '6px',
                          borderRadius: '999px',
                          background:
                            passwordStrengthScore > index
                              ? passwordStrengthScore === 4
                                ? '#00C851'
                                : passwordStrengthScore === 3
                                ? '#33B5E5'
                                : passwordStrengthScore === 2
                                ? '#FFBB33'
                                : '#FF4444'
                              : '#E5E7EB',
                        }}
                      />
                    ))}
                  </div>
                  <div className={`${styles.inputdivalertset || ''}`}>
                    <span 
                      className={passwordStrengthClass}
                      style={{
                        color:
                          passwordStrengthScore === 4
                            ? '#00C851'
                            : passwordStrengthScore === 3
                            ? '#33B5E5'
                            : passwordStrengthScore === 2
                            ? '#FFBB33'
                            : '#FF4444'
                      }}
                    >
                      {passwordStrengthLabel}
                    </span>
                  </div>
                </div>
              )}

              <div className={styles.inputdiv}>
                <div className="inputinsideimg">
                  <input
                    placeholder="Confirm your password"
                    type={showConfirmPassword ? "text" : "password"}
                    value={confirmPassword}
                    onChange={(e) => {
                      setConfirmPassword(e.target.value);
                      setPasswordError("");
                    }}
                  />
                  <span className="insideimg">
                    <img src={locklogin} alt="Lock Icon" />
                  </span>
                  <img
                    className="eyeclose"
                    src={showConfirmPassword ? eyeopen : eyeclosed}
                    onClick={() => setShowConfirmPassword((prev) => !prev)}
                    alt="Toggle password visibility"
                  />
                </div>
              </div>

              {trimmedNewPassword.length > 0 && (
                <div className={styles.dontaccnt}>
                  <p className={!isNoPersonalInfoRule ? styles.wrong : ''} style={{ margin: 0, display: 'flex', alignItems: 'center', gap: '8px' }}>
                    {isNoPersonalInfoRule ? (
                      <span style={{ color: '#00C851' }}>✔</span>
                    ) : (
                      <span style={{ color: '#FF4444' }}>✖</span>
                    )}
                    Cannot contain your name or e-mail
                  </p>
                  <p className={!isMinLengthRule ? styles.wrong : ''} style={{ margin: 0, display: 'flex', alignItems: 'center', gap: '8px' }}>
                    {isMinLengthRule ? (
                      <span style={{ color: '#00C851' }}>✔</span>
                    ) : (
                      <span style={{ color: '#FF4444' }}>✖</span>
                    )}
                    At least 8 characters
                  </p>
                  <p className={!isNumberOrSymbolRule ? styles.wrong : ''} style={{ margin: 0, display: 'flex', alignItems: 'center', gap: '8px' }}>
                    {isNumberOrSymbolRule ? (
                      <span style={{ color: '#00C851' }}>✔</span>
                    ) : (
                      <span style={{ color: '#FF4444' }}>✖</span>
                    )}
                    Contains a number or symbol
                  </p>
                </div>
              )}

              {passwordError ? (
                <span className={`deletetxt ${styles.noacc} ${styles.errorText}`}>{passwordError}</span>
              ) : null}

              <div className={styles.lucasmaininput}>
                <button className={styles.btnSignin} onClick={onUpdatePassword} disabled={passwordLoading}>
                  {passwordLoading ? "Updating..." : "Create password"}
                </button>
              </div>

              <p>
                Once you've set your new password, you can use it to log in and reset it anytime in the settings.              </p>
            </div>
          ) 
          : null
          } 
        
        </div>
      </div>
    </div>
  );
};

export default Login;
