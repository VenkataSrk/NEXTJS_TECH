import styles from "./scss/marketing.module.scss";
import SetUpCommonEditor from "./commoneditor";
import { useDispatch } from "react-redux";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { useNavigate } from "react-router-dom";
import Nodata from "../component/nodata";
import { useEffect, useState } from "react";
import axios from "axios";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import Loader from "../component/loader/loader";
import SkeletonBox from "./skeletonbox";
import { config } from "../../base/constant";
import bgImage1 from "../../../src/assets/images/bluelinearbg.jpg";
import bgImage2 from "../../../src/assets/images/brownlbg.png";
import bgImage3 from "../../../src/assets/images/greenlgbg.png";
import bgImage4 from "../../../src/assets/images/lightbluelgbg.png";
import bgImage5 from "../../../src/assets/images/safronlgbg.png";
const EmailSvg = () => {
    return (
        <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none">
            <path d="M11.5409 9.97323C11.8208 10.1478 12.1816 10.1468 12.4595 9.97033L20.8442 4.67293C20.3324 4.26503 19.6926 4.01123 18.9873 4.01123H5.01178C4.28708 4.01123 3.63138 4.27893 3.11328 4.70723L11.5409 9.97323Z" fill="#BFBFBF" />
            <path d="M13.2597 11.2363C12.8769 11.4791 12.4376 11.6 11.9978 11.6C11.5624 11.6 11.127 11.481 10.7462 11.2431L2.22673 5.91962C2.09453 6.25752 2.01562 6.62222 2.01562 7.00722V16.9931C2.01562 18.6477 3.35693 19.9889 5.01143 19.9889H18.9869C20.6414 19.9889 21.9827 18.6478 21.9827 16.9931V7.00722C21.9827 6.60352 21.9004 6.21952 21.7556 5.86792L13.2597 11.2363Z" fill="#BFBFBF" />
        </svg>
    )
}

