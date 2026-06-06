import { useEffect, useState } from "react";
import styles from "../../../assets/styles/accountinfo.module.scss";
import CustomSwitch from "../../../components/CustomSwitch";
import Apple from '../../../assets/images/myplan/apple.svg'
import Microsoft from '../../../assets/images/myplan/microsoft.svg'
import { getSecurityInfo, getSyncUpMyaccountDetails, getUserExtensionInfo, insertSocialLogin } from "../../../apiservice/myplanapi";
// import { GetItemFromStorage } from "../../../../../../custom_hooks/useStorage";
// import { LocalStorage } from "../../../../../../custom_hooks/localStorageKeys";
import moment from "moment";
import { LoginSocialApple, LoginSocialFacebook, LoginSocialGoogle, LoginSocialMicrosoft } from "reactjs-social-login";
// import jwtDecode from "jwt-decode";
import { jwtDecode } from "jwt-decode";
import axios from "axios";
import { GetItemFromStorage } from "../../../../base/customhooks/useStorage";
import { LocalStorage } from "../../../../base/customhooks/localStorageKeys";

export default function AccountSecurity() {
    const userDetails = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN)) ;
    const [securityInfo, setSecurityInfo] = useState<any>([])
    console.log("securityinfo>>>",securityInfo)
    const [ipAddress, setIpAddress] = useState('');
    const [syncedMyAccount, setSyncedMyAccount] = useState<any[]>([]);
    console.log('syncedMyAccount>>>>>>>>>>>', syncedMyAccount)
    const [profileData,setProfileData]=useState<any>([]);
    console.log("profileData",profileData);
    const twoStepVerify = async () => {
        try {
            if (userDetails) {
                const body = {
                    company_id: userDetails.company_id,
                    orderid: userDetails.order_id,
                    extension_number: Number(userDetails.ext)
                }

                getUserExtensionInfo(body)
                    .then((res: any) => {
                        // console.log("API RESPONSE ---->", res);
                        setProfileData(res.data.finalresponse[0]);  // store in state
                    })
                    .catch((err: any) => console.log(err));
            }
            return () => {
                console.log("cleanup");
            };
        } catch (error) {

        }
    }

    useEffect(() => {
        const fetchSecurityInfo = async () => {
            try {
                const securityList = await getSecurityInfo();
                console.log("seclist",securityList)
                setSecurityInfo(securityList);
            } catch (error) {
                console.error("Error fetching securityInfo list:", error);
            }
        };
        twoStepVerify();
        fetchSecurityInfo();
        getUserData();
        getSyncUpMyaccountFunction();
    }, []);

    const getConnectionStatus = (type: string) => {
        console.log('dsdasdasdasdas', type)
        if (!Array.isArray(syncedMyAccount)) return false;
        return syncedMyAccount.some(
            (account: any) =>
                account.social_account_type?.toLowerCase() === type?.toLowerCase()
        );
    };


    const getSyncUpMyaccountFunction = async () => {
        const payload = {
            extension: userDetails?.ext
        }
        const getSyncUpMyaccountData = await getSyncUpMyaccountDetails(payload);
        if (getSyncUpMyaccountData?.length > 0) {
            setSyncedMyAccount(getSyncUpMyaccountData);
        } else {
            setSyncedMyAccount([]);
        }
    }

    const getUserData = async () => {
        const res: any = await axios.get('https://getip.worktual.co.uk/');
        setIpAddress(res?.data?.clientIPv6);
    }

    const socialMediaLogin = async (email: any, type: any, data: any) => {

        const addresses: any = await fetch("https://people.googleapis.com/v1/people/me?personFields=names,emailAddresses,addresses", {
            headers: {
                Authorization: `Bearer ${data.access_token}`,
            },
        })
            .then((res) => res.json())
            .then((personInfo) => {
                return personInfo?.addresses?.[0]?.formattedValue;
            })
            .catch((err) => console.error("Failed to fetch address:", err));

        const payload = {
            myaccount_email: userDetails?.Email,
            social_account_email: email,
            processtype: 1,
            social_account_type: type,
            login_source: userDetails?.source,
            login_device_id: userDetails?.deviceId,
            login_ipaddress: ipAddress,
            extension: userDetails?.ext,
            address: addresses,

        }
        const response = await insertSocialLogin(payload)
        if (response) {
            getSyncUpMyaccountFunction()
        }
    }

    const handleDisConnect = async (type: any) => {
        const data = syncedMyAccount?.filter((item: any) => item?.social_account_type == type)
        console.log('data>SDFSDFDS>>>', data)

        const payload = {
            myaccount_email: userDetails?.Email,
            social_account_email: data[0]?.social_account_email,
            processtype: 3,
            social_account_type: type,
            extension: userDetails?.ext,
        }
        console.log('payload>>>>>>', payload)

        const response = await insertSocialLogin(payload);
        if (response) {
            await getSyncUpMyaccountFunction();
            getConnectionStatus(type)
        }

    }



    return (
        <div className={styles.yourinfo}>
            <div className={`${styles.titlearea} ${styles.first}`}>
                <div className={styles.title}>Security</div>
                <div className={styles.desc}>Settings and recommendations to help you keep your account secure</div>
            </div>

            <div className={styles.titlearea}>
                <div className={styles.title}>Password</div>
                <div className={styles.desc}>A secure password helps protect your Google Account</div>
            </div>

            <div className={styles.password}>
                <div className={styles.passwordleft}>
                    <div className={styles.passwordinpuw}>
                        <p>Password</p>
                    </div>

                    {/* <div className={styles.passwordinput}>
                        <input type="password" name="" />
                    </div> */}
                    <div className={styles.passwordinpuw}>
                        <span>{securityInfo?.password ? '*'.repeat(securityInfo.password.length) : ''}</span>
                        <p className={styles.passwordchan}>Last changed {moment(securityInfo?.last_password_changed).format("MMM D, YYYY")}</p>
                    </div>
                </div>
                {/* <div className={styles.passwordright}>
                    <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                        <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#404040" />
                    </svg>
                </div> */}
            </div>

            <div className={styles.titlearea}>
                <div className={styles.title}>Two-step verification</div>
                <div className={styles.desc}>Prevent hackers from accessing your account with an additional layer of security.</div>
            </div>
                <div className={`${styles.fieldareamain} ${styles.verifywithblock}`}>
                    <div>
                       <div className={styles.label}>Verify with email</div>
                    <div className={styles.value}>{`${profileData?.Email}(always available)`}</div>
                    </div>
               
                    <div className={styles.icon}>
                        {/* <div className={styles.recom}>Recommended</div> */}
                        <CustomSwitch
                            checked={true}
                            label="Enable Notifications"
                            onChange={(val: any) => console.log('Switch toggled:', val)}
                        />
                    </div>
                </div>
                  <div className={`${styles.fieldareamain} ${styles.verifywithblock}`}>
                    <div>
                       <div className={styles.label}>Verify with text message (SMS)</div>
                    <div className={styles.value}>{profileData?.Mobileno}</div>
                    </div>
               
                    <div className={styles.icon}>
                        {/* <div className={styles.recom}>Recommended</div> */}
                         <CustomSwitch
                            checked={true}
                            label="Enable Notifications"
                            onChange={(val: any) => console.log('Switch toggled:', val)}
                        />
                    </div>
                </div>
            {/* <div className={`${styles.yourinfotop} ${styles.workaccessapp}`}>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Sign in with Worktual access app</div>
                    <div className={styles.value}>Not set up</div>
                    <div className={styles.icon}>
                        <CustomSwitch
                            checked={true}
                            label="Enable Notifications"
                            onChange={(val: any) => console.log('Switch toggled:', val)}
                        />
                    </div>
                </div>
                
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Verify with email</div>
                    <div className={styles.value}>ithelp@soldsolutins.com (always available)</div>
                    <div className={styles.icon}>
                        <div className={styles.recom}>Recommended</div>
                        <CustomSwitch
                            checked={true}
                            label="Enable Notifications"
                            onChange={(val: any) => console.log('Switch toggled:', val)}
                        />
                    </div>
                </div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Verify with text message (SMS)</div>
                    <div className={styles.value}>+91 96001 16707</div>
                    <div className={styles.icon}>
                        <CustomSwitch
                            checked={true}
                            label="Enable Notifications"
                            onChange={(val: any) => console.log('Switch toggled:', val)}
                        />
                    </div>
                </div>
            </div> */}

            <div className={styles.titlearea}>
                <div className={styles.title}>Social Sign-in</div>
                <div className={styles.desc}>Connect your Adobe account with your social media accounts for simple and secure sign-in. Social sign-in skips two-step verification and may reduce your account’s security.</div>
            </div>

            <div className={styles.yourinfotop}>
                <div className={`${styles.fieldareamain} ${styles.fieldconnect}`}>
                    <div className={styles.label}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                            <path d="M17.825 7.23735H17.1V7.2H9V10.8H14.0864C13.3443 12.8957 11.3504 14.4 9 14.4C6.01785 14.4 3.6 11.9822 3.6 9C3.6 6.01785 6.01785 3.6 9 3.6C10.3765 3.6 11.6289 4.1193 12.5825 4.96755L15.1281 2.4219C13.5207 0.92385 11.3706 0 9 0C4.02975 0 0 4.02975 0 9C0 13.9703 4.02975 18 9 18C13.9703 18 18 13.9703 18 9C18 8.39655 17.9379 7.8075 17.825 7.23735Z" fill="#FFC107" />
                            <path d="M1.03763 4.81095L3.99458 6.9795C4.79468 4.9986 6.73245 3.6 9 3.6C10.3765 3.6 11.6289 4.1193 12.5825 4.96755L15.1281 2.4219C13.5207 0.92385 11.3706 0 9 0C5.5431 0 2.54513 1.95165 1.03763 4.81095Z" fill="#FF3D00" />
                            <path d="M9 18C11.3247 18 13.4371 17.1098 15.0341 15.663L12.2486 13.3059C11.345 13.9904 10.2218 14.4 9 14.4C6.6591 14.4 4.67151 12.9068 3.92271 10.8237L0.987808 13.085C2.47731 15.9996 5.50215 18 9 18Z" fill="#4CAF50" />
                            <path d="M17.8248 7.23559H17.0999V7.19824H8.99986V10.7982H14.0862C13.7298 11.8049 13.0823 12.6729 12.2471 13.3052L12.2484 13.3043L15.0339 15.6614C14.8368 15.8405 17.9999 13.4982 17.9999 8.99824C17.9999 8.39479 17.9378 7.80574 17.8248 7.23559Z" fill="#1976D2" />
                        </svg>Google</div>
                        <div className={`${styles.badge} ${styles.blue}`}>
                            connected
                        </div>
                    <div className={styles.icon}>
                        {!getConnectionStatus("Google") ? (
                            <LoginSocialGoogle
                                client_id="672189196887-a63iclna3qh1145iousgui9ks1adgt1p.apps.googleusercontent.com"
                                scope="https://www.googleapis.com/auth/userinfo.email https://www.googleapis.com/auth/userinfo.profile"
                                redirect_uri="https://ccaas.worktual.co.uk/"
                               
                                onResolve={async ({ provider, data }) => {
                                    console.log(data, "Google login data");
                                    const email =
                                        data?.email ||
                                        data?.profile?.email ||
                                        data?.profileObj?.email;
                                    if (email) {
                                        await socialMediaLogin(email, "Google", data);
                                    } else {
                                        console.error("Email missing in Google response", data);
                                    }
                                }}
                                onReject={(err) => {
                                    console.log("Error during Google authentication", err);
                                }}
                            >
                                <div className={styles.socialmediainner}>
                                    <button>Connect</button>
                                </div>
                            </LoginSocialGoogle>
                        ) : (
                            <div className={styles.socialmediainner}>
                                <button onClick={() => handleDisConnect("Google")}>Connected</button>
                            </div>
                        )}
                    </div>
                </div>


                {/* <div className={`${styles.fieldareamain} ${styles.fieldconnect}`}>
                    <div className={styles.label}>
                        <img src={Facebook} alt="" />
                        Facebook</div>
                    <div className={styles.value}>
                        <div className={styles.recom}>Not Connected</div>
                    </div>
                    <div className={styles.icon}>
                        <LoginSocialFacebook
                             appId="YOUR_FACEBOOK_APP_ID"
                             redirect_uri="https://ccaas.worktual.co.uk/"
                             scope="email,public_profile"
                             onResolve={async ({ provider, data }) => {
                                console.log(data, "Facebook data");
                                console.log(provider, "provider");
                                const email = data?.email || data?.profile?.email;
                                if (email) {
                                    socialMediaLogin(email, "Facebook");
                                } else {
                                    console.error("Email is undefined in the response", data);
                                }
                            }}
                            onReject={(err) => {
                                console.log("Facebook login error:", err);
                            }}
                        >
                            <div className={styles.socialmediainner}><button>Connect</button></div>
                        </LoginSocialFacebook>
                        

                    </div>
                </div> */}

                <div className={`${styles.fieldareamain} ${styles.fieldconnect}`}>
                    <div className={styles.label}>
                        <img src={Apple} alt="" />
                        Apple</div>
                    {/* <div className={styles.value}>
                        <div className={styles.recom}>Not Connected</div>
                    </div> */}
                     <div className={`${styles.badge} ${styles.blue}`}>
                            connected
                        </div>
                    <div className={styles.icon}>

                        {!getConnectionStatus("Apple") ? (
                            <LoginSocialApple
                                client_id="com.worktual.login"
                                redirect_uri="https://ccaas.worktual.co.uk/"
                                onResolve={async ({ provider, data }: any) => {
                                    console.log("data", data)
                                    const decodedToken: any = jwtDecode(data.authorization.id_token);
                                    console.log("decoded token", decodedToken)
                                    data.social_account_email = decodedToken.email;
                                    data.social_account_type = "Apple";
                                    console.log("provider", provider)

                                    const email = data.social_account_email || decodedToken?.email

                                    console.log("Final Apple login object:", email);

                                    await socialMediaLogin(email, "Apple", data);
                                }}
                                onReject={(err: any) => {
                                    console.log(err);
                                }}>
                                <div className={styles.socialmediainner}>
                                    <button>Connect</button>
                                </div>
                            </LoginSocialApple>
                        ) : (
                            <div className={styles.socialmediainner}>
                                <button onClick={() => handleDisConnect("Apple")}>Connected</button>
                            </div>
                        )}

                    </div>
                </div>
                <div className={`${styles.fieldareamain} ${styles.fieldconnect}`}>
                    <div className={styles.label}>
                        <img src={Microsoft} alt="" />
                        Microsoft</div>
                    {/* <div className={styles.value}>
                        <div className={styles.recom}>Not Connected</div>
                    </div> */}
                     <div className={`${styles.badge} ${styles.blue}`}>
                            connected
                        </div>
                    <div className={styles.icon}>
                        {!getConnectionStatus("Microsoft") ? (
                            <LoginSocialMicrosoft
                                client_id="651c69df-eea7-4624-a1cf-cecdf82ec80e"
                                redirect_uri="https://ccaas.worktual.co.uk/"
                                onResolve={({ provider, data }: any) => {
                                    console.log("data", data)
                                    console.log("provider", provider)
                                    const decodedToken: any = jwtDecode(data.access_token);
                                    console.log("decoded token", decodedToken)
                                    data.social_account_email = decodedToken.email;

                                    data.social_account_email = data.mail ? data.mail : decodedToken.email ? decodedToken.email : decodedToken.unique_name;
                                    data.social_account_type = "microsoft";

                                    socialMediaLogin(data.social_account_email, "Microsoft", data);;
                                }}
                                onReject={(err) => {
                                    console.log("iiiiiiiiii", err);
                                }}>
                                <div className={styles.socialmediainner}>
                                    <button>Connect</button>
                                </div>
                            </LoginSocialMicrosoft>
                        ) : (
                            <div className={styles.socialmediainner}>
                                <button onClick={() => handleDisConnect("Microsoft")}>Connected</button>
                            </div>
                        )}
                    </div>
                </div>
            </div>
        </div>
    );
}
