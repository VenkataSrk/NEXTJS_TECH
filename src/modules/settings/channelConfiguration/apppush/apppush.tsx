import React, { useEffect, useState } from 'react'
import styles from "./scss/apppush.module.scss"
import RightDrawer from '../../../../components/custom_components/drawer/drawer'
import { assets } from '../../../../styles/assets/assets'
import WorktualModal from '../../../../components/custom_components/worktualModal';
import { config } from '../../../../base/constant';
 import axios from "axios";
import { GetItemFromStorage, SetItemInStorage } from '../../../../base/customhooks/useStorage';
import { jwtDecode } from 'jwt-decode';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import { whatsappService } from '../../../../base/service/apiservice';
import Loaders from '../../../../components/loader/loader';
import { useSearchParams } from 'react-router-dom';
import { useDispatcher } from '../../../../store/redux-store/dispatch';
import { decryptAESCVM } from '../../../../base/utils';
import Cookies from '../../../../utils/cookies';
import SocketService from '../../../../base/service/socketService';
import Spinner from '../../../../components/custom_components/spinner/spinner';

function  Apppush() {
    const loginUserDetails: any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    const[mainModal,setMainModal]=useState<any>(false);
    const initialCopied :any = {
        "appCopy":false,
        "secretCopy":false,
        "urlCopy":false
    }
    const [copied,setCopied] = useState<any>(initialCopied);

    const [open, setOpen] = useState(false);
    const initialEncryptData: any = {
        "id":null,
        "appId": '',
        "secretKey": ''
    }
    const [encrypt, setEncrypt] = useState<any>(initialEncryptData);
    console.log("encrypt",encrypt);
    const initialMainConfig :any ={
        'id':null,
        'type':'',
        "projectId":'',
        "privateKeyId":'',
        "privateKey":'',
        "clientEmail":'',
        "clientId":'',
        "authUrl":'',
        "tokenUrl":'',
        "authProviderUrl":'',
        "clientUrl":'',
        "universeDomain":'',       
    }
    const [mainConfig,setMainConfig] = useState<any>(initialMainConfig);
    const [validation,setValidation] = useState<any>(false);
    const [isNew,setIsNew] = useState<any>(false);
    const [loader,setLoader] = useState<any>(true);
    const [searchParams] = useSearchParams();

    const dispatch = useDispatcher();


    const fetchEncryptionKeys = async () => {
        try {
            const url :string=`${config().BASE_URL}/get_encryption_keys`;
            // const url :string="http://localhost:5007/campaignService/v1/get_encryption_keys";
            const body :any={
                domainId:loginUserDetails?.domainId
            }
            const response = await axios.post(
                url,
                body, // body payload
                {
                    headers: {
                        "Content-Type": "application/json",
                    },
                }
            );
            if(response?.data?.statusCode==200 && response?.data?.contacts?.length>0){
                setEncrypt({
                    "id":response?.data?.contacts?.[0]?.configId,
                    "appId":response?.data?.contacts?.[0]?.appId,
                    "secretKey":response?.data?.contacts?.[0]?.secretKey,
                })
                setIsNew(false);      
            }else{
                setIsNew(true);              
            }

            console.log("fetched keys",response.data); // already parsed JSON
        } catch (error) {
            console.error("Error fetching encryption keys:", error);
        }
    };

    const handleMainConfig=(event :any)=>{
        const {name,value} = event?.target;
        setMainConfig({...mainConfig,[name]:value});        
    }

    const insertEncryptionKeys=async()=>{
        try {
            let body: any = {
                "domainId": loginUserDetails?.domainId,
                "id":encrypt?.id?encrypt?.id: null,
                "appKey": encrypt?.appId ? encrypt?.appId :null,
                "secretKey": ""
            }                
            
             let res: any = await whatsappService.post(body,'/insert_app_push_encryptionDetails').then((res: any) => {
                console.log("resss", res);
                if(res?.data?.contacts?.[0]?.errCode==0 || res?.data?.contacts?.[0]?.errCode==1){
                  fetchEncryptionKeys();
                }        
              
            }).catch((err) => {
                console.log("err", err);
            })
        } catch (error :any) {
            console.log("err",error);
        }
    }

    const handleMainSubmit=async(type :string)=>{  
        
        try {
            setValidation(true);
            if (!mainConfig?.projectId?.trim() || !mainConfig?.projectId?.trim() || !mainConfig?.privateKeyId?.trim() || !mainConfig?.privateKey?.trim() || !mainConfig?.clientEmail?.trim() || !mainConfig?.clientId?.trim() || !mainConfig?.authUrl?.trim() || !mainConfig?.tokenUrl?.trim() || !mainConfig?.authProviderUrl?.trim() || !mainConfig?.clientUrl?.trim() || !mainConfig?.universeDomain?.trim()) {
                return;
            }
            setValidation(false);
            let body :any={
                id: mainConfig?.id ? mainConfig?.id : null,             
                type: mainConfig?.type?.trim()?mainConfig?.type?.trim():null,
                projectId: mainConfig?.projectId?.trim()?mainConfig?.projectId?.trim():'',
                privateKeyId: mainConfig?.privateKeyId?.trim()?mainConfig?.privateKeyId?.trim():null,
                privateKey: mainConfig?.privateKey?.trim()?mainConfig?.privateKey?.trim():null,
                clientEmail: mainConfig?.clientEmail?.trim()?mainConfig?.clientEmail?.trim():null,
                clientId: mainConfig?.clientId?.trim()?mainConfig?.clientId?.trim():null,
                authURI: mainConfig?.authUrl?.trim()?mainConfig?.authUrl?.trim():null,
                tokenURI: mainConfig?.tokenUrl?.trim()?mainConfig?.tokenUrl?.trim():null,
                authProviderURL: mainConfig?.authProviderUrl?.trim()?mainConfig?.authProviderUrl?.trim():null,
                clientURL: mainConfig?.clientUrl?.trim()?mainConfig?.clientUrl?.trim():null,
                universe_domain: mainConfig?.universeDomain?.trim()?mainConfig?.universeDomain?.trim():null,
            }

            let res: any = await whatsappService.post(body,'/insert_update_app_push_main_config').then(async(res: any) => {
                console.log("resss", res);
                if(res?.data?.contacts?.[0]?.errCode==0){
                    setMainModal(false);
                    setOpen(false);
                }
                if(type=='New'){
                    await insertEncryptionKeys();
                }
              
            }).catch((err) => {
                console.log("err", err);
            })
           
        } catch (error :any) {
            console.log("err",error);
        } 
    }

    const fetchAppMainConfig= async() =>{
        try {

            let response: any = await whatsappService.get('/get_app_push_main_config').then((res: any) => {           
                if(res?.data?.statusCode==200 && res?.data?.configData?.length>0){
                    setMainModal(false);
                    setMainConfig({...res?.data?.configData?.[0]});
                }
               
            }).catch((err) => {
                console.log("err", err);
            })
            
        } catch (error) {
            console.log("error",error);
        }
    }

    const handleRefresh=()=>{
        insertEncryptionKeys();
    }
    
    
    useEffect(()=>{
          
              const init = async () => {
                try {
                    
                  setLoader(true);
                  const queryparams: any = searchParams.get('cvm_data');
                  if(queryparams?.length>0){                       
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
                          console.log("error", e)
                      } 
                  }
                } catch (error) {
                  console.log("API ERROR",error)
                }finally{  
                    await fetchEncryptionKeys();
                    await fetchAppMainConfig();      
                    setLoader(false);
                    const loaders = document.getElementById('contact_center_mainloader');
                    if (loaders) {
                        loaders.style.display = 'none';
                    }
                    
                }  
              }
              init();
           },[])

        //    useEffect(()=>{
        //     if(!loader){
                
        //     }
        //    },[loader])
    

  return (
    <>
    {loader ? 
        <div className='loadersinner'><Spinner /></div>
        // <div className='loadersinner'><Loaders /></div>
        //  <Loaders/>
         :
        <>
    <div>
          <div className={styles.topheading}>
              <div className={styles.tophdlefts}>
                  <h2>App Push Configuration</h2>
                  <p className={styles.headerdesctxt}>Set up your website for push notifications.</p>
              </div>
              <div className='formbtns'>
                  {!isNew && <button onClick={() => setOpen(true)} className='primarybtn'>Manage App </button>}
                  <RightDrawer open={open} onClose={() => setOpen(false)}>
                      <>
                       <div className={styles.topheading}>
                        <div style={{padding:"0 16px"}} className={styles.tophdlefts}>
                            <h2>Update  App</h2>
                            <p className={styles.headerdesctxt}>Set up your Application for push notifications.</p>
                        </div>
                    </div>
                          <div className='formsec '>
                              <div className='formrow'>
                                 <div className='formlabels'>
                                      <label htmlFor="domainname">Type <span className='required'>*</span></label>
                                      <span className='formelemdesc'>Unique identifier for your project Type.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="type" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.type} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.type?.trim()) && <p className='error'>This field is required</p>}
                              </div>
                               <div className='formrow'>
                                 <div className='formlabels'>
                                      <label htmlFor="domainname">Project Id <span className='required'>*</span></label>
                                      <span className='formelemdesc'>Unique identifier for your Google Cloud project.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="projectId" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.projectId} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.projectId?.trim()) && <p className='error'>This field is required</p>}
                              </div>
                              <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Private Key Id<span className='required'>*</span></label>
                                      <span className='formelemdesc'>ID of the private key used to authenticate requests.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="privateKeyId" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.privateKeyId} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.privateKeyId?.trim()) && <p className='error'>This field is required</p>}
                              </div> <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Private Key <span className='required'>*</span></label>
                                      <span className='formelemdesc'>The actual private key string that secures your app’s push communication.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="privateKey" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.privateKey} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.privateKey?.trim()) && <p className='error'>This field is required</p>}
                              </div> <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Client Email <span className='required'>*</span></label>
                                      <span className='formelemdesc'>Service account email authorized for push notifications</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="clientEmail" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.clientEmail} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.clientEmail?.trim()) && <p className='error'>This field is required</p>}
                              </div> <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Client Id<span className='required'>*</span></label>
                                      <span className='formelemdesc'>Unique ID assigned to the service account client.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="clientId" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.clientId} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.clientId?.trim()) && <p className='error'>This field is required</p>}
                              </div> <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Auth URL <span className='required'>*</span></label>
                                      <span className='formelemdesc'>The endpoint used for authentication requests.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="authUrl" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.authUrl} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.authUrl?.trim()) && <p className='error'>This field is required</p>}
                              </div> <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Token URL <span className='required'>*</span></label>
                                      <span className='formelemdesc'>The endpoint to obtain OAuth 2.0 access tokens.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="tokenUrl" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.tokenUrl} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.tokenUrl?.trim()) && <p className='error'>This field is required</p>}
                              </div>
                              <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Auth Provider URL <span className='required'>*</span></label>
                                      <span className='formelemdesc'>URL for the provider’s public certificates to verify tokens.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="authProviderUrl" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.authProviderUrl} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.authProviderUrl?.trim()) && <p className='error'>This field is required</p>}
                              </div>
                              <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Client URL <span className='required'>*</span></label>
                                      <span className='formelemdesc'>URL for the client’s public certificates to authenticate requests.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="clientUrl" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.clientUrl} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.clientUrl?.trim()) && <p className='error'>This field is required</p>}
                              </div>
                              <div className='formrow'>
                                  <div className='formlabels'>
                                      <label htmlFor="domainname">Universe Domain<span className='required'>*</span></label>
                                      <span className='formelemdesc'>The Google domain scope used for authentication.</span>
                                  </div>
                                  <div className='formelements'>
                                      <input type="text" name="universeDomain" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.universeDomain} placeholder='worktual_c2768' />
                                  </div>
                                  {(validation && !mainConfig?.universeDomain?.trim()) && <p className='error'>This field is required</p>}
                              </div>
                              <div className='formbtns'>
                                  <button onClick={() => setOpen(false)} className='cancelbtn'>Cancel</button><button className='primarybtn' onClick={()=>handleMainSubmit("old")}>Save</button>
                              </div>
                          </div>
                      </>

                  </RightDrawer>
              </div>
          </div>
          {/* empty card initial  */}
          {isNew &&
              <div style={{ display: "grid", placeItems: "center", height: "calc(100% - 132px)" }}>
                  <div className='pannelcenter'>
                      <div className='imgcon'>
                          <img src={assets.emptyapppush} />



                      </div>
                      <div className='info'>
                          <h5>Configure App Push</h5>
                          <p>App push Configuration</p>
                      </div>
                      <button className='primarybtn' onClick={()=>setMainModal(true)}>Add Account</button>
                  </div>
              </div>
          }
{!isNew &&
          <div className='formsec '>
{/* sample form input */}
              {/* <div className='formrow'>
                  <div className='formlabels'>
                      <label htmlFor="domainname">Project Id <span className='required'>*</span></label>
                      <span className='formelemdesc'>Microcopy</span>
                  </div>
                  <div className='formelements'>
                      <input type="text" name="" id="domainname" placeholder='worktual_c2768' />
                  </div>
              </div> */}
              <div className='formrow'>
                  <div className='formlabels'>
                      <label htmlFor="domainname">App Id <span className='required'>*</span></label>
                      <span className='formelemdesc'>Identifies your app so notifications are delivered to the right application</span>
                  </div>
                  <div style={{ display: "flex", alignItems: "center", gap: "12px" }} className='formelements'>
                      <input type="text" name="" id="domainname" value={encrypt?.appId} placeholder='e.g., How satisfied are you with our service?' readOnly/>
                      <span  style={{display: "flex", alignItems: "center", gap: "8px" , fontSize:"13px" }} onClick={()=> {navigator.clipboard.writeText(encrypt?.appId); setCopied({...copied,appCopy:true}); setTimeout(()=>setCopied({...initialCopied}),3000)} }>
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                          <path d="M16.6887 1.47334C15.5505 0.39324 13.9787 0.210641 12.4509 0.174541C11.2068 0.142341 10.3259 0.142341 9.08575 0.174541C7.55695 0.210641 5.98515 0.39234 4.84695 1.47334C4.37575 1.92014 4.08805 2.46164 3.90635 3.03044C2.86425 3.22774 2.03965 3.60684 1.41085 4.20374C0.160347 5.39024 0.124747 7.21744 0.0988472 8.55234L0.0959473 11.9644L0.0988472 12.1119C0.124747 13.4459 0.160347 15.273 1.41085 16.4693C2.59835 17.5963 4.24335 17.7867 5.84245 17.8248C6.49625 17.8414 7.05435 17.8492 7.61195 17.8492C8.16955 17.8492 8.72815 17.8414 9.38005 17.8248C10.9806 17.7867 12.6256 17.5963 13.8131 16.4693C14.5191 15.7999 14.8238 14.9312 14.9678 14.1514C15.5975 13.96 16.1865 13.6648 16.6886 13.188C17.8849 12.0542 17.9205 10.3296 17.944 9.17344V5.47324C17.9206 4.33264 17.885 2.60814 16.6887 1.47334ZM16.4316 5.06274C15.6751 5.05424 15.1289 5.02224 14.6984 4.89814C13.4782 4.56124 12.5573 3.43814 12.4587 2.16184C12.4453 2.00024 12.453 1.83834 12.4554 1.67674C13.8542 1.71214 14.9368 1.87794 15.6564 2.56124C16.2572 3.13154 16.3915 4.05804 16.4316 5.06274ZM12.781 15.3815C12.0071 16.1159 10.8479 16.2897 9.34355 16.3249C8.06425 16.3581 7.16235 16.3581 5.87915 16.3249C4.37625 16.2897 3.21705 16.1149 2.44555 15.3835C1.64765 14.6208 1.62185 13.2722 1.59885 12.0837L1.59595 8.73124L1.59885 8.58184C1.62175 7.39334 1.64765 6.04664 2.44305 5.29184C2.75205 4.99884 3.14195 4.78844 3.62465 4.63824C3.60115 4.98764 3.59405 5.32524 3.58805 5.63454L3.58515 8.87964L3.58805 9.02024C3.61245 10.2946 3.64615 12.0407 4.84635 13.1882C5.98455 14.2683 7.55635 14.4509 9.08415 14.487C9.70625 14.5026 10.2369 14.5104 10.7677 14.5104C11.298 14.5104 11.8292 14.5026 12.4493 14.487C12.7487 14.4799 13.0493 14.4641 13.3485 14.4401C13.2154 14.8339 13.0317 15.1437 12.781 15.3815ZM15.6565 12.1003C14.929 12.7907 13.8342 12.9538 12.4128 12.987C11.196 13.0173 10.3396 13.0173 9.12085 12.987C7.70095 12.9538 6.60625 12.7907 5.88115 12.1022C5.13405 11.3874 5.10965 10.1149 5.08765 8.99184L5.08525 5.80634L5.08765 5.66274C5.10965 4.54064 5.13405 3.26824 5.87965 2.56114C6.60625 1.87074 7.70095 1.70764 9.12235 1.67444C9.73075 1.65884 10.2488 1.65104 10.7678 1.65104C10.8201 1.65104 10.8746 1.65204 10.927 1.65224C10.9243 1.86364 10.9164 2.07604 10.9338 2.28384C11.0827 4.21744 12.4323 5.86004 14.2829 6.37074C14.8796 6.54284 15.5442 6.58214 16.444 6.59144V9.15784C16.4201 10.3375 16.3416 11.4509 15.6565 12.1003Z" fill="black" />
                      </svg>
                      {copied?.appCopy && <span style={{fontSize:"13px", fontFamily:"interregular"}}>Copied</span>}
                      </span>
                  </div>
              </div>
               <div className='formrow'>
                  <div className='formlabels'>
                      <label htmlFor="domainname">Secret Key <span className='required'>*</span></label>
                      <span className='formelemdesc'>Secret ID to authenticate and ensures only authorized requests can send push notifications</span>
                  </div>
                  <div style={{ display: "flex", alignItems: "center", gap: "12px" }} className='formelements'>
                    <div style={{position:"relative" , width:"100%"}}>
                        <input type="text" name="" id="domainname"  value={encrypt?.secretKey} placeholder='e.g., How satisfied are you with our service?' readOnly />
                        <span className='insideinptbtn' onClick={()=>handleRefresh()}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="19" height="20" viewBox="0 0 19 20" fill="none">
                                <path d="M17.3331 9.42894C16.9572 9.42894 16.6525 9.73374 16.6525 10.1095C16.6525 14.3181 13.2287 17.7423 9.01968 17.7423C4.81068 17.7423 1.38728 14.318 1.38728 10.1095C1.38728 5.90104 4.81108 2.47674 9.01968 2.47674C10.4139 2.47674 11.7553 2.86804 12.9386 3.57984L11.9526 3.59824C11.4564 3.60794 11.0616 4.01804 11.0708 4.51424C11.08 5.00514 11.4805 5.39684 11.9692 5.39684H11.9868L14.6378 5.34674C15.134 5.33704 15.5288 4.92694 15.5196 4.43074L15.4695 1.77934C15.4603 1.28224 15.044 0.880042 14.5535 0.896742C14.0573 0.906442 13.6625 1.31654 13.6717 1.81274L13.6835 2.44204C12.2775 1.58484 10.6795 1.11544 9.01958 1.11544C4.06058 1.11544 0.0258789 5.15014 0.0258789 10.1096C0.0258789 15.0691 4.06058 19.1037 9.01958 19.1037C13.9786 19.1037 18.0137 15.069 18.0137 10.1096C18.0137 9.73374 17.7089 9.42894 17.3331 9.42894Z" fill="#757676" />
                            </svg>
                        </span>   
                    </div>
                    <span  style={{display: "flex", alignItems: "center", gap: "8px" , fontSize:"13px" }} onClick={() => { navigator.clipboard.writeText(encrypt?.secretKey); setCopied({ ...copied, secretCopy: true }); setTimeout(() => setCopied({ ...initialCopied }), 3000) }}>
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                            <path d="M16.6887 1.47334C15.5505 0.39324 13.9787 0.210641 12.4509 0.174541C11.2068 0.142341 10.3259 0.142341 9.08575 0.174541C7.55695 0.210641 5.98515 0.39234 4.84695 1.47334C4.37575 1.92014 4.08805 2.46164 3.90635 3.03044C2.86425 3.22774 2.03965 3.60684 1.41085 4.20374C0.160347 5.39024 0.124747 7.21744 0.0988472 8.55234L0.0959473 11.9644L0.0988472 12.1119C0.124747 13.4459 0.160347 15.273 1.41085 16.4693C2.59835 17.5963 4.24335 17.7867 5.84245 17.8248C6.49625 17.8414 7.05435 17.8492 7.61195 17.8492C8.16955 17.8492 8.72815 17.8414 9.38005 17.8248C10.9806 17.7867 12.6256 17.5963 13.8131 16.4693C14.5191 15.7999 14.8238 14.9312 14.9678 14.1514C15.5975 13.96 16.1865 13.6648 16.6886 13.188C17.8849 12.0542 17.9205 10.3296 17.944 9.17344V5.47324C17.9206 4.33264 17.885 2.60814 16.6887 1.47334ZM16.4316 5.06274C15.6751 5.05424 15.1289 5.02224 14.6984 4.89814C13.4782 4.56124 12.5573 3.43814 12.4587 2.16184C12.4453 2.00024 12.453 1.83834 12.4554 1.67674C13.8542 1.71214 14.9368 1.87794 15.6564 2.56124C16.2572 3.13154 16.3915 4.05804 16.4316 5.06274ZM12.781 15.3815C12.0071 16.1159 10.8479 16.2897 9.34355 16.3249C8.06425 16.3581 7.16235 16.3581 5.87915 16.3249C4.37625 16.2897 3.21705 16.1149 2.44555 15.3835C1.64765 14.6208 1.62185 13.2722 1.59885 12.0837L1.59595 8.73124L1.59885 8.58184C1.62175 7.39334 1.64765 6.04664 2.44305 5.29184C2.75205 4.99884 3.14195 4.78844 3.62465 4.63824C3.60115 4.98764 3.59405 5.32524 3.58805 5.63454L3.58515 8.87964L3.58805 9.02024C3.61245 10.2946 3.64615 12.0407 4.84635 13.1882C5.98455 14.2683 7.55635 14.4509 9.08415 14.487C9.70625 14.5026 10.2369 14.5104 10.7677 14.5104C11.298 14.5104 11.8292 14.5026 12.4493 14.487C12.7487 14.4799 13.0493 14.4641 13.3485 14.4401C13.2154 14.8339 13.0317 15.1437 12.781 15.3815ZM15.6565 12.1003C14.929 12.7907 13.8342 12.9538 12.4128 12.987C11.196 13.0173 10.3396 13.0173 9.12085 12.987C7.70095 12.9538 6.60625 12.7907 5.88115 12.1022C5.13405 11.3874 5.10965 10.1149 5.08765 8.99184L5.08525 5.80634L5.08765 5.66274C5.10965 4.54064 5.13405 3.26824 5.87965 2.56114C6.60625 1.87074 7.70095 1.70764 9.12235 1.67444C9.73075 1.65884 10.2488 1.65104 10.7678 1.65104C10.8201 1.65104 10.8746 1.65204 10.927 1.65224C10.9243 1.86364 10.9164 2.07604 10.9338 2.28384C11.0827 4.21744 12.4323 5.86004 14.2829 6.37074C14.8796 6.54284 15.5442 6.58214 16.444 6.59144V9.15784C16.4201 10.3375 16.3416 11.4509 15.6565 12.1003Z" fill="black" />
                        </svg>
                        {copied?.secretCopy && <span style={{fontSize:"13px", fontFamily:"interregular"}}>Copied</span>}
                    </span>
                  </div>
              </div>
              <div className='formrow'>
                  <div className='formlabels'>
                      <label htmlFor="domainname">Instruction <span className='required'>*</span></label>
                      <span className='formelemdesc'>Use this APi to connect your contact details</span>
                  </div>
                  <div style={{ display: "flex", alignItems: "center", gap: "12px" }} className='formelements'>
                      <input type="text" name="" id="domainname" value={`${config().BASE_URL}/campaignService/v1/insert_update_tpi_contact`} placeholder='e.g., How satisfied are you with our service?' readOnly/>
                      <span  style={{display: "flex", alignItems: "center", gap: "8px" , fontSize:"13px" }}onClick={()=> {navigator.clipboard.writeText(`${config().BASE_URL}/campaignService/v1/insert_update_tpi_contact`); setCopied({...copied,urlCopy:true}); setTimeout(()=>setCopied({...initialCopied}),3000) } }>
                           <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                          <path d="M16.6887 1.47334C15.5505 0.39324 13.9787 0.210641 12.4509 0.174541C11.2068 0.142341 10.3259 0.142341 9.08575 0.174541C7.55695 0.210641 5.98515 0.39234 4.84695 1.47334C4.37575 1.92014 4.08805 2.46164 3.90635 3.03044C2.86425 3.22774 2.03965 3.60684 1.41085 4.20374C0.160347 5.39024 0.124747 7.21744 0.0988472 8.55234L0.0959473 11.9644L0.0988472 12.1119C0.124747 13.4459 0.160347 15.273 1.41085 16.4693C2.59835 17.5963 4.24335 17.7867 5.84245 17.8248C6.49625 17.8414 7.05435 17.8492 7.61195 17.8492C8.16955 17.8492 8.72815 17.8414 9.38005 17.8248C10.9806 17.7867 12.6256 17.5963 13.8131 16.4693C14.5191 15.7999 14.8238 14.9312 14.9678 14.1514C15.5975 13.96 16.1865 13.6648 16.6886 13.188C17.8849 12.0542 17.9205 10.3296 17.944 9.17344V5.47324C17.9206 4.33264 17.885 2.60814 16.6887 1.47334ZM16.4316 5.06274C15.6751 5.05424 15.1289 5.02224 14.6984 4.89814C13.4782 4.56124 12.5573 3.43814 12.4587 2.16184C12.4453 2.00024 12.453 1.83834 12.4554 1.67674C13.8542 1.71214 14.9368 1.87794 15.6564 2.56124C16.2572 3.13154 16.3915 4.05804 16.4316 5.06274ZM12.781 15.3815C12.0071 16.1159 10.8479 16.2897 9.34355 16.3249C8.06425 16.3581 7.16235 16.3581 5.87915 16.3249C4.37625 16.2897 3.21705 16.1149 2.44555 15.3835C1.64765 14.6208 1.62185 13.2722 1.59885 12.0837L1.59595 8.73124L1.59885 8.58184C1.62175 7.39334 1.64765 6.04664 2.44305 5.29184C2.75205 4.99884 3.14195 4.78844 3.62465 4.63824C3.60115 4.98764 3.59405 5.32524 3.58805 5.63454L3.58515 8.87964L3.58805 9.02024C3.61245 10.2946 3.64615 12.0407 4.84635 13.1882C5.98455 14.2683 7.55635 14.4509 9.08415 14.487C9.70625 14.5026 10.2369 14.5104 10.7677 14.5104C11.298 14.5104 11.8292 14.5026 12.4493 14.487C12.7487 14.4799 13.0493 14.4641 13.3485 14.4401C13.2154 14.8339 13.0317 15.1437 12.781 15.3815ZM15.6565 12.1003C14.929 12.7907 13.8342 12.9538 12.4128 12.987C11.196 13.0173 10.3396 13.0173 9.12085 12.987C7.70095 12.9538 6.60625 12.7907 5.88115 12.1022C5.13405 11.3874 5.10965 10.1149 5.08765 8.99184L5.08525 5.80634L5.08765 5.66274C5.10965 4.54064 5.13405 3.26824 5.87965 2.56114C6.60625 1.87074 7.70095 1.70764 9.12235 1.67444C9.73075 1.65884 10.2488 1.65104 10.7678 1.65104C10.8201 1.65104 10.8746 1.65204 10.927 1.65224C10.9243 1.86364 10.9164 2.07604 10.9338 2.28384C11.0827 4.21744 12.4323 5.86004 14.2829 6.37074C14.8796 6.54284 15.5442 6.58214 16.444 6.59144V9.15784C16.4201 10.3375 16.3416 11.4509 15.6565 12.1003Z" fill="black" />
                            </svg>
                           {copied?.urlCopy && <span style={{fontSize:"13px", fontFamily:"interregular"}}>Copied</span>}
                      </span>
                  </div>
              </div>
              <div className='formrow'>
                  <div className='formlabels'>
                      <label htmlFor="domainname">Example Payload<span className='required'>*</span></label>
                      <p className={styles.txtcont}></p>

                  </div>
                  <div className='formelements'>
                      <div className={styles.notescard}>
                          {/* <p>To configure your app with campaign management for app push notification, please ensure that the payload follows the sample format provided below.</p> */}
                          <p>{`{
    "appId":"U2FsdGVkX1/QdH6anqB7MGFaQCxWDYZYqTttqmZWtQN9lr15Mb2IfXgTB76/47e2",
    "secretKey":"U2FsdGVkX18MPz0tgpVLNg4LzzSH/795I+mK0Z/hnmBeZ9A56kT6LhR4a1JOUdU9",
    "contactEmail":"[{\"firstName\":\"Alice\",\"email\":\"alice@example.com\",\"deviceId\":\"device123\",\"deviceOS\":\"Android\",\"pushNotificationId\":\"ekqtD6FTQzqdZNnwoFLmdA:APA91bGHvgwdHhGaq-ikymVeh3DoqJ4IfvEKhT2B6dxcirzg2YF9_JluzV2pInyRjz8TOxjnuGCvuF_t0SdyxO-A4v60bgzVI7yn99v3jpiFvfgC4Ie6b3I\"},{\"firstName\":\"Bob\",\"email\":\"bob@example.com\",\"deviceId\":\"device456\",\"deviceOS\":\"Android\"},{\"firstName\":\"Charlie\",\"email\":\"charlie@example.com\",\"deviceId\":\"device789\",\"deviceOS\":\"\IOS\"}]"

}`}</p>
                      </div>

                  </div>
              </div>


              {/* <div className='formbtns'>
                  <button className='cancelbtn'>Cancel</button><button className='primarybtn'>Next</button>
              </div> */}
          </div>
}
          {/* initial modal */}
          <WorktualModal
              show={mainModal}
              // show={false}
              title='Setup App'
              mdsize='modalsm'         >
              <>
                  <div className='formsec '>
                        
                        <p>Set up your Application for push notifications.</p>
                      <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Type <span className='required'>*</span></label>
                              <span className='formelemdesc'>Unique identifier for your project Type.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="type" id="domainname" onChange={(e: any) => { handleMainConfig(e) }} value={mainConfig?.type} placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.type?.trim()) && <p className='error'>This field is required</p>}
                      </div>
                      <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Project Id <span className='required'>*</span></label>
                              <span className='formelemdesc'>Unique identifier for your Google Cloud project.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="projectId" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.projectId?.trim()) && <p className='error'>This field is required</p>}
                      </div>
                      <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Private Key Id<span className='required'>*</span></label>
                              <span className='formelemdesc'>ID of the private key used to authenticate requests.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="privateKeyId" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.privateKeyId?.trim()) && <p className='error'>This field is required</p>}
                      </div> <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Private Key <span className='required'>*</span></label>
                              <span className='formelemdesc'>The actual private key string that secures your app’s push communication.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="privateKey" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.privateKey?.trim()) && <p className='error'>This field is required</p>}
                      </div> <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Client Email <span className='required'>*</span></label>
                              <span className='formelemdesc'>Service account email authorized for push notifications</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="clientEmail" id="domainname"  onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.clientEmail?.trim()) && <p className='error'>This field is required</p>}
                      </div> <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Client Id<span className='required'>*</span></label>
                              <span className='formelemdesc'>Unique ID assigned to the service account client.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="clientId" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.clientId?.trim()) && <p className='error'>This field is required</p>}
                      </div> <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Auth URL <span className='required'>*</span></label>
                              <span className='formelemdesc'>The endpoint used for authentication requests.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="authUrl" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.authUrl?.trim()) && <p className='error'>This field is required</p>}
                      </div> <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Token URL <span className='required'>*</span></label>
                              <span className='formelemdesc'>The endpoint to obtain OAuth 2.0 access tokens.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="tokenUrl" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.tokenUrl?.trim()) && <p className='error'>This field is required</p>}
                      </div>
                      <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Auth Provider URL <span className='required'>*</span></label>
                              <span className='formelemdesc'>URL for the provider’s public certificates to verify tokens.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="authProviderUrl" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.authProviderUrl?.trim()) && <p className='error'>This field is required</p>}
                      </div>
                      <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Client URL <span className='required'>*</span></label>
                              <span className='formelemdesc'>URL for the client’s public certificates to authenticate requests.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="clientUrl" id="domainname" onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.clientUrl?.trim()) && <p className='error'>This field is required</p>}
                      </div>
                      <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="domainname">Universe Domain<span className='required'>*</span></label>
                              <span className='formelemdesc'>The Google domain scope used for authentication.</span>
                          </div>
                          <div className='formelements'>
                              <input type="text" name="universeDomain" id="domainname"  onChange={(e :any)=>{handleMainConfig(e)}}  placeholder='worktual_c2768' />
                          </div>
                          {(validation && !mainConfig?.universeDomain?.trim()) && <p className='error'>This field is required</p>}
                      </div>                    

                      <div className='formbtns'>
                          <button className='cancelbtn' onClick={()=>setMainModal(false)}>Cancel</button><button className='primarybtn' onClick={()=>handleMainSubmit("New")}>Next</button>
                      </div>
                  </div>
              </>

          </WorktualModal>



    </div>
    </>
}
</>
  )
}

export default Apppush