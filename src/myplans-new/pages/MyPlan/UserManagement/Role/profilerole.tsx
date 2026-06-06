import React, { useEffect, useState } from 'react';
import styles from '../../../../assets/styles/profileteam.module.scss'
import { getUserManagementInfo } from '../../../../apiservice/myplanapi';
import { GetItemFromStorage } from '../../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../../base/customhooks/localStorageKeys';
// import { GetItemFromStorage } from '../../../../../../../custom_hooks/useStorage';
// import { LocalStorage } from '../../../../../../../custom_hooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
const Profilerole = () => {
  const userDetails = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
  const [profileData, setProfileData] = useState<any>(null);
  console.log("ProfileroleProfileroleProfilerole", profileData)

  useEffect(() => {
    if (userDetails) {
      const body = {
        OrderId: userDetails.order_id,
        Order_Type: 0
      }

      getUserManagementInfo(body)
        .then((res: any) => {
          // console.log("API RESPONSE ---->", res);
          setProfileData(res.data.finalresponse);  // store in state
        })
        .catch((err: any) => console.log(err));
    }
    return () => {
      console.log("cleanup");
    };
  }, []);
  return (
    <>
      <div className={styles.useract}>
        <div className={styles.useracttitle}>Users</div>
        {profileData?.map((item: any, index: number) => (
          <div className={styles.useractmain} key={index}>
            <div className={styles.useractmaininner}>
              <div className={styles.useractmainleft}>21:22</div>
              <div className={styles.useractmaincenter}>
                <div className={styles.img}>
                  <img src="" alt="" />
                </div>
                <div className={styles.title}>
                  <div className={styles.name}>{item.name}</div>
                  <div className={styles.para}>{item.position ? item.position : "Admin"}</div>
                </div>
              </div>
              <div className={styles.useractmainright}>
                <svg xmlns="http://www.w3.org/2000/svg" width="5" height="18" viewBox="0 0 5 18" fill="none">

                </svg>
              </div>
            </div>
          </div>
        ))}

      </div>
    </>
  )
}
export default Profilerole;