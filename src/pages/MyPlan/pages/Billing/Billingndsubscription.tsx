import React, { useEffect, useMemo, useState } from "react";
import styles from "../../assets/styles/billingaddons.module.scss";
import moment from "moment";
import { useNavigate, useSearchParams } from "react-router-dom";
import { getCurrencyByDomain, stripHtmlTags } from "../../../../base/utils";
import { useSelector } from "react-redux";
import { getBillingAndSubcriptionApi, getBotSessionsDomain_ccaas, getStorageDatas_ccaas } from "../../apiservice/myplanapi";
import Skeleton from "../../skeleton";
import aiChatAssistSocketservice from "../../../../services/aiChatService";
import { MyplanProfileActions } from "../../../../store/slice/my_plans/profile";
import { GetItemFromStorage } from "../../../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../../../base/custom_hooks/localStorageKeys";
import { useDispatcher } from "../../../../store/redux-store/dispatch";
import { commonDataSliceActions } from "../../../../store/slice/commonSilce/commonSlice";
// import Successicon from '../../../../../../assets/images/agentworkspace/successtick.svg'
import { v4 as uuidv4 } from "uuid";

function Billingndsubscription() {

  const [sessionExpired, setSessionExpired] = useState(false)
  const [sessionExpiredData, setSessionExpiredData] = useState<any>({})
  const currency = getCurrencyByDomain()
  const { loginUserDetails } = useSelector((state: any) => state.loginUser);
  const [loading, setLoading] = useState(true)
  const [storage_ccaas, setStorage_ccaas] = useState<any>({});
  const navigate = useNavigate();

  const [searchParams, setSearchParams] = useSearchParams()
  const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
  const chatsessionId = searchParams.get("sessionId");
  const dispatch = useDispatcher();

  const [currentProductName, setCurrentProductName] = useState('');
  const [ProductList, setProductList] = useState([]);
  const bilingProducts: any = useMemo(() => {
    return ProductList.filter((item: any) => item?.product_id === currentProductName)[0]
  }, [ProductList, currentProductName]);

  useEffect(() => {
    getBillingAndSubcription();
    getBotSessionsDomainApi()
    getStorageDatas_ccaasHandler()
  }, [])

  const getStorageDatas_ccaasHandler = async () => {
    try {
      const res = await getStorageDatas_ccaas();
      if (res) {
        setStorage_ccaas((prev: any) => ({
          ...prev,
          '1': res[0] ?? {}
        }));
      }
    } catch (error) {
      console.log("getStorageDatas_ccaas error:", error);
    }
  };

  const getBotSessionsDomainApi = async () => {
    try {

      getBotSessionsDomain_ccaas()
        .then((response: any) => {
          const [sessionData] = response;
          setSessionExpiredData(sessionData[0])
          const isSessionExpired = sessionData[0]?.remainingSessions == 0 && sessionData[0]?.isUnilimitedChat == 0;
          setSessionExpired(isSessionExpired)
        })

    } catch (error) {
      console.log("getBotSessionsDomainApi error:", error);
      setSessionExpiredData({});
      setSessionExpired(false);
    }
  }

  const [planNames, setPlanNames] = useState([{
    lable: 'AI Contact center',
    value: '1'
  },
  {
    lable: 'Campaign management',
    value: 'Campaign management'
  },
  {
    lable: 'CRM',
    value: '4'
  }]);

  console.log("[lllll]planNamesplanNames", planNames)
  console.log("[lllll]currentProductName", currentProductName)
  console.log("[lllll]ProductList", ProductList)


  const getBillingAndSubcription = async () => {
    try {
      const response: any = await getBillingAndSubcriptionApi();
      if (response?.statusCode === 200) {
        const products = Array.isArray(response?.result) ? (response?.result).flat() : [];

        const defaultData = {
          '1': 'AI Contact center',
          'Campaign management': "Campaign management",
          '4': 'CRM'
        }

        const productList = products.filter((item: any) => item?.product_id);
        const proname = productList.map((item: any) => ({
          label: defaultData[item?.product_id] || item?.prd_name,
          value: item?.product_id
        }))
        setPlanNames(proname);
        setCurrentProductName(proname[0].value || '');
        setProductList(productList)

      } else {
      }
    } catch (error) {
      console.log("getBillingAndSubcription error:", error);
    } finally {
      setLoading(false);
    }
  }


  const openChat = (value: any, files: any) => {
    const sessionId: any = uuidv4() + "setup";
    const body: any = {
      query: stripHtmlTags(value?.trim()),
      sessionId,
      roleId: `${LoginCcaasUserDetails?.roleId || ""}`,
      org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
      userId: `${LoginCcaasUserDetails?.userId || ""}`,
      domainId: `${LoginCcaasUserDetails?.domainId || ""}`,
      module: "",
      ext: `${LoginCcaasUserDetails?.ext || ""}`,
      agent: "myplans",
      isBot: 0,
      customerId: "",
      files: files ?? [],
    };

    aiChatAssistSocketservice.io.emit("crm_chat", body);
    dispatch(MyplanProfileActions.setSetupAiAgentChat({ ...body, isBot: 0, query: body?.query }))
    dispatch(MyplanProfileActions.setSetupAiAgentChatTyping({ sessionId: chatsessionId, typing: true }));
    dispatch(commonDataSliceActions.setTabMenuPush({
      path: `/myplans/chat-conversation?sessionId=${sessionId}`,
      menuName: "New Conversation",
      close: 1,
      type: "myplans",
    })
    );
    navigate(`/myplans/chat-conversation?sessionId=${sessionId}`);
  }

  const handleTab = (value: any) => {
    setCurrentProductName(value)
    const x = document.getElementById("header_crm");
    if (x) {
      x.scrollIntoView({ behavior: "smooth", block: "end" });
    }
  }

  return (
    <>
      <div className={styles.billingndsubscriptionbody}>
        {loading ? (
          <Skeleton count={5} />
        ) : (
          <>
            <div className={styles.companydetails} id="header_crm">
              <div className={styles.compname}>
                {loginUserDetails?.companyName || ""}
              </div>
            </div>

            {sessionExpired && (
              <div className={styles.redtoast}>
                <div className={styles.inner}>
                  Bot session limit will reach soon. Add sessions now to avoid interruption.
                  <span onClick={() => setSessionExpired(false)}>
                    ✕
                  </span>
                </div>
              </div>
            )}

            <div className={styles.blackbanner}>
              <div className={styles.section}>
                <div className={styles.top}>
                  <div className={styles.lefts}>
                    {bilingProducts?.isPaid === 0 ? (
                      <div className={styles.label}>Amount due</div>
                    ) : (
                      <div className={styles.paid}>
                        {/* <img src={Successicon} alt="" /> Paid */}
                      </div>
                    )}

                    <div className={styles.amount}>
                      {currency}{bilingProducts?.Amount_Due ? bilingProducts?.Amount_Due : 0}
                    </div>

                    {!!(bilingProducts?.originalPrice && bilingProducts?.plan_discount) && (
                      <div className={styles.orginalamount}>
                        <div className={styles.cross}>
                          {currency}{bilingProducts?.originalPrice}
                        </div>
                        <div>
                          Discount {Math.round(bilingProducts?.plan_discount) || 0}% •{" "}
                          {(bilingProducts?.originalPrice || 0) - (bilingProducts?.Amount_Due || 0)} off
                        </div>
                      </div>
                    )}
                  </div>

                  <div className={styles.rights}>
                    {bilingProducts?.isPaid === 0 && (
                      <button onClick={() => openChat("My Plan Pay now", [])}>
                        Pay now
                      </button>
                    )}
                    <span onClick={() => openChat("My Plan View Invoice", [])}>
                      View Invoice
                    </span>
                  </div>
                </div>

                <div className={styles.datetxt}>
                  {bilingProducts?.DueDate ? "Due " + moment(bilingProducts?.DueDate).format("MMM DD YYYY") : ""}
                  {bilingProducts?.days_remaining ? ` • ${Math.abs(Number(bilingProducts?.days_remaining))} days remaining` : ""}
                </div>

                <div className={styles.datetxt}>
                  {bilingProducts?.plan_start_date
                    ? `Usage period ${moment(bilingProducts?.plan_start_date).format("MMM D")} - ${moment(bilingProducts?.plan_end_date).format("MMM D YYYY")}`
                    : ""}
                </div>
              </div>
            </div>

            <div className={styles.subscriptions}>
              <div className={styles.header}>
                <h2 className={styles.title}>Subscriptions</h2>
                <p className={styles.subtitle}>About Usage & Limits</p>
              </div>

              <div className={styles.tabs}>
                {planNames.map((item: any, index: number) => (
                  <div className={`${styles.tab} ${currentProductName === item.value ? styles.active : ""}`} onClick={() => handleTab(item.value)} key={index}> {item.label}</div>
                ))}
              </div>

              {/* PLAN CARD */}
              <div className={styles.planCard}>
                <div className={styles.planHeader}>
                  <div className={styles.planPricecard}>
                    <h3>{bilingProducts?.prd_name}</h3>
                    <p className={styles.planType}>
                      {loginUserDetails?.parent_id == "33" ? "Pro plan" : "Enterprise plan"}
                    </p>
                  </div>

                  <div className={styles.planPrice}>
                    <h3>
                      {currency}
                      {bilingProducts?.purchase_amount || bilingProducts?.Amount_Due || 0} / month
                    </h3>

                    {!!(bilingProducts?.originalPrice && bilingProducts?.plan_discount) ? (
                      <div className={styles.orginalamount}>
                        <div className={styles.cross}>
                          {currency}{bilingProducts?.originalPrice}
                        </div>
                        <div>
                          {currency}{(bilingProducts?.originalPrice || 0) - (bilingProducts?.Amount_Due || 0)} • Discount {Math.round(bilingProducts?.plan_discount) || 0}%
                        </div>
                      </div>
                    ) : (
                      <p>{bilingProducts?.days_remaining} days left in this billing cycle/Overdue notification</p>
                    )}
                  </div>
                </div>

                <div className={styles.planRow}>
                  <div>
                    <p className={styles.label}>Contract term</p>
                    <p className={styles.value}>
                      Usage cycle on {bilingProducts?.contract_period ? bilingProducts?.contract_period : ""}
                    </p>
                  </div>
                  <div className={styles.rowRight}>
                    <span>{bilingProducts?.billing_frequency}</span>
                    <button className={styles.editBtn} onClick={() => openChat("My Plan Edit Contract term", [])}>
                      Edit
                    </button>
                  </div>
                </div>

                <div className={styles.planRow}>
                  <div>
                    <p className={styles.label}>Billing frequency</p>
                    <p className={styles.value}>
                      Next invoice on{" "}
                      {bilingProducts?.DueDate ? moment(bilingProducts?.DueDate).format("MMM DD YYYY") : ""}
                    </p>
                  </div>
                  <div className={styles.rowRight}>
                    <span>{bilingProducts?.billing_frequency ?? 0}</span>
                    <button className={styles.editBtn} onClick={() => openChat("My Plan Edit Billing frequency", [])}>
                      Edit
                    </button>
                  </div>
                </div>
              </div>

              {/* USAGE CARD */}
              <div className={styles.planCard}>
                <div className={styles.planHeader}>
                  <h3>Usage & limits</h3>
                </div>

                {bilingProducts?.product_id == "1" && (
                  <>
                    <div className={styles.planRow}>
                      <p className={styles.label}>Sessions</p>
                      <div className={styles.rowRight}>
                        {sessionExpiredData?.isUnilimitedChat === 1 ? (
                          <span>Unlimited</span>
                        ) : (
                          <span>
                            Available sessions {sessionExpiredData?.remainingSessions || 0} out of {sessionExpiredData?.TotalSession || 0}
                          </span>
                        )}
                        <button className={styles.editBtn} onClick={() => openChat("My Plan Add Sessions", [])}>Add</button>
                      </div>
                    </div>

                    <div className={styles.planRow}>
                      <p className={styles.label}>Concurrent voices</p>
                      <div className={styles.rowRight}>
                        <span>{bilingProducts?.concurrent_voices || 0}</span>
                        <button className={styles.editBtn} onClick={() => openChat("My Plan Add Concurrent voices", [])}>Add</button>
                      </div>
                    </div>

                    <div className={styles.planRow}>
                      <p className={styles.label}>Languages</p>
                      <div className={styles.rowRight}>
                        <span>{bilingProducts?.usedlanguages || 0} / {bilingProducts?.totlanguage || 0} languages</span>
                        <button className={styles.editBtn} onClick={() => openChat("My Plan Add Languages", [])}>Add</button>
                      </div>
                    </div>
                  </>
                )}

                <div className={styles.planRow}>
                  <p className={styles.label}>Channels</p>
                  <div className={styles.rowRight}>
                    <span>{bilingProducts?.usedChannel || 0} / {bilingProducts?.totChannel || 0} channels</span>
                    <button className={styles.editBtn} onClick={() => openChat("My Plan Add Channels", [])}>Add</button>
                  </div>
                </div>

                <div className={styles.planRow}>
                  <p className={styles.label}>Users</p>
                  <div className={styles.rowRight}>
                    <span>Available user {bilingProducts?.availableUser || 0} out of {bilingProducts?.TotalUser || 0}</span>
                    <button className={styles.editBtn} onClick={() => openChat("My Plan Add Users", [])}>Add</button>
                  </div>
                </div>

                <div className={styles.planRow}>
                  <p className={styles.label}>Storage</p>
                  <div className={styles.rowRight}>

                    <span>
                      Available Storage {storage_ccaas?.[bilingProducts?.product_id]?.total_size_gb_used ?? 0} GB out of {storage_ccaas?.[bilingProducts?.product_id]?.total_size_gb ?? 0} GB
                    </span>

                    <button className={styles.editBtn} onClick={() => openChat("My Plan Add Storage", [])}>Add</button>
                  </div>
                </div>
              </div>

            </div>
          </>
        )}
      </div>
    </>
  );
}

export default Billingndsubscription;
