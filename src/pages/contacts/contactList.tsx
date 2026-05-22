"use-client"
import { useEffect, useRef } from 'react';
import styles from './scss/contact.module.scss'
import Contactname from './contactname';
import { getContactDetailsDomainList } from '../../base/service/core_serviceApi';
import { formatDate, stripHtmlTags } from '../../base/utils';
import Companyname from './companyname';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { Outlet, useNavigate } from 'react-router-dom';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { v4 as uuidv4 } from 'uuid';
import { useSelector } from 'react-redux';
import SkeletonBox from '../marketing/skeletonbox';
import Dynamicnodata from '../component/Dynamicnodata';
import useBottomScrollPagination from '../../base/custom_hooks/bottom_scrollpagination';
import aiChatAssistSocketservice from '../../services/aiChatService';
import { contactDataSliceActions } from '../../store/slice/contactSlice';
import { GetItemFromStorage } from '../../base/custom_hooks/useStorage';
import { LocalStorage } from '../../base/custom_hooks/localStorageKeys';
import Modal from '../component/modal/modal';


const getInitials = (firstName?: string, lastName?: string) => {
    if (!firstName && !lastName) return "N";
    return `${firstName?.[0] || ""}${lastName?.[0] || ""}`.toUpperCase();
};

const getDisplayName = (contact :any) => {

    const {first_name, last_name, source_type} = contact
    const name = `${first_name || ""} ${last_name || ""}`.trim();
    let result = first_name ? name : "New Customer"
    if(!first_name){
        // IMPORT, API, Website, email, Facebook, CRM Chat, Web_chatbot, crm_chatbot, Lola_chatbot
        if(source_type?.toLowerCase() === "email") 
            result = contact?.primary_email
        else if(source_type?.toLowerCase() === "call") 
            result = contact?.primary_phone_number
    }
    return result || "New Customer";
};

