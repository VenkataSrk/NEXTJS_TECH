import styles from "./scss/marketing.module.scss";
import SetUpCommonEditor from "./commoneditor";
import { useDispatch } from "react-redux";
import { useNavigate } from "react-router-dom";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import Nodata from "../component/nodata";
import { v4 as uuidv4 } from "uuid";
import { useEffect, useState } from "react";
import { CoreServices } from "../../base/service/apiservices";
import SkeletonBox from "./skeletonbox";
import moment from "moment";
const EmailSvg = () => {
    return (
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect width="24" height="24" rx="12" fill="#E06E3A" />
            <g clip-path="url(#clip0_1998_9300)">
                <path d="M11.6933 10.6433C11.88 10.7566 12.12 10.7566 12.3066 10.6433L17.9066 7.10992C17.5666 6.83659 17.14 6.66992 16.6666 6.66992H7.33331C6.84665 6.66992 6.41331 6.84992 6.06665 7.13659L11.6933 10.6499V10.6433Z" fill="white" />
                <path d="M18.5133 7.90326L12.84 11.4899V11.4833C12.5866 11.6433 12.2933 11.7233 12 11.7233C11.7066 11.7233 11.42 11.6433 11.1666 11.4833L5.47331 7.93659C5.38665 8.16326 5.33331 8.40326 5.33331 8.66326V15.3299C5.33331 16.4366 6.22665 17.3299 7.33331 17.3299H16.6666C17.7733 17.3299 18.6666 16.4366 18.6666 15.3299V8.66326C18.6666 8.39659 18.6133 8.13659 18.5133 7.90326Z" fill="white" />
            </g>
            <defs>
                <clipPath id="clip0_1998_9300">
                    <rect width="16" height="16" fill="white" transform="translate(4 4)" />
                </clipPath>
            </defs>
        </svg>
    )
}

