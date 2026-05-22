import { useEffect, useRef } from "react";
import { useSelector } from "react-redux";
import axios from "axios";
import { GetItemFromStorage } from "../base/custom_hooks/useStorage";
import { LocalStorage } from "../base/custom_hooks/localStorageKeys";
import { useDispatcher } from "../store/redux-store/dispatch";
import { 
    getContactDetailsDomainList, getCustomerDealsApi, getProductDetailsApi, 
    getProductInsightPerformanceRankingApi, getProductInsightProductRevenueApi, 
    getProductInsightRevenueGrowthApi, getProjectInsightApi 
} from "../base/service/core_serviceApi";
import { dealsDataSliceActions } from "../store/slice/dealsSlice";
import { contactDataSliceActions } from "../store/slice/contactSlice";
import { projectDataSliceActions } from "../store/slice/projectSlice";
import { productDataSliceActions } from "../store/slice/productSlice";
import { MarketingAgentChatAction } from "../store/slice/marketingSlice/marketingSlice";
import { homeDataSliceActions } from "../store/slice/homeSlice";
import { config } from "../base/constant";
import { getMarketingHomePage } from "../base/service/nlp_service";

export const useGetCustomerDeals = () => {
    const dispatch = useDispatcher();
    const userDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const { dealsListFilters } = useSelector((store: any) => store.dealsStore);
    const {
        fromDate = null,
        toDate = null,
        contact = null,
        dealName = null,
        companyName = null,
        pipelineStage = null,
        dealEstimatedClosed = null,
        location = null,
        totalAmount = null,
        closedAmount = null,
        ownerName = null,
    } = dealsListFilters ?? {};

    const getCustomerDeals = async () => {
        try {
            dispatch(dealsDataSliceActions.SetCustomerDealData({ data: [], loader: true }));
            const body = {
                customerId: null,
                agentId: userDetails?.org_role_id_crm == 3 ? null : userDetails?.userId,
                contact: contact ?? null,
                dealName: dealName ?? null,
                companyName: companyName ?? null,
                pipelineStage: pipelineStage ?? null,
                fromDate: fromDate ?? null,
                toDate: toDate ?? null,
                dealEstimatedClosed: dealEstimatedClosed ?? null,
                location: location ?? null,
                totalAmount: totalAmount ?? null,
                closedAmount: closedAmount ?? null,
                ownerName: ownerName ?? null,
            };
            const customerDeals = await getCustomerDealsApi(body);
            if (customerDeals?.statusCode === 200) {
                dispatch(dealsDataSliceActions.SetCustomerDealData({
                    data: customerDeals?.customerDealsData,
                    loader: false
                }));
            } else {
                dispatch(dealsDataSliceActions.SetCustomerDealData({ data: [], loader: false }));
            }
        } catch (error) {
            console.log('getCustomerDeals Error', error);
            dispatch(dealsDataSliceActions.SetCustomerDealData({ data: [], loader: false }));
        }
    };

    useEffect(() => {
        getCustomerDeals();
    }, [fromDate, toDate, contact, dealName, companyName, pipelineStage, dealEstimatedClosed, location, totalAmount, closedAmount, ownerName]);
};


