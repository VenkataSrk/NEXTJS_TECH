import React, { useEffect, useState } from 'react';
import styles from '../../modules/settings/scss/settings.module.scss';
import ContentLayout from '../../layout/contentLayout';

import { assets } from '../../styles/assets/assets'
import Chatcomponent from '../../components/chatcomponent/chatcomponent';
import Createknowledge from './createknowledge';
import SipTrunk from './siptrunk';
import Scrapedwebsite from './scrapedwebsite';
import Library from './library';



const Knowledgebase = () => {
 

  return (
    <ContentLayout noPadding={true}>
      <div className={styles.settingssec}>
        <div className={styles.leftsettingstabsec}>
          <div style={{marginTop:"8px"}} className={styles.createcamdiv}>
            <span 
              style={{ cursor: "pointer" }}>
              <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                <path d="M9.99999 0.00416565C4.48228 0.00416565 0.00415039 4.4823 0.00415039 10C0.00415039 15.5177 4.48228 19.9958 9.99999 19.9958C15.5177 19.9958 19.9958 15.5177 19.9958 10C19.9958 4.4823 15.5277 0.00416565 9.99999 0.00416565ZM14.2982 10.7497H10.7497V14.2982C10.7497 14.718 10.4198 15.0479 9.99999 15.0479C9.58016 15.0479 9.2503 14.718 9.2503 14.2982V10.7497H5.70178C5.29195 10.7497 4.95209 10.4198 4.95209 10C4.95209 9.58018 5.29195 9.25031 5.70178 9.25031H9.2503V5.70179C9.2503 5.29196 9.59016 4.9521 9.99999 4.9521C10.4098 4.9521 10.7497 5.29196 10.7497 5.70179V9.25031H14.2982C14.718 9.25031 15.0579 9.59017 15.0579 10C15.0579 10.4098 14.718 10.7497 14.2982 10.7497Z" fill="#00E2A4" />
              </svg>
            </span>
             Add Knowledge
          </div>
          <div  style={{marginBottom:"8px"}} className={`${styles.createcamdiv} inputsearch`}>
            <input type='search' placeholder='Search' />
          </div>
               
    
          
            <ul>
              <li
                className={`${styles.activemenuitem}`}
               
              >
                {/* Elevate the Brand */}
                dsd
              </li>
              <li
                
               
              >
                {/* Elevate the Brand */}
                Library
              </li>
               <li
                
               
              >
                {/* Elevate the Brand */}
                Saved
              </li>
            </ul>
            
          
          
          
        </div>
        <div className={`${styles.rightsettingsmenu} rightsettingmenu`}>
          <Createknowledge/>
          {/* <SipTrunk /> */}
          {/* <Scrapedwebsite /> */}
          {/* <Library /> */}
        </div>
      </div>

        
    </ContentLayout>
  );
};

export default Knowledgebase;
