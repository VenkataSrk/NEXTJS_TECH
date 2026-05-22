import { createSlice } from "@reduxjs/toolkit";
import { COMMON_DATA } from "../../constants/reducer_names";
import inbox_icon from "../../../assets/images/menu/minbox.svg";
import contact_icon from "../../../assets/images/menu/mcontact.svg";
import deals_icon from "../../../assets/images/menu/mshake.svg";
import products_icon from "../../../assets/images/menu/mproduct.svg";
import projects_icon from "../../../assets/images/menu/mproject.svg";
import marketing_icon from "../../../assets/images/menu/mcampaign.svg";
import operations_icon from "../../../assets/images/menu/operation.svg";

type CommonDataState = {
  companyOnboarded: boolean;
  agentStatus: any;
  AllQueueList: any[];
  filterQueueList: any[];
  aiChatNavigate: any;
  aiChatInitMsg: any;
  tabMenu: {
    marketing: any[];
    [key: string]: any[];
  };
  appTabs: any[];
  activeTab: string | null;
  activityFlag: boolean;
  dropdownTabs: any[];
  popupChat: boolean;
};

export const initialState: CommonDataState= {
  companyOnboarded:false,
  agentStatus: null,
  AllQueueList: [],
  filterQueueList: [],
  aiChatNavigate: null,
  aiChatInitMsg: null,
  tabMenu: {
    marketing:[
      {path:'/marketing/insights',menuName:'Insights',close:0,index:0},
      {path:'/marketing/campaign',menuName:'Active Campaigns',close:0,index:1},
      {path:'/marketing/schedule-campaign',menuName:'Scheduled Campaigns',close:0,index:2},
      {path:'/marketing/pending-campaign',menuName:'Pending Approval',close:0,index:3},
      {path:'/marketing/past-campaign',menuName:'Past Campaigns',close:0,index:4}
    ],
    contact: [
      {path:'/contact/contactlist',menuName:'Contacts',close:0,index:0},
      {path:'/contact/companyList',menuName:'Company',close:0,index:1},
    ],
    deals: [
      {path:'/deals',menuName:'Deals',close:0,index:0},
    ],
    home: [
      {path:'/home/sales',menuName:'Sales',close:0,index:0},
      {path:'/home/marketing',menuName:'Marketing',close:0,index:1},
      {path:'/home/commerce',menuName:'Commerce',close:0,index:2},
      {path:'/home/project',menuName:'Project',close:0,index:3},
    ],
    projects:[],
    setup:[
      {path:'/setup',menuName:'Operations Hub',close:0,index:0},
    ],
    myplans:[
      {path:'/myplans',menuName:'My Plan',close:0,index:0},
    ],
    products:[
      {path:'/products/insights',menuName:'Insights',close:0,index:0},
      // {path:'/products/tracker',menuName:'Tracker',close:0,index:1},
      {path:'/products/catalog',menuName:'Product Catalog',close:0,index:1},
    ]
  },
  appTabs: [],
  activeTab: null,
  activityFlag: false,

  dropdownTabs : [
    // { id: "aichat", title: "AI Chat", path: "/aichat", img: marketing_icon },
    { id: "inbox", title: "Inbox", path: "/inbox", img: inbox_icon, moduleId: '21' },
    { id: "contact", title: "Contact", path: "/contact/contactlist", img: contact_icon, moduleId: '2' },
    { id: "deals", title: "Deals", path: "/deals", img: deals_icon, moduleId: '1' },
    { id: "products", title: "Product", path: "/products/insights", img: products_icon, moduleId: '4' },
    { id: "projects", title: "Projects", path: "/projects/projectinsights", img: projects_icon, moduleId: '5' },
    { id: "marketing", title: "Marketing", path: "/marketing/insights", img: marketing_icon, moduleId: '3' },
    { id: "setup", title: "Operations Hub", path: "/setup", img: operations_icon, moduleId: '22' },
  ],
  popupChat:false
};

