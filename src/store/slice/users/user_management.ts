import { createAsyncThunk } from "@reduxjs/toolkit";
import { COREURL } from "../../../base/api_constants";
import { ROLES } from "../../../base/constant";
import { ApiServices } from "../../../base/service/api_sevice";
import { getCallSessionDetails, getContactDetails } from "../../../base/service/core_serviceApi";

const getAllUsers = createAsyncThunk("users/getAllUserList", async () => {
  return await ApiServices.get(
    COREURL,
    `/get_user_details`
  );
});

const getUserDetails = createAsyncThunk("users/getUserById", async (id: any) => {
  return await ApiServices.get(COREURL, `/get_user_details_list/${id}`);
});

const createUserDetails = createAsyncThunk("users/createUser", async (data: any) => {
  return await ApiServices.put(COREURL, "/update_user", data);
});

export const getCallNecessaryDetails = createAsyncThunk("data/contact_queue", async (data: any) => {
  let [contact, sessionDetail]: any = await Promise.all([getContactDetails(null, data.contact)

  ]);
  return { contact, sessionDetail };
});

export { getAllUsers, getUserDetails, createUserDetails };
