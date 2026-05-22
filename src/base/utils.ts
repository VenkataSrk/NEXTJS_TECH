import { config, ROLES } from "./constant";
import { ClearStorage, GetItemFromStorage, SetItemInStorage } from "../base/custom_hooks/useStorage";
import jwtDecode from "jwt-decode";
import moment from "moment";
import { useNavigate } from "react-router-dom";
import { LocalStorage } from "../../src/base/custom_hooks/localStorageKeys";
import axios from "axios";
import { countryTimezones } from "../../src/base/countryConstants";
import { Cookies } from "../../src/base/cookies";
import { differenceInDays, format } from 'date-fns';
import _ from "lodash";
import store from "../store/redux-store/root_store";

export class Cookie {
  static host = window.location.hostname;

  static set(name: any, value: any, days: any) {
    let domain, domainParts, date: any, expires;

    if (days) {
      date = new Date();

      date.setTime(date.getTime() + days * 24 * 60 * 60 * 1000);

      expires = "; expires=" + date.toGMTString();
    } else {
      expires = "";
    }

    if (this.host.split(".").length === 1) {
      document.cookie = name + "=" + value + expires + "; path=/";
    } else {
      domainParts = this.host.split(".");

      domainParts.shift();

      domain = domainParts.join(".");

      document.cookie = name + "=" + value + expires + "; path=/; domain=" + domain;
    }
  }

  static get = () => {
    return document.cookie
      .split(";")
      .map((cookie) => cookie.split("="))
      .reduce((accumulator, [key, value]) => {
        if (key.trim()) return { ...accumulator, [key.trim()]: decodeURIComponent(value) };
        else return "";
      }, {});
  };

  static clear = () => {
    const host = window.location.hostname;

    const domainParts = host.split(".");

    domainParts.shift();

    const domain = domainParts.join(".");

    document.cookie = "accessToken=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "roleId=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "stateChange=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "userDetials=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "workStatus=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "userEmail=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "refreshToken=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "MY_APP_STATE=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "agentExt=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "RememberMe=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "ssoToken=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
    document.cookie = "dashboardTour=; expires=Thu, 01 Jan 1970 00:00:00 UTC; path=/;domain=" + domain;
  };
}

export const determineIps = async () => {
  try {
    return new Promise((resolve) => {
      const servers = [
        { urls: 'stun:audioturnv4.worktual.co.uk:443' }
      ];
      const pc = new RTCPeerConnection({ iceServers: servers });
      const ips = new Set();
      pc.onicecandidate = (event) => {
        if (event.candidate) {
          const candidate = event.candidate.candidate;
          const ipMatch = candidate?.match(/([0-9]{1,3}(\.[0-9]{1,3}){3})/);
          if (ipMatch) {
            const ip = ipMatch[1];
            if (!ips.has(ip)) {
              console.log('[determineIps] ICE candidate IP:', ip, candidate.includes('srflx') ? '(public)' : '(local)');
              ips.add(ip);
            }
          }
        }
      };

      pc.createDataChannel('test');
      pc.createOffer().then(offer => pc.setLocalDescription(offer));

      setTimeout(() => {
        pc.close();
        const publicIps = Array.from(ips).filter((ip: any) => !ip.startsWith('192.168.') && !ip.startsWith('10.') && !ip.startsWith('172.') && ip !== '0.0.0.0');
        const publicIp = publicIps[0] || Array.from(ips)[0];
        if (publicIp) {
          SetItemInStorage("IP", publicIp);
        }
        console.log("[determineIps] setTimeout === ip", publicIp)
        resolve(publicIp);
      }, 3000);
    });
  } catch (error) {
    console.log("determineIps -- error", error)
  }
};


export const urlsplit = (url: any) => {

  const queryString = url.split('?')[1];
  const paramsArray = queryString.split('&');
  const params: any = {};

  paramsArray.forEach((param: any) => {
    const [key, value] = param.split('=');
    params[key] = value;
  });


  return ({ domainId: params['domainId'], channelTypeId: params['channelTypeId'], historyId: params['historyId'] })
}

export const customer: any = [
  "Profile ID",
  "First name",
  "Last name",
  "Position",
  "Customer Type",
  "Category",
  "Organization",
  "Website",
  "Address line",
  "City",
  "Zip_Postal Code",
  "Country",
  "State",
  "Email-Work",
  "Email-Personal",
  "Email-Others",
  "Phone-Work",
  "Phone-Personal",
  "Phone-Others",
  "Whatsapp",
  "Linked In",
  "Twitter",
  "Instagram",
  "Facebook",
  "Notes",
  "Product Interests",
  "Pain points",
  "Preferences",
];

export const system: any = [
  "Customer number",
  "Customer ID",
  "Dialed number",
  "Customer callback number",
  "Stored customer input",
  "Queue name",
  "Queue outbound number",
  "Contact id",
  "Initial Contact id",
  "Previous Contact id",
  "Channel",
  "Initiation method",
  "Language",
  "System Endpoint Type",
  "Queue Outbound Caller ID number",
  "Queue Outbound Caller ID number type",
  "Preferences",
];

export const agent: any = ["Agent User Name", "Agent First Name", "Agent Last Name"];

export const queue: any = [" Excalation Queue", "Qa test queue", "Check live call", "UMN queue", "Technical queue", "sales queue"];

