import React, { useEffect, useState } from 'react'
import styles from "../../assets/styles/orderhistory.module.scss"
import Skeleton from '../../skeleton';
import Contactcenter from '../../assets/images/myplan/contactcentericon.svg';
import aiOnboardinfSocketservice from '../../../../base/service/aiOnboardinfSocketservice';
import { AiSettingsAction } from '../../../../store/slice/my_plans/settings';
import { getOrderHistoryInfo } from '../../apiservice/myplanapi';
import { GetItemFromStorage } from '../../../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../../../base/custom_hooks/localStorageKeys';
import { useDispatch, useSelector } from 'react-redux';
import { useNavigate, useSearchParams } from 'react-router-dom';

function Orderhistory() {

  const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const [profileData, setProfileData] = useState<any>(null);
  const navigate = useNavigate();
  const dispatch = useDispatch();
  const [loader, setLoader] = useState(false);
  const [searchParams] = useSearchParams()
  const setUpSessionId = searchParams.get("sessionId")
  const searchSessionId = useSelector((store: any) => store?.aiSettings?.searchSessionId || "")
    
  useEffect(() => {
    const fetchData = async () => {
      try {
        setLoader(true);
        const body = {
          Search_type: "ALL",
          company_id: userDetails?.company_id,
          datefilter: 0,
          fromdate: "",
          invoice_id: "0",
          status_type: 0,
          todate: ""
        };
        const res = await getOrderHistoryInfo(body);
        setProfileData(res.data.finalresponse);
      } catch (err) {
        console.log('getOrderHistoryInfo Error', err);
      } finally {
        setLoader(false);
      }
    };
  
    fetchData();
  }, []);
  
  
  const formatDate = (d: string) => {
    const date = new Date(d);
    const day = String(date.getDate()).padStart(2, "0");
    const month = date.toLocaleString("en-US", { month: "short" });
    const year = date.getFullYear();
    return `${day} ${month} ${year}`;
  };

  const navigationHome = (invoiceId: any) => {

    const viewOrderQuery = 'I want a detailed summary of order purchase details.';
    const sessionId = setUpSessionId || searchSessionId

    const body = {
      query: '',
      sessionId: sessionId,
      uploadUrl: '',
      socialmedia: '',
      shopify: '',
      accessToken: '',
      first_message: 0,
      tags: '',
      messageId: Date.now(),
      queryFrom: 'Home',
      invoiceId: invoiceId || null
    }

    // aiOnboardinfSocketservice.io.emit("setUpChat", body)

    // dispatch(AiSettingsAction.setAiAnalyticsMessages({
    //   isBot: 0,
    //   message: viewOrderQuery,
    //   sentAt: Date.now(),
    //   sessionId: sessionId,
    // }));

    // dispatch(AiSettingsAction.setaiAnalyticsTyping(true));
    // navigate(`/details?sessionId=${sessionId || ""}`);
  };

  const [currencyName, setCurrencyName] = useState('');

  useEffect(() => {
    try {
     const host = window.location.host;
     const parts = host.split('.');
 
     if (parts.length > 2) {
       const loc = parts[parts.length - 1]
       switch(loc){
         case 'in':
           setCurrencyName('₹');
           break;
         case 'uk':
           setCurrencyName('£');
           break;
         case 'com':
           setCurrencyName('$');
           break;
       }
     }else{
       setCurrencyName('$');
     }
    } catch (error) {
     console.log(error)
    }
   }, []);

  return (
    <div className={`${styles.createmaincontainer} ${profileData?.[0]?.length === 0 ? styles.headerolecontainer : ""}`}>

      {/* <div className={styles.headerole}>
        <h3>Order History</h3>
        <p>View a list of all your past purchases and details.</p>
      </div> */}

      <div className={styles.addonblock}>

        {/* Skeleton (static) */}
        {loader && profileData?.[0]?.length === 0 && <Skeleton count={5} />}

        {/* LIST */}
        {Array.isArray(profileData?.[0]) &&
          profileData[0].map((item: any, index: number) => (
            <div key={index} className={styles.addonblockinner}>

              <div className={styles.addontop}>
                <div className={styles.addiconmidterm}>

                  <div className={styles.addonicon}>
                    <div className={styles.addonpricebtn}>Order #</div>
                    <div className={styles.addonprice}>
                      {item.payment_reference}
                    </div>
                  </div>

                  <div className={styles.addonicon}>
                    <div className={styles.addonpricebtn}>Ordered</div>
                    <div className={styles.addonprice}>
                      {formatDate(item.bill_date)}
                    </div>
                  </div>

                  <div className={styles.addonicon}>
                    <div className={styles.addonpricebtn}>Total</div>
                    <div className={styles.addonprice}>
                      {currencyName}{item.total_bill}
                    </div>
                  </div>

                </div>

                <button onClick={() => navigationHome(item?.invoice_id)}>View order</button>
              </div>

              <div className={styles.addonmid}>
                <div className={styles.lefts}>
                  <img src={Contactcenter} alt="icon" />
                </div>
                <div className={styles.rights}>
                  <div className={styles.title}>
                    {item.description}
                  </div>
                </div>
              </div>

            </div>
          ))
        }

        {/* NO DATA */}
        {!loader && Array.isArray(profileData?.[0]) && profileData?.[0]?.length === 0 && (
          <div className={styles.nodatacontainer}>
            <div className={styles.nodatabox}>
              {/* <img src={assets.emptyData} alt="No data" /> */}
              <div className={styles.connected}>
                <div className={styles.callstitle}>
                  No order history available
                </div>
                <div className={styles.callmessage}>
                  There are no order history to display at the moment.
                </div>
              </div>
            </div>
          </div>
        )}

      </div>
    </div>
  )
}

export default Orderhistory;
