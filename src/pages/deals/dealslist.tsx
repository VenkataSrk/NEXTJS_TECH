import React, { useEffect, useState } from "react";
import mailicon from "../../assets/images/inbox/mailicon.svg"
import styles from "./scss/dealslist.module.scss"
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { useNavigate } from "react-router-dom";
import { getCustomerDealsApi } from "../../base/service/core_serviceApi";
import Nodata from "../component/nodata";
import SkeletonBox from "../marketing/skeletonbox";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { useSelector } from "react-redux";
import Tooltip from "../component/tooltip/tooltip";

const Dealslist = () => {

    const dispatch = useDispatcher();
    const navigate = useNavigate();
    const { customerDealData, dealsloader } = useSelector((store: any) => store.dealsStore);
    const getIndivitualDeals = (deal: any) => {
        const path = `/deals/dealscontent?coid=${deal?.customer_id}&dealsId=${deal?.deal_id}`;
        dispatch(commonDataSliceActions.setTabMenuPush({
            path,
            menuName: deal?.deal_name || 'Deal Name',
            close: 1,
            type: "deals"
        }));
        navigate(path);
    }

    const formatDateLabel = (dateValue: string | null) => {
        if (!dateValue) return '-';
        const dateObj = new Date(dateValue);
        if (Number.isNaN(dateObj.getTime())) return '-';
        return dateObj.toLocaleDateString("en-GB", {
            day: "2-digit",
            month: "short",
            year: "numeric",
        });
    };

    return (
        <>



            {Array.isArray(customerDealData) && customerDealData?.length > 0 && <div className={styles.dealslistcontainer}>
                <div className={styles.namebadgedit}>
                    {Array.isArray(customerDealData) && customerDealData?.length > 0 && customerDealData?.map((deal: any) => {
                        const dealDetails = deal?.deal_details || {};
                        const solutionsArray = Array.isArray(dealDetails?.solutions) && dealDetails?.solutions?.length
                            ? dealDetails.solutions
                            : Array.isArray(dealDetails?.interested_product) && dealDetails?.interested_product?.length
                                ? dealDetails.interested_product
                                : [];
                        const normalizedSolutions = solutionsArray.map(item =>
                            typeof item === "string" ? item : item?.name || "-"
                        );

                        const firstItem = normalizedSolutions[0];
                        const remainingItems = normalizedSolutions.slice(1);
                        const remainingCount = remainingItems.length;
                        const ownerName = dealDetails?.['Deal owner'] || dealDetails?.assigned_agent || '-';
                        const expectedClose = dealDetails?.['Expected close'] || '-';
                        const companyName = dealDetails?.company || '-';
                        const plan = dealDetails?.plan || '-';
                        const badgeText = deal?.pipeline_stage || '';
                        // const badgeText = deal?.deal_activity?.[0]?.status || deal?.deal_activity?.[0]?.title || deal?.deal_activity?.[0]?.action || 'Deal';

                        return (
                            <div className={styles.dealsinnerbody} key={deal?.deal_id} onClick={() => getIndivitualDeals(deal)}>
                                <div className={styles.card}>
                                    <div className={styles.top}>
                                        <div className={styles.namewithbadge}>
                                            <h5>{deal?.deal_name || '-'}</h5>
                                            {!!badgeText && <span className={`${styles.badge} ${styles.green}`}>{badgeText}</span>}
                                        </div>
                                        <div className={`${styles.secondline} hastooltip`}>
                                            <Tooltip text= {companyName} className="tooltipinsidedealslist" >
                                                          <p className={`truncate1`}><span className={styles.boldtxt}>Company</span>{companyName}</p>
                                                            </Tooltip>
                                            <p className={`truncate1`}>
                                                <span className={styles.boldtxt}>
                                                    Solutions
                                                </span>{" "}
                                                {firstItem || "-"}

                                                {remainingCount > 0 && (
                                                    <span
                                                        className={styles.moreWrapper}
                                                        onClick={(e) => e.stopPropagation()}
                                                    >
                                                        {" "}+{remainingCount}

                                                        <span className={styles.tooltip}>
                                                            {remainingItems.join(", ")}
                                                        </span>
                                                    </span>
                                                )}
                                            </p>
                                            <p className={`truncate1`}><span className={styles.boldtxt}>Plan</span>{plan}</p>
                                        </div>
                                    </div>
                                    <div className={styles.bottom}>
                                        <div className={styles.lefts}>
                                            <div className={styles.mailgroup}>
                                                <img src={mailicon} />
                                                <p><span className={styles.boldtxt}>Owner</span>{ownerName}</p>
                                            </div>
                                            <p><span className={styles.boldtxt}>Created on</span>{formatDateLabel(deal?.created_at)}</p>
                                            <p><span className={styles.boldtxt}>Estimated closed on</span>{expectedClose ?? '-'}</p>
                                        </div>
                                        <div className={styles.rights}>
                                            <p><span className={styles.boldtxt}>Deal Amount</span>{deal?.total_amount ?? '-'}</p>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        )
                    })}


                </div>
            </div>}
            {(customerDealData?.length === 0 && !dealsloader) &&
                <Nodata />
            }
            {dealsloader && (


                <div className={`${styles.dealslistcontainer} ${styles.skeletonStyles}`}>
                    <div className={styles.namebadgedit}>
                        {Array.from({ length: 6 }, (_, index) => (
                            <div className={styles.dealsinnerbody}>
                                <div className={styles.card}>
                                    <div className={styles.top}>
                                        <div className={styles.namewithbadge} style={{ marginBottom: 4 }}>
                                            <div className={styles.skeletonTitle}></div>
                                            <div className={styles.skeletonBadge}></div>
                                        </div>

                                        <div className={styles.secondline}>
                                            <p>
                                                <span className={styles.boldtxt}>Company</span>
                                                <span className={styles.skeletonTextsm}></span>
                                            </p>
                                            <p>
                                                <span className={styles.boldtxt}>Solutions</span>
                                                <span className={styles.skeletonTextsm}></span>
                                            </p>
                                            <p>
                                                <span className={styles.boldtxt}>Plan</span>
                                                <span className={styles.skeletonTextsm}></span>
                                            </p>
                                        </div>
                                    </div>

                                    <div className={styles.bottom}>
                                        <div className={styles.lefts}>
                                            <div className={styles.skeletonIcon}></div>
                                            <p>
                                                <span className={styles.boldtxt}>Owner</span>
                                                <span className={styles.skeletonTextsm}></span>
                                            </p>
                                            <p>
                                                <span className={styles.boldtxt}>Created on</span>
                                                <span className={styles.skeletonTextsm}></span>
                                            </p>
                                            <p>
                                                <span className={styles.boldtxt}>Estimated closed on</span>
                                                <span className={styles.skeletonTextsm}></span>
                                            </p>
                                        </div>

                                        <div className={styles.rights}>
                                            <p>
                                                <span className={styles.boldtxt}>Deal Amount</span>
                                                <span className={styles.skeletonTextsm}></span>
                                            </p>
                                        </div>
                                    </div>
                                </div>
                            </div>

                        ))}
                    </div>
                </div>
            )}
        </>
    )
}
export default Dealslist;
