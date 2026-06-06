import { useEffect, useMemo, useState } from "react";
import styles from "../../../assets/styles/device.module.scss";
import device1 from "../../../assets/images/myplan/device1.svg";
import {  getDevices } from "../../../apiservice/myplan";
import moment from "moment-timezone";
import { GetItemFromStorage } from "../../../../base/customhooks/useStorage";
import { jwtDecode } from "jwt-decode";
import { LocalStorage } from "../../../../base/customhooks/localStorageKeys";
import nodataimg from "../../../../../src/styles/images/icons/nodata.svg"

export default function AccountDevices() {
     const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN??"");
      const userData: any = userDetails?jwtDecode(userDetails):{};
    


    useEffect(()=>{
        // updateDeviceDetails()
        getDevices1()
    },[])

    const [rawDevicesData, setRawDevicesData] = useState<any[]>([])
    const getDevices1 = async () => {
        const res :any= await getDevices(userData?.Email);
        console.log("res-->",res)
        if(res?.data?.statusCode === 200 && res?.data?.result?.length > 0){
            console.log("res-->",res)
            setRawDevicesData(res?.data?.result)
        }
    }
     const devicesData= useMemo(() => {
        if (!rawDevicesData?.length) return [];
        return rawDevicesData.map((device: any, index: number) => ({
          ...device,
          isActiveUI: index === 0,   
        }));
      }, [rawDevicesData]);
    return (
        <div className={styles.mainlayout}>
            <div className={styles.titlearea}>
                <div className={styles.title}>Devices</div>
                <div className={styles.desc}>These signed-in devices have recently been active on this account.</div>
            </div>
            <div className={styles.productmain}>
                 {/* <div className={styles.productblock}>
                        <div className={styles.top}>
                            <div className={styles.left}>
                                <img src={device1} alt="" />
                                <div className={styles.devicename}>
                                <div className={styles.deviceproduct}>{product?.Device || '-'}<span>Active</span></div>
                                <span>{product?.Device_Model || '-'}</span></div>
                            </div>
                            <div className={styles.right}>
                                <button>{product?.action}</button>
                                 <div className={`${styles.badge} ${styles.blue}`}>
                            {product?.sts}
                        </div>
                            </div>
                        </div>
                        <div className={styles.bottom}>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>OS</div>
                                <div className={styles.value}>{product?.OS_Type || '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>Browser</div>
                                <div className={styles.value}>{product?.Browser_Type || '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>Last session</div>
                                <div className={styles.value}>{product?.updated_On ? moment(product?.updated_On).format('DD/MM/YYYY') : '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>IP Address</div>
                                <div className={styles.value}>{product?.IP_Address || '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>Location</div>
                                <div className={styles.value}>{product?.Location || '-'}</div>
                            </div>
                        </div>
                    </div> */}
                {!devicesData?.length &&  <div className={"nodataclass"}><img src={nodataimg}/>No Data found</div>}
                {devicesData?.map((product :any) => (
                    <div className={styles.productblock}>
                        <div className={styles.top}>
                            <div className={styles.left}>
                                <img src={device1} alt="" />
                                <div className={styles.devicename}>
                                {/* <div className={styles.deviceproduct}>{product?.Device || '-'}<span>Active</span></div> */}
                                <span>{product?.Device_Model || '-'}</span></div>
                            </div>
                             <div className={styles.right}>
                                    {/* <button>{product?.action}</button> */}
                                    <div className={`${styles.deviceproduct} ${styles.blue}`}>{product?.isActiveUI ? "Current Session" : "Inactive"}</div>
                                </div>
                        </div>
                        <div className={styles.bottom}>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>OS</div>
                                <div className={styles.value}>{product?.OS_Type || '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>Browser</div>
                                <div className={styles.value}>{product?.Browser_Type || '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>Last session</div>
                                <div className={styles.value}>{product?.updated_On ? moment(product?.updated_On).format('DD/MM/YYYY') : '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>IP Address</div>
                                <div className={styles.value}>{product?.IP_Address || '-'}</div>
                            </div>
                            <div className={styles.fieldareamain}>
                                <div className={styles.label}>Location</div>
                                <div className={styles.value}>{product?.Location || '-'}</div>
                            </div>
                        </div>
                    </div>
                ))}
            </div>
        </div>
    );
}
