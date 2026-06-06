import React, { useEffect, useState } from 'react'
import ContentLayout from '../../../../layout/contentLayout'
import WorktualSelect from '../../../../components/custom_components/WorktualSelect'
import { Link, useLocation, useNavigate, useSearchParams } from 'react-router-dom'
import styles from './scss/setupsmtp.module.scss';
import { CCAASEmailServices, CoreServices, whatsappService } from '../../../../base/service/apiservice';
import { jwtDecode } from 'jwt-decode';
import { GetItemFromStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import { useDispatcher } from '../../../../store/redux-store/dispatch';

const SetupSmtpEmail = () => {
    const onlyAlphabetsRegex = /^[a-zA-Z]+$/;
    const alphaNumericRegex = /^[a-zA-Z0-9]+$/;
    const numericRegex = /^[0-9]+$/;
    const emailRegex =/^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
    const hostRegex =/^([a-zA-Z0-9-]+\.)+[a-zA-Z]{2,6}|((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/;
 
    const loginUserDetails :any = jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN));
    const location = useLocation();
    const {state}:any = location;
      
    console.log("lg",loginUserDetails)
    const navigate = useNavigate();
     const selectOpt = [
        { value: 'one', label: 'One' }
    ]

    const initialSmtpDatas={
        aliasName:'',
        port:'',
        host:'',
        password:'',
        userName:'',
        fromEmail:'',
        isBot:false
    }

    const [SmtpEmailDatas,setSmtpEmailDatas] = useState<any>(initialSmtpDatas);

    console.log("smtpEmailDatas",SmtpEmailDatas);
    const [validation,setValidation]= useState<boolean>(false);

    const handleChange=(e :any)=>{
        const {name,value,checked} :any = e?.target;

        if(name =='aliasName' || name =='port' || name=='host'|| name=='password'|| name=='userName'|| name == "fromEmail"){
            setSmtpEmailDatas({...SmtpEmailDatas,[name]:value});
        }

        if(name =='isBot'){
            setSmtpEmailDatas({...SmtpEmailDatas,[name]:checked});
        }
        
    }

    const handleValidation = () => {
       
        setValidation(true);
        if (SmtpEmailDatas?.aliasName?.trim()?.length <=0 || !onlyAlphabetsRegex?.test(SmtpEmailDatas?.aliasName?.trim())) {
            return;
        }
        if(SmtpEmailDatas?.userName?.trim()?.length<=0 ){
            return;
        }
        if(SmtpEmailDatas?.fromEmail?.trim()?.length<=0 || !emailRegex?.test(SmtpEmailDatas?.fromEmail?.trim()) ){
            return;
        }
        if(SmtpEmailDatas?.port?.trim()?.length<=0 || !numericRegex?.test(SmtpEmailDatas?.port?.trim())){
            return;
        }
        if(SmtpEmailDatas?.password?.trim()?.length<=0 ){
            return;
        }
        if(SmtpEmailDatas?.host?.trim()?.length<=0 || !hostRegex?.test(SmtpEmailDatas?.host?.trim()) ){
            return;
        }
        setValidation(false);
    }

    const handleSubmit=(e :any)=>{  
        try {
            
            e.preventDefault(); 
            handleValidation();
            if(!validation){
                const body = {
                    id: null,
                    aliasName: SmtpEmailDatas?.aliasName || '',
                    port:SmtpEmailDatas?.port|| '',
                    host: SmtpEmailDatas?.host||'',
                    password: SmtpEmailDatas?.password||'',
                    userName: SmtpEmailDatas?.userName||'',
                    fromEmail:SmtpEmailDatas?.fromEmail|| '',
                    isBot: SmtpEmailDatas?.isBot?1:0          
                }
    
                whatsappService.post(body, `/insert_smtp_address`).then((response: any) => {
                    if (response?.data?.statusCode === 200) {
                        if(state?.data =="cvm"){
                            window.parent.postMessage(
                                {
                                    type: "CVM_IFRAME",
                                    message: {
                                        navigate: "settings"
                                    },
                                },
                                "*"
                            )
                        }else{
                            navigate('/settings',{ state: { data:{ activeHeader: 1, activeMenu:0} } });
                        }
                    } else {
                        console.error(`Failed to insert:`, response?.data);
                    }
                })
    
    
                let smtpBody = {
                    "emailConfigId": 0,
                    "emailAddress": SmtpEmailDatas?.host||'',
                    "aliasName":  SmtpEmailDatas?.aliasName || '',
                    "supervisorName": `${loginUserDetails?.userId}`,
                    "emailThreshold": 24,
                    "isEmail": 1,
                    "primaryEmail": 0,
                    "isBot": 0,
                    "autoAssignAgent": 1,
                    "knowledgeBasedId": null,
                    "product_id": 15,
                    "is_smtp": 1,
                    "smtp_json": JSON.stringify(SmtpEmailDatas)||'',
                    "emailtype": 2,
                    "isRepliedEmail": 0,
                    "repliedEmail": ""
                }
                CCAASEmailServices.post(smtpBody,'/create_crm_email_config').then((res :any)=>{
                        console.log("res",res);
                })
        }
        } catch (error) {
            console.log("API ERROR:",error)
        }    
    }

    useEffect(() => {
        const loaders = document.getElementById('contact_center_mainloader');
        if (loaders) {
            loaders.style.display = 'none';
        }
    }, [])

    return (
        <ContentLayout noPaddingBlock={true}>
        <div className={styles.addemailaddresssec}>
            <div className='breadcrumbs'>
                <ol>
                    <li>
                        
                        {/* <Link to='/settings'>   
                        Set Up SMTP Email
                        </Link> */}
                            <a onClick ={()=>{
                                if (state?.data == "cvm") {
                                window.parent.postMessage(
                                    {
                                        type: "CVM_IFRAME",
                                        message: {
                                            navigate: "settings"
                                        },
                                    },
                                    "*"
                                )

                            } else {

                                navigate('/settings')
                            }}}>
                                Set Up SMTP Email
                            </a>
                    </li>
                    <li>Add Email Address</li>
                </ol>
            </div>
            <div className={styles.topheading}>
                <div className={styles.tophdlefts}>
                    <h2>Set Up SMTP Email</h2>
                    <p className={styles.headerdesctxt}>Configure your SMTP server to send and receive emails via Worktual.</p>
                </div>
            </div>
            <form className='formsec'>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="">Alias name</label>
                        <span className='formelemdesc'>Display name shown to recipients instead of the full email address.</span>
                    </div>
                    <div className='formelements'>
                       <input type="text" name="aliasName" id="aliasName" placeholder='e.g., Customer Support' onChange={(e :any)=>handleChange(e)} value={SmtpEmailDatas?.aliasName} />
                    </div>
                     {validation && (SmtpEmailDatas?.aliasName?.trim()?.length <=0 ? <span className='errortxt'>This Field is Required.</span>:!onlyAlphabetsRegex?.test(SmtpEmailDatas?.aliasName?.trim())?<span className='errortxt'>The Alias Name must be Alphabets. </span>:<></>)}
                           
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="">Sender email</label>
                        <span className='formelemdesc'>The email address used to send outgoing messages.</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="fromEmail" id="fromEmail" placeholder='e.g., support@yourdomain.com' onChange={(e :any)=>handleChange(e)} value={SmtpEmailDatas?.fromEmail}/>
                    </div>
                    {validation && (SmtpEmailDatas?.fromEmail?.trim()?.length<=0  ?<span className='errortxt'>This Field is Required.</span>:!emailRegex?.test(SmtpEmailDatas?.fromEmail?.trim())?<span className='errortxt'>Email is Invalid</span>:<></>)}
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="ailasname">User name</label>
                        <span className='formelemdesc'>Your SMTP account username (often the same as your sender email).</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="userName" id="userName" placeholder='e.g., support@yourdomain.com' onChange={(e :any)=>handleChange(e)} value={SmtpEmailDatas?.userName}/>
                    </div>
                     {validation && (SmtpEmailDatas?.userName?.trim()?.length<=0 ?<span className='errortxt'>This Field is Required.</span>:<></>)}
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="ailasname">Password</label>
                        <span className='formelemdesc'>Password for your SMTP account.</span>
                    </div>
                    <div className='formelements'>
                        <div>
                            <input type="text" name="password" id="password" placeholder='Enter your SMTP password' onChange={(e :any)=>handleChange(e)} value={SmtpEmailDatas?.password}/>
                        </div>
                        
                    </div>
                    {validation && (SmtpEmailDatas?.password?.trim()?.length<=0?<span className='errortxt'>This Field is Required.</span>:<></>)}
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="ailasname">Host</label>
                        <span className='formelemdesc'>SMTP server address (e.g., smtp.mailprovider.com).</span>
                    </div>
                    <div className='formelements'>
                        <div>
                            <input type="text" name="host" id="host" placeholder='e.g., smtp.yourprovider.com' onChange={(e :any)=>handleChange(e)} value={SmtpEmailDatas?.host}/>
                        </div>
                    </div>
                    {validation && (SmtpEmailDatas?.host?.trim()?.length<=0 ?<span className='errortxt'>This Field is Required.</span>:!hostRegex?.test(SmtpEmailDatas?.host?.trim())?<span className='errortxt'>Host is Invalid</span>:<></>)}
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="ailasname">Port</label>
                        <span className='formelemdesc'>SMTP server port (e.g., 465 for SSL, 587 for TLS).</span>
                    </div>
                    <div className='formelements'>
                        <input type="text" name="port" id="port" placeholder='e.g., 465' onChange={(e :any)=>handleChange(e)} value={SmtpEmailDatas?.port}/>
                    </div>
                     {validation && (SmtpEmailDatas?.port?.trim()?.length<=0 ?<span className='errortxt'>This Field is Required.</span>:!numericRegex?.test(SmtpEmailDatas?.port?.trim())?<span className='errortxt'>Port is Invalid</span>:<></>)}                
                </div>
                <div className='formrow'>
                    <div className='formlabels'>
                        <label htmlFor="ailasname">Bot assistance</label>
                    </div>
                    <div className='formelements'>
                        <div className={styles.botassist}>
                            <span className={styles.bttxt}>Enable the bot to send automated replies and follow-ups using this SMTP setup.</span>
                            <label className="switch">
                                <input
                                    type="checkbox"
                                    name="isBot"
                                   onChange={(e :any)=>handleChange(e)}
                                   value={SmtpEmailDatas?.isBot}
                                />
                                <span className="slider round"></span>
                            </label>
                        </div>
                    </div>
                </div>
                <div className='formbtns'>
                    <button className='cancelbtn' onClick={()=>{

                            if (state?.data == "cvm") {
                                window.parent.postMessage(
                                    {
                                        type: "CVM_IFRAME",
                                        message: {
                                            navigate: "myaccount"
                                        },
                                    },
                                    "*"
                                )

                            } else {

                                navigate('/settings')
                            }
                        }
                        
                        }>Cancel</button>
                    <button className='primarybtn' onClick={(e :any)=>handleSubmit(e)}>Next</button>
                </div>
            </form>
        </div>
        </ContentLayout>
    )
}

export default SetupSmtpEmail