export const timeZoneList: any = [
  {
    id: "Pacific/Niue",
    name: "(GMT-11:00) Niue",
  },
  {
    id: "Pacific/Pago_Pago",
    name: "(GMT-11:00) Pago Pago",
  },
  {
    id: "Pacific/Honolulu",
    name: "(GMT-10:00) Hawaii Time",
  },
  {
    id: "Pacific/Rarotonga",
    name: "(GMT-10:00) Rarotonga",
  },
  {
    id: "Pacific/Tahiti",
    name: "(GMT-10:00) Tahiti",
  },
  {
    id: "Pacific/Marquesas",
    name: "(GMT-09:30) Marquesas",
  },
  {
    id: "America/Anchorage",
    name: "(GMT-09:00) Alaska Time",
  },
  {
    id: "Pacific/Gambier",
    name: "(GMT-09:00) Gambier",
  },
  {
    id: "America/Los_Angeles",
    name: "(GMT-08:00) Pacific Time",
  },
  {
    id: "America/Tijuana",
    name: "(GMT-08:00) Pacific Time - Tijuana",
  },
  {
    id: "America/Vancouver",
    name: "(GMT-08:00) Pacific Time - Vancouver",
  },
  {
    id: "America/Whitehorse",
    name: "(GMT-08:00) Pacific Time - Whitehorse",
  },
  {
    id: "Pacific/Pitcairn",
    name: "(GMT-08:00) Pitcairn",
  },
  {
    id: "America/Dawson_Creek",
    name: "(GMT-07:00) Mountain Time - Dawson Creek",
  },
  {
    id: "America/Denver",
    name: "(GMT-07:00) Mountain Time",
  },
  {
    id: "America/Edmonton",
    name: "(GMT-07:00) Mountain Time - Edmonton",
  },
  {
    id: "America/Hermosillo",
    name: "(GMT-07:00) Mountain Time - Hermosillo",
  },
  {
    id: "America/Mazatlan",
    name: "(GMT-07:00) Mountain Time - Chihuahua, Mazatlan",
  },
  {
    id: "America/Phoenix",
    name: "(GMT-07:00) Mountain Time - Arizona",
  },
  {
    id: "America/Yellowknife",
    name: "(GMT-07:00) Mountain Time - Yellowknife",
  },
  {
    id: "America/Belize",
    name: "(GMT-06:00) Belize",
  },
  {
    id: "America/Chicago",
    name: "(GMT-06:00) Central Time",
  },
  {
    id: "America/Costa_Rica",
    name: "(GMT-06:00) Costa Rica",
  },
  {
    id: "America/El_Salvador",
    name: "(GMT-06:00) El Salvador",
  },
  {
    id: "America/Guatemala",
    name: "(GMT-06:00) Guatemala",
  },
  {
    id: "America/Managua",
    name: "(GMT-06:00) Managua",
  },
  {
    id: "America/Mexico_City",
    name: "(GMT-06:00) Central Time - Mexico City",
  },
  {
    id: "America/Regina",
    name: "(GMT-06:00) Central Time - Regina",
  },
  {
    id: "America/Tegucigalpa",
    name: "(GMT-06:00) Central Time - Tegucigalpa",
  },
  {
    id: "America/Winnipeg",
    name: "(GMT-06:00) Central Time - Winnipeg",
  },
  {
    id: "Pacific/Galapagos",
    name: "(GMT-06:00) Galapagos",
  },
  {
    id: "America/Bogota",
    name: "(GMT-05:00) Bogota",
  },
  {
    id: "America/Cancun",
    name: "(GMT-05:00) America Cancun",
  },
  {
    id: "America/Cayman",
    name: "(GMT-05:00) Cayman",
  },
  {
    id: "America/Guayaquil",
    name: "(GMT-05:00) Guayaquil",
  },
  {
    id: "America/Havana",
    name: "(GMT-05:00) Havana",
  },
  {
    id: "America/Iqaluit",
    name: "(GMT-05:00) Eastern Time - Iqaluit",
  },
  {
    id: "America/Jamaica",
    name: "(GMT-05:00) Jamaica",
  },
  {
    id: "America/Lima",
    name: "(GMT-05:00) Lima",
  },
  {
    id: "America/Nassau",
    name: "(GMT-05:00) Nassau",
  },
  {
    id: "America/New_York",
    name: "(GMT-05:00) Eastern Time",
  },
  {
    id: "America/Panama",
    name: "(GMT-05:00) Panama",
  },
  {
    id: "America/Port-au-Prince",
    name: "(GMT-05:00) Port-au-Prince",
  },
  {
    id: "America/Rio_Branco",
    name: "(GMT-05:00) Rio Branco",
  },
  {
    id: "America/Toronto",
    name: "(GMT-05:00) Eastern Time - Toronto",
  },
  {
    id: "Pacific/Easter",
    name: "(GMT-05:00) Easter Island",
  },
  {
    id: "America/Caracas",
    name: "(GMT-04:30) Caracas",
  },
  {
    id: "America/Asuncion",
    name: "(GMT-03:00) Asuncion",
  },
  {
    id: "America/Barbados",
    name: "(GMT-04:00) Barbados",
  },
  {
    id: "America/Boa_Vista",
    name: "(GMT-04:00) Boa Vista",
  },
  {
    id: "America/Campo_Grande",
    name: "(GMT-03:00) Campo Grande",
  },
  {
    id: "America/Cuiaba",
    name: "(GMT-03:00) Cuiaba",
  },
  {
    id: "America/Curacao",
    name: "(GMT-04:00) Curacao",
  },
  {
    id: "America/Grand_Turk",
    name: "(GMT-04:00) Grand Turk",
  },
  {
    id: "America/Guyana",
    name: "(GMT-04:00) Guyana",
  },
  {
    id: "America/Halifax",
    name: "(GMT-04:00) Atlantic Time - Halifax",
  },
  {
    id: "America/La_Paz",
    name: "(GMT-04:00) La Paz",
  },
  {
    id: "America/Manaus",
    name: "(GMT-04:00) Manaus",
  },
  {
    id: "America/Martinique",
    name: "(GMT-04:00) Martinique",
  },
  {
    id: "America/Port_of_Spain",
    name: "(GMT-04:00) Port of Spain",
  },
  {
    id: "America/Porto_Velho",
    name: "(GMT-04:00) Porto Velho",
  },
  {
    id: "America/Puerto_Rico",
    name: "(GMT-04:00) Puerto Rico",
  },
  {
    id: "America/Santo_Domingo",
    name: "(GMT-04:00) Santo Domingo",
  },
  {
    id: "America/Thule",
    name: "(GMT-04:00) Thule",
  },
  {
    id: "Atlantic/Bermuda",
    name: "(GMT-04:00) Bermuda",
  },
  {
    id: "America/St_Johns",
    name: "(GMT-03:30) Newfoundland Time - St. Johns",
  },
  {
    id: "America/Araguaina",
    name: "(GMT-03:00) Araguaina",
  },
  {
    id: "America/Argentina/Buenos_Aires",
    name: "(GMT-03:00) Buenos Aires",
  },
  {
    id: "America/Bahia",
    name: "(GMT-03:00) Salvador",
  },
  {
    id: "America/Belem",
    name: "(GMT-03:00) Belem",
  },
  {
    id: "America/Cayenne",
    name: "(GMT-03:00) Cayenne",
  },
  {
    id: "America/Fortaleza",
    name: "(GMT-03:00) Fortaleza",
  },
  {
    id: "America/Godthab",
    name: "(GMT-03:00) Godthab",
  },
  {
    id: "America/Maceio",
    name: "(GMT-03:00) Maceio",
  },
  {
    id: "America/Miquelon",
    name: "(GMT-03:00) Miquelon",
  },
  {
    id: "America/Montevideo",
    name: "(GMT-03:00) Montevideo",
  },
  {
    id: "America/Paramaribo",
    name: "(GMT-03:00) Paramaribo",
  },
  {
    id: "America/Recife",
    name: "(GMT-03:00) Recife",
  },
  {
    id: "America/Santiago",
    name: "(GMT-03:00) Santiago",
  },
  {
    id: "America/Sao_Paulo",
    name: "(GMT-02:00) Sao Paulo",
  },
  {
    id: "Antarctica/Palmer",
    name: "(GMT-03:00) Palmer",
  },
  {
    id: "Antarctica/Rothera",
    name: "(GMT-03:00) Rothera",
  },
  {
    id: "Atlantic/Stanley",
    name: "(GMT-03:00) Stanley",
  },
  {
    id: "America/Noronha",
    name: "(GMT-02:00) Noronha",
  },
  {
    id: "Atlantic/South_Georgia",
    name: "(GMT-02:00) South Georgia",
  },
  {
    id: "America/Scoresbysund",
    name: "(GMT-01:00) Scoresbysund",
  },
  {
    id: "Atlantic/Azores",
    name: "(GMT-01:00) Azores",
  },
  {
    id: "Atlantic/Cape_Verde",
    name: "(GMT-01:00) Cape Verde",
  },
  {
    id: "Africa/Abidjan",
    name: "(GMT+00:00) Abidjan",
  },
  {
    id: "Africa/Accra",
    name: "(GMT+00:00) Accra",
  },
  {
    id: "Africa/Bissau",
    name: "(GMT+00:00) Bissau",
  },
  {
    id: "Africa/Casablanca",
    name: "(GMT+00:00) Casablanca",
  },
  {
    id: "Africa/El_Aaiun",
    name: "(GMT+00:00) El Aaiun",
  },
  {
    id: "Africa/Monrovia",
    name: "(GMT+00:00) Monrovia",
  },
  {
    id: "America/Danmarkshavn",
    name: "(GMT+00:00) Danmarkshavn",
  },
  {
    id: "Atlantic/Canary",
    name: "(GMT+00:00) Canary Islands",
  },
  {
    id: "Atlantic/Faroe",
    name: "(GMT+00:00) Faeroe",
  },
  {
    id: "Atlantic/Reykjavik",
    name: "(GMT+00:00) Reykjavik",
  },
  {
    id: "Etc/GMT",
    name: "(GMT+00:00) GMT (no daylight saving)",
  },
  {
    id: "Europe/Dublin",
    name: "(GMT+00:00) Dublin",
  },
  {
    id: "Europe/Lisbon",
    name: "(GMT+00:00) Lisbon",
  },
  {
    id: "Europe/London",
    name: "(GMT+00:00) London",
  },
  {
    id: "Africa/Algiers",
    name: "(GMT+01:00) Algiers",
  },
  {
    id: "Africa/Ceuta",
    name: "(GMT+01:00) Ceuta",
  },
  {
    id: "Africa/Lagos",
    name: "(GMT+01:00) Lagos",
  },
  {
    id: "Africa/Ndjamena",
    name: "(GMT+01:00) Ndjamena",
  },
  {
    id: "Africa/Tunis",
    name: "(GMT+01:00) Tunis",
  },
  {
    id: "Africa/Windhoek",
    name: "(GMT+02:00) Windhoek",
  },
  {
    id: "Europe/Amsterdam",
    name: "(GMT+01:00) Amsterdam",
  },
  {
    id: "Europe/Andorra",
    name: "(GMT+01:00) Andorra",
  },
  {
    id: "Europe/Belgrade",
    name: "(GMT+01:00) Central European Time - Belgrade",
  },
  {
    id: "Europe/Berlin",
    name: "(GMT+01:00) Berlin",
  },
  {
    id: "Europe/Brussels",
    name: "(GMT+01:00) Brussels",
  },
  {
    id: "Europe/Budapest",
    name: "(GMT+01:00) Budapest",
  },
  {
    id: "Europe/Copenhagen",
    name: "(GMT+01:00) Copenhagen",
  },
  {
    id: "Europe/Gibraltar",
    name: "(GMT+01:00) Gibraltar",
  },
  {
    id: "Europe/Luxembourg",
    name: "(GMT+01:00) Luxembourg",
  },
  {
    id: "Europe/Madrid",
    name: "(GMT+01:00) Madrid",
  },
  {
    id: "Europe/Malta",
    name: "(GMT+01:00) Malta",
  },
  {
    id: "Europe/Monaco",
    name: "(GMT+01:00) Monaco",
  },
  {
    id: "Europe/Oslo",
    name: "(GMT+01:00) Oslo",
  },
  {
    id: "Europe/Paris",
    name: "(GMT+01:00) Paris",
  },
  {
    id: "Europe/Prague",
    name: "(GMT+01:00) Central European Time - Prague",
  },
  {
    id: "Europe/Rome",
    name: "(GMT+01:00) Rome",
  },
  {
    id: "Europe/Stockholm",
    name: "(GMT+01:00) Stockholm",
  },
  {
    id: "Europe/Tirane",
    name: "(GMT+01:00) Tirane",
  },
  {
    id: "Europe/Vienna",
    name: "(GMT+01:00) Vienna",
  },
  {
    id: "Europe/Warsaw",
    name: "(GMT+01:00) Warsaw",
  },
  {
    id: "Europe/Zurich",
    name: "(GMT+01:00) Zurich",
  },
  {
    id: "Africa/Cairo",
    name: "(GMT+02:00) Cairo",
  },
  {
    id: "Africa/Johannesburg",
    name: "(GMT+02:00) Johannesburg",
  },
  {
    id: "Africa/Maputo",
    name: "(GMT+02:00) Maputo",
  },
  {
    id: "Africa/Tripoli",
    name: "(GMT+02:00) Tripoli",
  },
  {
    id: "Asia/Amman",
    name: "(GMT+02:00) Amman",
  },
  {
    id: "Asia/Beirut",
    name: "(GMT+02:00) Beirut",
  },
  {
    id: "Asia/Damascus",
    name: "(GMT+02:00) Damascus",
  },
  {
    id: "Asia/Gaza",
    name: "(GMT+02:00) Gaza",
  },
  {
    id: "Asia/Jerusalem",
    name: "(GMT+02:00) Jerusalem",
  },
  {
    id: "Asia/Nicosia",
    name: "(GMT+02:00) Nicosia",
  },
  {
    id: "Europe/Athens",
    name: "(GMT+02:00) Athens",
  },
  {
    id: "Europe/Bucharest",
    name: "(GMT+02:00) Bucharest",
  },
  {
    id: "Europe/Chisinau",
    name: "(GMT+02:00) Chisinau",
  },
  {
    id: "Europe/Helsinki",
    name: "(GMT+02:00) Helsinki",
  },
  {
    id: "Europe/Istanbul",
    name: "(GMT+02:00) Istanbul",
  },
  {
    id: "Europe/Kaliningrad",
    name: "(GMT+02:00) Moscow-01 - Kaliningrad",
  },
  {
    id: "Europe/Kiev",
    name: "(GMT+02:00) Kiev",
  },
  {
    id: "Europe/Riga",
    name: "(GMT+02:00) Riga",
  },
  {
    id: "Europe/Sofia",
    name: "(GMT+02:00) Sofia",
  },
  {
    id: "Europe/Tallinn",
    name: "(GMT+02:00) Tallinn",
  },
  {
    id: "Europe/Vilnius",
    name: "(GMT+02:00) Vilnius",
  },
  {
    id: "Africa/Khartoum",
    name: "(GMT+03:00) Khartoum",
  },
  {
    id: "Africa/Nairobi",
    name: "(GMT+03:00) Nairobi",
  },
  {
    id: "Antarctica/Syowa",
    name: "(GMT+03:00) Syowa",
  },
  {
    id: "Asia/Baghdad",
    name: "(GMT+03:00) Baghdad",
  },
  {
    id: "Asia/Qatar",
    name: "(GMT+03:00) Qatar",
  },
  {
    id: "Asia/Riyadh",
    name: "(GMT+03:00) Riyadh",
  },
  {
    id: "Europe/Minsk",
    name: "(GMT+03:00) Minsk",
  },
  {
    id: "Europe/Moscow",
    name: "(GMT+03:00) Moscow+00 - Moscow",
  },
  {
    id: "Asia/Tehran",
    name: "(GMT+03:30) Tehran",
  },
  {
    id: "Asia/Baku",
    name: "(GMT+04:00) Baku",
  },
  {
    id: "Asia/Dubai",
    name: "(GMT+04:00) Dubai",
  },
  {
    id: "Asia/Tbilisi",
    name: "(GMT+04:00) Tbilisi",
  },
  {
    id: "Asia/Yerevan",
    name: "(GMT+04:00) Yerevan",
  },
  {
    id: "Europe/Samara",
    name: "(GMT+04:00) Moscow+01 - Samara",
  },
  {
    id: "Indian/Mahe",
    name: "(GMT+04:00) Mahe",
  },
  {
    id: "Indian/Mauritius",
    name: "(GMT+04:00) Mauritius",
  },
  {
    id: "Indian/Reunion",
    name: "(GMT+04:00) Reunion",
  },
  {
    id: "Asia/Kabul",
    name: "(GMT+04:30) Kabul",
  },
  {
    id: "Antarctica/Mawson",
    name: "(GMT+05:00) Mawson",
  },
  {
    id: "Asia/Aqtau",
    name: "(GMT+05:00) Aqtau",
  },
  {
    id: "Asia/Aqtobe",
    name: "(GMT+05:00) Aqtobe",
  },
  {
    id: "Asia/Ashgabat",
    name: "(GMT+05:00) Ashgabat",
  },
  {
    id: "Asia/Dushanbe",
    name: "(GMT+05:00) Dushanbe",
  },
  {
    id: "Asia/Karachi",
    name: "(GMT+05:00) Karachi",
  },
  {
    id: "Asia/Tashkent",
    name: "(GMT+05:00) Tashkent",
  },
  {
    id: "Asia/Yekaterinburg",
    name: "(GMT+05:00) Moscow+02 - Yekaterinburg",
  },
  {
    id: "Indian/Kerguelen",
    name: "(GMT+05:00) Kerguelen",
  },
  {
    id: "Indian/Maldives",
    name: "(GMT+05:00) Maldives",
  },
  {
    id: "Asia/Kolkata",
    name: "(GMT+05:30) India Standard Time",
  },
  {
    id: "Asia/Colombo",
    name: "(GMT+05:30) Colombo",
  },
  {
    id: "Asia/Katmandu",
    name: "(GMT+05:45) Katmandu",
  },
  {
    id: "Antarctica/Vostok",
    name: "(GMT+06:00) Vostok",
  },
  {
    id: "Asia/Almaty",
    name: "(GMT+06:00) Almaty",
  },
  {
    id: "Asia/Bishkek",
    name: "(GMT+06:00) Bishkek",
  },
  {
    id: "Asia/Dhaka",
    name: "(GMT+06:00) Dhaka",
  },
  {
    id: "Asia/Omsk",
    name: "(GMT+06:00) Moscow+03 - Omsk, Novosibirsk",
  },
  {
    id: "Asia/Thimphu",
    name: "(GMT+06:00) Thimphu",
  },
  {
    id: "Indian/Chagos",
    name: "(GMT+06:00) Chagos",
  },
  {
    id: "Asia/Rangoon",
    name: "(GMT+06:30) Rangoon",
  },
  {
    id: "Indian/Cocos",
    name: "(GMT+06:30) Cocos",
  },
  {
    id: "Antarctica/Davis",
    name: "(GMT+07:00) Davis",
  },
  {
    id: "Asia/Bangkok",
    name: "(GMT+07:00) Bangkok",
  },
  {
    id: "Asia/Hovd",
    name: "(GMT+07:00) Hovd",
  },
  {
    id: "Asia/Jakarta",
    name: "(GMT+07:00) Jakarta",
  },
  {
    id: "Asia/Krasnoyarsk",
    name: "(GMT+07:00) Moscow+04 - Krasnoyarsk",
  },
  {
    id: "Asia/Saigon",
    name: "(GMT+07:00) Hanoi",
  },
  {
    id: "Indian/Christmas",
    name: "(GMT+07:00) Christmas",
  },
  {
    id: "Antarctica/Casey",
    name: "(GMT+08:00) Casey",
  },
  {
    id: "Asia/Brunei",
    name: "(GMT+08:00) Brunei",
  },
  {
    id: "Asia/Choibalsan",
    name: "(GMT+08:00) Choibalsan",
  },
  {
    id: "Asia/Hong_Kong",
    name: "(GMT+08:00) Hong Kong",
  },
  {
    id: "Asia/Irkutsk",
    name: "(GMT+08:00) Moscow+05 - Irkutsk",
  },
  {
    id: "Asia/Kuala_Lumpur",
    name: "(GMT+08:00) Kuala Lumpur",
  },
  {
    id: "Asia/Macau",
    name: "(GMT+08:00) Macau",
  },
  {
    id: "Asia/Makassar",
    name: "(GMT+08:00) Makassar",
  },
  {
    id: "Asia/Manila",
    name: "(GMT+08:00) Manila",
  },
  {
    id: "Asia/Shanghai",
    name: "(GMT+08:00) China Time - Beijing",
  },
  {
    id: "Asia/Singapore",
    name: "(GMT+08:00) Singapore",
  },
  {
    id: "Asia/Taipei",
    name: "(GMT+08:00) Taipei",
  },
  {
    id: "Asia/Ulaanbaatar",
    name: "(GMT+08:00) Ulaanbaatar",
  },
  {
    id: "Australia/Perth",
    name: "(GMT+08:00) Western Time - Perth",
  },
  {
    id: "Asia/Pyongyang",
    name: "(GMT+08:30) Pyongyang",
  },
  {
    id: "Asia/Dili",
    name: "(GMT+09:00) Dili",
  },
  {
    id: "Asia/Jayapura",
    name: "(GMT+09:00) Jayapura",
  },
  {
    id: "Asia/Seoul",
    name: "(GMT+09:00) Seoul",
  },
  {
    id: "Asia/Tokyo",
    name: "(GMT+09:00) Tokyo",
  },
  {
    id: "Asia/Yakutsk",
    name: "(GMT+09:00) Moscow+06 - Yakutsk",
  },
  {
    id: "Pacific/Palau",
    name: "(GMT+09:00) Palau",
  },
  {
    id: "Australia/Adelaide",
    name: "(GMT+10:30) Central Time - Adelaide",
  },
  {
    id: "Australia/Darwin",
    name: "(GMT+09:30) Central Time - Darwin",
  },
  {
    id: "Antarctica/DumontDUrville",
    name: "(GMT+10:00) Dumont D'Urville",
  },
  {
    id: "Asia/Magadan",
    name: "(GMT+10:00) Moscow+07 - Magadan",
  },
  {
    id: "Asia/Vladivostok",
    name: "(GMT+10:00) Moscow+07 - Yuzhno-Sakhalinsk",
  },
  {
    id: "Australia/Brisbane",
    name: "(GMT+10:00) Eastern Time - Brisbane",
  },
  {
    id: "Australia/Hobart",
    name: "(GMT+11:00) Eastern Time - Hobart",
  },
  {
    id: "Australia/Sydney",
    name: "(GMT+11:00) Eastern Time - Melbourne, Sydney",
  },
  {
    id: "Pacific/Chuuk",
    name: "(GMT+10:00) Truk",
  },
  {
    id: "Pacific/Guam",
    name: "(GMT+10:00) Guam",
  },
  {
    id: "Pacific/Port_Moresby",
    name: "(GMT+10:00) Port Moresby",
  },
  {
    id: "Pacific/Efate",
    name: "(GMT+11:00) Efate",
  },
  {
    id: "Pacific/Guadalcanal",
    name: "(GMT+11:00) Guadalcanal",
  },
  {
    id: "Pacific/Kosrae",
    name: "(GMT+11:00) Kosrae",
  },
  {
    id: "Pacific/Norfolk",
    name: "(GMT+11:00) Norfolk",
  },
  {
    id: "Pacific/Noumea",
    name: "(GMT+11:00) Noumea",
  },
  {
    id: "Pacific/Pohnpei",
    name: "(GMT+11:00) Ponape",
  },
  {
    id: "Asia/Kamchatka",
    name: "(GMT+12:00) Moscow+09 - Petropavlovsk-Kamchatskiy",
  },
  {
    id: "Pacific/Auckland",
    name: "(GMT+13:00) Auckland",
  },
  {
    id: "Pacific/Fiji",
    name: "(GMT+13:00) Fiji",
  },
  {
    id: "Pacific/Funafuti",
    name: "(GMT+12:00) Funafuti",
  },
  {
    id: "Pacific/Kwajalein",
    name: "(GMT+12:00) Kwajalein",
  },
  {
    id: "Pacific/Majuro",
    name: "(GMT+12:00) Majuro",
  },
  {
    id: "Pacific/Nauru",
    name: "(GMT+12:00) Nauru",
  },
  {
    id: "Pacific/Tarawa",
    name: "(GMT+12:00) Tarawa",
  },
  {
    id: "Pacific/Wake",
    name: "(GMT+12:00) Wake",
  },
  {
    id: "Pacific/Wallis",
    name: "(GMT+12:00) Wallis",
  },
  {
    id: "Pacific/Apia",
    name: "(GMT+14:00) Apia",
  },
  {
    id: "Pacific/Enderbury",
    name: "(GMT+13:00) Enderbury",
  },
  {
    id: "Pacific/Fakaofo",
    name: "(GMT+13:00) Fakaofo",
  },
  {
    id: "Pacific/Tongatapu",
    name: "(GMT+13:00) Tongatapu",
  },
  {
    id: "Pacific/Kiritimati",
    name: "(GMT+14:00) Kiritimati",
  },
  {
    id: "Africa/Addis_Ababa",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Africa/Bamako",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Bangui",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Banjul",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Blantyre",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Brazzaville",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Bujumbura",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Asia/Oral",
    name: "(UTC+05:00) Ashgabat, Tashkent",
  },
  {
    id: "Africa/Conakry",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Dakar",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Dar_es_Salaam",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Africa/Douala",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Freetown",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Gaborone",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Harare",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Juba",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Africa/Kampala",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Africa/Kinshasa",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Libreville",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Lome",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Kigali",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Luanda",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Lubumbashi",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Lusaka",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Malabo",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Mbabane",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Mogadishu",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Africa/Maseru",
    name: "(UTC+02:00) Harare, Pretoria",
  },
  {
    id: "Africa/Niamey",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "Africa/Nouakchott",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Ouagadougou",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Africa/Porto-Novo",
    name: "(UTC+01:00) West Central Africa",
  },
  {
    id: "America/Argentina/Salta",
    name: "(UTC-03:00) Buenos Aires",
  },
  {
    id: "America/Argentina/La_Rioja",
    name: "(UTC-03:00) Buenos Aires",
  },
  {
    id: "America/Argentina/Rio_Gallegos",
    name: "(UTC-03:00) Buenos Aires",
  },
  {
    id: "America/Argentina/San_Juan",
    name: "(UTC-03:00) Buenos Aires",
  },
  {
    id: "America/Argentina/San_Luis",
    name: "(UTC-03:00) Buenos Aires",
  },
  {
    id: "America/Argentina/Tucuman",
    name: "(UTC-03:00) Buenos Aires",
  },
  {
    id: "America/Argentina/Ushuaia",
    name: "(UTC-03:00) Buenos Aires",
  },
  {
    id: "America/Bahia_Banderas",
    name: "(UTC-06:00) Guadalajara, Mexico City, Monterrey",
  },
  {
    id: "America/Blanc-Sablon",
    name: "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan",
  },
  {
    id: "America/Boise",
    name: "(UTC-07:00) Mountain Time (US & Canada)",
  },
  {
    id: "America/Cambridge_Bay",
    name: "(UTC-07:00) Mountain Time (US & Canada)",
  },
  {
    id: "America/Cambridge_Bay",
    name: "(UTC-07:00) Mountain Time (US & Canada)",
  },
  {
    id: "America/Chihuahua",
    name: "(UTC-07:00) Chihuahua, La Paz, Mazatlan",
  },
  {
    id: "America/Coral_Harbour",
    name: "(UTC-05:00) Bogota, Lima, Quito",
  },
  {
    id: "America/Creston",
    name: "(UTC-07:00) Arizona",
  },
  {
    id: "America/Detroit",
    name: "(UTC-05:00) Eastern Time (US & Canada)",
  },
  {
    id: "America/Detroit",
    name: "(UTC-05:00) Eastern Time (US & Canada)",
  },
  {
    id: "America/Dominica",
    name: "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan",
  },
  {
    id: "America/Eirunepe",
    name: "(UTC-05:00) Bogota, Lima, Quito",
  },
  {
    id: "America/Glace_Bay",
    name: "(UTC-04:00) Atlantic Time (Canada)",
  },
  {
    id: "America/Goose_Bay",
    name: "(UTC-04:00) Atlantic Time (Canada)",
  },
  {
    id: "America/Indiana/Knox",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/Indiana/Vincennes",
    name: "(UTC-05:00) Eastern Time (US & Canada)",
  },
  {
    id: "America/Indiana/Winamac",
    name: "(UTC-05:00) Eastern Time (US & Canada)",
  },
  {
    id: "America/Inuvik",
    name: "(UTC-07:00) Mountain Time (US & Canada)",
  },
  {
    id: "America/Juneau",
    name: "(UTC-09:00) Alaska",
  },
  {
    id: "America/Kentucky/Monticello",
    name: "(UTC-05:00) Eastern Time (US & Canada)",
  },
  {
    id: "America/Kralendijk",
    name: "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan",
  },
  {
    id: "America/Matamoros",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/Menominee",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/Merida",
    name: "(UTC-06:00) Guadalajara, Mexico City, Monterrey",
  },
  {
    id: "America/Moncton",
    name: "(UTC-04:00) Atlantic Time (Canada)",
  },
  {
    id: "America/Monterrey",
    name: "(UTC-06:00) Guadalajara, Mexico City, Monterrey",
  },
  {
    id: "America/Montreal",
    name: "(UTC-05:00) Eastern Time (US & Canada)",
  },
  {
    id: "America/Nome",
    name: "(UTC-09:00) Alaska",
  },
  {
    id: "America/North_Dakota/Beulah",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/North_Dakota/Center",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/North_Dakota/New_Salem",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/Ojinaga",
    name: "(UTC-07:00) Mountain Time (US & Canada)",
  },
  {
    id: "America/Pangnirtung",
    name: "(UTC-05:00) Eastern Time (US & Canada)",
  },
  {
    id: "America/Rankin_Inlet",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/Rankin_Inlet",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/Resolute",
    name: "(UTC-06:00) Central Time (US & Canada)",
  },
  {
    id: "America/Santa_Isabel",
    name: "(UTC-08:00) Baja California",
  },
  {
    id: "America/Santarem",
    name: "(UTC-03:00) Cayenne, Fortaleza",
  },
  {
    id: "America/Sitka",
    name: "(UTC-09:00) Alaska",
  },
  {
    id: "America/St_Lucia",
    name: "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan",
  },
  {
    id: "America/St_Vincent",
    name: "(UTC-04:00) Georgetown, La Paz, Manaus, San Juan",
  },
  {
    id: "Arctic/Longyearbyen",
    name: "(UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna",
  },
  {
    id: "Asia/Aden",
    name: "(UTC+03:00) Kuwait, Riyadh",
  },
  {
    id: "Asia/Anadyr",
    name: "(UTC+12:00) Magadan",
  },
  {
    id: "Asia/Kathmandu",
    name: "(UTC+05:45) Kathmandu",
  },
  {
    id: "Asia/Kuching",
    name: "(UTC+08:00) Kuala Lumpur, Singapore",
  },
  {
    id: "Asia/Muscat",
    name: "(UTC+04:00) Abu Dhabi, Muscat",
  },
  {
    id: "Asia/Novokuznetsk",
    name: "(UTC+07:00) Novosibirsk",
  },
  {
    id: "Asia/Novosibirsk",
    name: "(UTC+07:00) Novosibirsk",
  },
  {
    id: "Asia/Phnom_Penh",
    name: "(UTC+07:00) Bangkok, Hanoi, Jakarta",
  },
  {
    id: "Asia/Pontianak",
    name: "(UTC+07:00) Bangkok, Hanoi, Jakarta",
  },
  {
    id: "Asia/Qyzylorda",
    name: "(UTC+06:00) Nur-Sultan (Astana)",
  },
  {
    id: "Asia/Sakhalin",
    name: "(UTC+11:00) Vladivostok",
  },
  {
    id: "Asia/Samarkand",
    name: "(UTC+05:00) Ashgabat, Tashkent",
  },
  {
    id: "Asia/Urumqi",
    name: "(UTC+06:00) Nur-Sultan (Astana)",
  },
  {
    id: "Asia/Vientiane",
    name: "(UTC+07:00) Bangkok, Hanoi, Jakarta",
  },
  {
    id: "Atlantic/Madeira",
    name: "(UTC) Dublin, Lisbon",
  },
  {
    id: "Atlantic/St_Helena",
    name: "(UTC) Monrovia, Reykjavik",
  },
  {
    id: "Australia/Broken_Hill",
    name: "(UTC+09:30) Adelaide",
  },
  {
    id: "Australia/Melbourne",
    name: "(UTC+10:00) Canberra, Melbourne, Sydney",
  },
  {
    id: "Europe/Bratislava",
    name: "(UTC+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague",
  },
  {
    id: "Europe/Isle_of_Man",
    name: "(UTC) Edinburgh, London",
  },
  {
    id: "Europe/Isle_of_Man",
    name: "(UTC+01:00) Edinburgh, London",
  },
  {
    id: "Europe/Ljubljana",
    name: "(UTC+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague",
  },
  {
    id: "Europe/Podgorica",
    name: "(UTC+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague",
  },
  {
    id: "Europe/Sarajevo",
    name: "(UTC+01:00) Sarajevo, Skopje, Warsaw, Zagreb",
  },
  {
    id: "Europe/Simferopol",
    name: "(UTC+03:00) Moscow, St. Petersburg, Volgograd, Minsk",
  },
  {
    id: "Europe/Skopje",
    name: "(UTC+01:00) Sarajevo, Skopje, Warsaw, Zagreb",
  },
  {
    id: "Europe/Uzhgorod",
    name: "(UTC+02:00) E. Europe",
  },
  {
    id: "Europe/Uzhgorod",
    name: "(UTC+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius",
  },
  {
    id: "Europe/Vaduz",
    name: "(UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna",
  },
  {
    id: "Europe/Volgograd",
    name: "(UTC+03:00) Moscow, St. Petersburg, Volgograd, Minsk",
  },
  {
    id: "Europe/Zagreb",
    name: "(UTC+01:00) Sarajevo, Skopje, Warsaw, Zagreb",
  },
  {
    id: "Europe/Zaporozhye",
    name: "(UTC+02:00) E. Europe",
  },
  {
    id: "Europe/Zaporozhye",
    name: "(UTC+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius",
  },
  {
    id: "Indian/Antananarivo",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Indian/Comoro",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Indian/Mayotte",
    name: "(UTC+03:00) Nairobi",
  },
  {
    id: "Pacific/Johnston",
    name: "(UTC-10:00) Hawaii",
  },
  {
    id: "Pacific/Midway",
    name: "(UTC-11:00) Coordinated Universal Time-11",
  },
  {
    id: "Pacific/Saipan",
    name: "(UTC+10:00) Guam, Port Moresby",
  },
];

