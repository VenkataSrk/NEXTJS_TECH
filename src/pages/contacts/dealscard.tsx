import React, { useEffect, useState } from "react";
import styles from './scss/dealscard.module.scss'
import { useSearchParams } from "react-router-dom";
import { getCustomerDealsApi } from "../../base/service/core_serviceApi";
import moment from "moment";
import Nodata from "../component/nodata";
import SkeletonBox from "../marketing/skeletonbox";


const Dealscard = () => {

    const [params, setSearchParams] = useSearchParams()
    const customerId = params.get('coid')
    const [customerDealData, SetCustomerDealData] = useState<any>([]);
    const [loading, setLoading] = useState(false);

    const getCustomerDeals = async (customerId: string | null) => {
        if (!customerId) {
            SetCustomerDealData([])
            return;
        }
        try {
            const body = {
                customerId: customerId ?? null,
            }
            setLoading(true)
            const customerDeals = await getCustomerDealsApi(body);
            setLoading(false)

            if (customerDeals?.statusCode === 200) {
                SetCustomerDealData(customerDeals?.customerDealsData)
            } else {
                SetCustomerDealData([])
            }
        } catch (error) {
            console.log('getCustomerDeals Error', error)
            SetCustomerDealData([])
        }
    }

    useEffect(() => {
        getCustomerDeals(customerId);
    }, [customerId])


    const navigatePerticular = (deal_id: any) => {
        params.set("deal_id", deal_id);
        setSearchParams(params, { replace: true });
    }

    return (
        <>
            {loading ? (
                <>
                    <SkeletonBox width="30%" height="20px" />
                    {Array.from({ length: 6 }).map((_, index) => (
                        <SkeletonBox key={`activity-skeleton-${index}`} width="100%" height="60px" />
                    ))}
                </>

            ) : (customerDealData.length > 0 ? (
                <div className={styles.dealscardcontainer}>
                    {customerDealData.map((deal: any, index: number) => (
                        <div key={index} className={styles.dealscardblock} onClick={() => navigatePerticular(deal.deal_id)}>
                            <div className={styles.dealscardblockleft}>
                                <div className={styles.dealnametitle}>
                                    {deal.deal_name} {!!deal.status && <span className={styles.closed}>{deal.status}</span>}
                                </div>

                                <div className={styles.companylist}>
                                    <div className={styles.companylistitle}>
                                        <span>Company</span>{deal.deal_details?.company}
                                    </div>
                                    <div className={styles.companylistitle}>
                                        <span>Solutions</span>{Array.isArray(deal.solutions) && deal.solutions?.join(', ')}
                                    </div>
                                    <div className={styles.companylistitle}>
                                        <span>Plan</span>{deal.plan}
                                    </div>
                                </div>
                            </div>
                            <div className={styles.dealscardblockright}>
                                <div className={styles.companytitleblock}>

                                    {/* <div className={styles.companyprofilecard}>
                                    <img src='' alt="" />
                                </div> */}

                                    <div className={styles.companylistitle}>
                                        <span>Owner</span>
                                        {deal['Deal owner'] || '-'}
                                    </div>

                                    <div className={styles.companylistitle}>
                                        <span>Created on</span>
                                        {deal.created_at ? moment(deal.created_at).format('DD MMM YYYY') : ''}
                                    </div>

                                    <div className={styles.companylistitle}>
                                        <span>Estimated closed on</span>
                                        {deal['Expected close'] || '-'}
                                    </div>

                                </div>

                                <div className={styles.companylistitle}>
                                    <span>Deal Amount</span>
                                    {deal.total_amount || '-'}
                                </div>
                            </div>

                        </div>
                    ))}
                </div>
            ) : <div style={{height: "calc(100vh - 367px)"}}> <Nodata /></div>)}


        </>
    )
}
export default Dealscard