export const useGetMainContactList = () => {
    const dispatch = useDispatcher();
    const { contactList, contactListLoading, contactListFilters, contactListRefresh } = useSelector((store: any) => store.contactStore)
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

    const buildFilterBody = (offset: number, limit: number) => ({
        offset,
        limit,
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
    });

    useEffect(() => {
        getContactDeatils(0);
    }, [
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
    ])

    const getContactDeatils = async (offset = 0, limit = 50) => {
        dispatch(contactDataSliceActions.setContactList({ contactListLoading: true }));
        const contactRes = await getContactDetailsDomainList(buildFilterBody(offset, limit))
        const [contactPagination, totalCount] = contactRes;
        if (!contactPagination?.length && offset > 0) {
            dispatch(contactDataSliceActions.setContactList({ contactListLoading: false }));
            return;
        }
        dispatch(contactDataSliceActions.setContactList({
            contactListLoading: false,
            contactList: offset === 0 ? contactPagination : [...(contactList || []), ...contactPagination]
        }))
    }

    // Silent refresh: fetch first page and merge into the current list by customer_id.
    // Preserves scroll position and any pages the user already paginated into —
    // new records are prepended, existing rows are patched in place.
    const refreshContactsMerge = async () => {
        try {
            const contactRes = await getContactDetailsDomainList(buildFilterBody(0, 50));
            const [freshFirstPage] = contactRes;
            if (!freshFirstPage?.length) return;

            const existing = contactList || [];
            const freshById = new Map(freshFirstPage.map((c: any) => [c?.customer_id, c]));
            const existingIds = new Set(existing.map((c: any) => c?.customer_id));

            const patched = existing.map((c: any) => freshById.get(c?.customer_id) ?? c);
            const newOnes = freshFirstPage.filter((c: any) => !existingIds.has(c?.customer_id));

            dispatch(contactDataSliceActions.setContactList({
                contactListLoading: false,
                contactList: [...newOnes, ...patched],
            }));
        } catch (error) {
            console.log('refreshContactsMerge Error', error);
        }
    };

    const isFirstRefreshTick = useRef(true);
    useEffect(() => {
        if (isFirstRefreshTick.current) {
            isFirstRefreshTick.current = false;
            return;
        }
        refreshContactsMerge();
        // Only react to the refresh toggle; filters have their own effect above.
        // eslint-disable-next-line react-hooks/exhaustive-deps
    }, [contactListRefresh]);
};

export const useFetchProjectInsight = () => {

    const LoginCrmUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const dispatch = useDispatcher();

    const fetchProjectInsight = async () => {
        try {
            dispatch(projectDataSliceActions.setProjectInsight({ loader: true }));

            const body = { userId: LoginCrmUserDetails?.userId }
            const res = await getProjectInsightApi(body);
            const data = res?.response?.[0];

            if (!data) {
                dispatch(projectDataSliceActions.setProjectInsight({ data: [], loader: false }))
                return;
            };

            const transformed = {
                predictiveSummary: {
                    overAllOutlook: data?.predictive_project_summary?.overall_completion_outlook?.description || '',
                    onTimeProbability: data?.predictive_project_summary?.overall_completion_outlook?.on_time_probability_assessment?.description || '',
                    majorRisk: data?.predictive_project_summary?.overall_completion_outlook?.major_predicted_risks || [],
                    aiRecommendation: data?.predictive_project_summary?.overall_completion_outlook?.ai_recommendations || '',
                    projectAssessment: [
                        {
                            title: 'Projects Status',
                            value: data?.predictive_project_summary?.overall_completion_outlook?.summary_metrics?.project_status
                        },
                        {
                            title: 'Risk Level',
                            value: data?.predictive_project_summary?.overall_completion_outlook?.summary_metrics?.risk_level
                        },
                        {
                            title: 'Attention Required',
                            value: data?.predictive_project_summary?.overall_completion_outlook?.summary_metrics?.attention_required
                        }
                    ]
                },

                recentActivity: data?.recent_activity?.items?.map((item: any) => ({
                    title: item?.title,
                    subtitle: item?.description,
                    time: item?.time_label
                })),

                requirementList: data?.requirements_list?.items?.map((item: any) => ({
                    customerLabel: 'Customer name',
                    customerValue: item?.company_name,
                    priority: item?.priority,
                    requirementLabel: item?.requirement_name,
                    requirementDescription: item?.summary,
                    status: item?.status,
                    teamTitle: item?.owner_process,
                    teamSubtitle: item?.owner_note
                })),

                clientDeliverables: data?.client_deliverables?.items?.map((item: any) => ({
                    company: item?.company_name,
                    requirement: item?.requirement_name,
                    status: item?.status,
                    due: `Due: ${item?.due_date}`,
                    remaining: item?.days_left_label
                })),

                delayRiskWarn: data?.upcoming_delays_risk_warnings?.items?.map((item: any) => ({
                    requirement: item?.requirement_name,
                    priority: item?.priority,
                    predictedDelay: `+${item?.predicted_delay_days} days`,
                    riskType: item?.risk_type,
                    description: item?.summary
                })),

                workloadCapacity: {
                    teamCapacity: data?.workload_capacity_predictions?.team_capacity_overview?.map((item: any) => ({
                        requirement: item?.team_name,
                        priority: item?.status,
                        description: item?.summary
                    })),
                    predictions: data?.workload_capacity_predictions?.predictions || [],
                    aiRecommendation: data?.workload_capacity_predictions?.ai_estimated_workload_impact || ''
                },

                resourceAllocation: {
                    shortageForecast: data?.resource_allocation_insights?.resource_shortage_forecast?.map((item: any) => ({
                        requirement: item?.team_name,
                        priority: item?.priority,
                        description: item?.summary,
                        cardtitle: item?.expected_impact
                    })),
                    recommendedHiring: data?.resource_allocation_insights?.hiring_reassignment_recommendations?.map((item: any) =>
                        `<p>${item.recommendation} (Priority: ${item.priority}, Timeline: ${item.timeline})</p>`
                    ),
                    aiRecommendation: ''
                },

                reworkPrediction: {
                    highRiskRework: data?.requirement_stability_rework_prediction?.high_risk_requirements?.map((item: any) => ({
                        requirement: item?.requirement_name,
                        priority: `${item?.rework_probability_percentage}% Risk`,
                        description: item?.summary,
                        cardtitle: `Impact: ${item?.impact}`
                    })),
                    reworkImpactAnalysis: data?.requirement_stability_rework_prediction?.rework_impact_analysis || '',
                    aiRecommendation: data?.requirement_stability_rework_prediction?.recommendations || ''
                },

                operationalEfficiency: {
                    predictedTeamOperation: data?.operational_efficiency_signals?.predicted_time_drift_across_teams?.map((item: any) => ({
                        requirement: item?.signal_name,
                        priority: item?.time_drift_percentage,
                        description: item?.summary
                    })),
                    aiRecommendation: data?.operational_efficiency_signals?.ai_commentary || ''
                }
            };

            dispatch(projectDataSliceActions.setProjectInsight({ data: [transformed], loader: false }))
        } catch (error) {
            console.log("err", error);
            dispatch(projectDataSliceActions.setProjectInsight({ data: [], loader: false }))
        }
    };

    useEffect(() => {
        fetchProjectInsight();
    }, []);

}