export const getToken = () => {
  return document.cookie
    .split(";")
    .map((cookie) => cookie.split("="))
    .reduce(
      (accumulator, [key, value]) => ({
        ...accumulator,
        [key.trim()]: decodeURIComponent(value),
      }),
      {}
    );
};

export function decryptCodes(content: any, passcode: any) {
  const result: any = [];
  let str = "";

  const codesArr = JSON.parse(content);
  const passLen = passcode.length;

  for (let i = 0; i < codesArr.length; i++) {
    const passOffset = i % passLen;

    const calAscii: any = codesArr[i] - passcode.charCodeAt(passOffset);

    result.push(calAscii);
  }

  for (let j = 0; j < result.length; j++) {
    const ch = String.fromCharCode(result[j]);
    str += ch;
  }

  return str;
}

export const SignOut = () => {
  const navigate = useNavigate();
  Cookie.clear();
  navigate("/login");
  ClearStorage();

  // localStorage.clear();
  // sessionStorage.clear();
};


export function groupbyday(data: any) {
  if (!data) return [];

  const pinnedArr = {};
  const groups: any = data.reduce((acc: any, game: any) => {

    if (game?.is_pin === 1) {
      if (!pinnedArr['Pinned']) pinnedArr['Pinned'] = [];
      pinnedArr['Pinned'].push(game);

    } else {
      let date: any = get_recent_date(game.updatedAt);

      if (!acc[date]) acc[date] = [];
      acc[date].push(game);
    }
    return acc;
  }, {});

  const groupArrays: any = Object.keys(groups).map((date: any) => {
    return {
      [date]: groups[date]?.sort((a: any, b: any) => new Date(b.updatedAt) - new Date(a.updatedAt))
    };
  });

  if (pinnedArr['Pinned']) {
    groupArrays.unshift(pinnedArr);
  }
  return groupArrays;
}

