import { createSlice } from "@reduxjs/toolkit";
import { AGENT_STATUS } from "../../constants/reducer_names";
import { agentSearchData, agentStatusDetails, FailedUserFetchResponse, resetFilter } from "../../../base/utils";

const initialState = {
  loading: false,
  deleteLoading: false,
  setloading: false,
  checkStatusName: false,
  isModalOpen: false,
  buttonsave: false,
  noAgentStatus: false,
  isCreateModelOpen: false,
  editStatusName: "" as string,
  agentStatusSearchData: "" as string,
  error: {
    hasError: false,
    errorMessage: "",
  },
  agentstatusList: [] as any,
  agentstatusdata: [] as any,
  agentstatusFilter: {} as any,
  agentDetails: {
    statusName: "" as string,
    description: "" as string,
    isVoice: 0,
    isVoiceCallTransfer: 0,
    isChat: 0,
    isChatTransfer: 0,
    isEmail: 0,
    isEmailTransfer: 0,
    colorCode: "" as string,
  } as any,
  editAgentDetails: {} as any,
  constant: {} as any,
  checkChanges: false,
  socketDisconnectStatus: false
};

const agentStatus = createSlice({
  name: AGENT_STATUS,
  initialState,
  reducers: {
    pushagentlist: (state, { payload }) => {
      state.agentstatusList.push(payload)
    },
    agentstatusList: (state, { payload }) => {
      state.agentstatusList = payload;
    },
    agentstatusdata: (state, { payload }) => {
      state.agentstatusdata = payload;
    },
    agentStatusSearchData: (state, { payload }) => {
      state.agentStatusSearchData = payload;
      if (payload.length < 1) {
        resetFilter(state);
      } else {
        if (state.agentstatusdata.length > 0) {
          state.agentstatusList = agentSearchData(
            state.agentstatusdata,
            payload
          );
        }
      }
    },
    statusName: (state, { payload }) => {
      state.agentDetails.statusName = payload;
    },
    description: (state, { payload }) => {
      state.agentDetails.description = payload;
    },
    setSocketDisconnectStatus: (state, {payload})=>{
      state.socketDisconnectStatus = payload
    },
    setagentDetails: (state, { payload }) => {
      state.agentDetails[payload] = state.agentDetails[payload] === 1 ? 0 : 1;
      if (state.agentDetails.isVoice === 0) {
        state.agentDetails.isVoiceCallTransfer = 0
      }
      if (state.agentDetails.isChat === 0) {
        state.agentDetails.isChatTransfer = 0
      }
      if (state.agentDetails.isEmail === 0) {
        state.agentDetails.isEmailTransfer = 0
      }

    },

    colorCode: (state, { payload }) => {
      state.agentDetails.colorCode = payload;
    },
    isModalOpen: (state, { payload }) => {
      state.isModalOpen = payload;
    },
    isCreateModelOpen: (state, { payload }) => {
      state.isCreateModelOpen = payload;
    },
    agentstatusFilter: (state, { payload }) => {
      state.agentstatusFilter = payload;
    },
    buttonsave: (state, { payload }) => {
      state.buttonsave = payload;
    },
    setloading: (state, { payload }) => {
      state.setloading = payload;
    },
    checkStatusName: (state, { payload }) => {
      state.checkStatusName = payload;
    },
    agentDetails: (state, { payload }) => {
      state.agentDetails = payload;
    },
    noAgentStatus: (state, { payload }) => {
      state.noAgentStatus = payload;
    },
    checkChanges: (state, { payload }) => {
      state.checkChanges = payload;
    },
    editStatusName: (state, { payload }) => {
      state.editStatusName = payload;
    },
    constant: (state, { payload }) => {
      state.agentDetails = {
        statusName: "",
        description: "",
        isVoice: 0,
        isVoiceCallTransfer: 0,
        isChat: 0,
        isChatTransfer: 0,
        isEmail: 0,
        isEmailTransfer: 0,
        colorCode: "#575CFF",
      };
    },
    reset: () => { },
  },
  // extraReducers: (builder) => {
  //   builder.addCase(getAgentStatus.pending, (state) => {
  //     state.loading = true;
  //   });
  //   builder.addCase(getAgentStatus.fulfilled, (state, { payload }) => {
  //     if (payload?.data?.statusCode === 200) {
  //       state.agentstatusList = agentStatusDetails(
  //         payload?.data?.getAgentListRes
  //       );
  //       state.agentstatusdata = agentStatusDetails(
  //         payload?.data?.getAgentListRes
  //       );

  //       state.error.hasError = false;
  //       state.error.errorMessage = "";
  //     } else {
  //       state.error.hasError = true;
  //       state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     }
  //     state.loading = false;
  //   });
  //   builder.addCase(getAgentStatus.rejected, (state, { payload }: any) => {
  //     state.error.hasError = true;
  //     state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     state.loading = false;
  //   });

  //   builder.addCase(deleteAgentStatus.pending, (state) => {
  //     state.deleteLoading = true;
  //   });
  //   builder.addCase(deleteAgentStatus.fulfilled, (state, { payload }) => {
  //     if (payload?.data?.statusCode === 200) {
  //       state.error.hasError = false;
  //       state.error.errorMessage = "";
  //     } else {
  //       state.error.hasError = true;
  //       state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     }
  //     state.deleteLoading = false;
  //   });
  //   builder.addCase(deleteAgentStatus.rejected, (state, { payload }: any) => {
  //     state.error.hasError = true;
  //     state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     state.deleteLoading = false;
  //   });

  //   builder.addCase(getAgentStatusDetails.pending, (state) => {
  //     state.setloading = true;
  //   });
  //   builder.addCase(getAgentStatusDetails.fulfilled, (state, { payload }) => {
  //     if (payload?.data?.statusCode === 200) {
  //       state.agentDetails = payload?.data?.getAgentDetailsRes[0];
  //       state.editAgentDetails = payload?.data?.getAgentDetailsRes[0];
  //       state.editStatusName = payload?.data?.getAgentDetailsRes[0].statusName;
  //       state.error.hasError = false;
  //       state.error.errorMessage = "";
  //     } else {
  //       state.error.hasError = true;
  //       state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     }
  //     state.setloading = false;
  //   });
  //   builder.addCase(
  //     getAgentStatusDetails.rejected,
  //     (state, { payload }: any) => {
  //       state.error.hasError = true;
  //       state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //       state.setloading = false;
  //     }
  //   );

  //   builder.addCase(updateAgentStatus.pending, (state) => {
  //     state.loading = true;
  //   });
  //   builder.addCase(updateAgentStatus.fulfilled, (state, { payload }) => {
  //     if (payload?.data?.statusCode === 200) {
  //       state.error.hasError = false;
  //       state.error.errorMessage = "";
  //     } else {
  //       state.error.hasError = true;
  //       state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     }
  //     state.loading = false;
  //   });
  //   builder.addCase(updateAgentStatus.rejected, (state, { payload }: any) => {
  //     state.error.hasError = true;
  //     state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
  //     state.loading = false;
  //   });
  // },
});

export const agentstatusListAction = agentStatus.actions;
export default agentStatus.reducer;
