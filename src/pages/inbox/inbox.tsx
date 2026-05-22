import React, { useCallback, useEffect, useState } from "react";
import styles from "./scss/inbox.module.scss"
import chevrondown from "../../assets/images/inbox/chevrondown.svg";
import mailicon from "../../assets/images/inbox/mailicon.svg";
import customsms from "../../assets/images/inbox/emptysmss.svg";
import aicon from "../../assets/images/inbox/aiiconnew.svg";
import purpleicn from "../../assets/images/inbox/callpurple.svg"
import contactwindow from "../../assets/images/inbox/contactwindow.svg";
import bluechevron from "../../assets/images/inbox/bluechevron.svg";
import reply from "../../assets/images/inbox/reply.svg";
import forward from "../../assets/images/inbox/forward.svg";
import ComposeMail from "./composemail";
import threedot from "../../assets/images/inbox/threedot.svg";
import openemail from "../../assets/images/inbox/mailopen.svg";
import newemail from "../../assets/images/inbox/newemail.svg";
import filter from "../../assets/images/inbox/filter.svg";

import Transcription from "./transcription";
import LeftWorkspace from "./leftworkspace";
import RightWorkspace from "./rightworkspace";
import { useSelector } from "react-redux";
import EmailContainer from "./emailcontainer";
import { useLocation, useNavigate, useSearchParams } from "react-router-dom";
import { getContactDetails, getCustomerNotesApi, getDraftForEmailApi, getPerticularInboxSessionDetails } from "../../base/service/core_serviceApi";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice";
import Nodata from "../component/nodata";
import Aichat from '../../assets/images/projectinsights/aiconinsights.svg'
import Inboxdrawer from "./inboxdrawer";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";


function Inbox() {

    // const filterChip = ['Assigned to me', 'Unassigned', 'Email', 'Call', 'Sent', 'Trash',  "Draft"];
    const filterChip = ['Inbox','Sent','Draft',"Trash"];
    const [tabLoading, setTabLoading] = useState(false);
    const [offRightSpace, setOffRightSpace] = useState(true);
    const [draftCount, setDraftCount] = useState(0);
    const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);

    const { composeMailToggle, perticularInboxContactData, inboxFilter, unReadCount } = useSelector((state: any) => state.inboxChatStore);

    const [params] = useSearchParams()
    const channelType = params.get('channelType')
    const sessionId = params.get('sessionId')
    const historyId = params.get('historyId');
    const CustomerId = params.get('CustomerId')
    const dispatch = useDispatcher()
    const navigate = useNavigate()

 
    useEffect(() => {
        init()
    }, [sessionId, historyId])  

    const init = async () => {
        if(sessionId || historyId){
            getPerticularInboxSessionDetails({ channelType, sessionId, historyId }, dispatch)
            if (channelType === 'Call') {
                const body = {
                    customerId: CustomerId
                }
                dispatch(inboxDataSliceActions.setCustomerNotes([]));
                getCustomerNotesApi(body, dispatch);
            }
        }
        if (Number(CustomerId)) {
            const res = await getContactDetails(Number(CustomerId), null);
            dispatch(inboxDataSliceActions.setPerticularInboxContactData({
                ...perticularInboxContactData,
                contacts: res?.[0] ?? {}
            }));
        }
    }

    const handleFilterClick = (item: string) => {
        if (tabLoading) return;
        
        dispatch(inboxDataSliceActions.setInboxFilter({ 
            fromDate: null,
            toDate: null,
            search: null,
            tab: item,
            channelType: 'Email,Call',
            inboxType: null
        }));
        
        navigate({
            pathname: '/inbox',
            hash: `#${item}`,
        });

    };
    const [isOpen, setIsOpen] = useState(false);

    const closeAiChat = useCallback(()=>{
        setIsOpen(false)
        // dispatch(inboxDataSliceActions.resetInboxChat());
    },[])

    const handlesetTabLoading =(data :any) => {
        setTabLoading(data)
    }

    const openChat = () => {
        setIsOpen(!isOpen); 
        dispatch(commonDataSliceActions.setAiChat(true));
        dispatch(inboxDataSliceActions.resetInboxChat());
    }

    useEffect(()=> {
        
        getDraftCount()
    },[])
    
    const getDraftCount = async ()=> {
        const draftResponse: any = await getDraftForEmailApi({
            domainId: LoginUserDetails?.domainId ?? LoginUserDetails?.domain_id ?? null,
            agentId: LoginUserDetails?.userId ?? null,
            search: null,
            fromDate: null,
            toDate: null,
        });
        setDraftCount(Array.isArray(draftResponse) ? draftResponse?.length : 0);
    }

    return (<>
        <div className={`${styles.inbox} inboxpage`}>
            <div className={`${styles.topbar} ${styles.filterstop}`}>
              <div className={`${styles.filters} p16`}>

                {filterChip?.map((item, index) => {
                    return (
                        <span
                      
                            className={`${styles.filterchip} ${inboxFilter.tab === item ? styles.active : ''}`}
                            key={index}
                            onClick={() => handleFilterClick(item)}
                        >
                            <span style={{display:"inline-flex", gap:"8px", alignItems:"center"}}> 
                                {item}
                                {item === 'Inbox' && !!unReadCount && <span className={styles.counterspan}>{unReadCount}</span>}
                                {item === 'Draft' && !!draftCount && <span className={styles.counterspan}>{draftCount}</span>}
                            </span>
                          
                        </span>
                    );
                })}
              </div>
              <button onClick={openChat}><img src={Aichat} alt="Aichat" />Ai chat</button>
              {/* {isOpen && (<Inboxdrawer close={closeAiChat}/>)} */}
              
            </div>
           
             
         


            <div className={`${styles.body} ${
                (!offRightSpace && CustomerId)? `${styles.norightside} norightside`  :
                (composeMailToggle || !CustomerId) ? `${styles.norightside} norightside` : ""
                }`}>
                {/* LEFT – LIST */}

                <LeftWorkspace activeFilter={inboxFilter.tab} setTabLoading={handlesetTabLoading}/>

                {/* CENTER – MAIL */}

                {channelType === 'Email' ? <EmailContainer offRightSpace={offRightSpace} activeFilter={inboxFilter.tab} setOffRightSpace={setOffRightSpace}/>
                    : channelType === 'Call' ? <Transcription offRightSpace={offRightSpace} setOffRightSpace={setOffRightSpace}/>
                        : (
                        <div style={{ height: '100%', display: 'flex', flexDirection: "column", alignItems: 'center', justifyContent: 'center' }}>
                           <Nodata image={customsms} content="Select an item to read" subContent="Nothing selected" />
                        </div>)}


                {/* RIGHT – CONTACT */}
                {!composeMailToggle && !!CustomerId && <RightWorkspace offRightSpace={offRightSpace} setOffRightSpace={setOffRightSpace}/>}

            </div>
        </div>
    </>)
} export default Inbox;
