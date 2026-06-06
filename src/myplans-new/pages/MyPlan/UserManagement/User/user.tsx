import React, { useEffect, useState } from 'react';
import styles from '../../../../assets/styles/usermanagement.module.scss';
import Userinfo from '../../../../../../../aiccaas/assets/images/agentworkspace/userinfo.svg'
// import { GetItemFromStorage } from '../../../../../../../custom_hooks/useStorage';
// import { LocalStorage } from '../../../../../../../custom_hooks/localStorageKeys';
import { getUserManagementInfo } from '../../../../apiservice/myplanapi';
import { GetItemFromStorage } from '../../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../../base/customhooks/localStorageKeys';
import { jwtDecode } from "jwt-decode";
import { assets } from '../../../../../styles/assets/assets';
const Userfile = () => {
    const subscriptionList = {
  prd_name: "AI Contact Center",
  user_count: 25,
  bill_date: "2026-02-02",
  contract_end_date: "2026-12-31"
};
    const userDetails = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    const [profileData, setProfileData] = useState<any>([]);
    console.log("ProfileroleProfileroleProfilerole", profileData)

    const domainAllowedUser :any={
      "20667":"200"
    }
   
    useEffect(() => {
        if (userDetails) {

            getUserManagementInfo()
                .then((res: any) => {
                    setProfileData(res.data.result);
                })
                .catch((err: any) => console.log(err));
        }
        return () => {
            console.log("cleanup");
        };
    }, []);
    const formatToReadableDate = (isoDate: string) => {
        const d = new Date(isoDate);
        return d.toLocaleString("en-US", {
          month: "short",
          day: "2-digit",
          year: "numeric"
        }); 
      };
      
      const formatToTime = (isoDate: string) => {
        const d = new Date(isoDate);
        return d.toLocaleString("en-US", {
          hour: "2-digit",
          minute: "2-digit",
          hour12: true
        }); 
      };
      
const getAliasName = (username?: string, email?: string) => {
   if (username) {
    const initials=username.trim().split("")[0]?.toUpperCase()+username.trim().split("")[1]?.toUpperCase()
    return initials;
  }
  if (email) {
    return email[0].toUpperCase();
  }

  return "";
};

    return (
        <>
            <div className={styles.createmaincontainer}>
                <div className={styles.headerole}>
                    <h3>Users</h3>
                    <p>Create and Manage users of your Worktual account</p>
                </div>
                <div className={styles.usersmain}>

                    {Array.isArray(profileData) &&
                      profileData.map((item: any, index: number) => (
                        <>
                          {domainAllowedUser?.[`${userDetails?.domain_id}`] ? (
                            <>
                              {domainAllowedUser?.[`${userDetails?.domain_id}`] ==
                                item?.Extension_Number ? (
                                <>
                                  <div className={styles.cardblock} key={index}>
                                    <div className={styles.cardblocktop}>
                                      <div className={styles.cardblocktopleft}>
                                        <span className={styles.profileic}>
                                          {`${getAliasName(item?.username)}`}
                                        </span>

                                        <div className={styles.titlee}>
                                          <div className={styles.title}>{item.username}</div>
                                          <span className={styles.deptspan}>{item.Department}</span>
                                        </div>

                                        <span className={styles.status}>Active</span>
                                      </div>

                                      <div className={styles.cardblocktopright}></div>
                                    </div>

                                    <div className={styles.cardblockbottom}>
                                      <div className={styles.cardblockbottomleft}>
                                        <div className={styles.fieldmain}>
                                          <div className={styles.label}>Last login</div>
                                          <div className={styles.value}>
                                            {formatToTime(item.lastlogin)} • Joined{" "}
                                            {formatToReadableDate(item.Createdate)}
                                          </div>
                                        </div>
                                        <div className={styles.fieldmain}>
                                          <div className={styles.label}>Campaign Access:</div>
                                          <div className={`${styles.badge} ${styles.pink}`}>
                                            {"Outbound Campaign"}
                                          </div>
                                    </div>
                                      </div>
                                    </div>
                                  </div>
                                </>
                              ) : null}
                            </>
                          ) : (
                            <>
                              <div className={styles.cardblock} key={index}>
                                <div className={styles.cardblocktop}>
                                  <div className={styles.cardblocktopleft}>
                                    <span className={styles.profileic}>
                                      {`${getAliasName(item?.username)}`}
                                    </span>

                                    <div className={styles.titlee}>
                                      <div className={styles.title}>{item.username}</div>
                                      <span className={styles.deptspan}>{item.Department}</span>
                                    </div>

                                    <span className={styles.status}>Active</span>
                                  </div>

                                  <div className={styles.cardblocktopright}></div>
                                </div>

                                <div className={styles.cardblockbottom}>
                                  <div className={styles.cardblockbottomleft}>
                                    <div className={styles.fieldmain}>
                                      <div className={styles.label}>Last login</div>
                                      <div className={styles.value}>
                                        {formatToTime(item.lastlogin)} • Joined{" "}
                                        {formatToReadableDate(item.Createdate)}
                                      </div>
                                    </div>
                                    <div className={styles.fieldmain}>
                                      <div className={styles.label}>Campaign Access:</div>
                                      <div className={`${styles.badge} ${styles.pink}`}>
                                        {"Outbound Campaign"}
                                      </div>
                                    </div>
                                  </div>
                                </div>
                              </div>
                            </>
                          )}
                        </>
                      ))}
                    {Array.isArray(profileData) && profileData.length === 0 && (
                      <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
                    )}
             
                  
                </div>

            </div>
        </>
    )
}
export default Userfile;