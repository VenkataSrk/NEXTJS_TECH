import React, { useEffect, useState } from 'react'
import styles from "../../../../assets/styles/addon.module.scss";
import addon1 from "../../../../assets/images/myplan/addon1.svg";
import axios from 'axios';
import { GetItemFromStorage } from '../../../../../base/customhooks/useStorage';
import { jwtDecode } from 'jwt-decode';
import { LocalStorage } from '../../../../../base/customhooks/localStorageKeys';
import { config } from '../../../../../base/constant';
import { AICoreServices } from '../../../../../base/service/apiservice';

function Addon() {
  const userDetails = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
  const [addonList, setAddonList] = useState<any[]>([]);
  console.log("addonList", addonList);

  const getAddonList = async () => {
    // try {
    //   let url = `${config().WTMS_URL}/product/worktualGetProdWiseAddonList`;
    //   let body = {
    //     category_id: 0,
    //     tier_id: userDetails?.plan_id,
    //     product_id: 0
    //   }
    //   let response = await axios.post(url, body).then((res) => {
    //     return res;
    //   })
    //   setAddonList(response?.data?.result)
    //   return response?.data?.result;
    // } catch (error) {

    // }
    try {
      let url =`/get_addOn_purchase_list`
      AICoreServices.get(url).then((res :any)=>{
        console.log("res??",res);
        setAddonList(res?.data?.result?.[0])
      })

    } catch (error) {
      console.log("err",error)
    }
  }
  useEffect(() => {
    getAddonList();
  }, [])
  const apps = [
    {
      icon: addon1,
      title: "Agent",
      price: "£25/Agent/Month",
      description: "Purchase additional seats for agents to solve customer queries."
    },
    {
      icon: addon1,
      title: "Branding",
      price: "£20/Month",
      description: "Customise the chat widget to your brand by choosing colour, name and domain."
    },
    {
      icon: addon1,
      title: "Multilingual",
      price: "£50/Month",
      description: "Enable communication across multiple languages for global support."
    },
    {
      icon: addon1,
      title: "Verification",
      price: "£50/Month/Company",
      description: "Secure customer information by verifying during self-service assistance."
    },
    {
      icon: addon1,
      title: "Managed Support Service",
      price: "£50/Month/Company",
      description: "Get assistance in setting up APIs to perform bot actions."
    },
    {
      icon: addon1,
      title: "Live Chat Conversations",
      price: "£50/10,000 conversations",
      description: "Live interaction between customers and agents."
    }
  ];

  return (
    <>
      <div className={styles.createmaincontainer}>
        <div className={styles.headerole}>
          <h3>Apps</h3>
          <p>Microcopy</p>
        </div>
        <div className={styles.addonblock}>
          {addonList.map((app, index) => (
            <div key={index} className={styles.addonblockinner}>
              <div className={styles.addontop}>
                <div className={styles.addonicon}>

                  <div className={styles.addonprice}><span>Buy for</span> £{app.addon_price}/Month</div>
                </div>

                <button>Buy now</button>
              </div>
              <div className={styles.addonmid}>
                <div className={styles.lefts}>
                  <img src={app.image_name != '-' ? app.image_name : addon1} alt="addon" />
                </div>
                <div className={styles.rights}>

                  <div className={styles.title}>{app.addon_feature_name}</div>
                  <div className={styles.desc}>{app.addon_feature_description}</div>
                </div>
              </div>

            </div>
          ))}
        </div>
      </div>
    </>
  )
}

export default Addon