function get_recent_date(stamp: any) {
  try {
    const d = new Date(stamp);
    const now = new Date();

    const isToday = d.toDateString() === now.toDateString();

    const yesterday = new Date();
    yesterday.setDate(now.getDate() - 1);

    const isYesterday = d.toDateString() === yesterday.toDateString();

    if (isToday) return "Today";
    if (isYesterday) return "Yesterday";

    const date = d.getDate().toString().padStart(2, "0");
    const month = (d.getMonth() + 1).toString().padStart(2, "0");
    const year = d.getFullYear();

    return `${date}-${month}-${year}`;

  } catch {
    return "";
  }
}

export const getParticularAgentuser = (arrayList: any, ext: any) => {
  return _.filter(arrayList, (item: any) => {
    return item.ext === ext;
  });
};

export const getCallTimeFromTimestamp = (time: any) => {
  const day: any = new Date().toLocaleString("en-GB", { timeZone: 'UTC' });
  const day2: any = new Date(time * 1000).toLocaleString("en-GB", { timeZone: 'UTC' });
  const diff: any = new Date(day).getTime() - new Date(day2).getTime();
  if (diff < 658066000) {
    const day3: any = new Date(Math.abs(Math.abs(diff) - 152000)).toLocaleString("en-GB", { timeZone: 'UTC' }); // 02:27 => 147000
    return day3.slice(-8);
  } else {
    return "00:00:00";
  }
}

export function isJSON(str: any): boolean {
  if (str === null || str === undefined || typeof str !== "string") return false;
  const value = str.trim();
  if (!value) return false;
  const first = value[0];
  const last = value[value.length - 1];
  const validPairs: Record<string, string> = { "{": "}", "[": "]" };
  if (validPairs[first] !== last) return false;
  try {
    JSON.parse(value);
    return true;
  } catch {
    return false;
  }
}

export const getdateFormat = (date: any) => {
  let dateRes: any = "";
  dateRes = moment(date).format('hh:mm A')
  return dateRes;
}