export const useFetchProductInsights = () => {
    const dispatch = useDispatcher();

    const extractResponse = (res: any) => (Array.isArray(res?.response) ? res.response[0] : null);

    const fetchPerformanceRanking = async () => {
        dispatch(productDataSliceActions.setProductInsights({ loader: { performance: true } }));
        try {
            const res = await getProductInsightPerformanceRankingApi();
            const data = extractResponse(res);
            dispatch(productDataSliceActions.setProductInsights({
                data: { performanceRanking: data || {} },
                loader: { performance: false },
            }));
        } catch (error) {
            console.log("Performance Ranking Error", error);
            dispatch(productDataSliceActions.setProductInsights({ loader: { performance: false } }));
        }
    };

    const fetchProductRevenue = async () => {
        dispatch(productDataSliceActions.setProductInsights({ loader: { productRevenue: true } }));
        try {
            const res = await getProductInsightProductRevenueApi();
            const data = extractResponse(res);
            const chart = data?.total_revenue_by_product_chart;
            const overallPercentage = data?.total_revenue_by_product?.overall_growth_percent;

            const payload = chart
                ? {
                    percentage: overallPercentage,
                    chartname: chart.chartname,
                    data: chart.data,
                    title: chart.title || "Total Revenue by Product",
                }
                : null;

            dispatch(productDataSliceActions.setProductInsights({
                data: { productRevenue: payload },
                loader: { productRevenue: false },
            }));
        } catch (error) {
            console.log("Product Revenue Error", error);
            dispatch(productDataSliceActions.setProductInsights({ loader: { productRevenue: false } }));
        }
    };

    const fetchRevenueGrowth = async () => {
        dispatch(productDataSliceActions.setProductInsights({ loader: { revenueGrowth: true } }));
        try {
            const res = await getProductInsightRevenueGrowthApi();
            const data = extractResponse(res);
            const chart = data?.product_revenue_growth_chart;

            const payload = chart
                ? {
                    chartname: chart.chartname,
                    data: chart.data,
                    title: chart.title || "Product Revenue Growth",
                }
                : null;

            dispatch(productDataSliceActions.setProductInsights({
                data: { revenueGrowth: payload },
                loader: { revenueGrowth: false },
            }));
        } catch (error) {
            console.log("Revenue Growth Error", error);
            dispatch(productDataSliceActions.setProductInsights({ loader: { revenueGrowth: false } }));
        }
    };

    useEffect(() => {
        Promise.all([fetchPerformanceRanking(), fetchProductRevenue(), fetchRevenueGrowth()]);
    }, []);
};