const SMSSvg = () => {
    return (
        <svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
            <path d="M9.9617 0C4.4596 0 0 4.46 0 9.9616V18.5516C0 19.3041 0.61 19.9141 1.3623 19.9141H9.955C15.455 19.9141 19.9141 15.4553 19.9141 9.9552V9.9525C19.9141 4.4559 15.4581 0 9.9617 0ZM6.7122 11.6463C6.5666 11.8706 6.3545 12.0464 6.0753 12.1735C5.7956 12.3005 5.464 12.364 5.0812 12.364C4.6221 12.364 4.2429 12.277 3.9446 12.1036C3.733 11.9785 3.5608 11.8121 3.4287 11.6038C3.2959 11.3949 3.2299 11.1923 3.2299 10.9954C3.2299 10.8811 3.2701 10.7833 3.3486 10.702C3.4287 10.6201 3.5298 10.5795 3.6521 10.5795C3.7519 10.5795 3.8358 10.6112 3.9049 10.6747C3.9737 10.7382 4.0321 10.8323 4.0809 10.9574C4.14 11.1053 4.204 11.2292 4.2736 11.3288C4.3422 11.4285 4.4386 11.5105 4.5635 11.5746C4.6886 11.6394 4.8527 11.6718 5.0558 11.6718C5.335 11.6718 5.5625 11.6064 5.7367 11.4768C5.912 11.346 5.9987 11.1841 5.9987 10.9891C5.9987 10.8349 5.9519 10.709 5.8572 10.6131C5.763 10.5166 5.6412 10.4429 5.4928 10.3921C5.3429 10.3412 5.1433 10.2873 4.8938 10.2301C4.5591 10.1521 4.2797 10.0606 4.0537 9.9551C3.8285 9.851 3.6497 9.7074 3.5176 9.5271C3.3848 9.3455 3.3188 9.1206 3.3188 8.852C3.3188 8.5961 3.3887 8.3681 3.5286 8.1694C3.6681 7.9706 3.87 7.8175 4.1351 7.7109C4.3997 7.6036 4.7108 7.5502 5.0686 7.5502C5.3542 7.5502 5.6014 7.5858 5.8102 7.6563C6.0185 7.7274 6.1915 7.8214 6.3291 7.9396C6.467 8.0564 6.5673 8.1803 6.6308 8.3092C6.6943 8.438 6.7262 8.5644 6.7262 8.687C6.7262 8.7995 6.6865 8.9003 6.6067 8.99C6.5279 9.0801 6.4289 9.1252 6.3099 9.1252C6.2022 9.1252 6.12 9.0979 6.0642 9.0445C6.0076 8.9899 5.9467 8.9017 5.8812 8.7791C5.7968 8.6031 5.6952 8.4667 5.5764 8.3676C5.4577 8.2698 5.267 8.2203 5.005 8.2203C4.7619 8.2203 4.5651 8.2737 4.4154 8.3803C4.2667 8.4876 4.192 8.6159 4.192 8.7665C4.192 8.8598 4.2173 8.9398 4.268 9.0078C4.3187 9.0757 4.3885 9.1335 4.4776 9.1824C4.5667 9.2313 4.6565 9.2694 4.7476 9.2967C4.8384 9.324 4.9889 9.3646 5.1982 9.4174C5.4607 9.479 5.6985 9.5463 5.9119 9.6206C6.1238 9.6949 6.3055 9.7846 6.4539 9.8906C6.6038 9.9967 6.72 10.1306 6.8034 10.292C6.8871 10.4546 6.9293 10.6527 6.9293 10.8876C6.9293 11.1689 6.8567 11.4221 6.7122 11.6463ZM12.1782 11.8305C12.1782 12.0083 12.138 12.1416 12.0595 12.2306C11.9794 12.3195 11.8735 12.364 11.7402 12.364C11.6151 12.364 11.513 12.3195 11.4321 12.2306C11.3512 12.1416 11.3113 12.0083 11.3113 11.8305V8.617L10.5649 11.5829C10.516 11.7753 10.4769 11.9169 10.4464 12.0065C10.4152 12.0967 10.3589 12.1786 10.2762 12.2529C10.1935 12.3272 10.0794 12.3641 9.9333 12.3641C9.8233 12.3641 9.7299 12.3406 9.6539 12.2923C9.5776 12.2453 9.5185 12.1843 9.4761 12.11C9.4336 12.0358 9.4004 11.9538 9.3758 11.8642C9.3517 11.7741 9.3269 11.6801 9.3011 11.5829L8.5554 8.617V11.8305C8.5554 12.0083 8.5157 12.1416 8.4356 12.2306C8.357 12.3195 8.2516 12.364 8.1201 12.364C7.9929 12.364 7.8895 12.3202 7.8089 12.2325C7.728 12.1442 7.6884 12.0103 7.6884 11.8306V8.1471C7.6884 7.9439 7.7409 7.8067 7.847 7.7362C7.9529 7.6651 8.0957 7.6295 8.2754 7.6295H8.5679C8.7437 7.6295 8.8711 7.6454 8.9499 7.6772C9.0302 7.7089 9.0883 7.7661 9.1265 7.8486C9.1649 7.9312 9.2081 8.0658 9.257 8.2519L9.9333 10.8017L10.6096 8.2519C10.6585 8.0659 10.7015 7.9312 10.7398 7.8486C10.7779 7.7661 10.8363 7.7089 10.9164 7.6772C10.9952 7.6454 11.1226 7.6295 11.2987 7.6295H11.5909C11.7705 7.6295 11.9133 7.6651 12.0193 7.7362C12.1255 7.8067 12.1782 7.9439 12.1782 8.1471V11.8305ZM16.4669 11.6463C16.3216 11.8706 16.1094 12.0464 15.83 12.1735C15.5506 12.3005 15.2189 12.364 14.8361 12.364C14.3768 12.364 13.9976 12.277 13.6992 12.1036C13.4878 11.9785 13.3156 11.8121 13.1836 11.6038C13.0508 11.3949 12.9848 11.1923 12.9848 10.9954C12.9848 10.8811 13.025 10.7833 13.1035 10.702C13.1836 10.6201 13.2847 10.5795 13.407 10.5795C13.5068 10.5795 13.5907 10.6112 13.6598 10.6747C13.7286 10.7382 13.787 10.8323 13.8358 10.9574C13.8949 11.1053 13.9589 11.2292 14.0282 11.3288C14.0968 11.4285 14.1935 11.5105 14.3184 11.5746C14.4435 11.6394 14.6073 11.6718 14.8105 11.6718C15.0899 11.6718 15.3174 11.6064 15.4914 11.4768C15.6669 11.346 15.7537 11.1841 15.7537 10.9891C15.7537 10.8349 15.7069 10.709 15.6122 10.6131C15.518 10.5166 15.3962 10.4429 15.2476 10.3921C15.0979 10.3412 14.8983 10.2873 14.6488 10.2301C14.3138 10.1521 14.0344 10.0606 13.8082 9.9551C13.5835 9.851 13.4047 9.7074 13.2724 9.5271C13.1396 9.3455 13.0736 9.1206 13.0736 8.852C13.0736 8.5961 13.1435 8.3681 13.2834 8.1694C13.4231 7.9706 13.625 7.8175 13.8899 7.7109C14.1547 7.6036 14.4659 7.5502 14.8231 7.5502C15.1092 7.5502 15.3561 7.5858 15.5652 7.6563C15.7735 7.7274 15.9462 7.8214 16.0841 7.9396C16.222 8.0564 16.3221 8.1803 16.3855 8.3092C16.4492 8.438 16.4809 8.5644 16.4809 8.687C16.4809 8.7995 16.4415 8.9003 16.3617 8.99C16.2829 9.0801 16.1836 9.1252 16.0649 9.1252C15.9572 9.1252 15.8748 9.0979 15.8192 9.0445C15.7626 8.9899 15.7017 8.9017 15.6362 8.7791C15.5518 8.6031 15.4499 8.4667 15.3314 8.3676C15.2127 8.2698 15.022 8.2203 14.7597 8.2203C14.5164 8.2203 14.3201 8.2737 14.1701 8.3803C14.0217 8.4876 13.9467 8.6159 13.9467 8.7665C13.9467 8.8598 13.9723 8.9398 14.023 9.0078C14.0737 9.0757 14.1435 9.1335 14.2326 9.1824C14.3217 9.2313 14.4115 9.2694 14.5026 9.2967C14.5932 9.324 14.7439 9.3646 14.9532 9.4174C15.2157 9.479 15.4535 9.5463 15.6669 9.6206C15.8788 9.6949 16.0602 9.7846 16.2089 9.8906C16.3588 9.9967 16.475 10.1306 16.5582 10.292C16.6421 10.4546 16.6841 10.6527 16.6841 10.8876C16.6841 11.1689 16.6117 11.4221 16.4669 11.6463Z" fill="#BFBFBF" />
        </svg>
    )
}


