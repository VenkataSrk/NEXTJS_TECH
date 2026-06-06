import { Link, useNavigate } from 'react-router-dom';

import styles from './scss/hubspot.module.scss';

// import Mapvalue from './hubspotmap';
import { useState } from 'react';
import { whatsappService } from '../../base/service/apiservice';
import { assets } from '../../styles/assets/assets';
import Viewhubspot from './viewhunspot';

type hubspotConnectionState = {
    connectionDetails: {
        connectionName: string,
        privateAppToken: string
    },
    isHubPrivToken: boolean,
    parametersMapping: Array<object>,
    contactSyncObj: object,
    refreshTime: number
}

const Hubspot = () => {
    const navigate = useNavigate();
    const [token, isToken] = useState(false);

    const [getHubspotConnect, setHubspotConnect] = useState<hubspotConnectionState>(
        {
            connectionDetails: {
                connectionName: '',
                privateAppToken: ''
            },
            isHubPrivToken: false,
            parametersMapping: [
                {
                    "worktualParams": "firstName",
                    "connectorsParams": "firstname"
                },
                {
                    "worktualParams": "lastName",
                    "connectorsParams": "lastname"
                },
                {
                    "worktualParams": "phoneNumber",
                    "connectorsParams": "phone"
                },
                {
                    "worktualParams": "whatsappNumber",
                    "connectorsParams": "hs_whatsapp_phone_number"
                },
                {
                    "worktualParams": "email",
                    "connectorsParams": "email"
                },
                {
                    "worktualParams": "lifeCycleStage",
                    "connectorsParams": "lifecyclestage"
                },
                {
                    "worktualParams": "postCode",
                    "connectorsParams": "zip"
                }

            ],
            contactSyncObj: {},
            refreshTime: 15,


        })


    const handleInputChange = (e: any) => {
        const { name, value } = e.target;
        setHubspotConnect({ ...getHubspotConnect, connectionDetails: { ...getHubspotConnect.connectionDetails, [name]: value, } })
    };

    const handleSubmit = async (typeOfButton: string) => {
        try{
        console.log("typeOfButton>>>>>>>>>", typeOfButton);
        const { connectionDetails, parametersMapping } = getHubspotConnect;
        if (!connectionDetails.connectionName.trim()) {
            alert("Connection Name cannot be empty");
            return;
        }

        if (!connectionDetails.privateAppToken.trim()) {
            alert("Private App Token cannot be empty");
            return;
        }
        // setHubspotConnect({ ...getHubspotConnect, isHubPrivToken: true });

        if (!connectionDetails.privateAppToken) {
            return true;
        }
        if (typeOfButton === 'verifyToken') {
            const payload = {
                token: connectionDetails.privateAppToken.trim()
            }
            whatsappService.post(payload, "/verifyAppToken").then((resp: any) => {


                if (resp.data.statusCode === 200) {
                    const syncObj: any = {};
                    parametersMapping.forEach((elem: any) => {
                        if (elem.connectorsParams) {
                            syncObj[elem.connectorsParams] = elem.worktualParams;
                        }
                    });
                    setHubspotConnect({ ...getHubspotConnect, isHubPrivToken: true, contactSyncObj: syncObj });
                } else {
                    console.log("else coming-----------")
                    setHubspotConnect({ ...getHubspotConnect, isHubPrivToken: false });
                    alert(resp.data.message);
                }
            }).catch((err: any) => {
                console.log("err---------", err);

            })
        }

        if (typeOfButton === 'connect') {
            // setLoader(!loader);

            const payload = {
              
                privateAppToken: getHubspotConnect.connectionDetails.privateAppToken.trim(),
                connectionName: getHubspotConnect.connectionDetails.connectionName,
                refreshTime: getHubspotConnect.refreshTime,
                contactSyncParam: getHubspotConnect.parametersMapping,
                contactSyncObj: getHubspotConnect.contactSyncObj,
                contactSyncType: 'pull'
            }

            whatsappService.post(payload, "/privateAppConnection").then((resp: any) => {
                    if (resp.data.statusCode === 200) {
                        //   setLoader(!loader);
                        //   notification.info({
                        //     message: `Connected Successfully`,
                        //     placement: "top",
                        //     className: "toastNotification success",
                        //     icon: <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none">
                        //       <path d="M12 2C17.5228 2 22 6.47715 22 12C22 17.5228 17.5228 22 12 22C6.47715 22 2 17.5228 2 12C2 6.47715 6.47715 2 12 2ZM15.2197 8.96967L10.75 13.4393L8.78033 11.4697C8.48744 11.1768 8.01256 11.1768 7.71967 11.4697C7.42678 11.7626 7.42678 12.2374 7.71967 12.5303L10.2197 15.0303C10.5126 15.3232 10.9874 15.3232 11.2803 15.0303L16.2803 10.0303C16.5732 9.73744 16.5732 9.26256 16.2803 8.96967C15.9874 8.67678 15.5126 8.67678 15.2197 8.96967Z" fill="#237B4B" />
                        //     </svg>,
                        //     closeIcon: <svg xmlns="http://www.w3.org/2000/svg" width="10" height="10" viewBox="0 0 10 10" fill="none">
                        //       <path d="M5.80082 4.99937L9.70415 1.09571C9.92548 0.874973 9.92548 0.51564 9.70415 0.294906C9.48282 0.0735729 9.12342 0.0735729 8.90335 0.294906L5.00002 4.19851L1.09668 0.294906C0.876616 0.0735729 0.515949 0.0735729 0.295883 0.294906C0.0745495 0.51564 0.0745495 0.874973 0.295883 1.09571L4.19922 4.99937L0.295883 8.90304C0.0745495 9.12377 0.0745495 9.48311 0.295883 9.70384C0.406549 9.81451 0.551083 9.86984 0.696283 9.86984C0.841483 9.86984 0.986616 9.81451 1.09668 9.70384L5.00002 5.80024L8.90335 9.70384C9.01335 9.81451 9.15855 9.86984 9.30375 9.86984C9.44895 9.86984 9.59348 9.81451 9.70415 9.70384C9.92548 9.48311 9.92548 9.12377 9.70415 8.90304L5.80082 4.99937Z" fill="#757676" />
                        //     </svg>,
                        //     duration: 3,
                        //     top: 70,
                        //   });
                        alert(resp.data.message);
                        navigate('/connectors_config');

                    } else {
                        alert(resp.data.message);
                        navigate('/connectors_config');
                        //   setLoader(!loader);
                        //   notification.info({
                        //     message: data.message,
                        //     placement: "topRight",
                        //     className: "toastNotification error",
                        //     icon: <span className="icon__Error24" />,
                        //     closeIcon: <span className="icon__IconCloseNew16" />,
                        //     duration: 3,
                        //     top: 70,
                        //   });
                    }
                })

                .catch((err) => {
                    console.log("hubspot/verifyAppToken catch", err);
                     navigate('/connectors_config');
                })
        }
    }
    catch(error :any){
        console.log("API ERROR:",error)
    }
}



    return (
        //  <ContentLayout noPadding={false} >
<div className={styles.ticketright}>
            {
                !getHubspotConnect.isHubPrivToken ? <>
                    <div className='breadcrumbs'>
                        <ol>
                            <li>
                                <Link to='/connectors_config'>Connectors</Link>
                            </li>
                            <li>HubSpot</li>
                        </ol>
                    </div>
                    <div className={styles.topheading}>
                        <div className={styles.tophdlefts}>
                            <h2>Connect with HubSpot account</h2>
                            <p className={styles.headerdesctxt}>Enter Your connect name and API key to connect to your Worktual account</p>
                        </div>
                    </div>
                    <form action="" className='formsec'>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="domainname">Connection  Name<span className='required'>*</span></label>
                            </div>
                            <div className='formelements'>
                                <input onChange={handleInputChange} type="text" name="connectionName" id="" placeholder='Enter connection name' />
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="domainname">Private app access token<span className='required'>*</span></label>
                            </div>
                            <div className='formelements'>
                                <input onChange={handleInputChange} type="password" name="privateAppToken" id="" placeholder='Enter API key' />
                            </div>
                        </div>
                        <div className='formbtns'>
                            <button type='button' onClick={() => { navigate('/connectors_config') }} className='cancelbtn'> Cancel</button>
                            <button type='button' className='primarybtn' onClick={() => handleSubmit(getHubspotConnect.isHubPrivToken ? "connect" : "verifyToken")}
                            >Verify Token</button>
                        </div>
                    </form>
                </> :
                    <>
                        <div className={styles.mainheader}>
                            <h5>Hubspot to Campaign management</h5>
                            <p>Microcopy for Hubspot to Campaign management</p>
                        </div>
                        <div className={`${styles.parameterssec} ${styles.parasechunspot}`}>

                            <div className={styles.paramslefts}>
                                <h4 className={styles.headertxt}>Contacts</h4>
                                <p className={styles.subtxt}>Microcopy for Hubspot to Campaign management</p>
                                <div className={`${styles.paramsrow} ${styles.paramsrowheader}`}>
                                    <div className={styles.parmbox}>
                                        <h5>
                                            HubSpot
                                        </h5>
                                    </div>
                                    <div className={styles.parmbox}>
                                        <h5>
                                            Campaign Management
                                        </h5>
                                    </div>

                                </div>
                                {getHubspotConnect.parametersMapping.map((data: any, indx: number) => {
                                    return (
                                        <div key={indx}>
                                            <div className={styles.paramsrow}>
                                                <div className={`${styles.parmbox} ${styles.paramboxgrey}`}>
                                                    {data.connectorsParams}
                                                </div>
                                                <img className={styles.arrow} src={assets.rightarrow_exchange} alt="rightarrow_exchange" />
                                                <div className={styles.parmbox}>
                                                    {data.worktualParams}
                                                </div>
                                            </div>
                                        </div>
                                    )

                                })}
                            </div>


                        </div>
                         <div className='formbtns'>
                            <button type='button' onClick={()=>{navigate('/connectors_config')}} className='cancelbtn'> Cancel</button>
                            <button type='button' className='primarybtn' onClick={() => handleSubmit( "connect")}
                            >Connect</button>
                        </div>
                    </>

            }

           {/* <Viewhubspot/> */}
</div>

    //    </ContentLayout>

    )
}

export default Hubspot