const commonDataSlice = createSlice({
  name: COMMON_DATA,
  initialState,
  reducers: {
    setCompanyOnboarded: (state, { payload }) => {
      state.companyOnboarded = payload;
    },
    setAgentStatus: (state, { payload }) => {
      state.agentStatus = payload;
    },
    setQueueList: (state, { payload }) => {
      const queueList = Array.isArray(payload) ? payload : [];
      const filterList = queueList?.filter((queue) => queue?.outBoundCallStatus === 1);
      state.AllQueueList = queueList;
      state.filterQueueList = filterList;
    },
    setAiChatNavigate:(state,{payload})=>{
      state.aiChatNavigate=payload;
    },
    setAiChatInitMsg:(state,{payload})=>{
      state.aiChatInitMsg=payload;
    },

    setRemoveTabMenuMarketing: (state, { payload }) => {
      state.tabMenu.marketing.splice(payload, 1);
      state.tabMenu.marketing = state.tabMenu.marketing.map((item, i) => ({
        ...item,
        index: i,
      }));
    },
    setModifyTabMenuMarketing:(state,{payload})=>{
      state.tabMenu.marketing =state.tabMenu.marketing.map((item :any)=>{
        if(item?.path?.split("sessionId=")?.[1]==payload?.sessionId){
            item.menuName=payload?.campaignName??"New Conversation";
        }
        return item;
      })
    },

    setTabTitleByAgentSessionId: (state, { payload }) => {
      const sessionId = payload?.sessionId;
      const title = payload?.title?.trim?.();
      if (!sessionId || !title) return;
      if(payload?.agent) {
        state.tabMenu[payload?.agent] = state.tabMenu[payload?.agent]?.map((item: any) => {
          const tabSessionId = item?.path?.split("sessionId=")?.[1]?.split("&")?.[0];
          if (tabSessionId === sessionId) {
            return { ...item, menuName: title };
          }
          return item;
        });
      }
    },

    setTabMenuPush:(state,{payload})=>{
      if(payload?.type){
        const exists = state.tabMenu[payload?.type].some(
          (item) => item.path === payload?.path
        );
        if(!exists){
          state.tabMenu[payload?.type].push({...payload,index:state?.tabMenu?.[payload?.type]?.length});
        }
      }
    },
    
    setRemoveTabMenu: (state, { payload }) => {
      if(payload?.type){
        state.tabMenu[payload?.type].splice(payload?.index, 1);
        state.tabMenu[payload?.type] = state.tabMenu[payload?.type]?.map((item, i) => ({
          ...item,
          index: i,
        }));
      }
    },
    setAppTabs: (state, { payload }) => {
      state.appTabs = payload || [];
    },
    
    addAppTab: (state, { payload }) => {
      const exists = state.appTabs.find((t) => t.path === payload.path);
    
      if (!exists) {
        state.appTabs.push(payload);
        state.activeTab = payload.id;
      } else {
        state.activeTab = exists.id;
      }
    },
    
    setActiveAppTab: (state, { payload }) => {
      state.activeTab = payload;
    },
    
    removeAppTab: (state, { payload }) => {
      const index = state.appTabs.findIndex((t) => t.id === payload);
    
      state.appTabs = state.appTabs.filter((t) => t.id !== payload);
    
      if (state.activeTab === payload) {
        if (state.appTabs.length > 0) {
          const newIndex = index > 0 ? index - 1 : 0;
          state.activeTab = state.appTabs[newIndex]?.id;
        } else {
          state.activeTab = null;
        }
      }
    },

    setActivityFlag: (state, { payload }) => {
      state.activityFlag = payload;
    },
    setAiChat:(state,{payload})=>{
      state.popupChat = payload
    },
    

    reset: () => initialState,
  },
});

export const commonDataSliceActions = commonDataSlice.actions;
export default commonDataSlice.reducer;
