import React, { useEffect, useRef, useState } from 'react';
import styles from '../../assets/styles/usermanagement.module.scss';
import { CoreServices } from '../../../../base/service/apiservices';
import { GetItemFromStorage } from '../../../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../../../base/custom_hooks/localStorageKeys';
import Skeleton from "../../skeleton";
const Useractivitylog = () => {

    const [userActlist, setUserActList] = useState<any>([]);
    const currentDate: any = useRef<any>(null);
    const [loader, setLoader] = useState(false)
    const loginUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);

    const getUserActivity = async () => {
        try {
            setLoader(true);
            const body: any = {
                userId:null,
                roleId : loginUserDetails?.org_role_id_crm,
                limit:100,
                offset : 0
              };
            const resp: any = await CoreServices.post(body,`/get_user_activity_log`);
            if (resp?.data?.statusCode === 200) {
                setUserActList(resp?.data?.result || []);
            } else {
                setUserActList([]);
            }
        } catch (err) {
            console.log(err)
            setUserActList([]);
        } finally {
            setLoader(false);
        }


    }

    useEffect(() => {
        getUserActivity();
    }, [])

    const timeformatFunc = (timeStr: string): string => {
        if (!timeStr) return "--:--";
      
        const date = new Date(timeStr); 
      
        const hours = date.getHours().toString().padStart(2, "0");
        const minutes = date.getMinutes().toString().padStart(2, "0");
      
        return `${hours}:${minutes}`;
      };
      
    const handleDate = (dt: any) => {
        const date = new Date(dt);
        currentDate.current = date.toDateString();

        // Format day name, day number, month name, and year
        const dayName = date.toLocaleString("en-US", { weekday: "long" });
        const day = date.getDate();
        const monthName = date.toLocaleString("en-US", { month: "long" });
        const year = date.getFullYear();

        // Check if it's today
        const today = new Date();
        const isToday = today.toDateString() === date.toDateString();

        // Build final string
        const formatted = `${isToday ? "Today - " : ""}${dayName} ${day} ${monthName} ${year}`;
        return formatted;
    }
    currentDate.current = null;

    return (
        <>
            <div className={styles.createmaincontainer}>

                {userActlist?.map((item: any, ind: any) => {
                    const createdDate = item?.createdat || item?.createdAt || "";
                    const dateFormt = createdDate ? new Date(createdDate).toDateString() : "";
                    return (
                        <React.Fragment key={ind}>
                            {currentDate.current !== dateFormt && (
                                <div className={styles.useracttitle}>
                                    {handleDate(createdDate)}
                                </div>
                            )}

                            <div className={styles.useractmain}>
                                <div className={styles.useractmaininner}>
                                    <div className={styles.useractmainleft}>
                                        {timeformatFunc(createdDate)}
                                    </div>

                                    <div className={styles.useractmaincenter}>
                                        <div className={styles.img}>
                                            {item?.profile_image_url ? (
                                                <img src={item?.profile_image_url} alt="No img" />
                                            ) : (
                                                <p>{item?.UserName?.[0]?.toUpperCase() || "-"}</p>
                                            )}
                                        </div>

                                        <div className={styles.title}>
                                            <div className={styles.name}>
                                                {item?.UserName || item?.email || "-"}
                                            </div>
                                            <div className={styles.para}>
                                                {item?.message_log || "-"}
                                            </div>
                                        </div>
                                    </div>

                                    <div className={styles.useractmainright}>
                                        <svg xmlns="http://www.w3.org/2000/svg" width="5" height="18" viewBox="0 0 5 18" fill="none">
                                            <path fillRule="evenodd" clipRule="evenodd" d="M2.2435 4.487C3.4774 4.487 4.487 3.4774 4.487 2.2435C4.487 1.0096 3.4774 0 2.2435 0C1.0096 0 0 1.0096 0 2.2435C0 3.4774 1.0096 4.487 2.2435 4.487ZM2.2435 6.7305C1.0096 6.7305 0 7.7401 0 8.974C0 10.2079 1.0096 11.2175 2.2435 11.2175C3.4774 11.2175 4.487 10.2079 4.487 8.974C4.487 7.7401 3.4774 6.7305 2.2435 6.7305ZM0 15.7045C0 14.4706 1.0096 13.461 2.2435 13.461C3.4774 13.461 4.487 14.4706 4.487 15.7045C4.487 16.9384 3.4774 17.948 2.2435 17.948C1.0096 17.948 0 16.9384 0 15.7045Z" fill="#404040" />
                                        </svg>
                                    </div>

                                </div>
                            </div>
                        </React.Fragment>
                    );
                })}
                {loader && <Skeleton count={4}/>}
            </div>
        </>
    );
};

export default Useractivitylog;
