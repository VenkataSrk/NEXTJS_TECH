import React, { useEffect, useRef, useState } from 'react';
import styles from './scss/webpush.module.scss';
import { assets } from '../../../../styles/assets/assets';
import { whatsappService } from '../../../../base/service/apiservice';
import Loaders from '../../../../components/loader/loader';
import { useSearchParams } from 'react-router-dom';
import { useDispatcher } from '../../../../store/redux-store/dispatch';
import { decryptAESCVM } from '../../../../base/utils';
import { SetItemInStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import Cookies from '../../../../utils/cookies';
import SocketService from '../../../../base/service/socketService';
import Spinner from '../../../../components/custom_components/spinner/spinner';

const WebPushConfig = () => {
  const initialWebPush :any ={
    webUrl:"",
    logoUrl:""
  }
  const [loader,setLoader]= useState<any>(false);
  const [searchParams] = useSearchParams();
  const dispatch = useDispatcher();

  const [webPushDetails,setWebPushDetails] =useState<any>(initialWebPush);
  const [websiteData,setWebsiteData] = useState<any>([]);
  console.log("webpushDetails",webPushDetails);
  const [validation,setValidation] = useState<any>(false);
  const uploadWebLogo=async(event :any)=>{
    try {
      const files = event?.target?.files;
      if (!files || files.length === 0) return;

      const file = files[0];

      const isImg = file.name.endsWith('.jpg') || file.name.endsWith('.jpeg') || file.name.endsWith('.png');
      if (isImg) {        
        let formDatas = new FormData();
        formDatas.append("doc", file);
        let url :any=await whatsappService.post(formDatas, '/upload_webPush_logo').then((res: any) => {
          console.log("resss", res);
          return res?.data?.fileList?.[0]?.url;
        }).catch((err) => {
          console.log("err", err);

        })
        return url;
      }
    } catch (error) {
        console.log("err", error);
    }
  }

  const handleChange=async(event :any)=>{
    try {
      const {name,value}=event?.target;
      if(name=="webUrl" ){
        setWebPushDetails({...webPushDetails,[name]:value});
      }
      if(name =="logoUrl"){
        console.log("come herr")
        let logoUrl =await uploadWebLogo(event);
        console.log("logourl??",logoUrl);
        setWebPushDetails({...webPushDetails,[name]:logoUrl});
      }
    } catch (error) {
      console.log("err",error)
    }
      
  }

  const handleSubmit =async(e :any)=>{
    try {
      e.preventDefault();
      setValidation(true)
      if(!webPushDetails?.webUrl?.trim() || !webPushDetails?.logoUrl?.trim()){
        return ;
      }
      setValidation(false)
      const body: any = {
        id:null,
        webUrl:webPushDetails?.webUrl,
        logoUrl:webPushDetails?.logoUrl
      }
      console.log("body???",body);
      await whatsappService.post(body,'/insert_webPush_config').then((res :any)=>{
          console.log("res",res);
          getWebPushConfig();
      });
    } catch (error) {
      console.log("error",error);
    }
  }

  const getWebPushConfig=async()=>{
      try {
        await whatsappService.get('/get_webpush_config').then((res :any)=>{
        console.log("webPush_config",res);
          if(res?.data?.finalResponse?.length>0){
            setWebsiteData(res?.data?.finalResponse)
          }
          else{
             setWebsiteData([])
             setWebPushDetails(initialWebPush);
          }
      });
      } catch (error) {
        console.log("err",error)
      }
    }

  const handleDisconnect=async()=>{
     try {
        let body:any ={
          id:websiteData?.[0]?.config_id
        }
        await whatsappService.post(body,'/delete_webpush_config').then((res :any)=>{
        console.log("webPush_config",res);
          getWebPushConfig();
          // if(res?.data?.finalResponse?.length>0){
          //   setWebsiteData(res?.data?.finalResponse)
          // }
      });
      } catch (error) {
        console.log("err",error)
      }
  }

 const fileInputRef = useRef(null);

  
  const handleRemoveImage = (e:any) => {
    e.stopPropagation(); 
    setWebPushDetails((prev:any) => ({
      ...prev,
      logoUrl: "", 
    }));

    if (fileInputRef.current) {
      fileInputRef.current.value = "";
    }
  };

 
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
          await getWebPushConfig();
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
     {loader ? 
     <div className='loadersinner'><Spinner /></div>
    //  <div className='loadersinner'><Loaders /></div>
    
        // <Loaders/>
        :
        <>
          <div className={styles.webpushconfigsec}>
            <div className={styles.topheading}>
              <div className={styles.tophdlefts}>
                <h2>Web Push Configuration</h2>
                <p className={styles.headerdesctxt}>Set up your website for push notifications.</p>
              
              </div>
              {/* <div className={styles.tophdrights}>
                        {websiteData?.length>0 &&<button  className="primarybtn" onClick={handleDisconnect}>Disconnect</button>}
                      </div> */}
            </div>
            <form className='formsec'>
                <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="">Website URL <span className='required'>*</span></label>
                              <span className='formelemdesc'>Enter the full URL where push notifications will be enabled.</span>
                          </div>
                          <div className='formelements'>
                            <input type="text" name="webUrl" id="webUrl" placeholder='e.g., https://www.yourwebsite.com' onChange={(e :any)=>handleChange(e)} value={websiteData?.[0]?.webUrl??(webPushDetails?.webUrl || "")} />
                          </div>
                      </div>
                <div className='formrow'>
                          <div className='formlabels'>
                              <label htmlFor="">Website Logo <span className='required'>*</span></label>
                          </div>
                          {websiteData?.length==0 && <div className='formelements'>
                        <div className="uploadsec uploadsecimgprev">
            <input
              ref={fileInputRef}
              type="file"
              name="logoUrl"
              id="logoUrl"
              accept=".jpeg,.jpg,.png"
              onChange={handleChange}
            />

            <img
              className="upimg"
              src={webPushDetails?.logoUrl || assets.imgpreview_icon}
              alt="upload preview"
            />

            <div className="upinfosec">
              <p className="upfiletxt">
                <span className="clickabletxt">Click to Upload</span> or Drag and drop here
              </p>
              <p className="updesc">
                JPEG or PNG format, up to 5 MB. <br /> Recommended size: 192×192 px for best display.
              </p>

              {webPushDetails?.logoUrl && (
                <span
                  className="clickabletxt remove-btn"
                  onClick={handleRemoveImage}
                >
                  Remove
                </span>
              )}
            </div>
          </div>
                          </div>
                          }
                          <div style={{width:"150px"}} >
                            <img style={{width:"100%"}} src={websiteData?.[0]?.logoUrl} alt=""  />
                          </div>
                      </div>
                      <div className='formbtns'>
                    {websiteData?.length==0 &&(<> <button className='cancelbtn'>Cancel</button><button  type="button" className='primarybtn' onClick={(e :any)=>handleSubmit(e)}>Save</button></>)}
                      </div>
            </form>
          </div>
        </>}
    </>
  )
}

export default WebPushConfig