export const sleep = async (ms: any) => {
  return new Promise(
    resolve => setTimeout(resolve, ms)
  )
}

export const getAuthorizationUrl = (connectorsName: any) => {

  const ccaasDomain = window?.location?.origin;
  const endDomain = ccaasDomain?.split('.')?.pop();

  let authorizationUrl = '';
  switch (connectorsName.toLowerCase()) {
    case 'outlook':
    case 'google':
    case 'calendly':
    case 'googlesheet':

      authorizationUrl = `${config().CONNECTORS_API_BASE_URL}/calendar/v1/${connectorsName.toLowerCase()}/login?domain=${endDomain}`;
      SetItemInStorage('selectedConName', connectorsName);

      break;

    default:

      authorizationUrl = `${config().CONNECTORS_API_BASE_URL}/${connectorsName.toLowerCase()}/v1/login?domain=${endDomain}`;
      connectorsName = connectorsName.replace('/', '_');
      SetItemInStorage('selectedConName', connectorsName);

      break;
  }

  const windowFeatures = "left=350,top=200,width=1300,height=700,toolbar=0,location=0,status=0,menubar=0,scrollbars=yes,resizable=yes,"
  window.open(
    authorizationUrl,
    "ccaaswindow",
    windowFeatures,
  );

}


export const getJSON = function (url: any, callback: any) {
  const xhr = new XMLHttpRequest();
  xhr.open('GET', url, true);
  xhr.responseType = 'json';
  xhr.onload = function () {
    const status = xhr.status;
    if (status === 200) {
      callback(null, xhr.response);
    } else {
      callback(status, xhr.response);
    }
  };
  xhr.send();
};

export const imageFormat = ["PNG", "jpeg", "jpg", "svg", "gif", "webp", 'png', "jfif", "ico", "tiff", "jfif", "nef", "orf", "dib", "eps", "raw", "heif", "apng", "avif", "jif", "tif", "xbm", "heic", "arw", "image/png",
  "image/jpeg", "image/jpg", "image/gif", "image/jfif", "image/heif", "application/octet-stream", "image/svg", "image/webp", "image/gif", "image/svg+xml", "image", "image/avif"
];
export const zipFormats = ["zip", "arj", "cab", "cfs", "application/x-zip-compressed", "cpt", "dar", "dmg", "war", "kgb", "txz", "dd", "ear", "wim", "jar", "rar", "yz1", "application/x-gzip", "application/zip", "application/octet-stream", "application/x-zip-compressed",
  "application/x-zip-compressed", "multipart/x-zip", "application/x-rar-compressed", "application/octet-stream", "application/x-gzip"
];
export const mp3Formats = ["mp3", "mpga", "wma", "ogg", "aif", "aifc", "aiff", "m4a", "aac", "audio/aac", "audio/wav", "audio/wave", "wav", "audio/mpeg", "audio/mp3", "audio"];
export const allFormats = ["xls", "xlsx", "xla", "xlt", "xml", "pptm", "xlw", "xltm", "pptx", "xlam", "xll", "aac", "csv", "zip", "arj", "cab", "cfs", "cpt", "dar", "dmg", "war", "kgb", "txz", "dd", "ear", "wim", "jar", "rar", "yz1", "ppt", "potx", "potm", "ppam", "ppsx", "sldx", "m4a", "ppsm", "sldm", "pdf", "docx", "dot", "doc", "dotm", "txt", "wpd", "odt", "wbk", "docm", "dotx", "docb", "rtf", "prn", "dif", "slk", "wav", "mp3", "mpga", "wma", "ogg", "aif", "aifc", "aiff", "htm", "html", "audio/mp3"];

export const pdfFormats = ["pdf", "application/pdf", "file", "document/pdf", "link"];
export const imageFormatType = ["image/png", "image/jpeg", "image/jpg", "image/jfif", "image/svg", "image/webp", "image/gif", "image/svg+xml"];

export const videoFormat = ["mp4", "video/mp4", "video/webm", "video/quicktime", "mov", "3gp", "video/3gpp", "video/x-ms-wmv", "video/x-msvideo", "video/x-flv", "ogg", "ogx", "wmv", "webm", "flv", "avi", "video"];

export const excelFormats = ["xla", "xlam", "xls", "xlsb", "xlsm", "application/vnd.ms-excel",
  "xlsx", "application/vnd.google-apps.spreadsheet", "xlt",
  "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "xltm", "xltx", "xlw", "csv", "application/vnd.ms-excel", "xll",
  'XLSX', "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "text/csv"
];
export const pptFormats = ["ppt", "pptx", "pptm", "potx", "potm", "ppam", "ppsx", "ppsm", "sldx", "sldm ", 'application/vnd.ms-powerpoint',
  'application/vnd.openxmlformats-officedocument.presentationml.presentation',
  'application/vnd.openxmlformats-officedocument.presentationml.template',
  'application/vnd.openxmlformats-officedocument.presentationml.slideshow',
  'application/vnd.ms-powerpoint.addin.macroEnabled.12',
  'application/vnd.ms-powerpoint.presentation.macroEnabled.12',
  'application/vnd.ms-powerpoint.template.macroEnabled.12',
  'application/vnd.ms-powerpoint.slideshow.macroEnabled.12'
];
export const wordFormats = ["docx", "dot", "doc", "text/plain",
  "dotm", "txt", "wpd", "odt", "wbk", "docm", "dotx", "docb", "rtf", "prn", "dif", "slk", "application/msword",
  "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
  "application/vnd.openxmlformats-officedocument.wordprocessingml.template",
  "application/vnd.ms-word.document.macroEnabled.12",
  "application/vnd.ms-word.template.macroEnabled.12"
];

export const detectCurrentZone_Time = () => {
  try {
    const userTimeZone = Intl.DateTimeFormat().resolvedOptions().timeZone;
    const findgetTime = countryTimezones.find((i: any) => i.label.includes(userTimeZone))
    const getTime = findgetTime?.label?.split('GMT ')[1]
    return getTime || null
  } catch (error) {
    console.log("error", error)
    return null
  }
}


export const aiGrouping = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      axios.post(`${config().NLP_DNS_URL}/merge_ticket`, data, {
        headers: {
          'Content-Type': 'application/json'
        }
      }).then((response) => {
        resolve(response.data)
      }).catch((error) => {
        reject(error);
        console.log(error.response);
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  }
  )
}


export const IndianPhPattern = /^(\+\d{12}|\d{10})$/;
export const GlobelPhPattern = /^\d{8,15}$/;
export const FBIdPattern = /^[a-zA-Z0-9._]{5,50}$/;

export const UKPostalCode = /^[A-Za-z]{1,2}\d[A-Za-z\d]?\s?\d[A-Za-z]{2}$/;
export const IndianPostalCode = /^\d{6}$/;



export const indianPhoneCheck = (value: any) => {
  try {
    if (window.location.origin === 'https://ccaas.worktual.in') {
      return IndianPhPattern.test(value)
    }
    return GlobelPhPattern.test(value)

  } catch (error) {
    console.log(error)
  }
}



const restrictedModules: any = {
  13490: ['Disposition/regenerate'],
  7179: ['Disposition/regenerate'],
  19726: [
    'Menubar/Ticket',
    'KeyboardShortcuts',
    'Contact/profile',
    'Contact/Create',
    'Leaderboard/admin',
    'CompareDashboard',
    'Dashboard/admin',
    'Dashboard/Supervisor',
    'Dashboard/agent',
    'Reports/AgentAnalytics',
    'Reports/QueueAnalytics',
    'settings/chatlimit',
    'settings/email',
    'settings/sms',
    'settings/whatsapp',
    'settings/facebook',
    'settings/instagram',
    'settings/customerSatisfaction',
    'settings/leadIntegration',
    'settings/conversationTrigger',
    'settings/connectors',
    'settings/apiconfiguration',
    'settings/privateapp',
    'settings/publicapp',
    'settings/paymentmethods',
    'settings/usermanagement/chatlimit',
    'settings/usermanagement/channels',
    'settings/contactsettings/socialmedia',
    'Reports/aibotAnalytics',
    'Reports/aibotReports',
    'Reports/visitorInsights',
    'allActivity/viewinteraction',
    'Disposition/createTicket',
    'myactivity/ticketId',
    'allActivity/channelFilter',
    'assignActivity/channelFilter',
    'contact/smsemail',
    'Reports/channelReports',
    'settings/voice/createcallconfig',
    'scheduleReport',
    'costpercall',
    'speedometer',
    'Disposition/regenerate',
    'dashboard/SentimentalAnalysis',
    'dashboard/agentAssist',
    'myactivity/TransferredTo',
    'allactivity/viewinteration/transcription',
    'settings/csat',
    'allactivity/viewinteration/channelfilter',
    'allactivity/Sentimentscorefilter',
    'Dashboard/Supervisor/SentimentalAnalysis',
    'Dashboard/Supervisor/agentassist',
    'dashboard/agentandbot',
    'allActivity/allviewrefresh',
    'recordings/sentimentalScore',
  ],   // Company : Calvary Temple

  19465: [
    'Menubar/Ticket',
    'allActivity/viewinteraction',
    'Disposition/createTicket',
    'myactivity/ticketId'
  ],   // Company : Ogrelogic Solutions LLC

  18878: [
    'Menubar/Ticket',
    'allActivity/viewinteraction',
    'Disposition/createTicket',
    'myactivity/ticketId'
  ],   // Company : Cinimini By Theatre Concepts

}

export const restrictedDomainId = (names: any) => {
  try {
    const LoginUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const domainId: any = LoginUserDetails?.domainId;
    if (!domainId || !restrictedModules[domainId]) {
      return true
    }
    return !restrictedModules[domainId].includes(names)
  } catch (error: any) {
    console.log("error", error);
    return true
  }
}

export const CONTACT_CENTRE_PARENT_ID = {
  "STARTER": 32,
  "GROWTH": 141,
  "PRO": 33,
  "ENTERPRISE": 42
}

