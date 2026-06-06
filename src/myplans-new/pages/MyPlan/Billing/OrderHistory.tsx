import React, { useEffect, useState } from 'react'
import styles from "../../../assets/styles/addon.module.scss"
// import { GetItemFromStorage } from '../../../../../../custom_hooks/useStorage';
// import { LocalStorage } from '../../../../../../custom_hooks/localStorageKeys';
import { getOrderHistoryInfo, getSubscriptionManagement } from '../../../apiservice/myplanapi';
import { GetItemFromStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import { jwtDecode } from "jwt-decode";
import { assets } from '../../../../styles/assets/assets';
import orderHistory from "../../../../myplans-new/assets/images/myplan/orderhist.svg";
function OrderhistoryNew() {
  const userDetails = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
  const [profileData, setProfileData] = useState<any>(null);
  const [planData, setPlanData] = useState<any>(null);
  console.log("ProfileroleProfileroleProfilerole", profileData)

  useEffect(() => {
    try {
      
      if (userDetails) {
        const body = {
          Search_type: "ALL",
          company_id: userDetails?.company_id,
          datefilter: 0,
          fromdate: "",
          invoice_id: "0",
          status_type: 0,
          todate: ""
        }
        const data = {
          company_id: userDetails?.company_id,
  
        }
        getOrderHistoryInfo(body)
          .then((res: any) => {
            setProfileData(res.data.finalresponse?.[0]);  // store in state
          })
          .catch((err: any) => console.log(err));
  
        // getSubscriptionManagement(body)
        // .then((res: any) => {
        //     // console.log("API RESPONSE ---->", res);
        //     setPlanData(res.data);  // store in state
        // })
        // .catch((err: any) => console.log(err));
      }
      return () => {
        console.log("cleanup");
      };
    } catch (error) {
      console.log("ERR:",error)
    }
  }, []);
  const formatDate = (d: string) => {
    const date = new Date(d);
    const day = String(date.getDate()).padStart(2, "0");
    const month = date.toLocaleString("en-US", { month: "short" });
    const year = date.getFullYear();
    return `${day} ${month} ${year}`;   // ex: 07 Nov 2025
  };


  return (
    <>
      <div className={styles.createmaincontainer}>
        <div className={styles.headerole}>
          <h3>Order History</h3>
          <p>Microcopy</p>
        </div>
        <div className={styles.addonblock}>
          {Array.isArray(profileData) && profileData.map((item: any, index: number) => (
            <div key={index} className={styles.addonblockinner}>
              <div className={styles.addontop}>
                <div className={styles.addiconmidterm}>
                  <div className={styles.addonicon}>
                    <div className={styles.addonpricebtn}>Order #</div>
                    <div className={styles.addonprice}> {item.payment_reference}</div>
                  </div>
                  <div className={styles.addonicon}>
                    <div className={styles.addonpricebtn}>Ordered</div>
                    <div className={styles.addonprice}>{formatDate(item.bill_date)}</div>
                  </div>
                  <div className={styles.addonicon}>
                    <div className={styles.addonpricebtn}>Total </div>
                    <div className={styles.addonprice}>{item.current_plan_price}</div>
                  </div>
                </div>
                <button>View order</button>
              </div>
              <div className={styles.addonmid}>
                <div className={styles.lefts}>
                  <img src={orderHistory} alt="" />
                  {/* <img src={item.Icon} alt="" /> */}
                </div>
                <div className={styles.rights}>

                  <div className={styles.title}>{item.billing_type}</div>
                  <div className={styles.desc}>{item.description}</div>
                </div>
              </div>

            </div>
          ))}
        </div>
      </div>
    </>
  )
}

export default OrderhistoryNew
