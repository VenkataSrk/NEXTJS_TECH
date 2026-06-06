import React, { useEffect, useState } from 'react'
import styles from './scss/connectors.module.scss';
import { assets } from '../../../styles/assets/assets';
import { useNavigate } from 'react-router-dom';
import _ from 'lodash';
import { whatsappService } from '../../../base/service/apiservice';
import { commonDataSliceActions } from '../../../store/slice/commonSlice/commonSlice';
import { useDispatch, useSelector } from 'react-redux';

const Connectors = () => {
    const navigate = useNavigate();
    const dispatch= useDispatch();
    const [connectorsList, setConnectorsList] : any = useState([]);
    const connectorsRefresh = useSelector((state: any) => state.commonData.connectorsRefresh);
    useEffect(() => {
        const getConnectorsList  = async() => {
            try {
                const resp = await whatsappService.get("/appList");
                console.log("resp--------", resp);
                if(resp.data.statusCode === 200){
                    setConnectorsList(resp?.data?.result)
                } 
            } catch (err) {
                console.error("err--------", err);
            }
        };
        getConnectorsList();
    },[])

    useEffect(()=>{
        if(connectorsRefresh){
            const getConnectorsList  = async() => {
            try {
                const resp = await whatsappService.get("/appList");
                console.log("resp--------", resp);
                if(resp.data.statusCode === 200){
                    setConnectorsList(resp?.data?.result)
                } 
            } catch (err) {
                console.error("err--------", err);
            }
        };
        getConnectorsList();
        dispatch(commonDataSliceActions.setConnectorsRefresh(false));
        }
    },[connectorsRefresh])


    const clickToList = (data:any) => {
       (data === 1) ? navigate("/hubspotconnectionlist") : navigate("/hubspotconnection");
    };

  const formattedConnectorsList : any = _(connectorsList)
    .groupBy((o: any) => (o.industryType))
    .map((list: any, type: any) => ({ type, list }))
    .value();


    return (
        <div className={`${styles.connectorssec} ${styles.ticketright} `}>
            <div className={styles.maincontainer}>

          
            {/* <div className={styles.headersec}>
                <h4>Connectors</h4>
                <p>Integrate your data sources to build a unified customer view for precise segmentation and measurable value delivery.</p>
            </div> */}
            <div className={styles.topheading}>
                        <div className={styles.tophdlefts}>
                            <h2>Connectors</h2>
                            <p className={styles.headerdesctxt}>Integrate your data sources to build a unified customer view for precise segmentation and measurable value delivery.</p>
                        </div>
                    </div>
            {/* {formattedConnectorsList.length > 0 && formattedConnectorsList.map((data : any) => {
                return(
                    <div className={styles.menusec}>
                        <div className={styles.headersec}>
                            <h4>{data[0]}</h4>
                            <p>Import and unify relationship data to enhance customer engagement and lifecycle management.</p>
                        </div>
                        <div className={styles.boxgroups}>
                        {data[1].length > 0 && data[1].map((data : any) => {
                            return(
                                    <div className={styles.boxes} onClick={()=> navigate("/hubspotconnectionlist")}>
                                        <div className={styles.header}>
                                            <img src={assets.hubspot_icon} alt="Salesforce" className={styles.logo} />
                                            <span className={styles.status}>{data?.isConnected === 1 ? "Connected" : "Not yet Connected"}</span>
                                        </div>
                                        <div className={styles.body}>
                                            <h4>{data?.name}</h4>
                                            <p>{data?.description}</p>
                                        </div>
                                    </div>
                            )})}
                        </div>
                    </div>
                )})} */}
                {formattedConnectorsList.length > 0 && formattedConnectorsList.map((data : any) => {
                
                    return(
                        <div className={styles.menusec}>
                            <div className={styles.headersec}>
                                <h4>{data?.type}</h4>
                                <p>Import and unify relationship data to enhance customer engagement and lifecycle management.</p>
                            </div>
                            <div className={styles.boxgroups}>
                                {data?.list.length > 0 && data?.list?.map((data : any) => {
                            return(
                                    <div className={styles.boxes} 
                                    // onClick={()=>clickToList(data?.isConnected)}
                                    >
                                        <div className={styles.header}>
                                            <img src={assets.hubspot_icon} alt="connectors_url" className={styles.logo} />
                                            {/* <span className={styles.status}>{data?.isConnected === 1 ? "Connected" : "Not yet Connected"}</span> */}
                                            <span className={`${styles.statuselem} ${data?.isConnected === 1 ? styles.connected : styles.notconnected}`}>{data?.isConnected === 1 ? "Connected" : "Not yet Connected"}</span>
                                        </div>
                                        <div className={styles.body}>
                                            <h4>{data?.connector_name}</h4>
                                            <p>Only the administrator needs to establish the connection, others can access it afterwards..</p>
                                        </div>
                                    </div>
                            )})}
                            </div>
                        </div>
                    )
                })}
            {/* <div className={styles.menusec}>
                <div className={styles.headersec}>
                    <h4>Online Advertising Platform</h4>
                    <p>Link advertising platforms to refine audience targeting and measure campaign ROI.</p>
                </div>
                <div className={styles.boxgroups}>
                    <div className={styles.boxes}>
                        <img src={assets.facebook_icon} alt="facebook_icon" />
                        <p>Facebook</p>
                    </div>
                </div>
            </div>
            <div className={styles.menusec}>
                <div className={styles.headersec}>
                    <h4>Cloud Data Platform</h4>
                    <p>Centralize customer data from multiple systems for analytics, segmentation, and value modeling.</p>
                </div>
                <div className={styles.boxgroups}>
                    <div className={styles.boxes}>
                        <img src={assets.twilio_icon} alt="twilio_icon" />
                        <p>Twilio Segment</p>
                    </div>
                    <div className={styles.boxes}>
                        <img src={assets.aws_icon} alt="aws_icon" />
                        <p>AWS S3</p>
                    </div>
                    <div className={styles.boxes}>
                        <img src={assets.redshift_icon} alt="redshift_icon" />
                        <p>Redshift</p>
                    </div>
                    <div className={styles.boxes}>
                        <img src={assets.bigquery_icon} alt="bigquery_icon" />
                        <p>BigQuery</p>
                    </div>
                    <div className={styles.boxes}>
                        <img src={assets.snowflake_icon} alt="snowflake_icon" />
                        <p>Snowflake</p>
                    </div>
                </div>
            </div>
            <div className={styles.menusec}>
                <div className={styles.headersec}>
                    <h4>Commerce Platform</h4>
                    <p>Connect commerce platforms to track purchase behavior and deliver personalized offers.</p>
                </div>
                <div className={styles.boxgroups}>
                    <div className={styles.boxes}>
                        <img src={assets.shopify_icon} alt="shopify_icon" />
                        <p>Shopify</p>
                    </div>
                    <div className={styles.boxes}>
                        <img src={assets.magento_icon} alt="magento_icon" />
                        <p>Magento</p>
                    </div>
                </div>
            </div> */}
        </div>
          </div>
    )
}

