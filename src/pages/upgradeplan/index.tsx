import React from "react";
import styles from './scss/upgrade.module.scss'
import Deals from '../../assets/images/upgradeplan/deals.svg'



const Upgradeplan = ({ title, description, image, buttonText, onButtonPress }: any) =>{


    return(
        <>
           <div className={styles.upgradeplancontainer}>
               <div className={styles.maincontainer}>
                    <div className={styles.leftcontainer}>
                      <div className={styles.unlockdeals}>{title}</div>
                      <div className={styles.updatestitle}>{description}</div>
                      <button onClick={()=> onButtonPress?.()}>{buttonText}</button>
                    </div>
                    <div className={styles.rightcontainer}>
                       <img src={image} alt="Deals" />
                    </div>
               </div>
           </div>
        </>
    )
}
export default Upgradeplan;