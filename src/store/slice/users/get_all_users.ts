
import _ from "lodash";
import { createSlice } from "@reduxjs/toolkit";
import { ALL_USER_LIST } from "../../constants/reducer_names";
import { dataSearch, FailedUserFetchResponse, filterData, filterSearch, getReportTo, mapResponseDataToTable, resetFilter } from "../../../base/utils";
import { getAllUsers } from "./user_management";

export const initialState = {
  loading: false,
  error: {
    hasError: false,
    errorMessage: "",
  },
  availableUsers: {} as any,
  isGetuser: false,
  users: [] as any,
  supervisorList: [] as any,
  userCopy: [] as any,
  usersecondCopy: [] as any,
  userTableData: [] as any,
  userSearchData: [] as any,
  userCopySearchData: [] as any,
  openAdvanceFilterSearch: false,
  openSelectDropdown: false,
  filterType: [] as any,
  newFilterType: "",
  reportTo: [] as any,
  routingProfile: [] as any,
  testTableData: [] as any,
  advancedSearchData: {
    role: [] as any,
    routingProfileName: [] as any,
    reportToName: [] as any,
  },
  searchName: "",
  isVisibleAgent: false,
  selectedAgent: [] as any
};

const allUserSlice = createSlice({
  name: ALL_USER_LIST,
  initialState,
  reducers: {
    // updateTableData: (state, { payload }) => {
    //   const index = helper.checkIfExist(state, payload?.emailId);
    //   if (index === -1) {
    //     state.users.push(payload);
    //     state.userTableData.push(helper.getMappedObject(state, payload));
    //     state.userSearchData.push(helper.getMappedObject(state, payload));
    //   } else {
    //     state.users[index] = payload;
    //     const subIndex = helper.checkIfTableRowExist(state, state.users[index]?.emailId);
    //     state.userTableData[subIndex] = helper.getMappedObjectForUpdate(state, payload, subIndex);
    //     state.userSearchData[subIndex] = helper.getMappedObjectForUpdate(state, payload, subIndex);
    //   }
    // },
    selectedAgent: (state, { payload }) => {
      let datavalue;
      let index = state.selectedAgent.findIndex((item: any) => item.uid === payload.uid);
      if (index > -1) {
        datavalue = [...state.selectedAgent];
        datavalue.splice(index, 1);
      } else {
        datavalue = [...state.selectedAgent];
        datavalue.push(payload);
      }
      state.selectedAgent = datavalue;
    },
    removeAgent: (state, { payload }) => {
      let datavalue;
      let index = state.selectedAgent.findIndex((item: any) => item.uid === payload.uid);
      if (index > -1) {
        datavalue = [...state.selectedAgent];
        datavalue.splice(index, 1);
      }
      state.selectedAgent = datavalue;
    },
    setVisible: (state, { payload }) => {
      state.isVisibleAgent = payload;
    },

    // deleteUsert: (state, { payload }) => {
    //   const index = helper.checkIfExist(state, payload?.agentId);
    //   const subIndex = helper.checkIfTableRowExist(state, payload?.agentId);
    //   if (index !== -1) {
    //     state.users.splice(index, 1);
    //     state.userTableData.splice(subIndex, 1);
    //     state.userSearchData.splice(subIndex, 1);
    //   }
    // },

    role: (state, { payload }) => {
      state.advancedSearchData.role = payload.role === undefined ? "" : payload.role;
      dataSearch(state);
    },
    routingProfileName: (state, { payload }) => {
      state.advancedSearchData.routingProfileName = payload.routingName === undefined ? "" : payload.routingName;
      dataSearch(state);
    },
    reportToName: (state, { payload }) => {
      state.advancedSearchData.reportToName = payload.reportName === undefined ? "" : payload.reportName;
      dataSearch(state);
    },

    openSearchFilter: (state) => {
      state.openAdvanceFilterSearch = !state.openAdvanceFilterSearch;
    },
    openDropDown: (state) => {
      state.openSelectDropdown = !state.openSelectDropdown;
    },

    clearFilter: (state) => {
      state.userSearchData = state.userTableData;
      state.advancedSearchData.role = [];
      state.advancedSearchData.routingProfileName = [];
      state.advancedSearchData.reportToName = [];
    },

    searchForAgent: (state, { payload }) => {
      state.searchName = payload;
      if (payload.length >= 1) {
        state.isVisibleAgent = true;
        state.users = state.userCopy.filter((data: any) => {
          return filterData(data, payload);
        });
      } else {
        state.isVisibleAgent = false;
        state.users = state.usersecondCopy;
      }
    },

    applySearchFilter: (state, { payload }) => {
      if (state.advancedSearchData.role.length === 0 || state.advancedSearchData.routingProfileName.length === 0 || state.advancedSearchData.reportToName.length === 0) {
        if (payload.length < 1) {
          resetFilter(state);
        } else {
          state.userSearchData = state.userTableData.filter((data: any) => {
            return filterSearch(data, payload);
          });
        }
      } else {
        if (
          state.advancedSearchData.role.length !== 0 ||
          state.advancedSearchData.routingProfileName.length !== 0 ||
          state.advancedSearchData.reportToName.length !== 0
        ) {
          if (payload.length < 2) {
            resetFilter(state);
          } else {
            state.userSearchData = state.userCopySearchData.filter((data: any) => {
              return filterSearch(data, payload);
            });
          }
        }
      }
    },

    updateStatus: (state, { payload }) => {
      const eventEmail = payload?.email?.toString()?.toLowerCase();
      if (!eventEmail) return;
      const nextStatus = payload?.status ?? "Offline";

      const updateUserRow = (user: any) => {
        const rowEmail = (user?.email ?? user?.emailId ?? "").toString().toLowerCase();
        if (rowEmail !== eventEmail) return user;
        return { ...user, statusName: nextStatus, status: nextStatus };
      };

      state.users = state.users.map(updateUserRow);
      state.userCopy = state.userCopy.map(updateUserRow);
      state.usersecondCopy = state.usersecondCopy.map(updateUserRow);

      state.userTableData = state.userTableData.map((row: any) => {
        const rowEmail = (row?.emailId ?? row?.email ?? "").toString().toLowerCase();
        if (rowEmail !== eventEmail) return row;
        return { ...row, statusName: nextStatus, status: nextStatus };
      });

      state.userSearchData = state.userSearchData.map((row: any) => {
        const rowEmail = (row?.emailId ?? row?.email ?? "").toString().toLowerCase();
        if (rowEmail !== eventEmail) return row;
        return { ...row, statusName: nextStatus, status: nextStatus };
      });
    },

    setRoutingProfile: (state, { payload }) => {
      state.routingProfile = payload
    },


    reset: () => { },
  },

  extraReducers: (builder) => {
 
    builder.addCase(getAllUsers.fulfilled, (state, { payload }) => {
      const apiData = payload?.data ?? {};
      const rawUsers = Array.isArray(apiData?.getUserListByRoleId)
        ? apiData.getUserListByRoleId
        : Array.isArray(apiData?.result)
        ? apiData.result
        : [];

      const users = rawUsers.map((user: any) => ({
        UserID: user?.UserID ?? user?.Idx ?? user?.user_id ?? user?.id,
        UserName:
          user?.UserName ??
          user?.username ??
          `${user?.Firstname ?? ""} ${user?.Surname ?? ""}`.trim(),
        roleid: user?.org_role_id_crm ?? user?.roleid ?? user?.Role_Id,
        routing_profile_name: user?.routing_profile_name ?? user?.routing_profile ?? "",
        routing_profile: user?.routing_profile ?? "",
        report_to_name: user?.report_to_name ?? "",
        report_to: user?.report_to ?? "",
        isActive: user?.isActive ?? user?.Status,
        ext: user?.ext ?? user?.Extension_Number,
        emailId: user?.emailId ?? user?.Email ?? user?.app_login_user_name,
        phone_number: user?.phone_number ?? user?.Mobileno,
        desk_phone_number: user?.desk_phone_number ?? user?.Direct_Number,
        inboundCall: user?.inboundCall,
        outboundCampaignCall: user?.outboundCampaignCall,
        callTypeId: user?.callTypeId,
      }));

      if (apiData?.statusCode === 200 || apiData?.status_code === 200) {
        state.users = users;
        state.userCopy = users;
        state.usersecondCopy = users;
        state.reportTo = getReportTo(users);
        // state.routingProfile = helper.getRoutingProfile(payload?.data?.getUserListByRoleId);
        mapResponseDataToTable(state, users);
        state.isGetuser = true;
        state.error.hasError = false;
        state.error.errorMessage = "";
      } else {
        state.isGetuser = false;
        state.error.hasError = true;
        state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
      }

      state.loading = false;
    });

    builder.addCase(getAllUsers.rejected, (state, { payload }: any) => {
      state.isGetuser = false;
      state.error.hasError = true;
      state.error.errorMessage = payload?.message ?? FailedUserFetchResponse;
      state.loading = false;
    });

    // builder.addCase(routingProfile.fulfilled, (state, { payload }: any) => {
    //   const data_ = (payload?.data?.getRoutingListRes ?? [])
    //     .map((element: any) => element?.routing_prof_name)
    //     .filter(Boolean);
    //   state.routingProfile = data_;
    // });
  },
});
export const GetAlluserActions = allUserSlice.actions;
export default allUserSlice.reducer;