export default Connectors

//  return (
//         <div className={styles.connectorssec}>
//             <div className={styles.headersec}>
//                 <h4>Connectors</h4>
//                 <p>Integrate your data sources to build a unified customer view for precise segmentation and measurable value delivery.</p>
//             </div>
//             <div className={styles.menusec}>
//                 <div className={styles.headersec}>
//                     <h4>CRM</h4>
//                     <p>Import and unify relationship data to enhance customer engagement and lifecycle management.</p>
//                 </div>
//                 <div className={styles.boxgroups}>
//                     {/* <div className={styles.boxes}>
//                         <img src={assets.salesforce_icon} alt="salesforce_icon" />
//                         <p>Salesforce</p>
//                     </div> */}
//                     {/* <div className={styles.boxes} onClick={()=> navigate("/hubspotconnectionlist")}>
//                         <img src={assets.hubspot_icon} alt="hubspot_icon" />
//                         <p>HubSpot</p>
//                     </div> */}
//                     <div className={styles.boxes} onClick={()=> navigate("/hubspotconnectionlist")}>
//                         <div className={styles.header}>
//                             <img src={assets.hubspot_icon} alt="Salesforce" className={styles.logo} />
//                             <span className={styles.status}>Not yet Connected</span>
//                         </div>

//                         <div className={styles.body}>
//                             <h4>HubSpot</h4>
//                             <p>Integrate HubSpot for seamless lead and campaign management.</p>
//                         </div>
//                     </div>
//                 </div>
//             </div>
//             {/* <div className={styles.menusec}>
//                 <div className={styles.headersec}>
//                     <h4>Online Advertising Platform</h4>
//                     <p>Link advertising platforms to refine audience targeting and measure campaign ROI.</p>
//                 </div>
//                 <div className={styles.boxgroups}>
//                     <div className={styles.boxes}>
//                         <img src={assets.facebook_icon} alt="facebook_icon" />
//                         <p>Facebook</p>
//                     </div>
//                 </div>
//             </div>
//             <div className={styles.menusec}>
//                 <div className={styles.headersec}>
//                     <h4>Cloud Data Platform</h4>
//                     <p>Centralize customer data from multiple systems for analytics, segmentation, and value modeling.</p>
//                 </div>
//                 <div className={styles.boxgroups}>
//                     <div className={styles.boxes}>
//                         <img src={assets.twilio_icon} alt="twilio_icon" />
//                         <p>Twilio Segment</p>
//                     </div>
//                     <div className={styles.boxes}>
//                         <img src={assets.aws_icon} alt="aws_icon" />
//                         <p>AWS S3</p>
//                     </div>
//                     <div className={styles.boxes}>
//                         <img src={assets.redshift_icon} alt="redshift_icon" />
//                         <p>Redshift</p>
//                     </div>
//                     <div className={styles.boxes}>
//                         <img src={assets.bigquery_icon} alt="bigquery_icon" />
//                         <p>BigQuery</p>
//                     </div>
//                     <div className={styles.boxes}>
//                         <img src={assets.snowflake_icon} alt="snowflake_icon" />
//                         <p>Snowflake</p>
//                     </div>
//                 </div>
//             </div>
//             <div className={styles.menusec}>
//                 <div className={styles.headersec}>
//                     <h4>Commerce Platform</h4>
//                     <p>Connect commerce platforms to track purchase behavior and deliver personalized offers.</p>
//                 </div>
//                 <div className={styles.boxgroups}>
//                     <div className={styles.boxes}>
//                         <img src={assets.shopify_icon} alt="shopify_icon" />
//                         <p>Shopify</p>
//                     </div>
//                     <div className={styles.boxes}>
//                         <img src={assets.magento_icon} alt="magento_icon" />
//                         <p>Magento</p>
//                     </div>
//                 </div>
//             </div> */}
//         </div>
//     )


//  <div className={styles.menusec}>
//                 <div className={styles.headersec}>
//                     <h4>CRM</h4>
//                     <p>Import and unify relationship data to enhance customer engagement and lifecycle management.</p>
//                 </div>
//                 <div className={styles.boxgroups}>
//                     {/* <div className={styles.boxes}>
//                         <img src={assets.salesforce_icon} alt="salesforce_icon" />
//                         <p>Salesforce</p>
//                     </div> */}
//                     {/* <div className={styles.boxes} onClick={()=> navigate("/hubspotconnectionlist")}>
//                         <img src={assets.hubspot_icon} alt="hubspot_icon" />
//                         <p>HubSpot</p>
//                     </div> */}
//                     <div className={styles.boxes} onClick={()=> navigate("/hubspotconnectionlist")}>
//                         <div className={styles.header}>
//                             <img src={assets.hubspot_icon} alt="Salesforce" className={styles.logo} />
//                             <span className={styles.status}>Not yet Connected</span>
//                         </div>

//                         <div className={styles.body}>
//                             <h4>HubSpot</h4>
//                             <p>Integrate HubSpot for seamless lead and campaign management.</p>
//                         </div>
//                     </div>
//                 </div>
//             </div>