const restrictedModules_2: any = {
  // starter
  32: [
    "Contact/integration",
    "Settings/dispositionstatus",
    "Settings/agentstatus",
    "Settings/customertag",
    "Settings/customertype",
    "Settings/callblast",
    "settings/customerSatisfaction",
    "settings/leadIntegration",
    "settings/conversationTrigger",
    "settings/connectors",
    "settings/apiconfiguration",
    "settings/privateapp",
    "settings/publicapp",
    "settings/paymentmethods",
    "settings/skillmanagement",
    "settings/webchatwidget",
    "settings/sms",
    "settings/facebook",
    "settings/whatsapp",
    "settings/instagram",
    "settings/chatlimit",
    "workspace/TransferredTo",
    "callTranscript",
    "workspace/agentAssist",
    'Reports/channelReports',
    "channelReport/Transfercall",
    "Analytics/agentanalyticsmain",
    "Analytics/queueanalytics",
    "Reports/aibotAnalytics",
    "Reports/aibotReports",
    "Reports/visitorInsights",
    "Reports/outboundcampaignreports",
    "Chatflow",
    "channelReport/ACWtime",
    "channelReport/transferredcalls",
    "agentReport/CSATscore",
    "agentreport/feedaback",
    "KnowledgeBase",
    "internalChat/lastintactions",
    "speedometer",
    "workspace/notes",
    "allactivity/summary",
    // "Contact/quickActions",
    "comparedashboardfilter/Voice",
    "livedashboard/channelfilter",
    "agentDashboard/channelfilter",
    "summary/recommendation",
    "liveDashboard/session",
    "livedashboard/sentimental&responseRating",
    "livedashboard/wevisitor",
    "CompareDash/Dropdown",
    "aiChat",
    "voiceFlow/makeApiCall",
    "voiceFlow/checkCondition",
    "voiceFlow/voiceAuthentication",
    'Disposition/regenerate',
    'workspace/emotion',
    'smsactivity/autoassign',
    'routingprofile/concurrentchat',
    'usermanagement/channels',
    'usermanagement/ChatLimit',
    'usermanagement/outboundcampaign',
    'channelreports/voice/Transferred',
    'channelreports/voice/Transfer',
    'channelreports/sms/transferredon',
    'agentreport/loginlogut/CustomTime',
    'agentemailreport/totalTransferred',
    'allactivity/viewinteration/transcription',
    "Dashboard/dynamicDash",
    "previousChatSummary",
    "voicechannel/virtualAgent",
    "ImageUpload"
  ],

  // Growth
  141: [
    'Menubar/Ticket',
    'KeyboardShortcuts',
    'Contact/profile',
    'Contact/Create',
    'Leaderboard/admin',
    'CompareDashboard',
    'Dashboard/admin',
    'Dashboard/Supervisor',
    'Dashboard/agent',
    'Reports/AgentAnalytics',
    'Reports/QueueAnalytics',
    'settings/chatlimit',
    // 'settings/email',
    // 'settings/sms',
    'settings/whatsapp',
    'settings/facebook',
    'settings/instagram',
    'settings/customerSatisfaction',
    'settings/leadIntegration',
    'settings/conversationTrigger',
    'settings/connectors',
    'settings/apiconfiguration',
    'settings/privateapp',
    'settings/publicapp',
    'settings/paymentmethods',
    'settings/usermanagement/chatlimit',
    'settings/usermanagement/channels',
    'settings/contactsettings/socialmedia',
    'Reports/aibotAnalytics',
    'Reports/aibotReports',
    'Reports/visitorInsights',
    'allActivity/viewinteraction',
    'Disposition/createTicket',
    'myactivity/ticketId',
    'allActivity/channelFilter',
    'assignActivity/channelFilter',
    'contact/sms',
    // 'contact/email',
    'Reports/channelReports',
    'settings/voice/createcallconfig',
    'scheduleReport',
    'costpercall',
    'speedometer',
    'Disposition/regenerate',
    'dashboard/SentimentalAnalysis',
    'dashboard/agentAssist',
    'myactivity/TransferredTo',
    'allactivity/viewinteration/transcription',
    'settings/csat',
    'allactivity/viewinteration/channelfilter',
    'allactivity/Sentimentscorefilter',
    'Dashboard/Supervisor/SentimentalAnalysis',
    'Dashboard/Supervisor/agentassist',
    'dashboard/agentandbot',
    'allActivity/allviewrefresh',
    'recordings/sentimentalScore',
    'callTranscript',
    'workspace/agentAssist',
    'livedashboard/channelfilter',
    // 'agentdashboard/channelfilter',
    'summary/recommendation',
    'Contact/integration',
    "Settings/dispositionstatus",
    "Settings/agentstatus",
    "Settings/customertag",
    "Settings/customertype",
    "Settings/callblast",
    "settings/skillmanagement",
    "settings/webchatwidget",
    "Analytics/agentanalyticsmain",
    "Analytics/queueanalytics",
    "Reports/outboundcampaignreports",
    "Chatflow",
    "channelReport/ACWtime",
    "agentReport/CSATscore",
    "agentreport/feedaback",
    "KnowledgeBase",
    "internalChat/lastintactions",
    "workspace/notes",
    "agentDashboard/channelfilter",
    "liveDashboard/session",
    "livedashboard/sentimental&responseRating",
    "livedashboard/wevisitor",
    "CompareDash/Dropdown",
    "aiChat",
    "voiceFlow/makeApiCall",
    "voiceFlow/checkCondition",
    "voiceFlow/voiceAuthentication",
    'smsactivity/autoassign',
    'usermanagement/channels',
    'usermanagement/ChatLimit',
    'usermanagement/outboundcampaign',
    'dashboard/comparedashboard',
    'workspace/emotion',
    "Dashboard/dynamicDash",
    "previousChatSummary",
    "voicechannel/virtualAgent",
    "ImageUpload"
  ],

  // Pro
  33: [
    // "Reports/aibotAnalytics",
    // "Reports/aibotReports",
    // "Knowledgebase/scrapping",
    // "Knowledgebase/qa",
    // "aiChat",
    // "voiceFlow/checkCondition",
    // "voiceFlow/makeApiCall",
    // "voiceFlow/voiceAuthentication",
    // "chatflow/carousel",
    // "chatflow/datePicker",
    // "chatflow/addFlow",
    // "chatflow/integration",
    // "chatflow/Payment",
    // "chatflow/checkCondition",
    // "chatflow/coupon",
    // "chatflow/randomise",
    //  "voicechannel/autoattendent",
    // "voicechannel/multilingualsupport",
    // "chatwidget/personalization&primaryform",
    // "dispositionstatus/create",
    // "agentStatus/create",
    // "agentStatus/customerTag",
    // "dispositionstatus/customerType",
    // "widgeticon/personalization",
    // "fontstyle/personalization",
    // "agentmessagecolour/personalization",
    // "actionmessagecolour/personalization",
    // "chatbackground/personalization",
    // "advancedsettings/uploaddocuments",
    // "advancedsettings/widgetlanguage",
    // "advancedsettings/emailtranscript",
    // 'usermanagement/outboundcampaign',\

    // -- New --

    // "workflow",
    'routingprofile/concurrentchat',
    "ImageUpload"

  ],
  //enterprise
  42: [
    "workflow",
    "KnowledgeBase/Hide",
    'routingprofile/concurrentchat',
    'voicechannel/virtualAgent',  // ====  virtual agent will show if servicePlan.Type_of_plan - condition directly writen in that flow
    'settings/VoiceConfiguration/callflow/Hide',
    "ImageUpload"
  ]
}

/**
 * 
 * Calvery - 19726;
 * POC - 19331
 * Calvary accounts team - 20441
 *  --------   USECASE : Ristricted & Allowded by Domain id   ---------
 * Calvery and POC have access on Growth plan but some modules will be diff from growth and clavary domain.
 * to avoid such issue, we will create a list of routes and modules that can be accessed by both Calvery and POC based on Domain Id.
 * 
*/

export const RistictedByDomain = [19726, 19331, 20441]
const RistictedDomain_Routes = [
  "callTranscript",
]

const AllowdedByDomain = [19726, 19331, 20441]
const AllowModules_domain = [
  "Reports/outboundcampaignreports",
  "Analytics/queueanalytics",
  "settings/webchatwidget",
  "Chatflow",
  "Analytics/agentanalyticsmain",
  "liveDashboard/session",
  "livedashboard/wevisitor",
  "Settings/callblast",
  'usermanagement/outboundcampaign'
]

export const ruyabank = {
  domainId: 20342,

  allowedNewModules: [
    "workflow",
    'settings/VoiceConfiguration/callflow/Hide'
  ],
  restrictedNewModules: [
    "Chatflow"
  ]
}

export const blox = {
  domainId: 19879,

  allowedNewModules: [
    "workflow",
    'settings/VoiceConfiguration/callflow/Hide'
  ],
  restrictedNewModules: [
    "Chatflow"
  ]
}
export const NickyCafe = {
  domainId: 20630,
  allowedNewModules: [
    "ImageUpload"
  ],
  prd_name: 'Lola',
  storage_conent: 'The chatbot includes up to 1 GB of storage. If you need additional storage, please contact our sales team for assistance.',
  agentAssist_content: 'Ask or instruct your assistant to take cake orders, generate payment links, and update customer profiles instantly'
}

// const AllowedProDomain = [19331]
// const AllowedPro = [
//    "Reports/aibotAnalytics",
//    "Reports/aibotReports",
// ]

const CHAT_BOX_RISTICT = [
  "Dashboard/dynamicDash"
]

export const hassAccess = (names: any) => {
  try {
    const tokenData: any = Cookies.get();
    const Authorization: any = tokenData['ssoToken'] ?? "";

    if (Authorization) {
      const LoginUserDetails: any = jwtDecode(Authorization);
      const parent_id = Number(LoginUserDetails?.parent_id);
      const product_id = Number(LoginUserDetails?.product_id);

      if (product_id === 15 && CHAT_BOX_RISTICT.includes(names)) {
        return false
      }

      if (RistictedByDomain.includes(LoginUserDetails.domainId) && RistictedDomain_Routes.includes(names)) {
        return false
      }
      if (AllowdedByDomain.includes(LoginUserDetails.domainId) && AllowModules_domain.includes(names)) {
        return true
      }

      if (ruyabank.domainId == LoginUserDetails.domainId) {
        if (ruyabank.allowedNewModules.includes(names)) {
          return true
        } else if (ruyabank.restrictedNewModules.includes(names)) {
          return false
        }
      }

      if (NickyCafe.domainId == LoginUserDetails.domainId) {
        if (NickyCafe.allowedNewModules.includes(names)) {
          return true
        }
      }

      if (blox.domainId == LoginUserDetails.domainId) {
        if (blox.allowedNewModules.includes(names)) {
          return true
        } else if (blox.restrictedNewModules.includes(names)) {
          return false
        }
      }
      if (!parent_id || !restrictedModules_2[parent_id]) {
        return true
      }
      return !restrictedModules_2[parent_id].includes(names)
    } else {
      return true
    }
  } catch (error: any) {
    console.log("error", error)
    return true
  }
}

export const navigateToTicketing = () => {
  try {
    const host = window.location.host;
    let countryCode = '';
    const parts = host.split('.');
    if (parts.length > 2) {
      countryCode = parts[parts.length - 1]
    }
    if (import.meta.env.VITE_APP_ENV === 'production') {
      switch (countryCode) {
        case 'uk': {
          window.open(
            "https://myaccount.worktual.co.uk/purchasemain",
            "_blank"
          );
          break;
        }
        case 'in': {
          window.open(
            "https://myaccount.worktual.in/purchasemain",
            "_blank"
          );
          break;
        }
        case 'com': {
          window.open(
            "https://myaccount.worktual.com/purchasemain",
            "_blank"
          );
          break;
        }
      }
    } else {
      window.open(
        "https://qa-myaccount.worktual.co.uk/purchasemain",
        "_blank"
      );
    }
  } catch (error) {
    console.log(error)
  }
}

export const formatDateTime = (timestamp: string) => {
  if (!timestamp) return '';
  const now = new Date();
  const notificationDate = new Date(timestamp);

  const diffInDays = differenceInDays(now, notificationDate);

  if (diffInDays === 0) {
    return 'Today';
  } else if (diffInDays === 1) {
    return 'Yesterday';
  } else if (diffInDays < 7) {
    return format(notificationDate, 'EEEE');
  } else {
    return format(notificationDate, 'MMMM d, yyyy');
  }
};



