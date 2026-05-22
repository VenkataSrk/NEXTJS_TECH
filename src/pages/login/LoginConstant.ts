import { config } from "../../base/constant";

export const loginConfig = () => {
  const LOGIN_API = "https://http://10.150.0.246:5002/urauth/v1/token/create_jwt";
  const APPID = config().APPID;
  const AppSECRET = config().AppSECRET;
  const BLANK_VALIDATION = " Username and Password field cannot be blank";
  const AUTHENTICATION_FAILED = "Something went wrong,try again";
  const EMAIL_BLANK = "Enter email Id";
  const INVLID_EMAIL = "Enter valid email Id";
  const BLANK_PASSWORD = "Enter Password";
  const COMMON_ERROR = "Enter valid email and password";
  const NO_ACCOUNT_FOUND = "Hmmm, no Worktual account matches the provided credentials.";
  const RESET_PASS_API = "http://10.22.7.115:3001/core_service/v1/forgot_password";
  return {
    APPID,
    AppSECRET,
    BLANK_VALIDATION,
    AUTHENTICATION_FAILED,
    EMAIL_BLANK,
    INVLID_EMAIL,
    BLANK_PASSWORD,
    LOGIN_API,
    COMMON_ERROR,
    NO_ACCOUNT_FOUND,
    RESET_PASS_API,
  };
};
