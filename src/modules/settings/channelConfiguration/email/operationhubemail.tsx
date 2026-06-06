import React, { useEffect, useRef, useState } from 'react';
import styles from './scss/email.module.scss';
import WorktualTab from '../../../../components/custom_components/worktualTab';
import EmailAdressList from './emailAdressList';
import SuspendedEmails from './suspendedEmails';
import WorktualModal from '../../../../components/custom_components/worktualModal';
import { assets } from '../../../../styles/assets/assets';
import { useNavigate, useSearchParams } from 'react-router-dom';
import { SetItemInStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import Cookies from '../../../../utils/cookies';
import { decryptAESCVM } from '../../../../base/utils';
import SocketService from '../../../../base/service/socketService';
import { useDispatcher } from '../../../../store/redux-store/dispatch';
import Loaders from '../../../../components/loader/loader';
import Spinner from '../../../../components/custom_components/spinner/spinner';


const OperationHubEmail = () => {
  const [showModal,setShowModal]= useState<boolean>(false);
  const [searchParams] = useSearchParams();
  const isCvm :boolean= useRef<boolean>(false);
  const [loader,setLoader] = useState<any>(true);
  const dispatch = useDispatcher();
  const navigate = useNavigate();
  const tabs = [
    {
      label:'Email Address List',
      content:<EmailAdressList />
    },
    {
      label:'Suspended Email',
      content:<SuspendedEmails />
    },
  ]
  

   useEffect(()=>{
  
      const init = async () => {
        try {
          const queryparams: any = searchParams.get('cvm_data');
          if(queryparams?.length>0){  
              isCvm.current = true;
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
          const loaders = document.getElementById('contact_center_mainloader');
          if (loaders) {
            loaders.style.display = 'none';
          }
        }  
      }
      init();
   },[])

  return (
    <>
    {loader ? <div className='loadersinner'><Spinner /></div> :
    // {loader ? <div className='loadersinner'><Loaders /></div> :
    <div className={styles.overallbody}>
      <div className={styles.emailsec}>
        <div className={`${styles.topheading} ${styles.header}`}>
                <div className={styles.tophdlefts}>
                    <h2>Email configuration</h2>
                    <p className={styles.headerdesctxt}>Manage and configure email addresses for communication and system notifications.</p>
                </div>
                {/* <div className={styles.tophdrights}>
                    <button className='primarybtn' onClick={()=>setShowModal(true)}>Add Email Address</button>
                </div> */}
            </div>
              <WorktualTab tabs={tabs} />
            <div className='emailaddressmodal'>
         <WorktualModal
              show={showModal}
              title='Add Email Address'
              onClose={()=>setShowModal(false)}
              footer={
                false
              }
              mdsize='modalsm'
      >
        <>
        <div className={styles.emailaddressmodal}>
           <div className={styles.row}>
               <div className={styles.block} 
              //  onClick={()=>navigate('')}
               >
                  <span><img src={assets.url}/></span>
                  <div className={styles.info}>
                      <h6>Private Domain</h6>
                      <p>Use your own domain to send emails.</p>
                  </div>
               </div>
               
           </div>
           <div className={styles.row}>
               <div className={styles.block} 
                      onClick={() => {
                        if (!isCvm.current) { 
                            navigate('/addWorktualAddress') 
                        } 
                        else { 
                          navigate('/cvmaddWorktualAddress',{ state: { data:"cvm" } }) 
                        }
                      }}
                    >
                  <span><img src={assets.gallery}/></span>
                  <div className={styles.info}>
                      <h6>Worktual Address</h6>
                      <p>Send emails using Worktual-provided email address.</p>
                  </div>
               </div>
               
           </div>
           <div className={styles.row}>
               <div className={styles.block} 
               onClick={()=>{
                 if (!isCvm.current) {
                  
                   navigate('/setupSmtpEmail')

                 }
                 else {
                   navigate('/cvmsetupSmtpEmail',{ state: { data:"cvm" } })
                 }

               }
                
                   }
               >
                  <span><img src={assets.smtb}/></span>
                  <div className={styles.info}>
                      <h6>SMTP Address</h6>
                      <p>SMTP address to securely send emails.</p>
                  </div>
               </div>
               
           </div>
        </div>
        </>
      </WorktualModal>
            </div>
       
      </div>
    </div>
   
   }
    </>
  )
}

export default OperationHubEmail