export const cleanHtml = (html: any) => {
  try {
    const parser = new DOMParser();
    const doc = parser.parseFromString(html, 'text/html');
    doc.querySelectorAll('span.ql-cursor').forEach(el => el.remove());
    return doc.body.innerHTML;

  } catch (error) {
    console.log(error)
    return ''
  }
}


export const getFileSize = (size: number) => {
  try {
    if (!size)
      return size
    if (size >= 1024 * 1024) {
      const sizeInMB = (size / (1024 * 1024)).toFixed(1);
      return `${sizeInMB} MB`;
    } else if (size >= 1024) {
      const sizeInKB = (size / 1024).toFixed(1);
      return `${sizeInKB} KB`;
    } else {
      return `${size} bytes`;
    }
  } catch (error) {
    console.error('getFileSize error::', error)
  }
}

export const getSummaryConvo = (data: any) => {
  try {
    const headers = {
      'Content-Type': 'application/json'
    };

    return axios
      .post(`${config().AGENT_SUMMARIZE_CONVO}`, data, { headers })
      .then(res => res?.data)
      .catch(() => ({ data: '' }));
  } catch (error) {
    console.log('error', error);
  }
};


export const stripHtmlTags = (html: string) => {
  return html.replace(/<[^>]*>/g, '').replace(/\n/g, ' ').trim();
};

export async function canLoadImageFast(url: string): Promise<boolean> {
  try {
    const response: any = await fetch(url, { method: "HEAD" });
    const contentType = response.headers.get("content-type");
    return response.ok && contentType?.startsWith("image/");
  } catch {
    return false;
  }
}


export const pothysDomain = '20556'


export function formatTimeFromSec(seconds: any) {
  const minutes = Math.floor(seconds / 60);
  const remainingSeconds = seconds % 60;
  const formattedMinutes = String(minutes).padStart(2, "0");
  const formattedSeconds = String(remainingSeconds).padStart(2, "0");
  return `${formattedMinutes}:${formattedSeconds}`;
}

export const getTimestampDiff = (timestamp: any) => {
  const diffDuration = moment.duration(timestamp);
  const hr = diffDuration.hours();
  const minutes = diffDuration.minutes();
  const seconds = diffDuration.seconds();

  let result = '';
  if (hr > 0) result += `${hr}hr`;
  result += `${minutes}m${seconds}s`;
  return result;
};

export function formatDurationTime(time: any) {
  const seconds = Math.floor(time / 1000) % 60;
  const minutes = Math.floor(time / 1000 / 60) % 60;
  const hours = Math.floor(time / 1000 / 60 / 60) % 24;
  return `${(hours ? hours.toString().padStart(2, "0") + 'h:' : '') + (minutes ? minutes.toString().padStart(2, "0") + 'm:' : '') + seconds.toString().padStart(2, "0") + 's'}`;
}

export function formatElapsedTime(time: any) {
  const seconds = Math.floor(time / 1000) % 60;
  const minutes = Math.floor(time / 1000 / 60) % 60;
  const hours = Math.floor(time / 1000 / 60 / 60) % 24;
  return `${hours ? hours.toString().padStart(2, "0") + ':' : ''}${minutes.toString().padStart(2, "0")}:${seconds.toString().padStart(2, "0")}`;
}


export function getTimeAgo(inputDate: any) {
  try {
    if (!inputDate) return '-'
    const date = new Date(inputDate);
    const now = new Date();
    const diff = (now.getTime() - date.getTime()) / 1000; // seconds

    if (diff < 60) return "Just now";

    const minutes = diff / 60;
    if (minutes < 60) return `${Math.floor(minutes)} min ago`;

    const hours = minutes / 60;
    if (hours < 24) return `${Math.floor(hours)} hrs ago`;

    const days = hours / 24;
    if (days < 30) return `${Math.floor(days)} days ago`;

    const months = days / 30;
    if (months < 12) return `${Math.floor(months)} months ago`;

    const years = months / 12;
    return `${Math.floor(years)} years ago`;
  } catch (error: any) {
    console.log("err", error);
    return '-'
  }
}

export const restrictModules: any = {
  5: [
    'numbersetup',
    'audioconfigure',
    'businessholiday',
    'queuemanagement',
    'voicecalls',
    'webchat',
    'hubemail',
    'hubsms',
    'socialmediachannel',
  ],
  6: [
    "recordings",
    "campaignhistory",
    "campaignreportsview",
    "contacts",
    "contactdetails",
    "callback",
    "interactionlog",
    "viewinteractionlog",
    "interactionchatscreen",
    "callbackdetails"
  ],
};

export const isRestrictedRoute = (roleId?: number, pathname: string = ''): boolean => {
  if (!roleId) return false;
  return restrictModules[roleId]?.includes(pathname) ?? false;
};

export function encryptCodes(content: any, passcode: any) {
  let result: any = [];
  const passLen = passcode.length;

  for (let i = 0; i < content.length; i++) {
    const passOffset = i % passLen;

    const calAscii = content.charCodeAt(i) + passcode.charCodeAt(passOffset);

    result.push(calAscii);
  }

  return JSON.stringify(result);
}

export const formatDate = (date: string) =>
  new Date(date).toLocaleDateString("en-US", {
    month: "short",
    day: "numeric",
    year: "numeric",
  });

export const findIndexWithGuid = (callDetails: any, guid: any) => {
  let index = _.findIndex(callDetails, (e: any) => e.guid === guid);
  return index;
};

export const TransferCallTimer = (state: any, index: any) => {
  let ltime: any = state.callDetails[index]?.TransferCallDuration ? state.callDetails[index]?.TransferCallDuration : "00:00:00";

  let ss: any = ltime.split(":");
  let dt = new Date();
  dt.setHours(ss[0]);
  dt.setMinutes(ss[1]);
  dt.setSeconds(ss[2]);
  let dt2 = new Date(dt.valueOf() + 1000);
  let temp = dt2.toTimeString().split(" ");
  let ts = temp[0].split(":");
  let timer: any;
  timer = ts[0] + ":" + ts[1] + ":" + ts[2];

  state.callDetails[index].TransferCallDuration = timer;
};

export const CallTimer = (state: any, index: any) => {
  let ltime: any = state.callDetails[0]?.callduration ? state.callDetails[0]?.callduration : "00:00:00";

  let ss: any = ltime.split(":");
  let dt = new Date();
  dt.setHours(ss[0]);
  dt.setMinutes(ss[1]);
  dt.setSeconds(ss[2]);
  let dt2 = new Date(dt.valueOf() + 1000);
  let temp = dt2.toTimeString().split(" ");
  let ts = temp[0].split(":");
  let timer: any;
  timer = ts[0] + ":" + ts[1] + ":" + ts[2];

  state.incomingCallDetail.callduration = timer;
  state.callDetails[index].callduration = timer;
};

export const blistenBargeWisperTimer = (state: any) => {
  let ltime: any = state.trackAgentDetail?.Duration ? state.trackAgentDetail?.Duration : "00:00:00";
  let ss: any = ltime.split(":");
  let dt = new Date();
  dt.setHours(ss[0]);
  dt.setMinutes(ss[1]);
  dt.setSeconds(ss[2]);
  let dt2 = new Date(dt.valueOf() + 1000);
  let temp = dt2.toTimeString().split(" ");
  let ts = temp[0].split(":");
  let timer: any;
  timer = ts[0] + ":" + ts[1] + ":" + ts[2];

  state.trackAgentDetail.Duration = timer;
};


export const findIndexWithCalltypeandConference = (callDetails: any) => {
  let index = _.findIndex(callDetails, (item: any) => item.callType === "direct" && item.isConference !== true);
  return index;
};

export const findIndexWithGuidIncoming = (callDetails: any, Incoming: any) => {
  let index = _.findIndex(callDetails, (e: any) => e.guid === Incoming.guid);
  return index;
};

export const forUnavailable = (callDetails: any) => {
  let detailIndex = _.findIndex(callDetails, (item: any) => item.isCurrent === true);
  return detailIndex;
};

export const terminateNormal = (callDetails: any, fromCaller: any) => {
  let detailIndex = _.findIndex(callDetails, (item: any) => item.from === fromCaller);
  return detailIndex;
};

export const forSipFailure = (callDetails: any, fromCaller: any) => {
  let detailIndex = _.findIndex(callDetails, (item: any) => {
    return item.from === fromCaller
  });
  return detailIndex;
};

export const findIndexWithOutboundCampaign = (callDetails: any) => {
  let index = _.findIndex(callDetails, (e: any) => e.outboundCampaign === true);
  return index;
};

export const resetFilter = (state: any) => {
  state.agentstatusList = state.agentstatusdata;
};

export const agentSearchData = (data: any, text: any) => {
  let searchText = text?.toLowerCase();
  let searchResp = _.filter(data, (data: any) => {
    let name = data?.statusName || "";
    return name?.toLowerCase().includes(searchText);
  });
  return searchResp;
};

export const agentStatusDetails = (agentresultdata: any) => {
  return agentresultdata.map((agentdata: any) => ({
    aid: agentdata?.aid,
    statusName: agentdata?.statusName,
    type: agentdata?.type,
    description: agentdata?.description,
    domainId: agentdata?.domainId,
    channels: agentdata?.channels,
    colorCode: agentdata?.colorCode,
  }));
};

export const getCurrentUserCallerID = () => {
  try {
    const { filterQueueList }: any = store.getState().commonData;
    const userList: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const login: any = GetItemFromStorage(LocalStorage.LOGIN_USER_CRM_DETAILS);
    const currentUser = login?.find((user: any) => user?.UserID == userList?.userId);
    const queueIds = `${currentUser?.queueId ?? ""}`
      .split(",")
      .map((id: string) => id.trim())
      .filter(Boolean);

    const currentQueue = (Array.isArray(filterQueueList) ? filterQueueList : []).find((item: any) =>
      queueIds.includes(`${item?.qid}`)
    );
    if (!currentQueue) return {};

    const res = {
      key: currentQueue.qid,
      value: currentQueue.name,
      OutboundCallerId: currentQueue.OutboundCallerId,
      outBoundNumber: currentQueue.outBoundNumber,
    }
    return res;
  } catch (error) {
    console.log("errorerrorerrorerror", error)
    return {};
  }
}