const ContactsList = () => {
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const dispatch = useDispatcher()
    const navigate = useNavigate()
    const { contactList, contactListLoading, contactListFilters } = useSelector((store: any) => store.contactStore)
    const offsetData = useRef(0);
    const hasMore = useRef(true);
    const limit = 50

    const {
        from_date,
        to_date,
        is_active,
        customer_name,
        account_owner,
        email,
        phone_number,
        address,
        industry_type,
        source_type,
        company_name,
        company_size,
        tags,
        lead_status
    } = contactListFilters ?? {}

    const containerRef = useBottomScrollPagination({ onLoadMore: loadMore, hasMore: hasMore.current });

    const handleContactClick = async (item: any) => {
        navigate('/contact/contactdetais?coid=' + item?.customer_id)
    };

    const getContactDeatils = async (offset = 0, limit = 50)=> {
        dispatch(contactDataSliceActions.setContactList({ contactListLoading: true }));
        const contactRes = await getContactDetailsDomainList({
            offset: offset,
            limit: limit,
            from_date: from_date ?? null,
            to_date: to_date ?? null,
            is_active: is_active ?? null,
            customer_name: customer_name ?? null,
            account_owner: account_owner ?? null,
            email: email ?? null,
            phone_number: phone_number ?? null,
            address: address ?? null,
            industry_type: industry_type ?? null,
            source_type: source_type ?? null,
            company_name: company_name ?? null,
            company_size: company_size ?? null,
            tags: tags ?? null,
            lead_status: lead_status ?? null,
        })
        const [contactPagination, totalCount] = contactRes;
        if(!contactPagination?.length && offset > 0){
            dispatch(contactDataSliceActions.setContactList({ contactListLoading: false }));
            hasMore.current = false;
            return;
        }
        hasMore.current = true;
        dispatch(contactDataSliceActions.setContactList({
            contactListLoading: false,
            contactList: offset === 0 ? contactPagination : [...(contactList || []), ...contactPagination]
        }))
        offsetData.current += 1;
    }

    async function loadMore(){
        if(contactList?.length > ((offsetData.current * limit) - 1)){
            console.log("Fetching next page...");
            await getContactDeatils(offsetData.current, limit)
        }
    };

    const handleConvoStart = (value: any, files: any) => {

        let uploadfiles: any = files;
        if (uploadfiles && uploadfiles.length > 0) {
            uploadfiles = uploadfiles?.map((item: any) => {
                return (
                    {
                        "filename": item?.filename,
                        "mimetype": item?.mimetype,
                        "size": item?.size,
                        "uploadUrl": item?.url,
                        "uniqueId": item?.uuid
                    }
                )
            })
        }
        const sessionId: any = uuidv4() + 'contact';
        let body: any = {
            query: stripHtmlTags(value?.trim()),
            sessionId: sessionId,
            roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
            org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
            userId: `${LoginCcaasUserDetails?.userId || ''}`,
            domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
            module: "contactList",
            ext: `${LoginCcaasUserDetails?.ext || ''}`,
            agent: 'contact',
            isBot: 0,
            customerId: '',
            companyId: '',
            companyName: '',
            uploadUrl: uploadfiles ?? []
        }

        aiChatAssistSocketservice.io.emit('crm_chat', body);

        dispatch(contactDataSliceActions.setcontactAiAgentChat({ ...body }))
        dispatch(contactDataSliceActions.setcontactAiAgentChatTyping({ sessionId: sessionId, typing: true }));

        const path = `/contact/chat-conversation?sessionId=${sessionId}`
        dispatch(commonDataSliceActions.setTabMenuPush({ path: path, menuName: "New Conversation", close: 1, type: "contact" }));
        navigate(path);
    }

    return (
        <>
            <div className={styles.content}>
                <div className={`${styles.companyheader} ${styles.companytil}`}>
                    <div className={styles.listheader}>
                        <div className={styles.listcompany}>Contacts</div>
                        <p className={styles.typolist}>Access contact details, owners, and current status for this account.</p>
                    </div>
                    <div className={`${styles.companylist} ${styles.companytopbar}`}>
    
                        {contactListLoading || contactList?.length > 0 ? (
                            <div className={styles.companycontent}>    

                                {contactList?.map((contact, idx) => (
                                    <div
                                        key={contact?.customer_id ?? contact?.id ?? `contact-${idx}`}
                                        className={styles.companyitem}
                                        onClick={() => handleContactClick(contact)}
                                    >
                                        <div className={styles.companyinfo}>
                                            <div className={`${styles.logo} ${styles.color}`}>{getInitials(contact?.first_name, contact?.last_name)}</div>
                                            <div className={styles.companyleftcard}>
                                                <div className={`${styles.companyleftcardtitle} truncate1`}>
                                                    {getDisplayName(contact)}
                                                </div>
                                                <div className={styles.companyleftcardsub}>{contact.primary_email}</div>
                                                <div style={{padding:"0px"}} className={styles.greywrapper}>
                                                    <div className={`${styles.companyleftcardhint} ${styles.greybadge}`}>Owner: {contact.sales_name ?? '-'}</div>
                                                    <div className={`${styles.companyleftcardhint} ${styles.greybadge}`}>Company name: {contact.company_name ?? '-'}</div>
                                                </div>

                                            </div>
                                        </div>
                                        <div className={styles.companymeta}>
                                            <span
                                                className={`${styles.status} ${contact.is_active === 1 ? styles.active : styles.inactive
                                                    }`}
                                            >
                                                {contact?.is_active === 1 ? 'Active' : 'Inactive'}
                                            </span>
                                            <span className={styles.date}>
                                                Created {contact?.created_at && formatDate(contact.created_at)}
                                            </span>
                                        </div>
                                    </div>
                                ))}

                                {/* {contactListLoading && <SkeletonBox count={8} />} */}
                                {contactListLoading && !contactList?.length && (
                                    Array.from({ length: 8 }).map((_, index) => (
                                        <div className={styles.companycontent}>
                                            <div className={styles.companyitem}>
                                                <div className={styles.companyinfo}>
                                                    <div>
                                                        <SkeletonBox width='42px' height='42px' borderRadius='50%'></SkeletonBox></div>
                                                    <div className={styles.companyleftcard}>
                                                        <SkeletonBox width='186px' height='19px' />
                                                        <SkeletonBox width='129px' height='19px' />
                                                    </div>
                                                </div>
                                                <div className={styles.companymeta}>
                                                    <SkeletonBox width='89px' height='19px' />
                                                    <SkeletonBox width='93px' height='15px' />
                                                </div>
                                            </div>
                                        </div>
                                    ))
                                )} 
                              
                                <div ref={containerRef} style={{ height: "20px" }} />
                            </div>
                        ) : (
                            <Dynamicnodata
                                title='No contacts yet'
                                message='Your contact list is empty. Create your first contact to start making calls, assigning tickets, and tracking customer conversations.'
                                buttonCallback={() => handleConvoStart('Create contact', null)}
                            />
                        )}
  
                    </div>
                </div>
            </div>
        </>
    )
}
export default ContactsList
