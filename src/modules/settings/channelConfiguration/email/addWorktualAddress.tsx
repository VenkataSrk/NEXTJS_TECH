
import React, { useEffect, useState } from 'react'
import ContentLayout from '../../../../layout/contentLayout'
import styles from './scss/addworktualaddress.module.scss';
import { Link, useLocation, useNavigate } from 'react-router-dom';
import { whatsappService } from '../../../../base/service/apiservice';
import { GetItemFromStorage } from '../../../../base/customhooks/useStorage';
import { LocalStorage } from '../../../../base/customhooks/localStorageKeys';
import { jwtDecode } from 'jwt-decode';
import { config } from '../../../../base/constant';
import axios from 'axios';

const AddWorktualAddress = () => {
    const initialAddressState = {
        email:'',
        aliasName:'',
        isBot:0,
        isValidError:false
    }

    const [addWorktualAddress,setAddWorktualAddress] = useState<any>(initialAddressState);
       const initialLoginDetails :any= jwtDecode(GetItemFromStorage(LocalStorage.ACCESS_TOKEN)) ;
    console.log("logdetails",initialLoginDetails);
    const [emailDomain, setEmailDomain] = useState(`@${initialLoginDetails?.companyName?.replaceAll(' ', '')}${initialLoginDetails?.domainId}.worktual.tech`)
    console.log("address",addWorktualAddress);
    const onlyAlphabetsRegex =/^[a-zA-Z]+$/;
    const alphaNumericRegex =/^[a-zA-Z0-9]+$/;
    const navigate = useNavigate();
    const location = useLocation();
    const {state} :any= location;

    const handleChange=(e :any)=>{
        const {name,value,checked}=e?.target;
        if(name=='email' || name=='aliasName'){
            setAddWorktualAddress({...addWorktualAddress,[name]:value});
        }
        if(name=='isBot'){
            setAddWorktualAddress({...addWorktualAddress,[name]:checked?1:0});
        }
    }

    const handleValidation=()=>{
         setAddWorktualAddress({...addWorktualAddress,isValidError:true});

        if(addWorktualAddress?.email?.trim()?.length<=0 || !alphaNumericRegex?.test(addWorktualAddress?.email?.trim())){
            return
        }

        if(addWorktualAddress?.aliasName?.trim()?.length>0 && !onlyAlphabetsRegex?.test(addWorktualAddress?.aliasName?.trim())){
            return
        }
        setAddWorktualAddress({...addWorktualAddress,isValidError:false});
       
    }

    const insertWorktualAddress =async (e :any)=>{
      
        try {
                e.preventDefault();
                handleValidation();
                const body = {
                    id: null,
                    email: addWorktualAddress?.email|| '',
                    aliasName: addWorktualAddress?.aliasName||'',
                    isBot:addWorktualAddress?.isBot|| 0                   
                }

                whatsappService.post(body, `/add_worktual_address`).then((response: any) => {
                    if (response?.data?.statusCode === 200) {
                        
                    } else {
                        console.error(`Failed to insert:`, response?.data);
                    }
                })
                    let domain = emailDomain.split("@");
                    let username =  addWorktualAddress?.email + "" + emailDomain;
                    let data = {
                        p_domain: domain[1],
                        p_username: username,
                        p_password: "admin123",
                        p_name: "test",
                        p_maildir: domain[1] + "/" + addWorktualAddress?.email ,
                        p_quota: "512000",
                        p_local_part: addWorktualAddress?.email ,
                        p_active: 1,
                        p_phone: "",
                        p_email_other: "",
                        p_token: "",
                        p_processtype: 1,
                    };
                    const MyaccApiurl: any = config().MYACCOUNTSERVICE_URL;
                    const res_domain: any = await axios.post(MyaccApiurl + "/token/ccass_domain_mailbox", data);
                    let _body = {
                        "p_domain": domain[1],
                        "p_description": `${initialLoginDetails?.companyName} company`,
                        "p_aliases": 10,
                        "p_mailboxes": 10,
                        "p_maxquota": "10",
                        "p_quota": "2048",
                        "p_transport": "virtual",
                        "p_backupmx": 0,
                        "p_active": 1,
                        "p_password_expiry": 365,
                        "p_domain_id": initialLoginDetails?.domainId
                    }
                    await axios.post(MyaccApiurl + "/token/domain_create_info", _body);
                    if(state?.data=="cvm"){
                        window.parent.postMessage(
                            {
                                type: "CVM_IFRAME",
                                message: {
                                    navigate: "settings"
                                },
                            },
                            "*"
                        )
                    }
                    else{

                        navigate('/settings');
                    }
            
        } catch (error) {
            console.error('Error Inserting Worktual Address for Email:', error);
        }
    }
    useEffect(()=>{
        const loaders = document.getElementById('contact_center_mainloader');
        if (loaders) {
            loaders.style.display = 'none';
        }
    },[])


    return (
        <ContentLayout noPaddingBlock={true}>
            <div className={styles.addemailaddresssec}>
                <div className="breadcrumbs">
                    <ol>
                        <li>
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
                                Settings
                            </a>
                            {/* <Link to="/settings">Settings</Link> */}
                        </li>
                        <li>Add Worktual address</li>
                    </ol>
                </div>
                <div className={styles.topheading}>
                    <div className={styles.tophdlefts}>
                        <h2>Add Worktual address</h2>
                        <p className={styles.headerdesctxt}>Set up your Worktual email address and optional bot assistance.</p>
                    </div>
                </div>
                <form className='formsec' onSubmit={(e :any)=>insertWorktualAddress(e)}>

                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="">Email<span className='required'>*</span></label>
                            <span className='formelemdesc'>Enter the email username. Your domain is fixed as @mundio.worktual.com.</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="email" id="email" onChange={(e) => handleChange(e)} value={addWorktualAddress?.email} placeholder='eg: @mundio.worktual.com' />
                            {addWorktualAddress?.isValidError && (addWorktualAddress?.email?.trim()?.length <= 0) && <span className='errortxt'>This Field is Required.</span>}
                            {addWorktualAddress?.isValidError && !alphaNumericRegex?.test(addWorktualAddress?.email?.trim()) && addWorktualAddress?.email?.trim()?.length > 0 && <p>Alpha Numeric characters only Allowed.</p>}
                        </div>
                    </div>
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="ailasname">Ailas name</label>
                            <span className='formelemdesc'>Display name shown to recipients instead of the full email address.</span>
                        </div>
                        <div className='formelements'>
                            <input type="text" name="aliasName" id="aliasName"   onChange={(e) => handleChange(e)} value={addWorktualAddress?.aliasName} placeholder='Enter name' />
                        </div>
                    </div>
                    {addWorktualAddress?.isValidError &&  !onlyAlphabetsRegex?.test(addWorktualAddress?.aliasName?.trim())&&  addWorktualAddress?.aliasName?.trim()?.length>0  && <p>Alias name is characters only Allowed.</p>}
                    <div className='formrow'>
                        <div className='formlabels'>
                            <label htmlFor="ailasname">Bot assistance</label>
                        </div>
                        <div className='formelements'>
                            <div className={styles.botassist}>
                                <span className={styles.bttxt}>Enable your Bot for automated and regenerative assistance</span>
                                <label className="switch">
                                    <input
                                        type="checkbox"
                                        name="isBot"
                                        onChange={(e) => handleChange(e)}
                                        value={addWorktualAddress?.isBot}
                                    />
                                    <span className="slider round"></span>
                                </label>
                            </div>
                        </div>
                    </div>
                    <div className='formbtns'>
                        <button className='cancelbtn' type="button" onClick={()=> {
                            if(state?.data=="cvm"){


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
                            else {

                                navigate('/settings')
                            }
                            }}>Cancel</button>
                        <button className='primarybtn' 
                        type ='submit'
                        //  onClick={(e :any)=>insertWorktualAddress(e)}
                         >Next</button>
                    </div>
                </form>
            </div>
        </ContentLayout>
    )
}

export default AddWorktualAddress


