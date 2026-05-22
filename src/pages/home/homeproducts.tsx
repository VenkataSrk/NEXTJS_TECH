import React, { useState } from "react";
import Styles from './scss/home.module.scss'
import Sales from "./sale";
import Marketing from "./marketing";
import Commerce from "./commerce";
import Project from "./project";
const Homeproducts = () => {
    const [activeTab, setActiveTab] = useState("sales");
    return(
        <>
          <div className={Styles.homecontainer}>
                {/* <div className={Styles.homeleftcontainer}>
                    <div className={Styles.hometitle}>Home</div>
                    <div className={Styles.homesubtitle}>To capture the end-user's consent preferences, ensure that both the audio and video call are</div>
                </div> */}
                <div className={Styles.homerightcontainer}>
                    {/* <div className={Styles.filters}>
                        <span className={`${Styles.filterchip} ${activeTab === "sales" ? Styles.active : "" }`} onClick={() => setActiveTab("sales")} > Sales</span>
                        <span className={`${Styles.filterchip} ${activeTab === "marketing" ? Styles.active : "" }`} onClick={() => setActiveTab("marketing")} > Marketing</span>
                        <span className={`${Styles.filterchip} ${activeTab === "commerce" ? Styles.active : "" }`} onClick={() => setActiveTab("commerce")} > Commerce</span>
                        <span className={`${Styles.filterchip} ${activeTab === "project" ? Styles.active : "" }`} onClick={() => setActiveTab("project")} > Project</span>
                    </div> */}
                    <div className={Styles.filtercontainer}>
                         {activeTab === "sales" && <Sales /> }
                         {activeTab === "marketing" && <Marketing /> }
                         {activeTab === "commerce" && <Commerce /> }
                         {activeTab === "project" && <Project /> }
                    </div>
                </div>
          </div>
        </>
    )
}
export default Homeproducts