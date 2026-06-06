import React, { useEffect, useRef, useState } from 'react';
import styles from '../../../assets/styles/usermanagement.module.scss';
// import { CoreServices } from '../../../../../../base/service/apiServices';
import { whatsappService } from '../../../../base/service/apiservice';
import { GetItemFromStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import { assets } from '../../../../styles/assets/assets';
// import nextIcon from '../../../assets/images/myplan/nextext.svg'
// import prevIcon from '../../../assets/images/myplan/previoustxt.svg'

const Useractivitylog = () => {
    const accessToken = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
    const [userActlist, setUserActList] = useState<any>([]);
    const currentDate: any = useRef<any>(null);
    console.log("useract", userActlist)
    const getUserActivity = async () => {
    try {
        const body = {
            loginSource: "CAMPAIGN"
        };
        const resp = await whatsappService.post(
            body, `/get_campaign_user_activity_log`);
        if (resp?.data?.statusCode === 200) {
            setUserActList(Array.isArray(resp?.data?.response) ? resp.data.response : []);
        }

    } catch (err) {
        console.log("API Error:", err);
    }
};

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

    const handleDate = (dt: any) => {
        currentDate.current = dt;
        const date = new Date(dt);

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

    return (
        <div className={styles.createmaincontainer}>
                <div className={styles.headerole}>
                    <h3>User activity logs</h3>
                    <p>View all the activities done across Worktual platform</p>
                </div>

                {/* <div className={styles.useracttitle}>Today - Wednesday 5 November 2025</div> */}
                {userActlist?.map((item: any, ind: any) => {
                    return (
                        <>
                            {currentDate.current !== item?.create_date?.split(' ')?.[0] && <div className={styles.useracttitle}>{handleDate(item?.create_date?.split(' ')?.[0])}</div>}
                            <div key={ind}>
                                <div className={styles.useractmain}>
                                    <div className={styles.useractmaininner}>
                                        <div className={styles.useractmainleft}>{timeformatFunc(item?.create_date)}</div>
                                        <div className={styles.useractmaincenter}>
                                            <div className={styles.img}>
                                                {item?.profile_image_url ? <img src={item?.profile_image_url} 
                                                    onError={(e:any) => {
                                                       e. target.onerror = null;
                                                        e.target.src = "https://nlp-storage.worktual.co.uk/wsnlp/ccaas_settings_file_upload/11165/38_38.png";
                                                    }}
                                                alt="No img" /> : <p>{item?.AgentName?.[0]?.toUpperCase()}</p>}
                                            </div>
                                            <div className={styles.title}>
                                                <div className={styles.name}>{item?.AgentName}</div>
                                                <div className={styles.para}>{item?.errmsg}</div>
                                            </div>
                                        </div>
                                        {/* <div className={styles.useractmainright}>
                                            <svg xmlns="http://www.w3.org/2000/svg" width="5" height="18" viewBox="0 0 5 18" fill="none">
                                                <path fill-rule="evenodd" clip-rule="evenodd" d="M2.2435 4.487C3.4774 4.487 4.487 3.4774 4.487 2.2435C4.487 1.0096 3.4774 0 2.2435 0C1.0096 0 0 1.0096 0 2.2435C0 3.4774 1.0096 4.487 2.2435 4.487ZM2.2435 6.7305C1.0096 6.7305 0 7.7401 0 8.974C0 10.2079 1.0096 11.2175 2.2435 11.2175C3.4774 11.2175 4.487 10.2079 4.487 8.974C4.487 7.7401 3.4774 6.7305 2.2435 6.7305ZM0 15.7045C0 14.4706 1.0096 13.461 2.2435 13.461C3.4774 13.461 4.487 14.4706 4.487 15.7045C4.487 16.9384 3.4774 17.948 2.2435 17.948C1.0096 17.948 0 16.9384 0 15.7045Z" fill="#404040" />
                                            </svg>
                                        </div> */}
                                    </div>
                                </div>
                            </div>
                        </>)
                })

                }
                {userActlist.length==0 &&
                <>
                    <div className={"nodataclass"}><img src={assets.nodataimg} />No Docs Found</div>
                </>

                }

            
        </div>
    )
}
export default Useractivitylog