const SMSSvg = () => {
    return (
        <svg width="24" height="24" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect width="24" height="24" rx="12" fill="#3A9EE0" />
            <g clip-path="url(#clip0_718_2710)">
                <path d="M12.003 5.66657C8.50329 5.66657 5.66669 8.50337 5.66669 12.0028V17.4666C5.66669 17.9452 6.05469 18.3332 6.53322 18.3332H11.9987C15.4971 18.3332 18.3334 15.4972 18.3334 11.9987V11.997C18.3334 8.50077 15.499 5.66657 12.003 5.66657ZM9.93609 13.0744C9.84349 13.217 9.70855 13.3289 9.53095 13.4097C9.35309 13.4905 9.14209 13.5309 8.89862 13.5309C8.60662 13.5309 8.36542 13.4756 8.17562 13.3652C8.04102 13.2857 7.93149 13.1798 7.84749 13.0473C7.76302 12.9144 7.72102 12.7856 7.72102 12.6604C7.72102 12.5876 7.74655 12.5254 7.79655 12.4737C7.84749 12.4216 7.91182 12.3958 7.98962 12.3958C8.05309 12.3958 8.10649 12.4159 8.15042 12.4563C8.19422 12.4967 8.23129 12.5565 8.26242 12.6361C8.30002 12.7302 8.34069 12.809 8.38495 12.8724C8.42855 12.9358 8.48995 12.9879 8.56935 13.0287C8.64895 13.0699 8.75329 13.0905 8.88249 13.0905C9.06009 13.0905 9.20475 13.0489 9.31562 12.9665C9.42709 12.8833 9.48229 12.7803 9.48229 12.6563C9.48229 12.5582 9.45249 12.4782 9.39229 12.4172C9.33242 12.3558 9.25489 12.3089 9.16049 12.2766C9.06509 12.2443 8.93815 12.21 8.77949 12.1736C8.56662 12.124 8.38889 12.0658 8.24515 11.9987C8.10195 11.9324 7.98815 11.8412 7.90415 11.7264C7.81969 11.6109 7.77769 11.4679 7.77769 11.297C7.77769 11.1343 7.82215 10.9892 7.91115 10.8628C7.99989 10.7364 8.12829 10.639 8.29689 10.5712C8.46515 10.5029 8.66309 10.469 8.89062 10.469C9.07229 10.469 9.22949 10.4916 9.36229 10.5364C9.49475 10.5816 9.60482 10.6414 9.69235 10.7166C9.78009 10.791 9.84389 10.8697 9.88422 10.9518C9.92455 11.0337 9.94495 11.1141 9.94495 11.1921C9.94495 11.2636 9.91975 11.3278 9.86895 11.3848C9.81882 11.4422 9.75582 11.4708 9.68015 11.4708C9.61162 11.4708 9.55935 11.4534 9.52389 11.4195C9.48789 11.3848 9.44915 11.3286 9.40749 11.2507C9.35375 11.1388 9.28915 11.052 9.21362 10.989C9.13809 10.9268 9.01682 10.8952 8.85015 10.8952C8.69555 10.8952 8.57035 10.9292 8.47515 10.997C8.38062 11.0652 8.33309 11.1468 8.33309 11.2426C8.33309 11.302 8.34922 11.3528 8.38142 11.3961C8.41362 11.4393 8.45809 11.476 8.51475 11.5072C8.57142 11.5383 8.62855 11.5625 8.68649 11.5799C8.74429 11.5973 8.83995 11.6231 8.97309 11.6566C9.14009 11.6958 9.29129 11.7386 9.42702 11.7858C9.56175 11.8331 9.67735 11.8901 9.77175 11.9576C9.86715 12.025 9.94102 12.1102 9.99409 12.2129C10.0473 12.3163 10.0742 12.4423 10.0742 12.5918C10.0742 12.7707 10.0281 12.9318 9.93609 13.0744ZM13.4128 13.1916C13.4128 13.3046 13.3873 13.3894 13.3373 13.446C13.2864 13.5026 13.219 13.5309 13.1342 13.5309C13.0546 13.5309 12.9896 13.5026 12.9382 13.446C12.8868 13.3894 12.8614 13.3046 12.8614 13.1916V11.1476L12.3866 13.034C12.3556 13.1564 12.3306 13.2465 12.3113 13.3034C12.2914 13.3608 12.2556 13.4129 12.203 13.4602C12.1504 13.5074 12.0779 13.5309 11.985 13.5309C11.915 13.5309 11.8556 13.516 11.8072 13.4852C11.7587 13.4553 11.7211 13.4166 11.6941 13.3692C11.6671 13.322 11.6459 13.2699 11.6303 13.2129C11.615 13.1556 11.5992 13.0958 11.5828 13.034L11.1085 11.1475V13.1915C11.1085 13.3046 11.0833 13.3894 11.0323 13.446C10.9823 13.5025 10.9153 13.5308 10.8316 13.5308C10.7508 13.5308 10.685 13.503 10.6337 13.4472C10.5823 13.391 10.557 13.3058 10.557 13.1915V10.8486C10.557 10.7193 10.5904 10.632 10.658 10.5872C10.7254 10.542 10.8162 10.5194 10.9304 10.5194H11.1165C11.2283 10.5194 11.3094 10.5294 11.3595 10.5497C11.4106 10.5699 11.4476 10.6062 11.4718 10.6588C11.4962 10.7113 11.5238 10.797 11.5548 10.9153L11.985 12.5371L12.4152 10.9153C12.4463 10.797 12.4736 10.7113 12.4981 10.6588C12.5224 10.6063 12.5594 10.5699 12.6104 10.5497C12.6606 10.5295 12.7416 10.5194 12.8536 10.5194H13.0394C13.1537 10.5194 13.2445 10.542 13.3119 10.5872C13.3794 10.6321 13.413 10.7193 13.413 10.8486L13.4128 13.1916ZM16.1407 13.0744C16.0482 13.217 15.9133 13.3289 15.7356 13.4097C15.5578 13.4905 15.3469 13.5309 15.1034 13.5309C14.8113 13.5309 14.5701 13.4756 14.3803 13.3652C14.2458 13.2857 14.1363 13.1798 14.0524 13.0473C13.9679 12.9144 13.9259 12.7856 13.9259 12.6604C13.9259 12.5876 13.9514 12.5254 14.0014 12.4737C14.0524 12.4216 14.1167 12.3958 14.1945 12.3958C14.258 12.3958 14.3114 12.4159 14.3553 12.4563C14.3991 12.4967 14.4362 12.5565 14.4673 12.6361C14.5049 12.7302 14.5456 12.809 14.5897 12.8724C14.6333 12.9358 14.6948 12.9879 14.7743 13.0287C14.8539 13.0699 14.958 13.0905 15.0873 13.0905C15.265 13.0905 15.4097 13.0489 15.5204 12.9665C15.6321 12.8833 15.6872 12.7803 15.6872 12.6563C15.6872 12.5582 15.6574 12.4782 15.5972 12.4172C15.5374 12.3558 15.4598 12.3089 15.3653 12.2766C15.2701 12.2443 15.1431 12.21 14.9844 12.1736C14.7714 12.124 14.5936 12.0658 14.4498 11.9987C14.3068 11.9324 14.1931 11.8412 14.109 11.7264C14.0245 11.6109 13.9825 11.4679 13.9825 11.297C13.9825 11.1343 14.027 10.9892 14.116 10.8628C14.2048 10.7364 14.3332 10.639 14.5017 10.5712C14.6702 10.5029 14.8681 10.469 15.0953 10.469C15.2772 10.469 15.4343 10.4916 15.5673 10.5364C15.6998 10.5816 15.8096 10.6414 15.8974 10.7166C15.9851 10.791 16.0488 10.8697 16.0891 10.9518C16.1296 11.0337 16.1498 11.1141 16.1498 11.1921C16.1498 11.2636 16.1248 11.3278 16.074 11.3848C16.0238 11.4422 15.9607 11.4708 15.8852 11.4708C15.8166 11.4708 15.7642 11.4534 15.7289 11.4195C15.6929 11.3848 15.6542 11.3286 15.6125 11.2507C15.5588 11.1388 15.494 11.052 15.4186 10.989C15.3431 10.9268 15.2218 10.8952 15.055 10.8952C14.9002 10.8952 14.7754 10.9292 14.68 10.997C14.5856 11.0652 14.538 11.1468 14.538 11.2426C14.538 11.302 14.5542 11.3528 14.5865 11.3961C14.6187 11.4393 14.6632 11.476 14.7198 11.5072C14.7765 11.5383 14.8336 11.5625 14.8916 11.5799C14.9492 11.5973 15.045 11.6231 15.1782 11.6566C15.3452 11.6958 15.4964 11.7386 15.6321 11.7858C15.7668 11.8331 15.8822 11.8901 15.9768 11.9576C16.0722 12.025 16.1461 12.1102 16.199 12.2129C16.2524 12.3163 16.2791 12.4423 16.2791 12.5918C16.2791 12.7707 16.2328 12.9318 16.1407 13.0744Z" fill="white" />
            </g>
            <defs>
                <clipPath id="clip0_718_2710">
                    <rect width="16" height="16" fill="white" transform="translate(4 4)" />
                </clipPath>
            </defs>
        </svg>
    )
}


