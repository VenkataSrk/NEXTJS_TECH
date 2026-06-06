'use client';

import { useEffect, useRef, useState } from "react";
import styles from "./loginnew.module.scss";


// import { useRouter } from "next/navigation";
// import dynamic from "next/dynamic";
// import NextAuth from "next-auth";
// import GoogleProvider from "next-auth/providers/google";
// import { signIn, signOut, useSession } from "next-auth/react";
// import { API_ROUTER_CONTROLLER, API_VERBS, otherApiservice, websiteService } from "../../../servcies/apiservice";
// import { SetItemInStorage } from "../../../utils/useStorage";
// import { LocalStorage } from "../../../utils/localStorageKeys";
// import { apiMyaccountURL } from "../../../utils/config";
// import { assets } from "../../styles/assets/assets";
// import { SetItemInStorage } from "../../custom_hooks/useStorage";
// import { ApiServices } from "../../service/apiServices";
// import { LoginService } from "../../service/loginService";
// import { loginConfig } from "./LoginConstant";
// import { v4 as uuidv4 } from "uuid";
// import { LocalStorage } from "../../custom_hooks/localStorageKeys";
// import { useNavigate } from "react-router-dom";
// import { LoginSocialApple, LoginSocialGoogle, LoginSocialMicrosoft } from "reactjs-social-login";
// import { jwtDecode } from "jwt-decode";
// import { cvm_user_login } from "../../apiservice/aiLycaPilot";



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
        validationError: String;
    };
    isForgetPassword: string;
    isChattour: number;
};

