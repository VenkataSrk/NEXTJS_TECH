import React, { useEffect, useRef, useState } from 'react'
import styles from "./scss/smsconfig.module.scss"
import BootstrapTable from 'react-bootstrap-table-next';
import paginationFactory from 'react-bootstrap-table2-paginator';
import { tableOptions } from '../../../../components/custom_components/tableOptions';
import { assets } from '../../../../styles/assets/assets';
import { whatsappService } from '../../../../base/service/apiservice';
import { useToast } from '../../../../components/custom_components/useToast';
import BlurToastDemo from '../../../../components/custom_components/blurtast/blurtoast';
import Loaders from '../../../../components/loader/loader';
import ActionsDropdown from '../../../../components/custom_components/actionsdropdown/actionsdropdown';
import StatusDropdown from '../../../../components/custom_components/statusdropdown/statusdropdown';
import WorktualSearch from '../../../../components/custom_components/woktualSearch';
import WorktualModal from '../../../../components/custom_components/worktualModal';
import { config } from '../../../../base/constant';
import { useSearchParams } from 'react-router-dom';
import { decryptAESCVM } from '../../../../base/utils';
import { SetItemInStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import Cookies from '../../../../utils/cookies';
import SocketService from '../../../../base/service/socketService';
import { useDispatcher } from '../../../../store/redux-store/dispatch';
import Spinner from '../../../../components/custom_components/spinner/spinner';

function Smsconfig() {
    const [smsDetails, setSmsDetails] = useState<any>([]);
    const { toastOpen, toastMessage, showToast, closeToast } = useToast();
    const [loading, setLoading] = useState(true);
    const [loader, setLoader] = useState<any>(false);
    const [onEdit, setOnEdit] = useState<any>(false);
    const [smsDetailsListCopy, setSmsDetailsListCopy] = useState<any>([]);
    const [searchName, setSearchName] = useState<string>('');
    const [smsData, setSmsDatas] = useState<any>({
        senderNumber: '',
        aliasName: '',
        senderId: '',
        isBot: 0
    });
    const [searchParams] = useSearchParams();

    const dispatch = useDispatcher();
    const isCVM :any= useRef<any>(false);


       useEffect(()=>{
      
          const init = async () => {
            try {
              const queryparams: any = searchParams.get('cvm_data');
              if(queryparams?.length>0){  
                  isCVM.current = true;
                  setLoader(true);
                  const decodedQueryparams: any = await decryptAESCVM(queryparams.replaceAll(" ", "+"));
                  const params =  JSON?.parse(decodedQueryparams);                      
                  const token = params?.token ;
                  console.log("token",token)
                  // const token = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1dWlkIjoiNjkzMDNjYjEtOWU0OC00YzQwLWIxYTYtMjQzZTkwMmQ0NzZlIiwidXNlcm5hbWUiOiJtZWRld2F4NTk4QGt5dHN0b3JlLmNvbSIsImRldmljZUlkIjoiMWViODgwNTItMzZlYi00NDdlLTljZWQtOWM4MzU3MjM3MjM4Iiwic2lwTG9naW5JZCI6IjI5NzkwIiwicm9sZUlkIjoiMyIsImRvbWFpbklkIjoxODY3NywiZXh0IjoiMjAwIiwiY29tcGFueUlkIjoxODc1MCwiZW5ldGVwcmlzZWlkIjoyMzQ4MSwiaG9zdEFkZHJlc3MiOiIyMzQ4MS51cmNoYXQud29ya3R1YWwuY28udWsiLCJvcmRlcklkIjoyNjQxOSwiZGlyVXNlcklkIjoxNTI0MTAsInByb2ZpbGVOYW1lIjoibWVkZXdheCBreXRzdG9yZSIsInByb2ZpbGVJbWciOiJodHRwczovL3Vyc3RvcmFnZS53b3JrdHVhbC5jby51ay9maWxlcy91c2VyLzE4NzUwLzI5NzkwXzIwMC9ib29rbmcuanBlZ18xNzUxOTg1NjA5OTUyLnBuZyIsInNvdXJjZSI6IldlYiIsImlwYWRkcmVzcyI6IjEzNi4xNDQuNTYuMjU1OjQ0MyIsImNsaSI6IiIsInNpcF9wYXNzd29yZCI6IjlUM2lkYnBEIiwiY3VzdG9tZXJfaWQiOjE4ODUxLCJjb3VudHJ5X2NvZGUiOiIwIiwibG9jYWxfbnVtYmVyIjoiMjE4NDU0OTg0MSIsImFwcF9sb2dfaWQiOjI4NDkyLCJvcmRlcl9pZCI6MjY0MTksImRwX3Bhc3N3b3JkIjoiZDg3YzQzYWI2NTY1YjQzIiwicGxhdGZvcm0iOjIzNDgxLCJ1c2VyX3N0YXR1cyI6IkF2YWlsYWJsZSIsInNpdGVfaWQiOjE3Mjg0LCJkb21haW5fbmFtZSI6IjIzNDgxLmNhbGwud29ya3R1YWwuY28udWsiLCJtZWV0aW5nX2lkIjoiNjY5LTYzMS0wNjI2IiwidmlkZW9fbWVldF9kb21haW4iOiIyMzQ4MS51cm1lZXQud29ya3R1YWwuY28udWsiLCJwcm9maWxlX2ltYWdlX3RodW1ibmFpbCI6Imh0dHBzOi8vdXJzdG9yYWdlLndvcmt0dWFsLmNvLnVrL2ZpbGVzL3VzZXIvMTg3NTAvMjk3OTBfMjAwL2Jvb2tuZy5qcGVnXzE3NTE5ODU2MDk5NTIucG5nIiwiZGlyX3VzZXJfaWQiOjE1MjQxMCwiZG9tYWluX2lkIjoxODY3Nywic2lwX2xvZ2luX2lkIjoiMjk3OTAiLCJjYWxsZXJfaWQiOiJtZWRld2F4IGt5dHN0b3JlIiwiY29tcGFueV9pZCI6MTg3NTAsImVudGVycHJpc2VpZCI6MjM0ODEsInJvbGVfaWQiOiIzIiwicHJvZmlsZV91cmwiOiJodHRwczovL3Vyc3RvcmFnZS53b3JrdHVhbC5jby51ay9maWxlcy91c2VyLzE4NzUwLzI5NzkwXzIwMC9ib29rbmcuanBlZ18xNzUxOTg1NjA5OTUyLnBuZyIsIkVtYWlsIjoibWVkZXdheDU5OEBreXRzdG9yZS5jb20iLCJob3N0X2FkZHJlc3MiOiIyMzQ4MS51cmNoYXQud29ya3R1YWwuY28udWsiLCJmaXJzdF9uYW1lIjoibWVkZXdheHMiLCJsYXN0X25hbWUiOiJreXRzdG9yZSIsImNsaWVudF9kb21haW4iOnsiaXZyIjoiaHR0cHM6Ly9pdnIud29ya3R1YWwuY28udWsiLCJ1cmFwaSI6Imh0dHBzOi8vdXJhcGkud29ya3R1YWwuY28udWsiLCJ1cnNpcCI6Imh0dHA6Ly91cnNpcC1hcGkud29ya3R1YWwuY28udWsiLCJzdG9yYWdlIjoiaHR0cHM6Ly91cnN0b3JhZ2Uud29ya3R1YWwuY28udWsiLCJhdWRpb3NndyI6ImF1ZGlvc2d3dGgud29ya3R1YWwuY28udWsiLCJ2aWRlb3NndyI6InZpZGVvc2d3dGgud29ya3R1YWwuY28udWsiLCJyZWNvcmRpbmciOiJodHRwczovL3JlY29yZGluZy53b3JrdHVhbC5jby51ayIsInZpZGVvbWVldCI6Imh0dHBzOi8vbWVldC53b3JrdHVhbC5jby51ayIsInZvaWNlbWFpbCI6Imh0dHBzOi8vdm9pY2VtYWlsLndvcmt0dWFsLmNvLnVrIiwiY2hhdHNvY2tldCI6Imh0dHBzOi8vdXJjaGF0LWlvLndvcmt0dWFsLmNvLnVrIiwibWVldHNvY2tldCI6Imh0dHBzOi8vdXJtZWV0LWlvLndvcmt0dWFsLmNvLnVrIiwiYXVkaW90dXJudjQiOiJ0dXJuOmF1ZGlvdHVybnY0dGgud29ya3R1YWwuY28udWs6MzQ3OCIsImF1ZGlvdHVybnY2IjoidHVybjphdWRpb3R1cm52NnRoLndvcmt0dWFsLmNvLnVrOjM0NzgiLCJ2aWRlb3R1cm52NCI6InR1cm46dmlkZW90dXJudjR0aC53b3JrdHVhbC5jby51azozNDc4IiwidmlkZW90dXJudjYiOiJ0dXJuOnZpZGVvdHVybnY2dGgud29ya3R1YWwuY28udWs6MzQ3OCJ9LCJpc190b3VyX2ZsYWciOjEsImRhc2hib2FyZF90b3VyIjoxLCJjaGF0Ym90X3RvdXIiOjEsImNoYXRmbG93X3RvdXIiOjAsInBsYW5faWQiOiIxNjAiLCJpc19jYXNfdXNlciI6MSwiY2Nhc19yb2xlX2luZm8iOiIzIiwiY2FzX2RvbWFpbiI6IiIsImlzX3VjYXNfdXNlciI6MCwidWNhc19yb2xlX2luZm8iOiIwIiwidXNyX2lkIjoyNTI4OSwidXNlcklkIjoyNzc3NiwiaXNfY3JtX3VzZXIiOjAsImNybV9yb2xlX2luZm8iOiIiLCJjcm1fZG9tYWluIjoiIiwiY29tcGFueU5hbWUiOiJ3c3Rlc3RtZWRld2F4IiwicHJvZHVjdF9pZCI6IjE1IiwicGFyZW50X2lkIjo1NiwiaWF0IjoxNzU5NDA2NDIxLCJleHAiOjE3NjQ5MzYwMjF9.pg6Xj9x7zUZIGVG0rMaMBTSkGQeRHqbrElihzDfCreg" ;
                 
                  try {                
                      SetItemInStorage(LocalStorage.ACCESS_TOKEN, token);
                      let tokenData: any = Cookies.get();
                      if (!tokenData['ssoToken']) {
                        Cookies.set("ssoToken", token, 365);
                      } 
                       new SocketService(token, dispatch);                
                  
                  } catch (e: any) {
                   console.log("error",e)
                  } 
              }
            } catch (error) {
              console.log("API ERROR",error)
            }finally{        
             setLoader(false);        
             await getSmsList();
             const loaders = document.getElementById('contact_center_mainloader');
                if (loaders) {
                    loaders.style.display = 'none';
                }
            }  
          }
          init();
       },[])

    const getSmsList = async() => {
        try {
            setLoader(true);
            await whatsappService.get(`/get_sms_number`)
                .then((response: any) => {
                    if (response.data.statusCode === 200 && response?.data?.getSmsNumbers?.length > 0) {
                        setSmsDetails(response?.data?.getSmsNumbers);
                        setSmsDetailsListCopy(response?.data?.getSmsNumbers);
                        setLoader(false);
                    } else {
                        setSmsDetails([])
                        setLoader(false);
                    }
                }).catch((error: any) => {
                    console.log(error)
                    setSmsDetails([])
                    setLoader(false);
                }).finally(() => {
                    setLoading(false);
                    setLoader(false);
                });
        } catch (error) {
            console.log(error)
            setLoader(false);
        }
    }

    const editFunction = async (smsDetail: any) => {
        setOnEdit(true);
        setSmsDatas({
            senderNumber: smsDetail?.senderNumber ?? '',
            aliasName: smsDetail?.aliasName ?? '',
            senderId: smsDetail?.senderId ?? '',
            isBot: smsDetail?.isBot ?? smsDetail?.SMSBot ?? 0,
        });
    };

    const updateSmsDeatils = async () => {
        try {
            setLoader(true);
            const data = {
                senderNumber: smsData?.senderNumber ?? null,
                aliasName: smsData?.aliasName ?? null,
                senderId: smsData?.senderId ?? null,
                SMSBot: smsData?.isBot ?? null,
            };
            const response = await whatsappService.put(data, `/update_sms_details`);
            if (response?.data?.statusCode === 200) {
                setSmsDatas({ aliasName: '', senderId: '', isBot: 0 });
                setOnEdit(false);
                getSmsList();
                showToast("✅ Configuration Update successfully!");
            } else {
                setSmsDatas({ aliasName: '', senderId: '', isBot: 0 });
                setOnEdit(false);
                getSmsList();
                showToast("❌ Failed to update SMS configuration");
            }
        } catch (error) {
            setSmsDatas({ aliasName: '', senderId: '', isBot: 0 });
            setOnEdit(false);
            getSmsList();
            showToast("⚠️ Failed to update SMS configuration");
        }
    };

    const handleToggleSMS = async (row: any) => {
        const updatedStatus = row?.SMSEnable === 1 ? 0 : 1;
        try {
            const data: any = {
                domainId: row.domainId,
                senderNumber: row.senderNumber,
                SMSEnable: updatedStatus
            };
            const response = await whatsappService.put(data, `/update_sms_details`);
            if (response?.data?.statusCode === 200) {
                getSmsList();
                if (row?.SMSEnable) {
                    showToast("SMS status disabled successfully!");
                } else {
                    showToast("SMS status enabled successfully!");
                }
            } else {
                getSmsList();
                showToast("❌ Failed to update SMS status");
            }
        } catch (err) {
            getSmsList();
            console.error("❌ Error updating SMS status", err);
            showToast("⚠️ Error updating SMS status");
        }
    };

    const setSearchCampaignName = (data: string) => {
        setSearchName(data);
        if (data && data.trim() !== '') {
            const lowerCaseData = data.toLowerCase();
            const result = smsDetailsListCopy?.filter((list: any) =>
                list?.senderNumber?.toLowerCase().includes(lowerCaseData) ||
                list?.aliasName?.toLowerCase().includes(lowerCaseData) ||
                list?.senderId?.toLowerCase().includes(lowerCaseData)
            );
            setSmsDetails(result);
        } else {
            setSmsDetails(smsDetailsListCopy);
        }
    };

    const columns = [
        {
            dataField: "sender_number",
            text: "Sender Number",
            sort: true,
            formatter: (cell: string, row: { senderNumber: string }) => {
                return <span className='clickabletxt'>{row?.senderNumber}</span>;
            }
        },
        // {
        //     dataField: "action",
        //     text: "",
        //     formatter: (_: any, row: any) => {
        //         return (
        //             <ActionsDropdown
        //                 id={row.id}
        //                 onEdit={() => { editFunction(row) }}
        //             />
        //         );
        //     }
        // },
        // {
        //     dataField: "SMSEnable",
        //     text: "Enable For SMS",
        //     formatter: (cell: any, row: any) => {
        //         return (
        //             <StatusDropdown
        //                 initialStatus={cell === 1 ? "Active" : "Inactive"}
        //                 onChange={() => handleToggleSMS(row)}
        //             />
        //         );
        //     }
        // },
        {
            dataField: "alias_name",
            text: "Alias Name",
            formatter: (cell: string, row: { aliasName: string }) => {
                return <div>{row?.aliasName}</div>;
            },
            sort: true,
        },
         {
      dataField: "isactive",
      text: "Status",
      sort: true,
      formatter: (item: number) =>
       
          <span className={`${styles.statuselem} ${styles.pending}`}>Connected</span>
       
         
       
    },
        {
            dataField: "senderId",
            text: "Sender Id",
            sort: true,
            formatter: (cell: string, row: { senderId: string }) => {
                return <div>{row?.senderId}</div>;
            },
        },
    ];

    const tableOpt = {
        ...tableOptions, sizePerPageList: [...tableOptions.sizePerPageList, {
            text: "All",
            value: 20,
        }]
    }

    return (
        <>
            {loader ? (
                 <div className='loadersinner'><Spinner /></div>
                //  <div className='loadersinner'><Loaders /></div>
                // <Loaders />
            ) : (
                <>
                    <div className={styles.innerbody}>
                        {/* <div className={styles.headerarea}>
                            <div className={styles.lefts}>
                                <h5>SMS configuration</h5>
                                <p>Microcopy for SMS configuration</p>
                            </div>
                            <div className={styles.rights}>
                                <button className='primarybtn' onClick={() => window.location.href = `${config().MYACCOUNT_URL}/number-management`}>Add Number</button>
                            </div>
                        </div> */}
                         <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>SMS configuration</h2>
                    <p className={styles.headerdesctxt}>Set up SMS to deliver campaigns and notifications smoothly.</p>
                </div>
                <div className='formbtns'>
                    <button className='primarybtn' onClick={() => {
                                        if (!isCVM.current) {

                                            window.location.href = `${config().MYACCOUNT_URL}/number-management`
                                        }

                                        else {
                                            window.parent.postMessage(
                                                {
                                                    type: "CVM_IFRAME",
                                                    message: {
                                                        navigate: "myaccount"
                                                    },
                                                },
                                                "*"
                                            )
                                        }
                    }}>Add Number</button>
                </div>
            </div>   
            { smsDetails?.length !== 0 ? 
                        <div style={{ width: "220px", margin: "20px 0" }}>
                            <WorktualSearch onChange={(e: any) => setSearchCampaignName(e.target.value)} value={searchName} />
                        </div>:""}
                        {!loading && smsDetails?.length > 0 && (
                            <div className="table-wrapper">
                            <div className='desktoptable'>
                                <BootstrapTable
                                    keyField="id"
                                    data={smsDetails}
                                    columns={columns}
                                    pagination={paginationFactory(tableOpt)}
                                />
                            </div></div>
                        
                        )}
                        {!loading && smsDetails?.length === 0 && (
                            <div style={{ display: "grid", placeItems: "center", height: "calc(100% - 132px)" }}>
                                <div className='pannelcenter'>
                                    <div className='imgcon'>
                                        <img src={assets.emptysms}></img>
                                    </div>
                                    <div className='info'>
                                        <h5>Configure SMS Channel</h5>
                                        <p>Set up SMS to deliver campaigns and notifications smoothly.</p>
                                    </div>
                                    <button className='primarybtn' onClick={() => {
                                            if (!isCVM.current) {

                                                window.location.href = `${config().MYACCOUNT_URL}/number-management`
                                            }
                                        
                                        else{
                                            window.parent.postMessage(
                                            {
                                                type: "CVM_IFRAME",
                                                message: {
                                                    navigate:"myaccount"
                                                },
                                            },
                                            "*"
                                        )
                                            }
                                     } }>Add Number</button>
                                </div>
                            </div>)}
                    </div>

                    {/* ✅ Toast Component */}
                    <BlurToastDemo
                        open={toastOpen}
                        message={
                            <div style={{ display: "flex", alignItems: "center", gap: "8px" }}>
                                <span>{toastMessage}</span>
                                <svg
                                    style={{ cursor: "pointer" }}
                                    onClick={closeToast}
                                    xmlns="http://www.w3.org/2000/svg"
                                    width="11"
                                    height="10"
                                    viewBox="0 0 11 10"
                                    fill="none"
                                >

                                    <path
                                        d="M6.29984 4.99937L10.2032 1.09571C10.4245 0.874973 10.4245 0.51564 10.2032 0.294906C9.98184 0.0735729 9.62244 0.0735729 9.40237 0.294906L5.49904 4.19851L1.59571 0.294906C1.37564 0.0735729 1.01497 0.0735729 0.794906 0.294906C0.573573 0.51564 0.573573 0.874973 0.794906 1.09571L4.69824 4.99937L0.794906 8.90304C0.573573 9.12377 0.573573 9.48311 0.794906 9.70384C0.905573 9.81451 1.05011 9.86984 1.19531 9.86984C1.34051 9.86984 1.48564 9.81451 1.59571 9.70384L5.49904 5.80024L9.40237 9.70384C9.51237 9.81451 9.65757 9.86984 9.80277 9.86984C9.94797 9.86984 10.0925 9.81451 10.2032 9.70384C10.4245 9.48311 10.4245 9.12377 10.2032 8.90304L6.29984 4.99937Z"
                                        fill="white"
                                    />
                                </svg>
                            </div>
                        }
                    />
                </>
            )}
            <WorktualModal
                show={onEdit}
                title="Edit SMS Number"
                onClose={() => setOnEdit(false)}
            >
                <>
                    <div className={`formsec ${styles.modalform} `}>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="domainname">Alias Name
                                    <span className='required'>*</span></label>
                            </div>
                            <div className='formelements'>
                                <input type="text"
                                    name="alias"
                                    id="alias"
                                    placeholder='Enter name'
                                    value={smsData?.aliasName}
                                    onChange={(e) => setSmsDatas({ ...smsData, aliasName: e.target.value })}
                                />
                            </div>
                        </div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="domainname">Sender Id<span className='required'>*</span></label>
                            </div>
                            <div className='formelements'>
                                <input
                                    type="text"
                                    name="senderId"
                                    id="senderId"
                                    placeholder="Enter Id"
                                    value={smsData?.senderId}
                                    onChange={(e) => setSmsDatas({ ...smsData, senderId: e.target.value })}
                                />
                            </div></div>
                        <div className='formrow'>
                            <div className='formlabels'>
                                <label htmlFor="domainname">Bot Enable / Disable</label>
                                <div className={styles.enableblock}>
                                    <div className={styles.lefts}>
                                        <h5>SMS Bot</h5>
                                        <p>Enable your Bot for automated and regenerative assistance</p>
                                    </div>
                                    <div className={styles.rights}>
                                        <label className="switch">
                                            <input
                                                type="checkbox"
                                                checked={smsData?.isBot === 1}
                                                onChange={(e) => setSmsDatas({ ...smsData, isBot: e.target.checked ? 1 : 0 })}
                                            />
                                            <span className="slider round"></span>
                                        </label>
                                    </div>
                                </div>

                            </div>

                        </div>
                        <div className={`formbtns ${styles.modalbtn} `}>
                            <button className='cancelbtn' onClick={() => setOnEdit(false)}>Cancel</button>
                            <button className='primarybtn' onClick={() => updateSmsDeatils()}>Save</button>
                        </div>
                    </div>
                </>
            </WorktualModal>
        </>
    );
}
export default Smsconfig;