export default function MarketingCampaign() {
    const dispatch = useDispatch();
    const navigate = useNavigate();
    const [activeCampaignData, setActiveCampaignData] = useState<any>([]);
    console.log("activeCampDAta", activeCampaignData)
    const [loader, isLoader] = useState<any>(true);

    const handleActiveCampaigns = (item: any) => {
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/marketing/template-conversation?campaignId=${item?.campaignId ?? item}&page=activeCampaign`, menuName: item?.name, close: 1, type: "marketing" }));
        navigate(`/marketing/template-conversation?campaignId=${item?.campaignId ?? item}&page=activeCampaign`);
    }
    const bgImages: any = [bgImage1, bgImage2, bgImage3, bgImage4, bgImage5];
    const activeCampaign = async () => {
        try {
            const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
            const domainId: any = LoginUserDetails?.domainId;
            const endPoint = `${config().NLP_DNS_URL}/active_campaigns`;
            const payload = {
                domain_id: domainId,
            };
            const response = await axios.post(endPoint, payload);
            console.log("res>>>>", response)
            setActiveCampaignData(response?.data);


        } catch (error) {
            console.log("activeCampaign error", error);
            return null;
        }
        finally {
            isLoader(false)

        }
    }

    useEffect(() => {
        activeCampaign();
    }, [])
    return (<>
        <div>


            <div className={`${styles.marketmain} maincontainer`}>
                <div className={styles.marketmaininner}>
                    <div className={`${styles.topblockarea} ${(activeCampaignData?.length == 0) && styles.datafound}`}>
                        {activeCampaignData?.length == 0 && !loader &&
                            <div style={{ height: '100%', display: 'flex', flexDirection: "column", alignItems: 'center', justifyContent: 'center' }}>
                                <Nodata />
                            </div>
                        }
                        {loader ? <>
                            <div className={"skeletonTextGroup"}>
                                <SkeletonBox width="20%" height="24px" />
                            </div>
                            <div style={{ display: "flex", gap: 12 }}>
                                <SkeletonBox width='35%' height="240px" />
                                <SkeletonBox width='35%' height="240px" />
                                <SkeletonBox width='35%' height="240px" />
                            </div>
                            <div style={{ display: "flex", gap: 12 }}>
                                <SkeletonBox width='35%' height="240px" />
                                <SkeletonBox width='35%' height="240px" />
                                <SkeletonBox width='35%' height="240px" />
                            </div>
                            {/* <div style={{ display: "flex", gap: 12,margin:"1%",marginLeft:"10%",marginRight:"10%" }}>
                                    <SkeletonBox width='35%' height="240px" />
                                    <SkeletonBox width='35%' height="240px" />
                                    <SkeletonBox width='35%' height="240px" />
                                </div> */}
                        </> : <>{activeCampaignData?.length > 0 &&
                            <>
                                <div className={styles.datacamapign}>
                                    <div className={styles.titlearea}>
                                        <div className={styles.title}>Active Campaigns</div>
                                    </div>
                                    <div className={styles.activemain} >
                                        {activeCampaignData?.length > 0 && activeCampaignData.map((item: any, index: any) => {
                                            return (
                                                <div className={styles.activemaininner} onClick={() => handleActiveCampaigns({ name: `${item?.campaignName}`, campaignId: `${item?.campaignChatId}` })}>

                                                    <div style={{ backgroundImage: `url(${bgImages[index % 5]})` }} className={styles.activetop}>
                                                        <div className={styles.title}>{item?.openrate}</div>
                                                        <p>Engaged Type</p>
                                                        <p>Campaign Type<span>Retention</span></p>
                                                    </div>
                                                    <div className={styles.activebottom}>
                                                        <div className={styles.subtitle}>{item?.campaignName}<span>Running</span></div>
                                                        <p>{item?.CampaginDate}</p>
                                                        <p>{item?.summary}</p>
                                                        {/* <div className={styles.line}>
                                                <div className={styles.innerline} style={{ width: "70%" }}></div>
                                            </div> */}
                                                        <div className={styles.activeicons}>
                                                            {item?.channel?.includes("email") && <EmailSvg />}
                                                            {item?.channel?.includes("sms") && <SMSSvg />}
                                                        </div>
                                                    </div>
                                                </div>
                                                // <div className={styles.activemaininner} onClick={() => handleActiveCampaigns({ name: "OTTSubscriptionCampaign2", campaignId: "OTTSubscriptionCampaign2id" })}>

                                                //     <div className={styles.activetop}>
                                                //         <div className={styles.title}>33856</div>
                                                //         <p>Engaged Type</p>
                                                //         <p>Campaign Type<span>Retention</span></p>
                                                //     </div>
                                                //     <div className={styles.activebottom}>
                                                //         <div className={styles.subtitle}>OTT Subscription Campaign<span>Running</span></div>
                                                //         <p>OTT-2025-001 | Jan 10, 2025 → Mar 10, 2025</p>
                                                //         <p>78% of retention goal achieved.</p>
                                                //         <div className={styles.line}>
                                                //             <div className={styles.innerline} style={{ width: "70%" }}></div>
                                                //         </div>
                                                //         <div className={styles.activeicons}>
                                                //             <EmailSvg />
                                                //             <SMSSvg />
                                                //         </div>
                                                //     </div>
                                                // </div>
                                            )
                                        })
                                        }
                                    </div>
                                </div>
                            </>}</>
                        }


                    </div>
                </div>
            </div>
        </div>
        <div className={`${styles.editorbottom} maincontainer`}>
            <SetUpCommonEditor pageSource={"marketing"} />
        </div>

    </>
    );
}
