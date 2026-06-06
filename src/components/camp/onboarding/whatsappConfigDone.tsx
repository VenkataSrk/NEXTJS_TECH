
import styles from "./scss/newonboard.module.scss";
// import logo from "../../assets/images/fulllogo.svg";
import logo from "./Archive/fulllogo.svg";
import Tickrounded from "../tickrounded.svg"

import { useNavigate, useSearchParams } from "react-router-dom";
import { useEffect, useState } from "react";
import { GetItemFromStorage } from "../../../base/customhooks/useStorage";
import { LocalStorage } from "../../../base/customhooks/localStorageKeys";
import { jwtDecode } from "jwt-decode";

export default function WhatsappConfigDone() {
     const navigate = useNavigate();
  const [searchParams] = useSearchParams();
  const [signUpStatus, setSignUpStatus] = useState<string | null>(null);
  const [timer,setTimer]=useState<any>(5);
  const userDetails = GetItemFromStorage(LocalStorage?.ACCESS_TOKEN);
  const data: any = jwtDecode(userDetails);
    useEffect(() => {
        const timer = setTimeout(() => {
            const loaders = document.getElementById('contact_center_mainloader');
            if (loaders) {
                loaders.style.display = 'none';
            }
        }, 1000); // 1 second

        // cleanup → remove timer
        return () => clearTimeout(timer);
    }, []);

      useEffect(() => {
    setSignUpStatus(searchParams.get("status"));
  }, [searchParams]);

  useEffect(()=>{
    if(timer==0){
        if(data?.domain_id==20667){

            navigate('/home')
        }else{

            navigate('/onboard',{ state: { data:"Integration" } })
        }
        return;
    }
     const interval = setInterval(() => {
    setTimer((prev :any) => prev - 1); 
  }, 1000);

  return () => clearInterval(interval); 
    
  },[timer])

    return (

        <>
{/* <p> {`${loader}`}</p> */}

            <div className={styles.mainlayout}>
                <div className={styles.mainlogo}>
                    <img src={logo} alt="" />
                </div>
                            <div className={styles.tickrounded}>
                                <img src={Tickrounded} alt="Tickrounded" />
                                <div className={styles.campaignloaded}>
                                   <div className={styles.titlearealeft}>Setup Journey Whatsapp Configuration.<span>Status:{signUpStatus}</span></div>
                                   <div className={styles.titlearearight}>Your campaign management setup has been successfully completed. Your workspace is ready to use.</div>
                               </div>
                               {/* <div className={styles.campaigntitle}>Redirecting you to the Homepage in <span>{timer}</span> seconds…</div> */}
                               <div className={styles.campaigntitle}>Redirecting you to the Integration Page in <span>{timer}</span> seconds…</div>
                            </div>
            </div>
        </>
    );
}
