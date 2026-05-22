import { config } from "../base/constant";

export const TimeoutDuration = 15000;

//export const BaseUrl = "http://localhost:3010/ai/v1";
export const BASE_URL = config().BASE_URL;
export const MEET_BASE_URL = config().MEET_RESTAPI_URL;
export const CORE = "core_service";
export const CALL = "callservice";
export const MEET = "meetings";
export const USER = "user";
export const MEET_VERSION = "v2";
export const VERSION = "v1";
export const COREURL = `${BASE_URL}/${CORE}/${VERSION}`;
export const CALLURL = `${BASE_URL}/${CALL}/${VERSION}`;
export const MEETURL = `${MEET_BASE_URL}/${VERSION}/`;
export const GUEST = `${MEET_BASE_URL}/${VERSION}/`;

const APP_ID = config().APPID;
const APP_SECRET = config().AppSECRET;

export const AppHeaders = {
  appId: APP_ID,
  appSecret: APP_SECRET,
  "Content-Type": "application/json",
};

//INFO: APP URL ENDPOINTS:

export const LOGIN_API = "http://10.150.0.246:5002/urauth/v1";
export const CORESERVICE_URL = "http://10.150.0.246:3001/core_service/v1";
export const CALLSERVICE_URL = "http://localhost:5007/callservice/v1";