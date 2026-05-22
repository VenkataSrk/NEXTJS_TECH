import {
  ClearStorage,
  GetItemFromStorage,
  SetItemInStorage,
} from "../../base/custom_hooks/useStorage";
import { decryptCodes, encryptCodes, getToken } from "../../base/utils";
import { config, getDynamicLocation } from "../../base/constant";
import { useEffect, useState, useRef } from "react";
import { useLocation, useNavigate } from "react-router-dom";
import { Cookies } from "../../base/cookies";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { LoginActions } from "../../store/slice/loginSlice/loginSlice";
import { LoginService } from "../../base/service/loginService";
import jwtDecode from "jwt-decode";
import { loginConfig } from "./LoginConstant";
import styles from "./loginnew.module.scss";
import { useDispatch, useSelector } from "react-redux";
import { v4 as uuidv4 } from "uuid";
import Loading from "../component/loader/loader";
import mainlogo from "../../assets/images/mainlogo.svg";
import Google from "../../assets/images/google.svg";
import Microsoft from "../../assets/images/microsoft.svg";
import Apple from "../../assets/images/apple.svg";
import Lockicon from "../../assets/images/locksvg.png";
import Messageicon from "../../assets/images/messageicon.svg";

import {
  LoginSocialApple,
  LoginSocialGoogle,
  LoginSocialMicrosoft,
} from "reactjs-social-login";
import {
  emailOtpVerification,
  fetchAndStoreCrmUserDetails,
  getUserRolesPermisions,
  resendOtp,
  updatePassword,
  emailotpverification,
  saveDeviceInfo,
  updateDeviceDetails,
  insertUserActivityLogInLog,
  insertLoginLog,
} from "../../base/service/core_serviceApi";
import {
  getUserOnboardDetails,
  inserUpdateOnboardDetails,
} from "../../base/onboardConfig";

import OTPInput from "../../login/otpinput";
import { getDeviceDetails } from "../../base/device";
import aiChatAssistSocketservice from "../../services/aiChatService";
import inboxAiChatAssistSocketservice from "../../services/inboxAiChatService";
import presenceService from "../../base/sockets/presence";
import socketService from "../../base/sockets/socketService";
import axios from "axios";

type loginState = {
  email: string;
  password: string;
  remember: boolean;
  togglePass: boolean;
  errors: boolean;
  showPass: boolean;
  LoggedIn: boolean;
  errorMessage: {
    emailError: string;
    passWordError: string;
    validationError: string;
  };
  isForgetPassword: string;
  isChattour: number;
};

const OTP_RESEND_COOLDOWN_SECONDS = 10 * 60;
const OTP_MAX_ATTEMPTS = 5;

