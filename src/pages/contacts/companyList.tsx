"use-client"
import { useCallback } from 'react';
import styles from './scss/contact.module.scss'
import { formatDate } from '../../base/utils';
import { useDispatcher } from '../../store/redux-store/dispatch';
import { useNavigate } from 'react-router-dom';
import { useSelector } from 'react-redux';
import SkeletonBox from '../marketing/skeletonbox';
import useBottomScrollPagination from '../../base/custom_hooks/bottom_scrollpagination';
import { contactDataSliceActions } from '../../store/slice/contactSlice';
import Nodata from '../component/nodata';

const CompanyList = () => {

    const { companyList, companyListLoading, companyListHasMore, companyListFilters } = useSelector((store: any) => store.contactStore)

    const dispatch = useDispatcher()
    const navigate = useNavigate()

    const { offset = 0 } = companyListFilters ?? {};

    const loadMore = useCallback(async () => {
        if (companyListLoading || !companyListHasMore) return;
        dispatch(contactDataSliceActions.setCompanyListFilters({ offset: (offset || 0) + 1 }));
    }, [companyListLoading, companyListHasMore, offset, dispatch]);

    const containerRef = useBottomScrollPagination({ onLoadMore: loadMore, hasMore: companyListHasMore });

    const handleCompanyClick = async (item: any) => {
        try {
            navigate(`/contact/companydetais?companyId=${encodeURI(item?.company_name)}&company_id=${item?.company_id}`)
        } catch (err) {
            console.error("Error fetching company detail:", err);
        }
    };


   

    return (
        <>
            <div className={styles.content}>
                <div className={`${styles.companyheader} ${styles.companytil}`}>
                    <div className={styles.listheader}>
                        <div className={styles.listcompany}>Company</div>
                        <p className={styles.typolist}>View and manage all companies associated with your contacts and deals.</p>
                    </div>
                    <div className={`${styles.companylist} ${styles.companytopbar}`}>
                        <div className={styles.companycontent}>
                            {companyList?.map((company, idx) => (
                                <div
                                    key={company?.customer_id ?? company?.id ?? `company-${idx}`}
                                    className={styles.companyitem}
                                    onClick={() => handleCompanyClick(company)}
                                >
                                    <div className={styles.companyinfo}>
                                        <div className={`${styles.logo} ${styles.color}`}>
                                            {(company?.company_name?.trim()?.charAt(0) || "").toUpperCase()}
                                        </div>

                                        <div className={styles.companyleftcard}>
                                            <div className={`${styles.companyleftcardtitle} truncate1`}>{(company?.company_name?.trim() || "")}</div>
                                            <div className={styles.companyleftcardsub}>{company.contact_person_email_id}</div>
                                            <div className={`${styles.companyleftcardhint} ${styles.graybadge}`}>Owner: {company.owner_name ? company.owner_name : '-'}</div>
                                        </div>
                                    </div>

                                    <div className={styles.companymeta}>
                                        <span
                                            className={`${styles.status} ${company.active_status === 1 ? styles.active : styles.inactive
                                                }`}
                                        >
                                            {company?.active_status === 1 ? 'Active' : "Inactive"}
                                        </span>

                                        <span className={styles.date}>
                                            Created {company?.created_at && formatDate(company.created_at)}
                                        </span>
                                    </div>
                                </div>
                            ))}
                            <div ref={containerRef} style={{ height: "20px" }} />
                            {/* {companyListLoading && <SkeletonBox count={8} />} */}

                        </div>
                        {companyListLoading && <div className={styles.companycontent}> <div className={styles.companyitem}>
                            <div className={styles.companyinfo}>

                                <div>

                                    <SkeletonBox width='42px' height='42px' borderRadius='50%' ></SkeletonBox></div>
                                <div className={styles.companyleftcard}>


                                    <SkeletonBox width='186px' height='19px' />

                                    <SkeletonBox width='129px' height='19px' />

                                </div>
                            </div>

                            <div className={styles.companymeta}>
                                <SkeletonBox width='89px' height='19px' />

                                <SkeletonBox width='93px' height='15px' />

                            </div>
                        </div></div>}
                    </div>
                    {companyList?.length === 0 && !companyListLoading && (
                        <Nodata
                            content='No Companies Found'
                        />)}
                </div>
            </div>
        </>
    )
}
export default CompanyList
