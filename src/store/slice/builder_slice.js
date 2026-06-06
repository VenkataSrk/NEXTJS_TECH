import { BUILDER } from "../redux-store/reducer_names";
import { createSlice } from "@reduxjs/toolkit";

const initialState = {
      addFlowStatus: false,
      addFlowChildId: '',
      oldChatFlowId: 0,
      connectorStatus: false,
      ecommercePurchase: { isPurchase: false, productDetails: [], customerDetail: {}, payment: {}, coupon: '', productIntent: [], token: '', recommendedIntent: [], nlpRecommended: {}, nlpProductIntent: {}, currencyType: '' },
      selectedCardItems: [],
      showCustomerForm: false,
      showAddress: false, //need to change
      showUpdateAddresss: false,
      addressList: [],
      catchEmail: false,
      customerEmail: '',
      selectedAddress: '',
      selectedAddressIndex: 0,
      connectionName: '',
      magentoDomain: '',
      coustomerPaymentData: {},
      bigCommercePaymentType: [],
      showBigCommercePayment: false,
      selectedPayment: '',
      bigCommerceOrderToken: '',
      bigCommerceSelectedId: [],
      commonPaymentForm: false,
      customerUpdateAddress: {},
      checkoutUrl: '',
      dynamicDomainURL: '',
      offersNone: false,
      multiLanguage: false,
      seletedLanguage: '',
      priceFilter: {},
      careHomeVerification: { verificationStep: 0, verificationProcessStatus: 0, userMail: '', userOtp: '', authenticationStatus: 0, authErrorCnt: '', otpErrorCnt: '' },
      customBlockError: { errorNode: '', errorCount: 0 },
      feedBackFormStatus: false,
      feedBackFormData: {},
      closedReferenceStatus: false,
      otpValidationId: '',
      otpValidCount: 0,
      otpResendButtonStatus: false,
      otpResendCount: 0,
      resendTimeOutData: '',
      flowIntentName: ''
};

const chatBuilderSice = createSlice({
      name: BUILDER,
      initialState,
      reducers: {
            setAddFlowStatus: (state, { payload }) => {
                  state.addFlowStatus = payload;
            },
            setAddFlowChildId: (state, { payload }) => {
                  state.addFlowChildId = payload;
            },
            setOldChatFlowId: (state, { payload }) => {
                  state.oldChatFlowId = payload;
            },
            setEcommercePurchase: (state, { payload }) => {
                  state.ecommercePurchase = { ...state.ecommercePurchase, ...payload };
            },
            setCustomBlockError: (state, { payload }) => {
                  if (state.customBlockError.errorNode === payload?.errorNode) {
                        state.customBlockError.errorCount = state.customBlockError.errorCount + 1;
                  } else {
                        state.customBlockError.errorNode = payload?.errorNode;
                        state.customBlockError.errorCount = 1;
                  }
            },
            setCareHomeVerification: (state, { payload }) => {
                  state.careHomeVerification = { ...state.careHomeVerification, ...payload };
            },
            setSelectedCardItems: (state, { payload }) => {
                  state.selectedCardItems.push(payload)
            },
            setTotalCardItems: (state, { payload }) => {
                  state.selectedCardItems = payload
            },
            setEnableCustomerForm: (state, { payload }) => {
                  state.showCustomerForm = payload;
            },
            setAddressList: (state, { payload }) => {
                  state.addressList = payload;
            },
            setShowAddress: (state, { payload }) => {
                  state.showAddress = payload;
            },
            setCatchEmail: (state, { payload }) => {
                  state.catchEmail = payload;
            },
            setCustomerEmail: (state, { payload }) => {
                  state.customerEmail = payload;
            },
            setSelectedAddress: (state, { payload }) => {
                  state.selectedAddress = payload?.selectedAddressValue;
                  state.selectedAddressIndex = payload?.indexValue
            },
            setConnectorStatus: (state, { payload }) => {
                  state.connectorStatus = payload
            },
            setSelectedCardItemsEmpty: (state, { payload }) => {
                  state.selectedCardItems = [];
            },
            setConnectionName: (state, { payload }) => {
                  state.connectionName = payload;
            },
            setMagentoDomain: (state, { payload }) => {
                  state.magentoDomain = payload;
            },
            setCustomerPaymentData: (state, { payload }) => {
                  state.coustomerPaymentData = payload;
            },
            setBigCommercePaymentType: (state, { payload }) => {
                  state.bigCommercePaymentType = payload
            },
            setShowBigCommercePayment: (state, { payload }) => {
                  state.showBigCommercePayment = payload
            },
            setSelectedPayment: (state, { payload }) => {
                  state.selectedPayment = payload
            },
            setBigCommerceOrderToken: (state, { payload }) => {
                  state.bigCommerceOrderToken = payload
            },
            setBigCommerceSelectedId: (state, { payload }) => {
                  state.bigCommerceSelectedId = payload
            },
            setCommonPaymentForm: (state, { payload }) => {
                  state.commonPaymentForm = payload
            },
            setShowUpdateAddresss: (state, { payload }) => {
                  state.showUpdateAddresss = payload
            },
            setCustomerUpdateAddress: (state, { payload }) => {
                  state.customerUpdateAddress = payload
            },
            setCheckoutUrl: (state, { payload }) => {
                  state.checkoutUrl = payload
            },
            setDynamicDomainURL: (state, { payload }) => {
                  state.dynamicDomainURL = payload
            },
            setOffers: (state, { payload }) => {
                  state.offersNone = payload
            },
            setMultiLanguage: (state, { payload }) => {
                  state.multiLanguage = payload
            },
            setSelectedLanguage: (state, { payload }) => {
                  state.seletedLanguage = payload
            },
            setPriceFilter: (state, { payload }) => {
                  state.priceFilter = payload
            },
            setFeedbackFormStatus: (state, { payload }) => {
                  state.feedBackFormStatus = payload
            },
            setFeedbackFormData: (state, { payload }) => {
                  state.feedBackFormData = payload
            },
            setClosedReferenceStatus: (state, { payload }) => {
                  state.closedReferenceStatus = payload
            },
            setOtpValidationId: (state, { payload }) => {
                  state.otpValidationId = payload
            },
            setOtpValidCount: (state, { payload }) => {
                  state.otpValidCount = payload
            },
            setOtpResendButtonStatus: (state, { payload }) => {
                  state.otpResendButtonStatus = payload
            },
            setOtpResendCount: (state, { payload }) => {
                  state.otpResendCount = payload
            },
            setResendTimeOutData: (state, { payload }) => {
                  state.resendTimeOutData = payload
            },
            setFlowIntentName: (state, { payload }) => {
                  state.flowIntentName = payload
            },
            reset: () => initialState,
      },
});

export const chatBuilderActions = chatBuilderSice.actions;
export default chatBuilderSice.reducer;
