import React, { useEffect, useState } from 'react';
import styles from './scss/settings.module.scss';
import ContentLayout from '../../layout/contentLayout';
import Connectors from './connectors/connectors';
import ApiConfiguration from './apiconfiguration/apiConfiguration';
import AddDomain from './apiconfiguration/addDomain';
import ImportCsv from './apiconfiguration/importCsv';
import Csat from './csat/scss/csat';
import Setupemail from './csat/setupemail';
import Setupwhatsapp from './csat/setupwhatsapp';
import Setupsms from './csat/setupsms';
import Domainconfig from './domainconfig/domainconfig';
import Email from './channelConfiguration/email';
import Whatsappconfig from './channelConfiguration/whatsapp/whatsappconfig';
import Smsconfig from './channelConfiguration/sms/smsconfig';
import WebPushConfig from './channelConfiguration/webpushconfig/webPushConfig';

import WhatsAppTemplates from './templates/whatsappTemplates';
import Hubspot from '../hubspot/hubspotindex';
import { useLocation, useSearchParams } from 'react-router-dom';
import Apppush from './channelConfiguration/apppush/apppush';
import Initialhomepagenew from '../home/initialhomepagenew/initialhomepagenew';



const Settings = () => {
    let  activatedTab :any =useLocation();
    let activeHeaderSelected =activatedTab?.state?.data?.activeHeader??0;
    let activeMenuSelected =activatedTab?.state?.data?.activeMenu??0;
    console.log("activemenusdf",activeHeaderSelected,activeMenuSelected)
    const [activeMenu, setActiveMenu] = useState({ activeHeader: activeHeaderSelected??0, activeMenu: activeMenuSelected??0 });
    console.log("actmenu",activeMenu);
    const [searchParams] = useSearchParams();
    const [signUpStatus, setSignUpStatus] = useState<string | null>(null);
    console.log("tabactive",activatedTab)
  

    const handleSignupStatus = () => {
        setSignUpStatus(null);
    }

    const settingTab = [
        {
            tabheader: 'Connectors',
            tabmenu: [
                {
                    label: 'Connectors',
                    labelComp: <Connectors />
                },
                // {
                //     label: 'API configuration',
                //     labelComp: <ApiConfiguration />
                // },
                {
                    label: 'Import .csv files',
                    labelComp: <ImportCsv />
                },
            ]
        },
        {
            tabheader: 'Channel configuration',
            tabmenu: [
                {
                    label: 'Email',
                    labelComp: <Email />
                },
                {
                    label: 'SMS',
                    labelComp: <Smsconfig />
                },
                {
                    label: 'WhatsApp',
                    labelComp: <Whatsappconfig signUpStatus={signUpStatus} handleSignupStatus={handleSignupStatus}/>
                },
                {
                    label: 'App push',
                    labelComp:  <Apppush/>
                },
                {
                    label: 'Web Push',
                    labelComp: <WebPushConfig />
                },
                // {
                //     label: 'In-App',
                //     labelComp: <div>In-App</div>
                // },
                // {
                //     label: 'RCS',
                //     labelComp: <div>RCS</div>
                // },
            ]
        },
        // {
        //     tabheader: 'Basic Configuration',
        //     tabmenu: [
        //         {
        //             label: 'CSAT Configuration',
        //             labelComp: <Csat/>,
        //         },
        //     ]
        // },
          
            // {
            //     tabheader: 'Templates',
            //     tabmenu: [
            //         {
            //             label: 'Whatsapp',
            //             labelComp: <WhatsAppTemplates />
            //         },
            //         {
            //             label: 'RCS',
            //             labelComp: <div>RCS</div>
            //         },
            //     ]
            // },
            // {
            //     tabheader: 'Domains & URL',
            //     tabmenu: [
            //         {
            //             label: 'Domain configuration',
            //             labelComp:<Domainconfig/>
            //         }
            //     ]
            // },
    ]

    const handleTabChange = (headerIndex: number, menuIndex: number) => {
        if (document.startViewTransition) {
            document.startViewTransition(() => setActiveMenu({ activeHeader: headerIndex, activeMenu: menuIndex }))
        }
        else {
            setActiveMenu({ activeHeader: headerIndex, activeMenu: menuIndex });            
            activeMenuSelected =headerIndex;
            activeHeaderSelected=menuIndex;
        }


    }


    useEffect(() => {
        if(searchParams.get('status')){
            setSignUpStatus(searchParams.get('status'));
            setActiveMenu({ activeHeader: '1', activeMenu: '2' });
        }        
    }, [searchParams])
   


    console.log("activemenuu", activeMenu)
    return (
        <ContentLayout noPadding={true}>
            <div className={styles.settingssec}>
                <div className={styles.leftsettingstabsec}>

                    {settingTab.map((tab, headerIndex) => (
                        <div key={headerIndex}>
                            <h4 style={{marginTop:"8px"}}>{tab.tabheader}</h4>
                            <ul>
                                {tab.tabmenu.map((menu, menuIndex) => (
                                    <li
                                        key={menuIndex}
                                        className={`${(activeMenu.activeHeader === headerIndex && activeMenu.activeMenu === menuIndex) ? styles.activemenuitem : ''}`}
                                        onClick={() => handleTabChange(headerIndex, menuIndex)}>{menu.label}</li>
                                ))}
                            </ul>
                        </div>
                    ))}
                </div>
                <div className={`${styles.rightsettingsmenu} rightsettingmenutab`}>
                    {
                        settingTab[activeMenu.activeHeader].tabmenu[activeMenu.activeMenu].labelComp
                    }

                </div>
            </div>

        </ContentLayout>
    )
}

export default Settings
