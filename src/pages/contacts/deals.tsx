"use-client"
import { useEffect, useState } from 'react';
import styles from './scss/deals.module.scss'
import { getCustomerDealsApi } from '../../base/service/core_serviceApi';
import { useNavigate, useSearchParams } from 'react-router-dom';
import Activitytimeline from '../deals/activitytimeline';
import Arrow from '../../assets/images/contacts/chevronbreadcrumb.svg'
import { v4 as uuidv4 } from 'uuid';
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice';
import { useDispatcher } from '../../store/redux-store/dispatch';
import moment from 'moment';


const Deals = () => {

    const [params, setSeachParams] = useSearchParams()
    const customerId = params.get('coid')
    const [customerDealData, SetCustomerDealData] = useState<any>({});
    // console.log("customerDealData---->>>>",customerDealData)
    const customer_deal_id = params.get('deal_id')
    const dispatch = useDispatcher()
    const navigate = useNavigate()

    const getCustomerDeals = async (customerId: string | null) => {
        if (!customerId) {
            SetCustomerDealData({})
            return;
        }
        try {
            const body = {
                customerId: customerId ?? null,
            }
            const customerDeals = await getCustomerDealsApi(body);
            if (customerDeals?.statusCode === 200) {
                const filter_temp = customerDeals?.customerDealsData.find((item: any) => item?.deal_id == customer_deal_id)
                SetCustomerDealData(filter_temp ?? {})
            } else {
                SetCustomerDealData({})
            }
        } catch (error) {
            console.log('getCustomerDeals Error', error)
            SetCustomerDealData({})
        }
    }

    useEffect(() => {
        getCustomerDeals(customerId);
    }, [customerId, customer_deal_id])

    const formatActivityText = (value: any) => {
        if (!value) return "";
        return String(value)
            .replace(/_/g, " ")
            .replace(/\b\w/g, (char) => char.toUpperCase());
    };

    const getBadgeClassName = (status: any) => {
        const normalizedStatus = String(status || '')
            .toLowerCase()
            .replace(/[\s_]+/g, '');

        if (normalizedStatus === 'completed') return styles.completed;
        if (normalizedStatus === 'inprogress') return styles.inprogress;
        return styles.upcoming;
    };

    const normalizeToArray = (value: any) => {
        if (Array.isArray(value)) return value;
        if (!value) return [];
        if (typeof value === "string") {
            try {
                const parsedValue = JSON.parse(value);
                if (Array.isArray(parsedValue)) return parsedValue;
                if (parsedValue && typeof parsedValue === "object") return [parsedValue];
                return [];
            } catch {
                return [];
            }
        }
        if (typeof value === "object") return [value];
        return [];
    };

    const dealDetailsList = normalizeToArray(customerDealData?.deal_details);
    const dealDetailsEntry = dealDetailsList[0] || {};
    const dealDetails = dealDetailsEntry?.deal_details || dealDetailsEntry || {};

    const aiSuggestionList = normalizeToArray(customerDealData?.ai_suggest_summary_step);
    const aiSuggestions = aiSuggestionList[0] || {};
    const dealActivity = normalizeToArray(customerDealData?.deal_activity).map((item: any, index: number) => ({
        step: index + 1,
        title: item?.title || '-',
        subtitle: item?.description || '-',
        badgeText: item?.status || formatActivityText(item?.action) || "Completed",
        lineLabel: item?.status || '-',
        active: true
    }));
    const activityTimelineData = normalizeToArray(customerDealData?.activity_timeline).map((item: any) => ({
        title: item?.title || '-',
        desc: item?.description || '-',
        time: item?.datetime || '-',
    }));
    const getSubscriptionDetails = (details: any) => ([
        { title: 'Billing Type', subtitle: details?.['Billing type'] || '-' },
        {
            title: 'Solutions',
            subtitle: Array.isArray(details?.solutions) && details?.solutions?.length
                ? details?.solutions?.join(', ')
                : '-'
        },
        { title: 'Deal owner', subtitle: details?.['Deal owner']?.toString() || '-' },
        { title: 'Plan', subtitle: details?.plan || '-' },
        { title: 'Add ons', subtitle: details?.add_ons || '-' },
        { title: 'Expected Close', subtitle: details?.['Expected close'] || '-' },
        { title: 'Price', subtitle: details?.['Price'] || '-' },
    ]);
    const amountData = [
        { title: 'Total Amount', value: customerDealData?.total_amount || '-' },
        { title: 'Weighted Amount', value: customerDealData?.weighted_amount || '-' },
        { title: 'Open Amount', value: customerDealData?.open_amount || '-' },
        { title: 'Pending Amount', value: customerDealData?.pending_amount || '-' },
        { title: 'Closed Amount', value: customerDealData?.closed_amount || '-' },
        { title: 'New amount', value: customerDealData?.new_amount || '-' },
    ];

    const updateActivityHandler =()=> {
        const sessionId: any = uuidv4() + 'contact';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/contact/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "contact" }));
        navigate(`/contact/chat-conversation?sessionId=${sessionId}`);
    }

    const goBack = () => {
        params.delete('deal_id')
        setSeachParams(params, { replace: true });
    }

    return (
        <>
            <div className={styles.dealactivity}>
                <div className={styles.activitydealcontact}>
                    <div className={styles.breadcrumb}>
                        <img src={Arrow} alt="Arrow" onClick={goBack}/>
                        <div className={styles.breadcrumbtitle}>
                           <div className={styles.negotiationtitle}>
                            {customerDealData?.deal_name} 
                            {!!dealDetails?.["Lead Category"]  && <span>{dealDetails?.["Lead Category"]}</span>}
                        </div>
                           {/* <div className={styles.dealcontent}>Content needed</div> */}
                        </div>
                    </div>
                    <div className={styles.breadcrumbsubtitle}><span>Estimated closed on</span>
                    {customerDealData?.deal_estimated_closed ? moment(customerDealData?.deal_estimated_closed).format('DD MMM YYYY') : '-'}</div>
                </div>
                <div className={styles.cardcontainerdeals}>
                    <div className={styles.summarycardblock}>
                        <div className={styles.summarycardtitle}>Summary</div>
                        <div className={styles.summarysubtitle}>{customerDealData?.deal_summary || '-'}</div>
                    </div>
                    <div className={styles.totalamountblock}>
                        {amountData?.map((item, index) => (
                            <div className={styles.totalamountlist} key={index}>
                            <div className={styles.totalamountitle}>{item?.title}</div>
                            <div className={styles.totalamounsubtitle}>{item?.value}</div>
                        </div>))}
                    </div>
                {dealActivity?.length > 0 ? <div className={styles.dealactivityleft}>
                    <div className={styles.dealblock}>
                        <div className={styles.dealblocktitle}>Deal Activity</div>
                        <button onClick={updateActivityHandler}>Update Status</button>
                    </div>

                    <div className={styles.dealcreatedblock}>
                        {dealActivity.map((item, index) => (
                            <div className={styles.loopcontainer} key={index}>
                                <div
                                    className={`${styles.loopcount} ${item.active ? styles.active : ''
                                        }`}
                                >
                                    {item.step}
                                </div>

                                <div className={styles.loopblock}>
                                    <div className={styles.loopblocktitle}>{item.title}<span className={`${styles.badge} ${getBadgeClassName(item?.badgeText)}`}>{item?.badgeText}</span></div>
                                    <div className={styles.loopblocksubtitle}>{item.subtitle}</div>
                                    <div className={styles.loopblocktitleline}>{item.lineLabel}</div>
                                </div>
                            </div>
                        ))}
                    </div>
                </div> : ''}
                {dealDetailsList.map((detailItem: any, index: number) => {
                    const details = detailItem?.deal_details || detailItem || {};
                    const productName = detailItem?.product || details?.product_name || '';
                    const currentAiSuggestion = aiSuggestionList.find((item: any) => item?.product === productName) || aiSuggestionList[index] || {};
                    const subscriptionDetails = getSubscriptionDetails(details);

                    return (
                        <div className={styles.dealactivityright} key={`${productName}-${index}`}>
                            <div className={styles.subscriptionmain}>
                                <div className={styles.subscriptioncontent}>{productName || customerDealData?.deal_name || 'Ccaas subscription'}</div>
                                <div className={styles.subscriptiondetails}>
                                    {subscriptionDetails?.map((item, itemIndex) => (
                                        <div className={styles.subscriptionupdate} key={itemIndex}>
                                            <div className={styles.billingtype}>{item?.title}</div>
                                            <div className={styles.billingtypesubtitle}>{item?.subtitle}</div>
                                        </div>
                                    ))}
                                </div>
                            </div>
                            <div className={styles.blockcontainersuggestion}>
                                <div className={styles.requiredtitle}>🧩 AI Suggested Requirement:</div>
                                <div className={styles.requiredtitleaction}>{currentAiSuggestion?.['AI suggested Requirement'] || '-'}</div>
                            </div>
                            <div className={styles.summarycontainerset}>
                                <div className={styles.summarytitlesection}>Summary</div>
                                <div className={styles.subsummary}>{currentAiSuggestion?.Summary || '-'}</div>
                            </div>
                            <div className={styles.summarycontainerset}>
                                <div className={styles.summarytitlesection}>Next Steps</div>
                                <div className={styles.subsummary}>{currentAiSuggestion?.['Next Steps'] || '-'}</div>
                            </div>
                        </div>
                    );
                })}
                {/* <div className={styles.dealactivityright}>
                    <div className={styles.subscriptionmain}>
                        <div className={styles.subscriptioncontent}>{'CRM subscription'}</div>
                        <div className={styles.subscriptiondetails}>
                            {subscriptionDetails?.map((item, index) => (
                                <div className={styles.subscriptionupdate} key={index}>
                                    <div className={styles.billingtype}>{item?.title}</div>
                                    <div className={styles.billingtypesubtitle}>{item?.subtitle}</div>
                                </div>
                            ))}
                        </div>
                    </div>
                    <div className={styles.totalblock}>
                        {amountData?.map((item, index) => (
                            <div className={styles.amountsetblock} key={index}>
                                <div className={styles.amountitle}>{item?.title}</div>
                                <div className={styles.amountsubtitle}>{item?.value}</div>
                            </div>
                        ))}
                    </div>
                    <div className={styles.blockcontainersuggestion}>
                        <div className={styles.requiredtitle}>🧩 AI Suggested Requirement:</div>
                        <div className={styles.requiredtitleaction}>{aiSuggestions?.['AI suggested Requirement'] || '-'}</div>
                    </div>
                    <div className={styles.summarycontainerset}>
                        <div className={styles.summarytitlesection}>Summary</div>
                        <div className={styles.subsummary}>{aiSuggestions?.Summary || '-'}</div>
                    </div>
                    <div className={styles.summarycontainerset}>
                        <div className={styles.summarytitlesection}>Next Steps</div>
                        <div className={styles.subsummary}>{aiSuggestions?.['Next Steps'] || '-'}</div>
                    </div>
                </div> */}
                {/* {activityTimelineData?.length > 0 ? <div className={styles.dealactivityleft}>
                    <div className={styles.dealblock}>
                        <div className={styles.dealblocktitle}>Activity Timeline</div>
                        <button onClick={updateActivityHandler}>Update activity</button>
                    </div>
                    <Activitytimeline activities={activityTimelineData} />
                </div> : ''} */}
                </div>
            </div>
        </>
    )
}
export default Deals;
