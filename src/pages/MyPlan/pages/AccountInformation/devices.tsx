import { useEffect, useMemo, useState } from "react";
import styles from "../../assets/styles/device.module.scss";
import device1 from "../../assets/images/myplan/addon1.svg";
import moment from "moment";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { getDevices } from "../../apiservice/myplan";
import Nodata from "../../../component/nodata";

export default function AccountDevices() {
  const [deviceData, setDeviceData] = useState<any[]>([]);
  const [devicesData, setDevicesData] = useState<any[]>([]);
  const [loader, setLoader] = useState(false);

  const loginCrmDetails = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);
  const loginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);

  const loginEmail =
    loginCrmDetails?.emailId ||
    loginUserDetails?.Email ||
    loginUserDetails?.emailId ||
    "";

  useEffect(() => {
    getDevices1();
  }, [loginEmail]);

  const getDevices1 = async () => {
    if (!loginEmail) {
      setDevicesData([]);
      return;
    }
    try {
      setLoader(true);
      const res: any = await getDevices(loginEmail);
      if (res?.data?.statusCode === 200) {
        setDevicesData(res?.data?.result || []);
      } else {
        setDevicesData([]);
      }
    } catch (error) {
      console.error("getDevices error:", error);
      setDevicesData([]);
    } finally {
      setLoader(false);
    }
  };

  const devicesWithStatus = useMemo(() => {
    if (!devicesData?.length) return [];
    return devicesData.map((device: any, index: number) => ({
      ...device,
      img: device?.img || device1,
      Device: device?.Device || device?.device || "-",
      Device_Model: device?.Device_Model || device?.device_model || "-",
      OS_Type: device?.OS_Type || device?.os_type || "-",
      Browser_Type: device?.Browser_Type || device?.browser_type || "-",
      updated_On: device?.updated_On || device?.updated_on || null,
      created_On: device?.created_On || device?.created_on || null,
      IP_Address: device?.IP_Address || device?.ip_address || "-",
      Location: device?.Location || device?.location || "-",
      isActiveUI: typeof device?.is_active === "number" ? device.is_active === 1 : index === 0,
    }));
  }, [devicesData]);

  useEffect(() => {
    setDeviceData(devicesWithStatus);
  }, [devicesWithStatus]);
  
  return (
    <div className={styles.yourinfo}>
      {/* <div className={styles.titlearea}>
        <div className={styles.title}>Devices</div>
        <div className={styles.desc}>
          Manage connected devices and active sessions to keep your account secure.
        </div>
      </div> */}

      <div className={styles.productmain}>
        {deviceData?.map((product: any, index: any) => {
          return (
            <div key={index} className={styles.productblock}>
              <div className={styles.top}>
                <div className={styles.left}>
                  <img src={product.img} alt="device" />
                  <div className={styles.devicename}>
                    <div className={styles.deviceproduct}>
                      {product.Device}
                      <span>{product?.isActiveUI ? "Active" : "InActive"}</span>
                    </div>
                    <span>{product.Device_Model}</span>
                  </div>
                </div>
              </div>

              <div className={styles.bottom}>
                <div className={styles.fieldareamain}>
                  <div className={styles.label}>OS :</div>
                  <div className={styles.value}>{product.OS_Type || "-"}</div>
                </div>

                <div className={styles.fieldareamain}>
                  <div className={styles.label}>Browser :</div>
                  <div className={styles.value}>{product.Browser_Type || "-"}</div>
                </div>

                <div className={styles.fieldareamain}>
                  <div className={styles.label}>Last session :</div>
                  <div className={styles.value}>
                    {product?.updated_On
                      ? moment(product?.updated_On).format("MMM DD, h:mm A")
                      : product?.created_On
                      ? moment(product?.created_On).format("MMM DD, h:mm A")
                      : '-'}
                  </div>
                </div>

                <div className={styles.fieldareamain}>
                  <div className={styles.label}>IP Address :</div>
                  <div className={styles.value}>{product.IP_Address || "-"}</div>
                </div>

                <div className={styles.fieldareamain}>
                  <div className={styles.label}>Location :</div>
                  <div className={styles.value}>{product.Location || "-"}</div>
                </div>
              </div>
            </div>
          )
        })}
      </div>
    </div>
  );
}