export default function MarketingPendingCampaign() {
    const dispatch = useDispatch();
    const navigate = useNavigate();
    const [pendingCampaign, setPendingCampaign] = useState<any>([]);
    const [loader, isLoader] = useState<any>(true);
    const handlePendingApprovalCampaigns = (item: any) => {

        const sessionId: any = item?.sessionId;
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/marketing/campaign-conversation?sessionId=${sessionId}&page=pendingApprovalCampaign`, menuName: "New Conversation", close: 1, type: "marketing" }));
        navigate(`/marketing/campaign-conversation?sessionId=${sessionId}&page=pendingApprovalCampaign`);
        // dispatch(commonDataSliceActions.setTabMenuPush({ path: `/marketing/template-conversation?campaignId=${item?.campaignId ?? item}&page=pendingCampaign`, menuName: item?.name, close: 1, type: "marketing" }));
        // navigate(`/marketing/template-conversation?campaignId=${item?.campaignId ?? item}&page=pendingCampaign`);
    }

    const fetchPendingApproval = async () => {
        try {

            await CoreServices.get('/get_campaign_approval_details').then((res: any) => {
                console.log("res????", res?.data?.getPendingApprovalListDataRes);
                // setPastCampaignData(res?.data?.getPastCampaignHandlerDataRes?.[0]);
                // isLoader(false);
                setPendingCampaign(res?.data?.getPendingApprovalListDataRes);
                isLoader(false);
            })
        } catch (error) {
            console.log("err:", error)
            isLoader(false);
        } finally {
            console.log("finally reached:")
            isLoader(false);
        }
    }

    useEffect(() => {

        fetchPendingApproval();
    }, [])
    return (
        <>
            {loader ?
                <div className={`${styles.marketmain} maincontainer`}>
                    <div className={styles.marketmaininner}>
                        <div className={`${styles.topblockarea} ${styles.skeletonStyles} ${pendingCampaign?.[0]?.length == 0 && styles.datafound}`} >
                            {Array.from({ length: 6 }, (_, index) => (
                                <div className={styles.campaignblockmain}>
                                    <div className={styles.campaignblock}>
                                        <div className={styles.campaignblocktopmain} >
                                            <div className={styles.campaignblocktop}>
                                                <div className={styles.campaignblocktopinner}>Created date<span className={styles.skeletonTextsm}></span></div>
                                                <div className={styles.campaignblocktopinner}>Scheduled time<span className={styles.skeletonTextsm}></span></div>
                                                <div className={styles.campaignblocktopinner}>Created by<span className={styles.skeletonTextsm}></span></div>
                                            </div>
                                            <div className={styles.campaignblocktopright}>Pending Approval</div>
                                        </div>
                                        <div className={styles.campaignblockbottom}>
                                            <div className={styles.campaignblockbottomleft}>
                                                <div className={styles.skeletonTitle}></div>
                                                <p>Primary Goal: <span className={styles.skeletonText}></span></p>
                                            </div>
                                            <div className={styles.campaignblockbottomright}>
                                                <div className={styles.skeletonIcon}></div>
                                                <div className={styles.skeletonIcon}></div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            ))}
                        </div>
                    </div>
                </div>
                :
                <div className={`${styles.marketmain} maincontainer`}>
                    <div className={styles.marketmaininner}>

                        <div className={`${styles.topblockarea} ${pendingCampaign?.[0]?.length == 0 && styles.datafound}`} >
                            {pendingCampaign?.[0]?.length == 0 &&
                                <div style={{ height: '100%', display: 'flex', flexDirection: "column", alignItems: 'center', justifyContent: 'center' }}>
                                    <Nodata />
                                </div>
                            }
                            {pendingCampaign?.[0]?.length > 0 && pendingCampaign?.[0]?.map((item: any) => {
                                return (
                                    <>
                                        <div className={styles.campaignblockmain}>
                                            <div className={styles.campaignblock} onClick={() => handlePendingApprovalCampaigns({ name: `${item?.campaign_name}`, campaignId: `${item?.campaignchatid}`, sessionId: `${item?.sessionid}` })}>
                                                <div className={styles.campaignblocktopmain} >
                                                    <div className={styles.campaignblocktop}>
                                                        <div className={styles.campaignblocktopinner}>Created date<span>{item?.createdat ? moment(item?.createdat).format("YYYY-MM-DD") : "-"}</span></div>
                                                        <div className={styles.campaignblocktopinner}>Scheduled time<span>{item?.scheduled_time ? moment(item?.scheduled_time).format("YYYY-MM-DD HH:mm:ss") : "-"}</span></div>
                                                        <div className={styles.campaignblocktopinner}>Created by<span>{item?.approved_by}</span></div>
                                                    </div>
                                                    <div className={styles.campaignblocktopright}>Pending Approval</div>
                                                </div>
                                                <div className={styles.campaignblockbottom}>
                                                    <div className={styles.campaignblockbottomleft}>
                                                        <div className={styles.title}>{item?.campaign_name}<span>Retention</span>
                                                            {/* <span></span> */}
                                                        </div>
                                                        <p>Primary Goal: {item?.goal}</p>
                                                    </div>
                                                    <div className={styles.campaignblockbottomright}>
                                                        {/* <EmailSvg />
                                        <SMSSvg /> */}
                                                    </div>
                                                </div>
                                            </div>
                                        </div>
                                    </>
                                )
                            })
                            }

                        </div>
                    </div>
                </div>
            }
            <div className={`${styles.editorbottom} maincontainer`}>
                <SetUpCommonEditor pageSource={"marketing"} />
            </div>
        </>
    );
}
