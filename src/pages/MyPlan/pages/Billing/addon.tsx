import React, { useEffect, useState } from 'react'
import styles from "../../assets/styles/addon.module.scss";
import addon1 from "../../assets/images/myplan/addon1.svg";
import Skeleton from '../../skeleton';
import { ChatActions } from '../../../../store/slice/chat_slice/chat_slice';
import { CoreServices } from '../../../../base/service/apiservices';
import { useDispatcher } from '../../../../store/redux-store/dispatch';
import { useNavigate } from 'react-router-dom';
import { GetItemFromStorage } from '../../../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../../../base/custom_hooks/localStorageKeys';

function Addon() {

  GetItemFromStorage(LocalStorage.USER_DETAILS);
  const [addonList, setAddonList] = useState<any[]>([]);
  useNavigate();
  const dispatch = useDispatcher();

  // const getAddonList = async () => {
  //   try {
  //     let url = `${config().WTMS_URL}/product/worktualGetProdWiseAddonList`;
  //     let body = {
  //       category_id: 0,
  //       tier_id: userDetails?.plan_id,
  //       product_id: 0
  //     }
  //     let response = await axios.post(url, body).then((res) => {
  //       return res;
  //     })
  //     setAddonList(response?.data?.result)
  //     return response?.data?.result;
  //   } catch (error) {

  //   }
  // }

  const [loading, setLoading] = useState(false)
  const getAddonList = async () => {
    try {
      setLoading(true)
      const response: any = await CoreServices.get(`/get_addOn_purchase_list`);
      if (response && response.data.statusCode === 200) {
        const result = response?.data?.result?.[0];
        setAddonList(Array.isArray(result) ? result : []);
      } else {
        setAddonList([]);
      }
    } catch (err) {
      console.log("getUserExtensionInfo err:", err);
      setAddonList([]);
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    getAddonList();
  }, [])
  const openBuyNow = () => {
    dispatch(ChatActions.setBuyNowModal(true));
  }
  const [currencyName, setCurrencyName] = useState('');

  useEffect(() => {
    try {
     const host = window.location.host;
     const parts = host.split('.');
     console.log("locloclocloc-host----->>>>",host,parts)

     if (parts.length > 2) {
       const loc = parts[parts.length - 1]
       console.log("locloclocloc------>>>>",loc)

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
      console.log("locloclocloc------>>>>")

       setCurrencyName('$');
     }
    } catch (error) {
     console.log(error)
    }
	   }, []);

  return (
    <>
      <div className={styles.createmaincontainer}>
        <div className={styles.addonblock}>
          
          {/* Skeleton (kept UI, but static) */}
          {loading && !addonList?.length && <Skeleton count={4} />}

          {/* Addon List */}
          {addonList?.length > 0 && addonList.map((addon: any, index: number) => {
            return (
              <div className={styles.addonblockinner} key={index}>
                <div className={styles.addontop}>
                  <div className={styles.addonicon}>
                    <div className={styles.addonprice}>
                      <span>Buy for</span> {currencyName}{addon?.addon_price}/Month
                    </div>
                  </div>
                  <button onClick={openBuyNow}>Buy now</button>
                </div>

                <div className={styles.addonmid}>
                  <div className={styles.lefts}>
                    <img 
                      src={addon.image_name !== '-' ? addon.image_name : addon1} 
                      alt="addon" 
                    />
                  </div>
                  <div className={styles.rights}>
                    <div className={styles.title}>{addon?.addon_feature_name}</div>
                    <div className={styles.desc}>{addon?.addon_feature_description}</div>
                  </div>
                </div>
              </div>
            )
          })}

          {/* No Data UI */}
          {!loading && Array.isArray(addonList) && addonList.length === 0 && (
            <div className={styles.nodatacontainer}>
              <div className={styles.nodatabox}>
                {/* <img src={assets.emptyData} alt="No data" /> */}
                <div className={styles.connected}>
                  <div className={styles.callstitle}>
                    No addon available
                  </div>
                  <div className={styles.callmessage}>
                    There are no addon to display at the moment.
                  </div>
                </div>
              </div>
            </div>
          )}

        </div>
      </div>
    </>
  )
}

export default Addon;