export const dataSearch = (state: any) => {
  const copyOfSearch = state.userTableData;

  state.testTableData.length = 0;

  copyOfSearch.filter((users: any) => {
    if (state.advancedSearchData.role.length !== 0 && state.advancedSearchData.routingProfileName.length !== 0 && state.advancedSearchData.reportToName.length !== 0) {
      if (
        state.advancedSearchData.role.includes(users.role) &&
        state.advancedSearchData.routingProfileName.includes(users.routingprofileName) &&
        state.advancedSearchData.reportToName.includes(users.reporttoname)
      ) {
        let index = checkIfSerachExist(state.testTableData, users.userid);
        if (index === -1) {
          state.testTableData.push(users);
        }
      }
    } else if (
      state.advancedSearchData.role.length === 0 &&
      state.advancedSearchData.routingProfileName.length === 0 &&
      state.advancedSearchData.reportToName.length === 0
    ) {
      state.testTableData = state.userTableData;
    } else {
      if (state.advancedSearchData.role.length !== 0 && state.advancedSearchData.routingProfileName.length !== 0 && state.advancedSearchData.reportToName.length === 0) {
        if (state.advancedSearchData.role.includes(users.role) && state.advancedSearchData.routingProfileName.includes(users.routingprofileName)) {
          let index = checkIfSerachExist(state.testTableData, users.userid);
          if (index === -1) {
            state.testTableData.push(users);
          }
        }
      } else if (
        state.advancedSearchData.role.length !== 0 &&
        state.advancedSearchData.routingProfileName.length === 0 &&
        state.advancedSearchData.reportToName.length !== 0
      ) {
        if (state.advancedSearchData.role.includes(users.role) && state.advancedSearchData.reportToName.includes(users.reporttoname)) {
          let index = checkIfSerachExist(state.testTableData, users.userid);
          if (index === -1) {
            state.testTableData.push(users);
          }
        }
      } else if (
        state.advancedSearchData.role.length === 0 &&
        state.advancedSearchData.routingProfileName.length !== 0 &&
        state.advancedSearchData.reportToName.length !== 0
      ) {
        if (state.advancedSearchData.routingProfileName.includes(users.routingprofileName) && state.advancedSearchData.reportToName.includes(users.reporttoname)) {
          let index = checkIfSerachExist(state.testTableData, users.userid);
          if (index === -1) {
            state.testTableData.push(users);
          }
        }
      } else {
        if (state.advancedSearchData.role.includes(users.role)) {
          let index = checkIfSerachExist(state.testTableData, users.userid);
          if (index === -1) {
            state.testTableData.push(users);
          }
        }

        if (state.advancedSearchData.routingProfileName.includes(users.routingprofileName)) {
          let index = checkIfSerachExist(state.testTableData, users.userid);
          if (index === -1) {
            state.testTableData.push(users);
          }
        }

        if (state.advancedSearchData.reportToName.includes(users.reporttoname)) {
          let index = checkIfSerachExist(state.testTableData, users.userid);
          if (index === -1) {
            state.testTableData.push(users);
          }
        }
      }
    }
  });

  state.userSearchData = state.testTableData;

  state.userCopySearchData = state.testTableData;
};

export const checkIfSerachExist = (testTableData: any, item: any) => {
  return testTableData?.findIndex((user: any) => user.userid === item);
};

export const filterData = (object: any, text: any) => {
  const name = object?.name?.toLowerCase() ?? "";
  const input = text?.toLowerCase();
  if (name.includes(input)) return true;
  else return false;
};

export const filterSearch = (object: any, text: any) => {
  const name = object?.name?.toLowerCase() ?? "";
  const reporttoname = object?.reporttoname?.toLowerCase() ?? "";
  const routingprofileName = object?.routingprofileName?.toLowerCase() ?? "";
  const role = object?.role?.toLowerCase() ?? "";
  const input = text?.toLowerCase();
  if (name.includes(input) || role.includes(input) || reporttoname.includes(input) || routingprofileName.includes(input)) return true;
  else return false;
};

export const getReportTo = (WholeArray: any) => {
  let userReporter = [] as any;
  WholeArray.map((user: any) => {
    if ([3, 5].includes(user?.roleid)) {
      userReporter.push(user?.UserName);
    }
  });
  return userReporter.filter((item: any, index: any) => userReporter.indexOf(item) === index);
};

export const mapResponseDataToTable = (state: any, data: any) => {
  const userList: any = [];
  for (const datum of data) {
    userList.push(getMappedObject(state, datum));
  }
  state.userTableData = userList;
  state.userSearchData = userList;
  state.userCopySearchData = userList;
};

export const getMappedObject = (state: any, data: any) => {
  state.availableUsers[`${data?.UserID}`] = 1;
  return {
    userid: data?.UserID,
    name: data?.UserName ? data?.UserName : "-",
    role: getrole(data?.roleid),
    routingprofileName: data?.routing_profile_name ? data?.routing_profile_name : "-",
    routingprofile: data?.routing_profile,
    reporttoname: data?.report_to_name ? data?.report_to_name : "-",
    addSkills: addSkills(data),
    reportingto: data?.report_to ? data?.report_to : "-",
    activeStatus: getstatus(data?.isActive),
    ext: data?.ext ? data?.ext : "-",
    emailId: data?.emailId,
    phoneNumber: data?.phone_number ? data?.phone_number : "-",
    deskNumber: data?.desk_phone_number ? data?.desk_phone_number : "-",
    inboundCall: data?.inboundCall,
    outboundCampaignCall: data?.outboundCampaignCall,
    callTypeId: data?.callTypeId
  };
};

export const getrole = (roleId: any) => {
  return roleId === parseInt(ROLES.AGENT_ROLE_ID)
    ? "Agent"
    : roleId === parseInt(ROLES.SUPERVISOR_ROLE_ID)
      ? "Supervisor"
      : roleId === parseInt(ROLES.OWNER_ROLE_ID)
        ? "Owner"
        : roleId === parseInt(ROLES.SUB_OWNER_ROLE_ID)
          ? "Subowner"
          : roleId === parseInt(ROLES.MEMBER_ROLE_ID)
            ? "Member"
            : "Admin";
};

export const getstatus = (statusvalue: any) => {
  return statusvalue === 0 ? "Inactive" : "Active";
};

export const addSkills = (data: any) => {
  let skills: any = typeof data?.addSkill === "string" ? JSON.parse(data?.addSkill.trim().length > 0 ? data?.addSkill : "null") ?? [] : data?.addSkill;
  let skillArray: any = skills?.length > 0 ? skills?.map((elem: any) => elem?.value) : [];
  return skillArray.length > 0 ? skillArray?.join() : "-";
};
export const FailedUserFetchResponse = "Unable to fetch users. Please Try again!";
export const FailedcustomerTagResponse = "Unable to fetch customer Tag. Please Try again";
export const FailedToFetchUserResponse = "Unable to fetch user details. Please Try again!";
export const SuccessUserEditResponse = "updated sucessfully!";
export const failedToUpdateuser = "Failed to update ";

export const debounce = (fn: any, delay: number) => {
  let timer: any;
  return (e: any) => {
    clearTimeout(timer);
    timer = setTimeout(() => fn(e), delay);
  };
};

export const createInfiniteScroll = ({
  lastScrollTopRef,
  hasMoreRef,
  loaderRef,
  onLoadMore,
}: any) => {
  const logic = (e: any) => {
    const current = e.target.scrollTop;

    if (current <= lastScrollTopRef.current) return;
    lastScrollTopRef.current = current;

    const bottom =
      e.target.scrollHeight - current <= e.target.clientHeight + 20;

    if (bottom && hasMoreRef.current && !loaderRef.current) {
      onLoadMore();
    }
  };

  return debounce(logic, 200);
};


export const validateEmail = (email: string) => {
  try {
    const regex =
      /^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$/;
    return regex.test(email.trim());
  } catch (err) {
    return false;
  }
};

export const removeSpaceNormalizetext = (text: any) => {
  try {
    return text
      ?.replace(/<br\s*\/?>/gi, "")
      ?.replace(/&nbsp;/gi, " ")
      ?.replace(/<[^>]*>/g, "")
      ?.replace(/\u00a0/g, " ")
      ?.trim()?.length === 0;
  } catch (error) {
    return "";
  }
}

export function findDifferenceBetweenTimeSlots(startTime, endTime) {
  try {
    const duration = moment.duration(Number(endTime) - Number(startTime));
    const h = duration.hours();
    const m = duration.minutes();
    const s = duration.seconds();
    return [
      h ? `${h}h` : "",
      m ? `${m}m` : "",
      s || (!h && !m) ? `${s}s` : ""
    ].filter(Boolean).join(" ");

  } catch (e) {
    console.error(e);
    return "";
  }
}

export function formatDateTimeLable(inputDate: any) {
  try {
    const m = moment(inputDate);
    if (!m.isValid()) throw new Error("Invalid date");
    const time = m.format("hh:mm a");
    if (m.isSame(moment(), "day")) {
      return `Today, ${time}`;
    }
    return m.format("DD MMM, hh:mm a"); // fallback
  } catch (e) {
    console.error(e);
    return "";
  }
}

export const getInitials = (firstName?: string, lastName?: string) => {
  if (!firstName && !lastName) return "N";
  return `${firstName?.[0] || ""}${lastName?.[0] || ""}`.toUpperCase();
};

export const getCurrencyByDomain = () => {
  try {
    const host = window.location.host;
    const parts = host.split('.');
    let res = ''
    if (parts.length > 2) {
      const loc = parts[parts.length - 1];

      if (loc === 'in') res = '₹';
      else if (loc === 'uk') res = '£';
      else if (loc === 'com') res = '$';
      else res = '$';

      return res;
    } else {
      return '$';
    }
  } catch (error) {
    return '$';
    console.log(error)
  }
}

export const getScheduleTime = (row: any) => {
  try {
    if (row?.channelType === 'Video' && row?.schedule_start) {
      const start = row?.schedule_start ? moment(row?.schedule_start).format("hh:mm A") : '';
      const end = row?.schedule_end ? moment(row?.schedule_end).format("hh:mm A") : '';
      const date = row?.schedule_start ? moment(row?.schedule_start).format("dddd MMM D, YYYY") : '';
      return `${start} - ${end} - ${date}`
    }
    return moment(row?.created_date).format("dddd MMM D, YYYY hh:mm A")
  } catch (error) {
    return moment(row?.created_date).format("dddd MMM D, YYYY hh:mm A")
  }
}

const adminModules = 'Your Info, Security, Privacy, Devices, Storage, Roles, Users, Teams, User Activity Logs, Order History, Add-ons, Purchases';
const agentModules = 'Your Info, Security, Privacy, Devices, Storage';

export const roleAccess: any = {
  3: adminModules,
  5: adminModules,
  6: agentModules,
};

export const sanitizePhoneNumber = (value: string) => {
  const trimmed = value.trim();
  if (!trimmed) return "";
  const withPlus = trimmed.startsWith("+");
  const digitsOnly = trimmed.replace(/\D/g, "");
  return withPlus ? `+${digitsOnly}` : digitsOnly;
};

export const getRegionCode = () => {
  try {
    const host = window.location.hostname;
    const parts = host.split('.');
    const tld = parts[parts.length - 1]?.toLowerCase();
    if (tld === 'in') return 'in';
    if (tld === 'uk') return 'uk';
    if (tld === 'com') return 'com';
    return 'in';
  } catch (e) {
    return 'in';
  }
};

export const getContactModuleByPath = () => {
  const currentPath = (location?.pathname || "").toLowerCase();
  if (currentPath === "/contact" || currentPath === "/contact/" || currentPath.includes("/contact/contactlist")) {
      return "contactList";
  }
  if (currentPath.includes("/contact/companylist")) {
      return "companyList";
  }
  return "";
};