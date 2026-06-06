import { SHOPING_SLICE } from "../redux-store/reducer_names";
import { createSlice } from "@reduxjs/toolkit";

const initialState = {
    cartData: [],
    showCartModel: false,
    itemDetails: {},
    showItemModel: false,
    productList: [],
    authToken: '',
    product: {},
}
const ShopingSice = createSlice({
    name: SHOPING_SLICE,
    initialState,
    reducers: {
        setCartData: (state, { payload }) => {
            state.cartData = payload;
        },
        setShowCartModel: (state, { payload }) => {
            state.showCartModel = payload;
        },
        setProductList: (state, { payload }) => {
            state.productList = payload;
        },
        setShowItemModel: (state, { payload }) => {
            state.itemDetails = payload.itemDetails
            state.showItemModel = payload.toggle;
        },
        updateItemDetails: (state, { payload }) => {
            state.itemDetails = payload
        },
        setAuthToken: (state, { payload }) => {
            state.authToken = payload
        },
        setProduct: (state, { payload }) => {
            state.product = payload
        },
        reset: () => initialState,
    }
})

export const ShopingSiceActions = ShopingSice.actions;
export default ShopingSice.reducer;