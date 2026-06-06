import { createSlice } from "@reduxjs/toolkit";
import { KNOWLEDGE_DATA } from "../../../constants/reducer_names";


export const initialState = {
    knowledgeMessage: [] as any[],
    isTyping: false,
    isModalOpen:false,
    progress:false,
    navigateMsg:{
        path:'',
        count:0
    }
};

const knowledge: any = createSlice({
    name: KNOWLEDGE_DATA,
    initialState,
    reducers: {
        setknowledgeMessage: (state, { payload }) => {
            state.knowledgeMessage = payload
        },
        setIsTyping: (state, { payload }) => {
            state.isTyping = payload
        },
         isModalOpen: (state, { payload }) => {
            state.isModalOpen = payload
        },
         progress: (state, { payload }) => {
            state.progress = payload
        },
        setNavigate:(state,{payload})=>{
            state.navigateMsg = payload
        },
        reset: () => initialState,
    },
});

export const knowledgeDataSliceActions = knowledge.actions;
export default knowledge.reducer;