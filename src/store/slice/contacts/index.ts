import { createSlice } from "@reduxjs/toolkit";
import { CONTACTS_DATA } from "../../../constants/reducer_names";


export const initialState = {
    contactsMessage: [] as any[],
    isTyping: false,
    isOpenContactModal: false,
};

const contacts: any = createSlice({
    name: CONTACTS_DATA,
    initialState,
    reducers: {
        setContactsMessage: (state, { payload }) => {
            state.contactsMessage = payload
        },
        setIsTyping: (state, { payload }) => {
            state.isTyping = payload
        },
        setIsOpenContactModal: (state, { payload }) => {
            state.isOpenContactModal = payload
        },
        reset: () => initialState,
    },
});

export const contactsSliceActions = contacts.actions;
export default contacts.reducer;