const Login = () => {
    // const navigate = useNavigate();

    // const [loader, setLoader] = useState<any>(false)
    // let uuidNew: any = uuidv4();

    // const [login, setLogin] = useState<loginState>({
    //     email: "",
    //     password: "",
    //     remember: false,
    //     showPass: false,
    //     togglePass: false,
    //     LoggedIn: false,
    //     errors: false,
    //     errorMessage: {
    //         emailError: "",
    //         passWordError: "",
    //         validationError: "",
    //     },
    //     isForgetPassword: "login",
    //     isChattour: 0,
    // });
    // const isSocialMedia = useRef(false);

    // const onChangeInput = (e: any) => {
    //     setLogin({ ...login, [e.target.name]: e.target.value, errors: false });
    //     setErrors({ ...error, email: false, password: false, wholeError: false });
    // };



    // const [loading, setLoading] = useState<boolean>(false);
    // const [error, setErrors] = useState<any>({
    //     email: false,
    //     password: false,
    //     wholeError: false,
    // });


    // const clearEmail = () => {
    //     setLogin({ ...login, email: "" });
    // };

    // const togglePassword = () => {
    //     setLogin({ ...login, togglePass: !login.togglePass });
    // };


    // const [countryCode, setCountryCode] = useState('');
    // useEffect(() => {
    //     if (typeof window !== 'undefined') {
    //         const hostname = window.location.hostname;
    //         const parts = hostname.split('.');
    //         setCountryCode(parts.length > 2 ? parts[0] : 'default');
    //     }
    // }, []);


    //   const getSignData = async (data: any, type: any) => {
    //     console.log("dataaa", data, type)
    //     try {
    //       let body = {
    //         email: data,
    //         device_type: "",
    //         browser: "",
    //         login_type: type
    //       }
    //       otherApiservice.post(API_ROUTER_CONTROLLER.socialmediaLogin, body)
    //         .then(async (response: any) => {
    //           const result = response?.data?.results?.[0];
    //           if (result?.v_errcode === 0) {
    //             const email = result.my_acc_email;
    //             const password = result.my_acc_password;
    //             await signOut({ redirect: false });
    //           } else if (result?.v_errcode === -1) {
    //             setLoader(false);
    //             alert(result.v_errmsg);
    //           }
    //         }).catch((error: any) => { console.log(error, "error") });


    //     } catch (err) {
    //       console.error("SocialMediaLogin error", err);
    //       return null;
    //     }
    //   };



    // const SignIn = async () => {
    //     console.log("sign>>>>", login)
    //     const emailRegx = /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
    //     setErrors({});
    //     if (login.password === "" && login.email === "") {
    //         setErrors({ wholeError: true });
    //         setLogin({
    //             ...login,
    //             errorMessage: {
    //                 ...login.errorMessage,
    //                 validationError: loginConfig().BLANK_VALIDATION,
    //             },
    //         });
    //         return;
    //     }
    //     if (!login.email || !emailRegx.test(login.email)) {
    //         setErrors({ email: true });
    //         setLogin((prev: any) => ({
    //             ...prev,
    //             errorMessage: {
    //                 emailError: !login.email
    //                     ? loginConfig().EMAIL_BLANK
    //                     : loginConfig().INVLID_EMAIL,
    //             },
    //         }));
    //         return;
    //     }

    //     if (login.password === "") {
    //         setErrors({ password: true });
    //         setLogin({
    //             ...login,
    //             errorMessage: {
    //                 ...login.errorMessage,
    //                 passWordError: loginConfig().BLANK_PASSWORD,
    //             },
    //         });
    //         return;
    //     }
    //     try {
    //         let userObject = {
    //             username: login.email.trim(),
    //             password: login.password.trim(),
    //             source: "Web",
    //             ipAddress: "136.144.56.255:443",
    //             isForceLogin: true,
    //             deviceId: uuidNew,
    //         };
    //         console.log("userObject", userObject)
    //         let LoginUserDetails: any = {};
    //         try {
    //             await LoginService.login(userObject).then(async (value: any) => {
    //                 if (value && value?.data?.statusCode === 200 && value?.data?.is_cas_user === 1) {
    //                     SetItemInStorage(LocalStorage.ACCESS_TOKEN, value.data.token);
    //                     try {
    //                         // const response = await cvm_user_login({ email_id: login.email, is_login: 1 });
    //                         // console.log('login success', response);
    //                         // navigate('/aiPilotAnimation');
    //                     } catch (err) {
    //                         console.error('login failed', err);
    //                     }
    //                     // navigate('/aiPilot')
    //                     navigate('/aiPilotAnimation')
    //                 } else if (value && value?.data?.statusCode === 301) {
    //                     if (value?.data.message) {
    //                         setLoading(false);
    //                         setLogin({
    //                             ...login,
    //                             errorMessage: {
    //                                 ...login.errorMessage,
    //                                 validationError: loginConfig().COMMON_ERROR,
    //                             },
    //                         });
    //                         setErrors({ wholeError: true });

    //                         sessionStorage.clear();
    //                         window.location.reload();
    //                     }
    //                 } else if (value && value?.data?.statusCode === 424) {
    //                     setLogin({ ...login, LoggedIn: true });
    //                 } else if (value) {
    //                     setLoading(false);
    //                     setLogin({
    //                         ...login,
    //                         errorMessage: {
    //                             ...login.errorMessage,
    //                             validationError: value?.data?.message || loginConfig().COMMON_ERROR,
    //                         },
    //                     });
    //                     setErrors({ wholeError: true });
    //                 } else {
    //                     setLoading(false);
    //                     setLogin({
    //                         ...login,
    //                         errorMessage: {
    //                             ...login.errorMessage,
    //                             validationError: value?.data?.message || loginConfig().AUTHENTICATION_FAILED,
    //                         },
    //                     });
    //                     setErrors({ wholeError: true });
    //                 }
    //             }).catch((error: any) => { console.log(error, "error") });




    //         } catch (error) {
    //             console.log('login error>>>>>>>>>>>', error)
    //         }


    //     } catch (error) {
    //         console.log('login error>>>>>>>>>>>', error)
    //     }
    // };


    //   useEffect(() => {

    //     const processSession = async () => {

    //       if (session?.user?.email) {
    //         console.log("User email:", session.user.email);
    //         await getSignData(session.user.email, "google");
    //         await signOut({ redirect: false });
    //       }
    //     };

    //     processSession();
    //   }, [session]);

    // const redirectToWorktual = () => {
    //     console.log("redirect ---------------");
    //     window.open('https://worktual.co.uk/pricing/#Ccaas');

    // };


    // const socialMediaLogin = async (data: any, type: any) => {
    //     const _body = {
    //         email: data,
    //         device_type: "", // You can enhance this with UA-parser
    //         browser: "",     // Or use navigator.userAgent
    //         login_type: type
    //     };

    //     try {
    //         const response = await LoginService.socialMediaLogin(_body);
    //         if (response?.data?.results[0].v_errcode == 0) {
    //             let email = response?.data?.results[0]?.my_acc_email;
    //             let password = response?.data?.results[0]?.my_acc_password;
    //             isSocialMedia.current = true;
    //             const updatedLogin = {
    //                 ...login,
    //                 email,
    //                 password,
    //                 errors: false,
    //             };
    //             setLogin(updatedLogin);
    //             setErrors({ ...error, email: false, password: false, wholeError: false });
    //         } else if (response?.data?.results[0]?.v_errcode == -1) {
    //             setLoader(false);
    //             alert(response?.data?.results[0]?.v_errmsg);
    //         }
    //     } catch (err) {
    //         console.error("SocialMediaLogin error", err);
    //         return null;
    //     }
    // };

    return (
        <>
            <div className="loginwhitepage">
                <div className={styles.loginheader}>
                    <div className={styles.headerview}>
                        {/* <img src={assets.worktual_logo} alt='worktual_logo' /> */}
                    </div>
                    <button style={{ borderRadius: '6px' }} className={`${styles.contactsign}`}>
                        <a
                            className={`${styles.forgot} `}
                            
                        >
                            Contact Us
                        </a>
                    </button>
                </div>
          
                    <div className={styles.body}>
                        <div className={styles.centeredcontent}>
                            <div className={styles.lucasmain}>
                                <h2 className={styles.inputopmanerh}>Log in to Your Account</h2>
                                <p className={styles.inputopmaner}>to access CVM</p>
                                <div className={styles.inputopmanerprimary}>
                                    {/* <button
                                        className={styles.google}
                                        onClick={() => { }
                                            // signIn('google', { callbackUrl: '/' })
                                        }
                                    >
                                        <img src={assets.google_icon} alt='google_icon' />
                                        Continue with Google
                                    </button>

                                    <button
                                        className={styles.appleicon}
                                        onClick={() => { }
                                            // signIn('azure-ad', { callbackUrl: '/' })
                                        }
                                    >
                                        <img src={assets.microsoft_icon} alt='microsoft_icon' />
                                        Continue with Microsoft
                                    </button>

                                    <button
                                        className={styles.microsofticon}
                                        onClick={() => { }
                                            //  signIn('apple', { callbackUrl: '/' })
                                        }
                                    >
                                        <img src={assets.apple_icon} alt='apple_icon' />
                                        Continue with Apple
                                    </button> */}
                                    {/* <LoginSocialGoogle
                                        client_id="672189196887-a63iclna3qh1145iousgui9ks1adgt1p.apps.googleusercontent.com"
                                        scope="https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/userinfo.profile"
                                        redirect_uri="https://qaccaas.worktual.co.uk/"
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
                                                    data
                                                );
                                            }
                                        }}
                                        onReject={(err) => {
                                            console.log("Error during authentication", err);
                                        }}
                                    >
                                        <button className={styles.google}>
                                            <img src={assets.google_icon} alt="appleicon" /> {" "}
                                            Continue with Google
                                        </button>
                                    </LoginSocialGoogle> */}
                                    {/* <button className={styles.google}> <img src={Google} alt="appleicon" /> Continue with Google */}

                                    {/* </button> */}
                                    {/* <button className={styles.appleicon}> <img src={Microsoft} alt="appleicon" /> Continue with Microsoft</button> */}
                                    {/* <button className={styles.microsofticon}> <img src={Apple} alt="microsofticon" /> Continue with Apple</button> */}
                                    {/* <LoginSocialMicrosoft

                                        client_id="651c69df-eea7-4624-a1cf-cecdf82ec80e"
                                        redirect_uri="https://qaccaas.worktual.co.uk/"
                                        onResolve={({ provider, data }: any) => {
                                            console.log("data", data)
                                            console.log("provider", provider)
                                            const decodedToken: any = jwtDecode(data.access_token);
                                            console.log("decoded token", decodedToken)
                                            data.social_account_email = decodedToken.email;

                                            data.social_account_email = data.mail ? data.mail : decodedToken.email ? decodedToken.email : decodedToken.unique_name;
                                            data.social_account_type = "microsoft";

                                            socialMediaLogin(data.social_account_email, "Microsoft");;
                                        }}
                                        onReject={(err) => {
                                            console.log("iiiiiiiiii", err);
                                        }}
                                    >
                                        <button className={styles.appleicon}>
                                            <img src={assets.microsoft_icon} alt="microsoft icon" />
                                            Continue with Microsoft
                                        </button>

                                    </LoginSocialMicrosoft> */}


{/* 
                                    <LoginSocialApple
                                        client_id="com.worktual.login"
                                        redirect_uri="https://qaccaas.worktual.co.uk/"
                                        onResolve={async ({ provider, data }: any) => {
                                            console.log("data", data)
                                            const decodedToken: any = jwtDecode(data.authorization.id_token);
                                            console.log("decoded token", decodedToken)
                                            data.social_account_email = decodedToken.email;
                                            data.social_account_type = "Apple";
                                            console.log("provider", provider)

                                            const email = data.social_account_email || decodedToken?.email

                                            console.log("Final Apple login object:", email);

                                            await socialMediaLogin(email, "Apple");
                                        }}
                                        onReject={err => {
                                            console.log(err);
                                        }}
                                    >
                                        <button className={styles.microsofticon}> <img src={assets.apple_icon} alt="microsofticon" /> Continue with Apple</button>
                                    </LoginSocialApple> */}
                                </div>
                                <div className={styles.loginid}>
                                    <p>or</p>
                                </div>
                                <div className={styles.inputdiv}>
                                    <input
                                        placeholder="Enter your E-mail"
                                        // className={styles.emailaddress}
                                   
                                        maxLength={250}
                                        name="email"
                                       
                                        autoFocus={true}
                                    />
                                  
                                 
                                </div>
                                <div className={styles.inputdiv}>
                                    <input
                                        placeholder="Enter your password"
                                        // className={styles.emailaddresslock}   
                                     
                                        name="password"
                                        maxLength={250}
                                       
                                    // onKeyDown={(e: any) => handleEnter(e)}
                                    />
                               
                                        <span  >
                                          
                                                <svg
                                                    className="details_eyeicon__2Y-h8 details_eyeiconpassword__1ggc1"
                                                    xmlns="http://www.w3.org/2000/svg"
                                                    width="20"
                                                    height="13"
                                                    viewBox="0 0 20 13"
                                                    fill="none"
                                                >
                                                    <path
                                                        d="M10 1.79307C13.4703 1.79307 16.6418 3.55157 18.4916 6.49957C16.6418 9.44757 13.4704 11.2061 10 11.2061C6.52965 11.2061 3.35805 9.44727 1.50845 6.49977C3.35805 3.55177 6.52955 1.79307 10 1.79307ZM10 0.292969C5.86775 0.292969 2.24915 2.47097 0.21525 5.73857C-0.07175 6.19947 -0.07175 6.79957 0.21525 7.26077C2.24915 10.5281 5.86775 12.7061 10 12.7061C14.1323 12.7061 17.7507 10.5281 19.7847 7.26077C20.0718 6.79957 20.0718 6.19947 19.7847 5.73857C17.7509 2.47097 14.1321 0.292969 10 0.292969Z"
                                                        fill="#788073"
                                                    />
                                                    <path
                                                        d="M9.99995 9.29557C11.5441 9.29557 12.7959 8.0438 12.7959 6.49967C12.7959 4.95553 11.5441 3.70377 9.99995 3.70377C8.45582 3.70377 7.20405 4.95553 7.20405 6.49967C7.20405 8.0438 8.45582 9.29557 9.99995 9.29557Z"
                                                        fill="#788073"
                                                    />
                                                </svg>
                                           
                                        </span>

                            

                                
                                </div>
                                <div className={styles.lucasmaininput}>
                               <span className={`${styles.errorText} ${styles.valer}`}></span>
                                
                                        <button  className={`${styles.btnSignin}  "disabled" : ""}`}>
                                            Login
                                        </button>
                                        :
                                        <div className='loadersetactive'>
                                            {/* <Loading /> */}
                                        </div>
                                    
                                    <p>Don't have a Worktual account yet?
                                        <span
                                         
                                        > Contact Sales</span></p>
                                    <div className={styles.lucaslogin}>
                                        <p onClick={() => { }}><span>Forgot Password?</span></p>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>
            
                {/* {login.isForgetPassword === "forget" && (
              <Container fluid className={styles.mainloginscreen}>
                <Row className={styles.logincenter}>
                  <Col sm={7} className={styles.logincenternew}>
                    {" "}
                    <img src={LoginImage} alt="Login" />
                  </Col>
                  <Col sm={5} className={styles.logincenternewright}>
                    <Row className={`${styles.innersections} ${styles.loginright}`}>
                      <Col sm={12}>
                        <h4>Forgot Password</h4>
                        <p>Enter your email address and we’ll send you a link to reset your password</p>
                        <div className={`${styles.loginborder} ${styles.loginborderforgot}`}>
                          <ForgotPassword updateforgetPassword={updateforgetPassword} email={login.email} />
                        </div>
                      </Col>
                    </Row>
                  </Col>
                </Row>
              </Container>
            )}
            {login.isForgetPassword === "reSet" && (
              <Container fluid className={styles.mainloginscreen}>
                <Row className={styles.logincenter}>
                  <Col sm={12}>
                    <Row className={`${styles.innersections} ${styles.loginright} ${styles.resetSucess}`}>
                      <Col sm={12}>
                        <ResetLink updateforgetPassword={updateforgetPassword} email={login.email} />
                      </Col>
                    </Row>
                  </Col>
                </Row>
              </Container>
            )} */}
            </div>
        </>
    );
};
export default Login;