const Login = () => {
  const dispatch = useDispatch();
  const loginSlice = useSelector((state: any) => state.loginUser);
  const [showLogin, setShowLogin] = useState(false);
  const [loader, setLoader] = useState<any>(false);
  const [countryCode, setCountryCode] = useState("uk");
  const { onboardingStaus } = useSelector((store: any) => store.onboarding);
  const [login, setLogin] = useState<loginState>({
    email: "",
    password: "",
    remember: false,
    showPass: false,
    togglePass: false,
    LoggedIn: false,
    errors: false,
    errorMessage: {
      emailError: "",
      passWordError: "",
      validationError: "",
    },
    isForgetPassword: "login",
    isChattour: 0,
  });

  const navigate = useNavigate();
  useEffect(() => {
    const init = async () => {
      const tokenData: any = Cookies.get();
      const Authorization: any = tokenData["ssoToken"] ?? "";
      if (Authorization !== "") {
        sessionStorage.clear();
        const auth: any = jwtDecode(Authorization);
        if (auth?.is_crm_user === 1) {
          try {
            setLoader(true);
            setShowLogin(false);
            SetItemInStorage(LocalStorage.IS_LOGIN_INIT_FUNC, true);
            SetItemInStorage(LocalStorage.ACCESS_TOKEN, Authorization);
            SetItemInStorage(LocalStorage.REFRESH_TOKEN, Authorization);
            const callServerWssUrl = config().CALL_SERVER_WSS_URL;
            if (callServerWssUrl) {
              SetItemInStorage(LocalStorage.WSS_URL, callServerWssUrl);
            }
            const LoginUserDetails: any = jwtDecode(Authorization);
            const loginTime: number = Date.now();
            SetItemInStorage(LocalStorage.LOGIN_TIME, loginTime);
            Cookies.set("allowMultipleLogin", true, 1);
            let uuidNew: any = uuidv4();
            if (!GetItemFromStorage("deviceid")) {
              SetItemInStorage("deviceid", uuidNew);
            } else {
              uuidNew = GetItemFromStorage("deviceid");
            }
            SetItemInStorage(
              LocalStorage.ROLE_ID,
              LoginUserDetails?.ccas_role_info,
            );
            SetItemInStorage(
              LocalStorage.PRODUCT_ID,
              LoginUserDetails.product_id,
            );
            SetItemInStorage(LocalStorage.PLAN_ID, LoginUserDetails.parent_id);
            LoginUserDetails.roleId = LoginUserDetails?.ccas_role_info;
            SetItemInStorage(
              LocalStorage.USER_EMAIL,
              LoginUserDetails?.Email?.trim?.() || "",
            );
            SetItemInStorage(LocalStorage.USER_DETAILS, LoginUserDetails);
            setLoading(false);
            SetItemInStorage(LocalStorage.REMEMBER_ME, login.remember);
            sessionStorage.setItem(
              "logindetails",
              JSON.stringify(LoginUserDetails),
            );
            ConnectSockets(Authorization, navigate, dispatch);
            SetItemInStorage("uuid", uuidv4());
            await fetchAndStoreCrmUserDetails();
            const onboardRes = await getUserOnboardDetails();

            // roles and permisions
            getUserRolesPermisions(dispatch);
            if (
              LoginUserDetails?.org_role_id_crm == 3 &&
              !onboardRes?.isCompleted
            ) {
              navigate("/onboarding");
            } else {
              navigate("/home/sales");
            }
          } catch (error) {
            console.log(error);
            setShowLogin(true);
          } finally {
            setLoader(false);
          }
        } else {
          clearData("");
          setShowLogin(true);
        }
      } else {
        clearData("");
        setShowLogin(true);
      }
    };
    init();
  }, []);

  const clearData = (error: any) => {
    Cookies.clear();
    ClearStorage();
    localStorage.clear();
    sessionStorage.clear();
    setLoading(false);
    setLogin({
      ...login,
      errorMessage: {
        ...login.errorMessage,
        validationError: error,
      },
    });
  };

  const [loading, setLoading] = useState<boolean>(false);
  const [error, setErrors] = useState<any>({
    email: false,
    password: false,
    wholeError: false,
  });
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
  const [passwordMessage, setPasswordMessage] = useState("");

  const location = useLocation();

  const togglePassword = () => {
    setLogin({ ...login, togglePass: !login.togglePass });
  };

    const loginCheck=async(email :any,type :any)=>{
    try {
      const body ={
        email: email,
        type: type
        // sessionId: `SessionId${uuidv4()}`
      }
    const response = await axios.post(
      `${config().CORESERVICE_URL}/loginCheck`,
      {
        ...body
      }
    );
    return response.data.loginCheckRes?.[0];
  } catch (error: any) {
    console.error("Error:", error.response?.data || error.message);
  }
  }

  const SignIn = async () => {
    if (loading) return;

    if (login.password === "" && login.email === "") {
      setErrors({ ...error, wholeError: true });
      setLogin({
        ...login,
        errorMessage: {
          ...login.errorMessage,
          validationError: loginConfig().BLANK_VALIDATION,
        },
      });
    } else if (login.email === "") {
      setErrors({ ...error, email: true });
      setLogin({
        ...login,
        errorMessage: {
          ...login.errorMessage,
          emailError: loginConfig().EMAIL_BLANK,
        },
      });
    } else if (login.password === "") {
      setErrors({ ...error, password: true });
      setLogin({
        ...login,
        errorMessage: {
          ...login.errorMessage,
          passWordError: loginConfig().BLANK_PASSWORD,
        },
      });
    } else {
      setErrors({ wholeError: false });
      let uuidNew: any = uuidv4();
      if (!GetItemFromStorage("deviceid")) {
        SetItemInStorage("deviceid", uuidNew);
      } else {
        uuidNew = GetItemFromStorage("deviceid");
      }

      const userObject = {
        username: login.email.trim(),
        password: login.password.trim(),
        source: "Web",
        ipAddress: "136.144.56.255:443",
        isForceLogin: false,
        loginCheck: getDynamicLocation(),
        deviceId: uuidNew,
      };

      setLoading(true);
      let LoginUserDetails: any = {};
      try {
    
        const value: any = await LoginService.login(userObject);
        if (
          value &&
          value?.data?.statusCode === 200 &&
          value?.data?.is_crm_user === 1
        ) {
          const activeCheck =await loginCheck(userObject?.username,0);

          if(!["Data Updated Successfully","Data Inserted Successfully"].includes(activeCheck?.errmsg)){
            setErrors({ ...error, wholeError: true });
             setLogin({
               ...login,
               errorMessage: {
                 ...login.errorMessage,
                 validationError: "Credential is Active in another device",
               },
             });
             setLoading(false);
             return;
          }
          SetItemInStorage(LocalStorage.ACCESS_TOKEN, value.data.token);
          await loginCheck(userObject?.username,1);
          const callServerWssUrl = config().CALL_SERVER_WSS_URL;
          if (callServerWssUrl) {
            SetItemInStorage(LocalStorage.WSS_URL, callServerWssUrl);
          }
          Cookies.set("ssoToken", value.data.token, 1);
          LoginUserDetails = jwtDecode(value.data.token);
          SetItemInStorage(LocalStorage.LOGIN_TIME, Date.now());
          SetItemInStorage(LocalStorage.USER_DETAILS, LoginUserDetails);
          if (login.remember) {
            SetItemInStorage(
              LocalStorage.PASSWORD,
              encryptCodes(login.password, "PassWord"),
            );
          }
          SetItemInStorage("uuid", uuidv4());
          ConnectSockets(value.data.token, navigate, dispatch);
          sessionStorage.setItem("logindetails", JSON.stringify(LoginUserDetails));
          await fetchAndStoreCrmUserDetails();
          dispatch(LoginActions.login({ token: value.data.token, loginUserDetails: LoginUserDetails }));
          setLoading(false);
          (window as any).onUserLoginSuccess?.({ email: userObject.username });
          const onboardRes = await getUserOnboardDetails();

          // roles and permisions
          getUserRolesPermisions(dispatch);

          if (
            LoginUserDetails?.org_role_id_crm == 3 &&
            !onboardRes?.isCompleted
          ) {
            navigate("/onboarding");
          } else {
            navigate("/home/sales");
          }

          const data = {
            folwTrack: onboardingStaus,
            isCompleted: 0,
          };

          try {
            const deviceInfo = await getDeviceDetails();
            await saveDeviceInfo(deviceInfo);
          } catch (error) {
            console.log("deviceInfo error>>>>", error);
          }
          updateDeviceDetails()
          insertLoginLog()
          
          
          if (onboardRes?.isEmpty) {
            await inserUpdateOnboardDetails(data);
          }
        } else if (value && value?.data?.statusCode === 301) {
          setLoading(false);
          setLogin({
            ...login,
            errorMessage: {
              ...login.errorMessage,
              validationError:
                value?.data?.message || loginConfig().COMMON_ERROR,
            },
          });
          setErrors({ wholeError: true });
        } else if (value && value?.data?.statusCode === 424) {
          setLoading(false);
          setLogin({
            ...login,
            errorMessage: {
              ...login.errorMessage,
              validationError:
                value?.data?.message || loginConfig().COMMON_ERROR,
            },
          });
          setErrors({ wholeError: true });
        } else {
          setLoading(false);
          setLogin({
            ...login,
            errorMessage: {
              ...login.errorMessage,
              validationError:
                value?.data?.message || loginConfig().AUTHENTICATION_FAILED,
            },
          });
          setErrors({ wholeError: true });
        }
    
      } catch (e: any) {
        setLoading(false);
        setLogin({
          ...login,
          errorMessage: {
            ...login.errorMessage,
            validationError:
              e?.response?.data?.message ?? loginConfig().AUTHENTICATION_FAILED,
          },
        });
        setErrors({ wholeError: true });
      }
    }
  };

  useEffect(() => {
    const getCookie: any = getToken();
    setLogin({ ...login, remember: login.remember });
    const isRemember: any =
      getCookie?.RememberMe && JSON.parse(getCookie.RememberMe);
    if (isRemember === true) {
      if (getCookie.passWord) {
        setLogin({
          ...login,
          email: getCookie.Email,
          password: decryptCodes(getCookie.passWord, "PassWord"),
          remember: isRemember,
        });
      }
    } else {
      setLogin({ ...login, email: "", password: "", remember: false });
    }
    const pathData = location?.pathname;
    if (pathData === "/login" || pathData === "/") {
      if (getCookie["accessToken"]) {
        dispatch(LoginActions.updateLoadingState({ isLoadingBack: true }));
      }
    }
  }, []);

  const onChangeInput = (e: any) => {
    setLogin({ ...login, [e.target.name]: e.target.value, errors: false });
    setErrors({ ...error, email: false, password: false, wholeError: false });
  };
  const handleEnter = (e: any) => {
    if (e.key === "Enter") {
      SignIn();
    }
  };

  const isSocialMedia = useRef(false);

  const socialMediaLogin = async (data: any, type: any) => {
    const _body = {
      email: data,
      device_type: "",
      browser: "",
      login_type: type,
    };

    try {
      const response = await LoginService.socialMediaLogin(_body);
      if (response?.data?.results[0].v_errcode == 0) {
        const email = response?.data?.results[0]?.my_acc_email;
        const password = response?.data?.results[0]?.my_acc_password;
        isSocialMedia.current = true;
        const updatedLogin = {
          ...login,
          email,
          password,
          errors: false,
        };
        setLogin(updatedLogin);
        setErrors({
          ...error,
          email: false,
          password: false,
          wholeError: false,
        });
      } else if (response?.data?.results[0]?.v_errcode == -1) {
        setLoader(false);
        alert(response?.data?.results[0]?.v_errmsg);
      }
    } catch (err) {
      console.error("SocialMediaLogin error", err);
      return null;
    }
  };

  useEffect(() => {
    if (isSocialMedia.current) {
      SignIn();
      isSocialMedia.current = false;
    }
  }, [login.email, login.password]);

  useEffect(() => {
    try {
      const host = window.location.host;
      const parts = host.split(".");
      const loc = parts[parts.length - 1];
      if (loc === "in" || loc === "uk" || loc === "com") {
        setCountryCode(loc);
      } else {
        setCountryCode("uk");
      }
    } catch (error) {
      setCountryCode("uk");
      console.log(error);
    }
  }, []);

  const openForgotFlow = () => {
    setForgotEmail(login.email || "");
    setForgotError("");
    setOtpError("");
    setOtpMessage("");
    setPasswordError("");
    setPasswordMessage("");
    setLogin((prev) => ({ ...prev, isForgetPassword: "forget" }));
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
    setPasswordMessage("");
    setLogin((prev) => ({ ...prev, isForgetPassword: "login" }));
  };

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
        setLogin((prev) => ({ ...prev, isForgetPassword: "otp" }));
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

  const handleOTPChange = (value: string) => {
    setOtp(value);
    setOtpError("");
    setOtpMessage("");
  };

  const formatTimer = (seconds: number) => {
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return `${String(mins).padStart(2, "0")}:${String(secs).padStart(2, "0")}`;
  };

  useEffect(() => {
    if (login.isForgetPassword !== "otp" || otpResendTimer <= 0) return;

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
  }, [login.isForgetPassword, otpResendTimer]);

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
        setLogin((prev) => ({ ...prev, isForgetPassword: "update" }));
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
      ? styles.stepverifystrong
      : passwordStrengthScore === 3
        ? styles.stepverifymoderate
        : passwordStrengthScore === 2
          ? styles.stepverifyfair
          : "";

  const passwordStrengthLabel =
    passwordStrengthScore >= 4
      ? "Strong"
      : passwordStrengthScore === 3
        ? "Moderate"
        : passwordStrengthScore === 2
          ? "Fair"
          : passwordStrengthScore === 1
            ? "Weak"
            : "";

  const passwordStrengthLabelClass =
    passwordStrengthScore >= 4
      ? styles.alertweakstrong
      : passwordStrengthScore === 3
        ? styles.alertweakmoder
        : passwordStrengthScore === 2
          ? styles.alertweakfair
          : styles.alertweak;

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
    setPasswordMessage("");
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

  const showLoginHeader = login.isForgetPassword === "login";
  const showForgotFlowHeader = login.isForgetPassword !== "login";

  // const handleSubmit = async () => {
  //   const normalizedEmail = email.trim();

  //   if (!normalizedEmail) {
  //     setError("Email is required");
  //     return;
  //   }

  //   setLoading(true);
  //   setError("");

  //   try {
  //     const response: any = await emailotpverification({
  //       email: normalizedEmail,
  //       emailId: normalizedEmail,
  //     });

  //     if (response?.statusCode === 200) {
  //       navigate("/email", { state: { email: normalizedEmail } });
  //     } else {
  //       setError(response?.message || "Invalid email address");
  //     }
  //   } catch (e) {
  //     setError("Something went wrong. Please try again.",e);
  //   } finally {
  //     setLoading(false);
  //   }
  // };
  const openContactUs = () => {
    switch (countryCode) {
      case "uk":
        window.open("https://worktual.co.uk/contact/");
        break;
      case "in":
        window.open("https://worktual.in/contact/");
        break;
      case "com":
        window.open("https://worktual.com/contact/");
        break;
    }
  }

  return (
    <>
      {loader && <Loading />}
      {!loginSlice?.isLoadingBack && showLogin && (
        <>
          {(showLoginHeader || showForgotFlowHeader) && (
            <div className={styles.loginheader}>
              <img src={mainlogo} alt="Login" />
              <div
                className={styles.contactsales}
                style={{ cursor: "pointer" }}
                onClick={openContactUs}
              >
                Contact Us
              </div>
            </div>
          )}
          {login.isForgetPassword === "login" && (
            <div className={styles.loginheightforscroll}>
              <div className={styles.loginmain}>
                <div className={styles.loginmainblock}>
                  <h2>Log in to Your Account</h2>
                  <p>to access AI Native CRM</p>
                  <div className={styles.socialmedia}>
                    <LoginSocialGoogle
                      client_id="672189196887-a63iclna3qh1145iousgui9ks1adgt1p.apps.googleusercontent.com"
                      scope="https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/userinfo.profile"
                      redirect_uri="https://ccaas.worktual.co.uk/"
                      onResolve={async ({ provider, data }) => {
                        console.log(data, "data1245");
                        console.log(provider, "provider");
                        const email =
                          data?.email ||
                          data?.profile?.email ||
                          data?.profileObj?.email;
                        if (email) {
                          socialMediaLogin(email, "Google");
                        } else {
                          setLoader(false);
                          console.error(
                            "Email is undefined in the response",
                            data,
                          );
                        }
                      }}
                      onReject={(err) => {
                        console.log("Error during authentication", err);
                      }}
                    >
                      <div className={styles.socialmediainner}>
                        <img src={Google} alt="appleicon" />
                        Continue with Google
                      </div>
                    </LoginSocialGoogle>
                    <LoginSocialMicrosoft
                      client_id="651c69df-eea7-4624-a1cf-cecdf82ec80e"
                      redirect_uri="https://ccaas.worktual.co.uk/"
                      onResolve={({ provider, data }: any) => {
                        console.log("data", data);
                        console.log("provider", provider);
                        const decodedToken: any = jwtDecode(data.access_token);
                        console.log("decoded token", decodedToken);
                        data.social_account_email = decodedToken.email;

                        data.social_account_email = data.mail
                          ? data.mail
                          : decodedToken.email
                            ? decodedToken.email
                            : decodedToken.unique_name;
                        data.social_account_type = "microsoft";

                        socialMediaLogin(
                          data.social_account_email,
                          "Microsoft",
                        );
                      }}
                      onReject={(err) => {
                        console.log("iiiiiiiiii", err);
                      }}
                    >
                      <div
                        className={`${styles.socialmediainner} ${styles.microsoft}`}
                      >
                        <img src={Microsoft} alt="microsoft icon" />
                        Continue with Microsoft
                      </div>
                    </LoginSocialMicrosoft>

                    <LoginSocialApple
                      client_id="com.worktual.login"
                      redirect_uri="https://ccaas.worktual.co.uk/"
                      onResolve={async ({ provider, data }: any) => {
                        console.log("data", data);
                        const decodedToken: any = jwtDecode(
                          data.authorization.id_token,
                        );
                        console.log("decoded token", decodedToken);
                        data.social_account_email = decodedToken.email;
                        data.social_account_type = "Apple";
                        console.log("provider", provider);

                        const email =
                          data.social_account_email || decodedToken?.email;

                        console.log("Final Apple login object:", email);

                        await socialMediaLogin(email, "Apple");
                      }}
                      onReject={(err) => {
                        console.log(err);
                      }}
                    >
                      <div
                        className={`${styles.socialmediainner} ${styles.apple}`}
                      >
                        <img src={Apple} alt="microsofticon" />
                        Continue with Apple
                      </div>
                    </LoginSocialApple>
                  </div>
                  <div className={styles.loginid}>or</div>
                  <div className={styles.loginblock}>
                    <div
                      className={`${styles.inputdiv} ${styles.inputdivmail}`}
                    >
                      <div className="inputinsideimg">
                        <input
                          placeholder="Enter your E-mail"
                          name="email"
                          // className={styles.emailaddress}
                          value={login.email}
                          maxLength={100}
                          onChange={onChangeInput}
                          autoFocus={true}
                        />
                        <span className={`${styles.msgicon}  insideimg`}>
                          <img src={Messageicon} alt="Messageicon" />
                        </span>
                        {error.email && (
                          <span
                            className={`${styles.errorText} ${styles.invaliderrorText}`}
                          >
                            {login.errorMessage.emailError}
                          </span>
                        )}
                      </div>
                    </div>
                    <div
                      className={`${styles.inputdiv} ${styles.inputdivmail}`}
                    >
                      <div className="inputinsideimg">
                        <input
                          placeholder="Enter your password"
                          // className={styles.emailaddresslock}
                          value={login.password}
                          type={login.togglePass ? "text" : "password"}
                          name="password"
                          onChange={onChangeInput}
                          onKeyDown={(e: any) => handleEnter(e)}
                        />
                        <span className={`${styles.msgicon} insideimg`}>
                          <img src={Lockicon} alt="Messageicon" />
                        </span>

                        <span
                          onClick={togglePassword}
                          className={styles.showClear}
                        >
                          {!login.togglePass ? (
                            <svg
                              width="24"
                              height="24"
                              viewBox="0 0 24 24"
                              fill="none"
                              xmlns="http://www.w3.org/2000/svg"
                            >
                              <g clipPath="url(#clip0_1076_18204)">
                                <path
                                  d="M12 7.29356C15.4703 7.29356 18.6418 9.05206 20.4916 12.0001C18.6418 14.9481 15.4704 16.7066 12 16.7066C8.52965 16.7066 5.35805 14.9478 3.50845 12.0003C5.35805 9.05226 8.52955 7.29356 12 7.29356ZM12 5.79346C7.86775 5.79346 4.24915 7.97146 2.21525 11.2391C1.92825 11.7 1.92825 12.3001 2.21525 12.7613C4.24915 16.0286 7.86775 18.2066 12 18.2066C16.1323 18.2066 19.7507 16.0286 21.7847 12.7613C22.0718 12.3001 22.0718 11.7 21.7847 11.2391C19.7509 7.97146 16.1321 5.79346 12 5.79346Z"
                                  fill="#AAABAD"
                                />
                                <path
                                  d="M12 14.7961C13.5441 14.7961 14.7959 13.5443 14.7959 12.0002C14.7959 10.456 13.5441 9.20426 12 9.20426C10.4558 9.20426 9.20405 10.456 9.20405 12.0002C9.20405 13.5443 10.4558 14.7961 12 14.7961Z"
                                  fill="#AAABAD"
                                />
                              </g>
                              <defs>
                                <clipPath id="clip0_1076_18204">
                                  <rect width="24" height="24" fill="white" />
                                </clipPath>
                              </defs>
                            </svg>
                          ) : (
                            <svg
                              width="24"
                              height="24"
                              viewBox="0 0 24 24"
                              fill="none"
                              xmlns="http://www.w3.org/2000/svg"
                            >
                              <g clipPath="url(#clip0_1074_1688)">
                                <path
                                  d="M11.995 7.29397C15.465 7.29397 18.645 9.05377 20.485 12.004C19.705 13.244 18.695 14.274 17.525 15.044L18.595 16.1138C19.875 15.2339 20.955 14.084 21.785 12.7639C22.075 12.3037 22.075 11.7038 21.785 11.2439C19.745 7.97387 16.135 5.79387 11.995 5.79387C10.875 5.79387 9.79504 5.95377 8.77504 6.26387L9.99504 7.49387C10.645 7.36377 11.315 7.29397 11.995 7.29397Z"
                                  fill="#AAABAD"
                                />
                                <path
                                  d="M11.5249 9.02397L14.9749 12.484C15.0049 12.3238 15.0149 12.1639 15.0149 12.004C15.0149 10.3341 13.6649 8.98397 11.9949 8.98397C11.8349 8.98397 11.6749 8.99397 11.5249 9.02397Z"
                                  fill="#AAABAD"
                                />
                                <path
                                  d="M3.32464 2.22757C3.03214 1.93367 2.55754 1.93367 2.26414 2.22557C1.97074 2.51747 1.96974 2.99217 2.26214 3.28607L6.27564 7.31637C4.62444 8.26607 3.23084 9.62037 2.21494 11.2439C1.92494 11.7039 1.92494 12.3037 2.21494 12.7639C4.24494 16.0239 7.86494 18.2038 11.9949 18.2038C13.5183 18.2038 14.9622 17.9155 16.2875 17.3701L20.6717 21.7724C20.8182 21.9199 21.0106 21.9931 21.2029 21.9931C21.3943 21.9931 21.5857 21.9199 21.7322 21.7743C22.0257 21.4823 22.0266 21.0077 21.7342 20.7138L3.32464 2.22757ZM10.5408 11.5994L12.3916 13.4578C12.2678 13.502 12.1367 13.524 11.995 13.524C11.165 13.524 10.475 12.8341 10.475 12.004C10.475 11.8629 10.4971 11.7312 10.5408 11.5994ZM11.995 16.7039C8.52504 16.7039 5.35504 14.9439 3.50514 12.0039C4.48144 10.4495 5.81604 9.22407 7.37854 8.42377L9.40644 10.4599C9.13244 10.9067 8.97504 11.4306 8.97504 12.0038C8.97504 13.6637 10.335 15.0238 11.995 15.0238C12.5591 15.0238 13.0835 14.8661 13.5304 14.6012L15.1304 16.2079C14.1346 16.5348 13.0797 16.7039 11.995 16.7039Z"
                                  fill="#AAABAD"
                                />
                              </g>
                              <defs>
                                <clipPath id="clip0_1074_1688">
                                  <rect width="24" height="24" fill="white" />
                                </clipPath>
                              </defs>
                            </svg>
                          )}
                        </span>

                        {error.password && (
                          <span
                            className={`${styles.errorText}  ${styles.invaliderrorText}`}
                          >
                            {" "}
                            {login.errorMessage.passWordError}
                          </span>
                        )}
                        {error.wholeError && (
                          <span
                            className={`${styles.errorText} ${styles.valer}`}
                          >
                            {login.errorMessage.validationError}
                          </span>
                        )}
                      </div>
                    </div>
                    <div className={styles.loginbutton}>
                      <div className={styles.keeplog}>
                        <span className={styles.chkboxspan}>
                          <input     className="custom-checkbox " type="checkbox" id="keeplogged" />
                          <label
                            style={{
                              fontSize: "14px",
                            }}
                            htmlFor="keeplogged"
                          >
                            Keep me logged in
                          </label>
                        </span>
                        {/* <p className="clickabletxt">Forgot password?</p> */}
                         <div className={styles.forgot} onClick={openForgotFlow}>
                        <span>Forgot Password?</span>
                      </div>
                      </div>
                      {!loading ? (
                        <div
                          onClick={() => SignIn()}
                          className={`${styles.btnSignin} ${loading ? "disabled" : ""}`}
                        >
                          Login
                        </div>
                      ) : (
                        <div className="spinloader"></div>
                      )}
                      <div className={styles.forgot}>
                        Don't have a Worktual account yet? <span onClick={openContactUs}> Sign up</span>
                      </div>
                      {/* <div className={styles.forgot} onClick={openForgotFlow}>
                        <span>Forget Password?</span>
                      </div> */}
                    </div>
                  </div>
                </div>
              </div>
            </div>
          )}

          {login.isForgetPassword === "forget" && (
            <div className={styles.loginmain}>
              <div
                className={`${styles.loginmainblock} ${styles.loginmainblockstp} ${styles.loginmainsecure}`}
              >
                <h2>Forgot password?</h2>
                <p>
                  Enter your registered email address and we’ll send you a OTP
                  to reset your password securely.
                </p>
                <div className={styles.loginblock}>
                  <div className={`${styles.inputdiv} ${styles.inputdivmail}`}>
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
                    <span className={styles.msgicon}>
                      <img src={Messageicon} alt="Messageicon" />
                    </span>
                  </div>
                  {forgotError ? (
                    <span className={styles.errorText}>{forgotError}</span>
                  ) : null}
                  <div className={styles.loginbutton}>
                    <div
                      className={`${styles.btnSignin}`}
                      onClick={handleForgotSubmit}
                    >
                      {forgotLoading ? "Please wait..." : "Confirm E-mail"}
                    </div>
                    <div className={styles.forgot}>
                      Don't have a Worktual account yet? <span onClick={openContactUs}> Sign up</span>
                    </div>
                    <div className={styles.forgot} onClick={backToLogin}>
                      Already have an account?<span>Log In</span>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          )}

          {login.isForgetPassword === "otp" && (
            <div className={styles.loginmain}>
              <div
                className={`${styles.loginmainblock} ${styles.loginmainblockstp}`}
              >
                <h2>E-mail Verification</h2>
                <p>
                  A verification code has been sent to you at{" "}
                  <span style={{ color: "green" }}>{forgotEmail}</span> Enter
                  the code below
                </p>
                <div className={styles.loginblock}>
                  <div
                    className={`${styles.inputdiv} ${styles.verificationinputchange}`}
                  >
                    <OTPInput onChange={handleOTPChange} />
                  </div>
                  <div className={styles.loginbutton}>
                    <div className={styles.loadingbtn}>
                      <div
                        className={`${styles.btnSignin}`}
                        onClick={onVerifyOtp}
                      >
                        {otpLoading ? "Verifying..." : "Verify"}
                      </div>
                      {otpError ? (
                        <div className={styles.errorText}>{otpError}</div>
                      ) : null}
                      <div
                        className={`${styles.btnSignin} ${styles.btnSigninemail}`}
                        onClick={openForgotFlow}
                      >
                        Change E-mail
                      </div>
                    </div>

                    {otpMessage ? (
                      <div className={styles.forgot}>{otpMessage}</div>
                    ) : null}
                    <div className={styles.loginbuttonst}>
                      <div className={styles.forgot}>
                        {Math.min(otpAttemptCount, 5)} / 5 attempts -{" "}
                        <span
                          style={{
                            color: "#000",
                            cursor: "default",
                            textDecoration: "none",
                          }}
                        >
                          {formatTimer(otpResendTimer)}
                        </span>{" "}
                        seconds remaining
                      </div>
                      <div className={styles.forgot}>
                        Didn’t receive the OTP?{" "}
                        <span
                          onClick={
                            otpResendTimer === 0 &&
                            otpAttemptCount < OTP_MAX_ATTEMPTS
                              ? onResendOtp
                              : undefined
                          }
                          style={{
                            color:
                              otpResendTimer === 0 &&
                              otpAttemptCount < OTP_MAX_ATTEMPTS
                                ? "#0084FF"
                                : "#9CA3AF",
                            cursor:
                              otpResendTimer === 0 &&
                              otpAttemptCount < OTP_MAX_ATTEMPTS
                                ? "pointer"
                                : "not-allowed",
                            textDecoration:
                              otpResendTimer === 0 &&
                              otpAttemptCount < OTP_MAX_ATTEMPTS
                                ? "underline"
                                : "none",
                          }}
                        >
                          Resend
                        </span>
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            </div>
          )}

          {login.isForgetPassword === "update" && (
            <div className={styles.loginmain}>
              <div
                className={`${styles.loginmainblock} ${styles.loginmainblockstp}`}
              >
                <h2>Create New Password</h2>
                <div className={styles.loginblock}>
                  <div className={styles.inputyourself}>
                    <div
                      className={`${styles.passwordtext} ${styles.inputdiv} ${styles.inputdivmail}`}
                    >
                      <input
                        placeholder="Enter your new password"
                        className={`${styles.textpassword} ${passwordError ? styles.eroreword : ""}`}
                        autoComplete="off"
                        type="password"
                        value={newPassword}
                        onChange={(e) => {
                          setNewPassword(e.target.value);
                          setPasswordError("");
                        }}
                      />
                      <span className={styles.msgicon}>
                        <img src={Lockicon} alt="Messageicon" />
                      </span>
                      {newPassword.length > 0 && (
                        <div
                          className={`${styles.placetold} ${passwordStrengthClass}`}
                        >
                          <span></span>
                          <span></span>
                          <span></span>
                          <span></span>
                        </div>
                      )}
                      {newPassword.length > 0 && (
                        <div className={styles.contented}>
                          {passwordStrengthLabel ? (
                            <span className={passwordStrengthLabelClass}>
                              {passwordStrengthLabel}
                            </span>
                          ) : null}
                        </div>
                      )}
                      {newPassword.length > 0 && passwordStrengthScore < 4 && (
                        <div className={styles.absolutedetails}>
                          <p
                            className={
                              !isNoPersonalInfoRule ? styles.wrong : ""
                            }
                          >
                            {isNoPersonalInfoRule ? (
                              <svg
                                xmlns="http://www.w3.org/2000/svg"
                                width="15"
                                height="14"
                                viewBox="0 0 15 14"
                                fill="none"
                              >
                                <path
                                  d="M7.50049 0.682617C10.9981 0.682793 13.8335 3.519 13.8335 7.0166C13.8333 10.5141 10.998 13.3494 7.50049 13.3496C4.00282 13.3496 1.16668 10.5143 1.1665 7.0166C1.1665 3.51876 4.00265 0.682617 7.50049 0.682617ZM10.7964 4.19434C10.4324 3.91463 9.90827 3.98341 9.62842 4.34766L6.61084 8.27832C6.57989 8.31869 6.53887 8.33986 6.48877 8.34277L6.48389 8.34375C6.43611 8.34717 6.39817 8.33327 6.35791 8.29199L5.30908 7.21094H5.30811C4.98809 6.88246 4.46069 6.87337 4.13037 7.19336C3.79978 7.51363 3.79254 8.04212 4.11279 8.37207L5.1626 9.4541C5.50947 9.81139 5.98474 10.0098 6.479 10.0098C6.51259 10.0098 6.54923 10.0084 6.5874 10.0059C7.11788 9.97426 7.60814 9.71598 7.93213 9.29395L10.9507 5.36328C11.2308 4.998 11.1617 4.47451 10.7964 4.19434Z"
                                  fill="#00B885"
                                  stroke="#00C851"
                                />
                              </svg>
                            ) : (
                              <svg
                                xmlns="http://www.w3.org/2000/svg"
                                width="15"
                                height="14"
                                viewBox="0 0 15 14"
                                fill="none"
                              >
                                <path
                                  d="M7.49983 0.351562C3.81949 0.351562 0.835938 3.33512 0.835938 7.01545C0.835938 10.6958 3.81949 13.6793 7.49983 13.6793C11.1802 13.6793 14.1637 10.6958 14.1637 7.01545C14.1637 3.33512 11.1802 0.351562 7.49983 0.351562ZM10.4206 9.15576C10.6159 9.35101 10.6159 9.66728 10.4206 9.86247C10.323 9.96009 10.1951 10.0089 10.0672 10.0089C9.93935 10.0089 9.81146 9.96009 9.71384 9.86247L7.49989 7.64852L5.28595 9.86247C5.18832 9.96009 5.06044 10.0089 4.93256 10.0089C4.80468 10.0089 4.6768 9.96009 4.57918 9.86247C4.38393 9.66721 4.38393 9.35095 4.57918 9.15576L6.79312 6.94182L4.57918 4.72787C4.38393 4.53262 4.38393 4.21635 4.57918 4.02117C4.77443 3.82598 5.0907 3.82592 5.28588 4.02117L7.49983 6.23511L9.71377 4.02117C9.90902 3.82592 10.2253 3.82592 10.4205 4.02117C10.6157 4.21642 10.6157 4.53269 10.4205 4.72787L8.20653 6.94182L10.4206 9.15576Z"
                                  fill="#FF4444"
                                />
                              </svg>
                            )}
                            Cannot contain your name or e-mail
                          </p>
                          <p className={!isMinLengthRule ? styles.wrong : ""}>
                            {isMinLengthRule ? (
                              <svg
                                xmlns="http://www.w3.org/2000/svg"
                                width="15"
                                height="14"
                                viewBox="0 0 15 14"
                                fill="none"
                              >
                                <path
                                  d="M7.50049 0.682617C10.9981 0.682793 13.8335 3.519 13.8335 7.0166C13.8333 10.5141 10.998 13.3494 7.50049 13.3496C4.00282 13.3496 1.16668 10.5143 1.1665 7.0166C1.1665 3.51876 4.00265 0.682617 7.50049 0.682617ZM10.7964 4.19434C10.4324 3.91463 9.90827 3.98341 9.62842 4.34766L6.61084 8.27832C6.57989 8.31869 6.53887 8.33986 6.48877 8.34277L6.48389 8.34375C6.43611 8.34717 6.39817 8.33327 6.35791 8.29199L5.30908 7.21094H5.30811C4.98809 6.88246 4.46069 6.87337 4.13037 7.19336C3.79978 7.51363 3.79254 8.04212 4.11279 8.37207L5.1626 9.4541C5.50947 9.81139 5.98474 10.0098 6.479 10.0098C6.51259 10.0098 6.54923 10.0084 6.5874 10.0059C7.11788 9.97426 7.60814 9.71598 7.93213 9.29395L10.9507 5.36328C11.2308 4.998 11.1617 4.47451 10.7964 4.19434Z"
                                  fill="#00B885"
                                  stroke="#00C851"
                                />
                              </svg>
                            ) : (
                              <svg
                                xmlns="http://www.w3.org/2000/svg"
                                width="15"
                                height="14"
                                viewBox="0 0 15 14"
                                fill="none"
                              >
                                <path
                                  d="M7.49983 0.351562C3.81949 0.351562 0.835938 3.33512 0.835938 7.01545C0.835938 10.6958 3.81949 13.6793 7.49983 13.6793C11.1802 13.6793 14.1637 10.6958 14.1637 7.01545C14.1637 3.33512 11.1802 0.351562 7.49983 0.351562ZM10.4206 9.15576C10.6159 9.35101 10.6159 9.66728 10.4206 9.86247C10.323 9.96009 10.1951 10.0089 10.0672 10.0089C9.93935 10.0089 9.81146 9.96009 9.71384 9.86247L7.49989 7.64852L5.28595 9.86247C5.18832 9.96009 5.06044 10.0089 4.93256 10.0089C4.80468 10.0089 4.6768 9.96009 4.57918 9.86247C4.38393 9.66721 4.38393 9.35095 4.57918 9.15576L6.79312 6.94182L4.57918 4.72787C4.38393 4.53262 4.38393 4.21635 4.57918 4.02117C4.77443 3.82598 5.0907 3.82592 5.28588 4.02117L7.49983 6.23511L9.71377 4.02117C9.90902 3.82592 10.2253 3.82592 10.4205 4.02117C10.6157 4.21642 10.6157 4.53269 10.4205 4.72787L8.20653 6.94182L10.4206 9.15576Z"
                                  fill="#FF4444"
                                />
                              </svg>
                            )}
                            At least 8 characters
                          </p>
                          <p
                            className={
                              !isNumberOrSymbolRule ? styles.wrong : ""
                            }
                          >
                            {isNumberOrSymbolRule ? (
                              <svg
                                xmlns="http://www.w3.org/2000/svg"
                                width="15"
                                height="14"
                                viewBox="0 0 15 14"
                                fill="none"
                              >
                                <path
                                  d="M7.50049 0.682617C10.9981 0.682793 13.8335 3.519 13.8335 7.0166C13.8333 10.5141 10.998 13.3494 7.50049 13.3496C4.00282 13.3496 1.16668 10.5143 1.1665 7.0166C1.1665 3.51876 4.00265 0.682617 7.50049 0.682617ZM10.7964 4.19434C10.4324 3.91463 9.90827 3.98341 9.62842 4.34766L6.61084 8.27832C6.57989 8.31869 6.53887 8.33986 6.48877 8.34277L6.48389 8.34375C6.43611 8.34717 6.39817 8.33327 6.35791 8.29199L5.30908 7.21094H5.30811C4.98809 6.88246 4.46069 6.87337 4.13037 7.19336C3.79978 7.51363 3.79254 8.04212 4.11279 8.37207L5.1626 9.4541C5.50947 9.81139 5.98474 10.0098 6.479 10.0098C6.51259 10.0098 6.54923 10.0084 6.5874 10.0059C7.11788 9.97426 7.60814 9.71598 7.93213 9.29395L10.9507 5.36328C11.2308 4.998 11.1617 4.47451 10.7964 4.19434Z"
                                  fill="#00B885"
                                  stroke="#00C851"
                                />
                              </svg>
                              
                            ) : (
                              <svg
                                xmlns="http://www.w3.org/2000/svg"
                                width="15"
                                height="14"
                                viewBox="0 0 15 14"
                                fill="none"
                              >
                                <path
                                  d="M7.49983 0.351562C3.81949 0.351562 0.835938 3.33512 0.835938 7.01545C0.835938 10.6958 3.81949 13.6793 7.49983 13.6793C11.1802 13.6793 14.1637 10.6958 14.1637 7.01545C14.1637 3.33512 11.1802 0.351562 7.49983 0.351562ZM10.4206 9.15576C10.6159 9.35101 10.6159 9.66728 10.4206 9.86247C10.323 9.96009 10.1951 10.0089 10.0672 10.0089C9.93935 10.0089 9.81146 9.96009 9.71384 9.86247L7.49989 7.64852L5.28595 9.86247C5.18832 9.96009 5.06044 10.0089 4.93256 10.0089C4.80468 10.0089 4.6768 9.96009 4.57918 9.86247C4.38393 9.66721 4.38393 9.35095 4.57918 9.15576L6.79312 6.94182L4.57918 4.72787C4.38393 4.53262 4.38393 4.21635 4.57918 4.02117C4.77443 3.82598 5.0907 3.82592 5.28588 4.02117L7.49983 6.23511L9.71377 4.02117C9.90902 3.82592 10.2253 3.82592 10.4205 4.02117C10.6157 4.21642 10.6157 4.53269 10.4205 4.72787L8.20653 6.94182L10.4206 9.15576Z"
                                  fill="#FF4444"
                                />
                              </svg>
                            )}
                            Contains a number or symbol
                          </p>
                        </div>
                      )}
                    </div>
                    <div
                      className={`${styles.passwordtextverify} ${styles.inputdiv} ${styles.inputdivmail}`}
                    >
                      <input
                        placeholder="Confirm your password"
                        className={styles.textpassword}
                        autoComplete="off"
                        type={showConfirmPassword ? "text" : "password"}
                        value={confirmPassword}
                        onChange={(e) => {
                          setConfirmPassword(e.target.value);
                          setPasswordError("");
                        }}
                      />
                      <span className={styles.msgicon}>
                        <img src={Lockicon} alt="Messageicon" />
                      </span>
                      <span
                        onClick={() => setShowConfirmPassword((prev) => !prev)}
                        className={styles.showClear}
                      >
                        {!showConfirmPassword ? (
                          <div className={styles.openeye}>
                          <svg
                            width="24"
                            height="24"
                            viewBox="0 0 24 24"
                            fill="none"
                            xmlns="http://www.w3.org/2000/svg"
                          >
                            <g clipPath="url(#clip0_confirm_eye_open)">
                              <path
                                d="M12 7.29356C15.4703 7.29356 18.6418 9.05206 20.4916 12.0001C18.6418 14.9481 15.4704 16.7066 12 16.7066C8.52965 16.7066 5.35805 14.9478 3.50845 12.0003C5.35805 9.05226 8.52955 7.29356 12 7.29356ZM12 5.79346C7.86775 5.79346 4.24915 7.97146 2.21525 11.2391C1.92825 11.7 1.92825 12.3001 2.21525 12.7613C4.24915 16.0286 7.86775 18.2066 12 18.2066C16.1323 18.2066 19.7507 16.0286 21.7847 12.7613C22.0718 12.3001 22.0718 11.7 21.7847 11.2391C19.7509 7.97146 16.1321 5.79346 12 5.79346Z"
                                fill="#AAABAD"
                              />
                              <path
                                d="M12 14.7961C13.5441 14.7961 14.7959 13.5443 14.7959 12.0002C14.7959 10.456 13.5441 9.20426 12 9.20426C10.4558 9.20426 9.20405 10.456 9.20405 12.0002C9.20405 13.5443 10.4558 14.7961 12 14.7961Z"
                                fill="#AAABAD"
                              />
                            </g>
                            <defs>
                              <clipPath id="clip0_confirm_eye_open">
                                <rect width="24" height="24" fill="white" />
                              </clipPath>
                            </defs>
                          </svg>
                          </div>
                        ) : (
                          <div className={styles.closeeye}> 
                          <svg
                            width="24"
                            height="24"
                            viewBox="0 0 24 24"
                            fill="none"
                            xmlns="http://www.w3.org/2000/svg"
                          >
                            <g clipPath="url(#clip0_confirm_eye_hide)">
                              <path
                                d="M11.995 7.29397C15.465 7.29397 18.645 9.05377 20.485 12.004C19.705 13.244 18.695 14.274 17.525 15.044L18.595 16.1138C19.875 15.2339 20.955 14.084 21.785 12.7639C22.075 12.3037 22.075 11.7038 21.785 11.2439C19.745 7.97387 16.135 5.79387 11.995 5.79387C10.875 5.79387 9.79504 5.95377 8.77504 6.26387L9.99504 7.49387C10.645 7.36377 11.315 7.29397 11.995 7.29397Z"
                                fill="#AAABAD"
                              />
                              <path
                                d="M11.5249 9.02397L14.9749 12.484C15.0049 12.3238 15.0149 12.1639 15.0149 12.004C15.0149 10.3341 13.6649 8.98397 11.9949 8.98397C11.8349 8.98397 11.6749 8.99397 11.5249 9.02397Z"
                                fill="#AAABAD"
                              />
                              <path
                                d="M3.32464 2.22757C3.03214 1.93367 2.55754 1.93367 2.26414 2.22557C1.97074 2.51747 1.96974 2.99217 2.26214 3.28607L6.27564 7.31637C4.62444 8.26607 3.23084 9.62037 2.21494 11.2439C1.92494 11.7039 1.92494 12.3037 2.21494 12.7639C4.24494 16.0239 7.86494 18.2038 11.9949 18.2038C13.5183 18.2038 14.9622 17.9155 16.2875 17.3701L20.6717 21.7724C20.8182 21.9199 21.0106 21.9931 21.2029 21.9931C21.3943 21.9931 21.5857 21.9199 21.7322 21.7743C22.0257 21.4823 22.0266 21.0077 21.7342 20.7138L3.32464 2.22757ZM10.5408 11.5994L12.3916 13.4578C12.2678 13.502 12.1367 13.524 11.995 13.524C11.165 13.524 10.475 12.8341 10.475 12.004C10.475 11.8629 10.4971 11.7312 10.5408 11.5994ZM11.995 16.7039C8.52504 16.7039 5.35504 14.9439 3.50514 12.0039C4.48144 10.4495 5.81604 9.22407 7.37854 8.42377L9.40644 10.4599C9.13244 10.9067 8.97504 11.4306 8.97504 12.0038C8.97504 13.6637 10.335 15.0238 11.995 15.0238C12.5591 15.0238 13.0835 14.8661 13.5304 14.6012L15.1304 16.2079C14.1346 16.5348 13.0797 16.7039 11.995 16.7039Z"
                                fill="#AAABAD"
                              />
                            </g>
                            <defs>
                              <clipPath id="clip0_confirm_eye_hide">
                                <rect width="24" height="24" fill="white" />
                              </clipPath>
                            </defs>
                          </svg>
                          </div>
                        )}
                      </span>
                    </div>
                  </div>
                  {passwordError ? (
                    <span className={styles.errorText}>{passwordError}</span>
                  ) : null}
                  <div className={styles.loginbutton}>
                    <div
                      className={`${styles.btnSignin}`}
                      onClick={onUpdatePassword}
                    >
                      {passwordLoading ? "Updating..." : "Create password"}
                    </div>
                    <div className={styles.forgot}>
                      Once you've set your new password, you can use it to log in and reset it anytime in the settings.
                    </div>
                  </div>
                </div>
              </div>
            </div>
          )}
        </>
      )}
    </>
  );
};


export const ConnectSockets = (accessToken :any, navigate :any, dispatch :any) => {
  new aiChatAssistSocketservice(accessToken, navigate, dispatch);
  new inboxAiChatAssistSocketservice(accessToken, navigate, dispatch);
  new presenceService(accessToken, navigate, dispatch, 0);
  new socketService(accessToken, navigate, dispatch);
}
export default Login;