export const useGetProductList = () => {
    const dispatch = useDispatcher();

    const fetchProductList = async () => {
        dispatch(productDataSliceActions.setProductCatalog({ loader: true }));
        try {
            const res = await getProductDetailsApi({ producId: null });
            const list = res?.response;
            dispatch(productDataSliceActions.setProductCatalog({
                data: Array.isArray(list) ? list : [],
                loader: false,
            }));
        } catch (error) {
            console.log('getProductDetails Error', error);
            dispatch(productDataSliceActions.setProductCatalog({ data: [], loader: false }));
        }
    };

    useEffect(() => {
        fetchProductList();
    }, []);
};


export const useFetchMarketingInsight = () => {
    const dispatch = useDispatcher();

    const fetchMarketingInsight = async () => {
        dispatch(MarketingAgentChatAction.setMarketingInsight({ loader: true }));
        try {
            const LoginUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
            const url = `${config().NLP_DNS_URL}/Executive_summary`;
            const response = await axios.post(url, { domain_id: LoginUserDetails?.domainId });
            dispatch(MarketingAgentChatAction.setMarketingInsight({
                data: response?.data ?? null,
                loader: false,
            }));
        } catch (error) {
            console.log("fetchMarketingInsight Error", error);
            dispatch(MarketingAgentChatAction.setMarketingInsight({ loader: false }));
        }
    };

    useEffect(() => {
        fetchMarketingInsight();
    }, []);
};


export const useGetHomeSales = () => {
    const dispatch = useDispatcher();

    const fetchHomeSales = async () => {
        dispatch(homeDataSliceActions.setHomeSales({ loader: true }));
        try {
            const LoginUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
            const url = `${config().NLP_DNS_URL}/sales_home_page_crm`;
            const response = await axios.post(url, { Domain_id: LoginUserDetails?.domainId });
            dispatch(homeDataSliceActions.setHomeSales({
                data: response?.data ?? null,
                loader: false,
            }));
        } catch (error) {
            console.log("getHomePageSales Error", error);
            dispatch(homeDataSliceActions.setHomeSales({ loader: false }));
        }
    };

    useEffect(() => {
        fetchHomeSales();
    }, []);
};


export const useGetHomeMarketing = () => {
    const dispatch = useDispatcher();

    const fetchHomeMarketing = async () => {
        dispatch(homeDataSliceActions.setHomeMarketing({ loader: true }));
        try {
            const LoginUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
            const response = await getMarketingHomePage({ domainId: LoginUserDetails?.domainId });
            dispatch(homeDataSliceActions.setHomeMarketing({
                data: response?.data ?? [],
                loader: false,
            }));
        } catch (error) {
            console.log("getHomePageMarketing Error", error);
            dispatch(homeDataSliceActions.setHomeMarketing({ loader: false }));
        }
    };

    useEffect(() => {
        fetchHomeMarketing();
    }, []);
};


export const useGetHomeProject = () => {
    const dispatch = useDispatcher();

    const fetchHomeProject = async () => {
        dispatch(homeDataSliceActions.setHomeProject({ loader: true }));
        try {
            const LoginUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
            const url = `${config().NLP_DNS_URL}/projects_home_page_crm`;
            const response = await axios.post(url, { Domain_id: LoginUserDetails?.domainId });
            dispatch(homeDataSliceActions.setHomeProject({
                data: response?.data ?? null,
                loader: false,
            }));
        } catch (error) {
            console.log("getHomePageProject Error", error);
            dispatch(homeDataSliceActions.setHomeProject({ loader: false }));
        }
    };

    useEffect(() => {
        fetchHomeProject();
    }, []);
};