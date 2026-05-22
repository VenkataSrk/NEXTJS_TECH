import { createSlice } from "@reduxjs/toolkit";
import { PRODUCT } from "../../constants/reducer_names";


export const initialState = {
    aiAgentChatMessage: [] as any,
    aiAgentChatTyping: {},
    productInsights: {
        performanceRanking: null as any,
        productRevenue: null as any,
        revenueGrowth: null as any,
    },
    productInsightsLoader: {
        performance: false,
        productRevenue: false,
        revenueGrowth: false,
    },
    productCatalog: [] as any,
    productCatalogLoader: false,
 };

const productDataSlice = createSlice({
  name: PRODUCT,
  initialState,
  reducers: {
    
    setproductAiAgentChat: (state, { payload }) => {
      state.aiAgentChatMessage.push(payload)
    },
    
    setproductAiAgentChatTyping: (state, { payload }) => {
      const typingValue = payload?.typing === true ? "Typing ..." : payload?.typing;
      state.aiAgentChatTyping[payload?.sessionId] = typingValue;
    },

    setproductAiAgentChatReset: (state) => {
      state.aiAgentChatMessage = []
    },

    setProductInsights: (state, { payload }) => {
      if (payload?.data) {
        state.productInsights = { ...state.productInsights, ...payload.data };
      }
      if (payload?.loader) {
        state.productInsightsLoader = { ...state.productInsightsLoader, ...payload.loader };
      }
    },

    setProductCatalog: (state, { payload }) => {
      if (payload?.data !== undefined) state.productCatalog = payload.data;
      if (payload?.loader !== undefined) state.productCatalogLoader = payload.loader;
    },

    reset: () => initialState,
  },
});

export const productDataSliceActions = productDataSlice.actions;
export default productDataSlice.reducer;
