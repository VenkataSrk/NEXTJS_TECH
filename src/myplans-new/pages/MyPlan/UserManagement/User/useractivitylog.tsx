import React, { useEffect, useState } from 'react';
import styles from '../../../../assets/styles/usermanagement.module.scss';

// import { CoreServices } from '../../../../../../../../src/base/service/apiServices';
import axios from 'axios';
// import { GetItemFromStorage } from '../../../../../../../../src/custom_hooks/useStorage';
// import { LocalStorage } from '../../../../../../../../src/custom_hooks/localStorageKeys';
import Planslogo from '../../../../assets/images/myplan/planslogo.svg';
import { jwtDecode } from "jwt-decode";
import { GetItemFromStorage } from '../../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../../base/customhooks/localStorageKeys';
import { AICoreServices } from '../../../../../base/service/apiservice';
const Useractivitylognew = () => {
    const accessToken = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
    const loginUserDetail = jwtDecode(accessToken);
    const [userActlist, setUserActList] = useState<any>([]);
    console.log("useract", userActlist)
    const getUserActivity = async () => {
        try {
            AICoreServices.get(`/get_user_login_log`)
                .then((resp: any) => {
                    console.log("rs", resp)
                    if (resp?.data?.statusCode == 0) {
                        setUserActList(resp?.data?.result)
                    }

                })
        } catch (err) {
            console.log(err)
        }
    }

    useEffect(() => {
        getUserActivity();
    }, [])


    const timeformatFunc = (time: any) => {
        const isoString = time;
        const date = new Date(isoString);

        const hours = date.getUTCHours().toString().padStart(2, "0");
        const minutes = date.getUTCMinutes().toString().padStart(2, "0");

        const formattedTime = `${hours}:${minutes}`;
        console.log(formattedTime); // "06:50"
        return formattedTime;

    }

    return (
        <>
            <div className={styles.createmaincontainer}>
                <div className={styles.headerole}>
                    <h3>User activity logs</h3>
                    <p>View all the activities done across Worktual platform</p>
                </div>

                <div className={styles.useracttitle}>Today - Wednesday 5 November 2025</div>
                {userActlist?.map((item: any, ind: any) => {
                    return (
                        <>
                            <div key={ind}>
                                <div className={styles.useractmain}>
                                    <div className={styles.useractmaininner}>
                                        <div className={styles.useractmainleft}>{timeformatFunc(item?.create_date)}</div>
                                        <div className={styles.useractmaincenter}>
                                            <div className={styles.img}>
                                                <img src={Planslogo} alt="" />
                                            </div>
                                            <div className={styles.title}>                                  
                                                <div className={styles.name}>{item?.email}</div>
                                                <div className={styles.para}>Administrator</div>
                                            </div>
                                        </div>
                                        <div className={styles.useractmainright}>
                                            <svg xmlns="http://www.w3.org/2000/svg" width="5" height="18" viewBox="0 0 5 18" fill="none">
                                                <path fill-rule="evenodd" clip-rule="evenodd" d="M2.2435 4.487C3.4774 4.487 4.487 3.4774 4.487 2.2435C4.487 1.0096 3.4774 0 2.2435 0C1.0096 0 0 1.0096 0 2.2435C0 3.4774 1.0096 4.487 2.2435 4.487ZM2.2435 6.7305C1.0096 6.7305 0 7.7401 0 8.974C0 10.2079 1.0096 11.2175 2.2435 11.2175C3.4774 11.2175 4.487 10.2079 4.487 8.974C4.487 7.7401 3.4774 6.7305 2.2435 6.7305ZM0 15.7045C0 14.4706 1.0096 13.461 2.2435 13.461C3.4774 13.461 4.487 14.4706 4.487 15.7045C4.487 16.9384 3.4774 17.948 2.2435 17.948C1.0096 17.948 0 16.9384 0 15.7045Z" fill="#404040" />
                                            </svg>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </>)
                })

                }

            </div>
        </>
    )
}
export default Useractivitylognew;