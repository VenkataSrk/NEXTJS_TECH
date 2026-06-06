import { useEffect, useState } from "react";
import styles from "../../../assets/styles/accountinfo.module.scss";
// import editicon from "../../../assets/images/editicon.svg";
import Planslogo from '../../../assets/images/myplan/planslogo.svg'
import { getUserExtensionInfo } from "../../../apiservice/myplanapi";
import { GetItemFromStorage } from "../../../../base/customhooks/useStorage";
import { LocalStorage } from "../../../../base/customhooks/localStorageKeys";
import { jwtDecode } from 'jwt-decode';

export default function YourInfo() {
    const userDetails :any= jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    const [profileData, setProfileData] = useState<any>(null);
    console.log("profileDataprofileData", profileData)
    console.log("profileDat", userDetails)

       useEffect(() => {
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
          .catch((err:any) => console.log(err));
        }
        return () => {
          console.log("cleanup");
        };
        } catch (error) {
            
        }
       
      }, []);
      

    return (
        <div className={styles.yourinfo}>
            <div className={`${styles.titlearea} ${styles.first}`}>
                <div className={styles.title}>Welcome, {`${userDetails?.first_name} ${userDetails?.last_name}`}</div>
                <div className={styles.desc}>Manage the personal details used across all your Worktual applications</div>
            </div>
            <div className={styles.yourinfotop}>
                <div className={styles.profile}>
                    <div className={styles.profileleft} >
                         {/* <img src={Planslogo} alt="" /> */}
                       <h1 >{`${userDetails?.first_name?.charAt(0)?.toUpperCase()}${userDetails?.last_name?.charAt(0)?.toUpperCase()}`}</h1>
                    </div>
                    <div className={styles.profileright}>
                        <div className={styles.profiletext}>As a Standard at {`${userDetails?.first_name?.charAt(0)?.toUpperCase()}${userDetails?.last_name?.charAt(0)?.toUpperCase()}`}, you can manage your account settings and permissions based on your assigned access.</div>
                        {/* <button>Change</button> */}
                    </div>
                </div>
              
            </div>
            <div className={styles.yourinfotop}>
                <div className={styles.yourtitle}>Contact Information</div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Contact Email</div>
                    <div className={styles.value}>{profileData?.Email}</div>
                    <div className={styles.icon}></div>
                </div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Contact Number</div>
                    <div className={styles.value}>{profileData?.Mobileno}</div>
                    <div className={styles.icon}></div>
                </div>
            </div>
            <div className={styles.yourinfotop}>
                <div className={styles.yourtitle}>Work Details</div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Team</div>
                    <div className={styles.value}>{profileData?.Department?profileData?.Department : "Not Applicable"}</div>
                    <div className={styles.icon}></div>
                </div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Role</div>
                    <div className={styles.value}>{profileData?.Role_Name?profileData?.Role_Name  : "Not Applicable"}</div>
                    <div className={styles.icon}></div>
                </div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Organization</div>
                    <div className={styles.value}>{profileData?.company_name?profileData?.company_name : "Not Applicable"}</div>
                    <div className={styles.icon}></div>
                </div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Company Website</div>
                    <div className={styles.value}>{profileData?.companyWebsite?profileData?.companyWebsite : "Not Applicable"}</div>
                    <div className={styles.icon}></div>
                </div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Company Size</div>
                    <div className={styles.value}>{profileData?.CompanySize ? profileData?.CompanySize : "Not Applicable"}</div>
                    <div className={styles.icon}></div>
                </div>
                <div className={styles.fieldareamain}>
                    <div className={styles.label}>Company VAT Number</div>
                    <div className={styles.value}>{profileData?.VAT_number || 'Not Provided'}</div>
                    <div className={styles.icon}></div>
                </div>
            </div>
            
            
        </div>
    );
}
