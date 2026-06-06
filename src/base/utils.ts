import CryptoJS from 'crypto-js';
import { AICoreServices } from './service/apiservice';
export const excelFormats = ["xla", "xlam", "xls", "xlsb", "xlsm", "application/vnd.ms-excel",
    "xlsx", "application/vnd.google-apps.spreadsheet", "xlt",
    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "xltm", "xltx", "xlw", "csv", "application/vnd.ms-excel", "xll",
    'XLSX', "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", "text/csv"
];

export const imageFormat = ["PNG", "jpeg", "jpg", "svg", "gif", "webp", 'png', "jfif", "ico", "tiff", "jfif", "nef", "orf", "dib", "eps", "raw", "heif", "apng", "avif", "jif", "tif", "xbm", "heic", "arw", "image/png",
    "image/jpeg", "image/jpg", "image/gif", "image/jfif", "image/heif", "application/octet-stream", "image/svg", "image/webp", "image/gif", "image/svg+xml", "image/avif"
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

export const wordFormats = ["DOC", "docx", "dot", "doc", "text/plain",
    "dotm", "txt", "wpd", "odt", "wbk", "docm", "dotx", "docb", "rtf", "prn", "dif", "slk", "application/msword",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.document",
    "application/vnd.openxmlformats-officedocument.wordprocessingml.template",
    "application/vnd.ms-word.document.macroEnabled.12",
    "application/vnd.ms-word.template.macroEnabled.12"
];

export const pdfFormats = ["pdf", "application/pdf", "PDF"];

export function isJSON(str: any) {
  try {
    return JSON.parse(str) && !!str;
  } catch (e) {
    return false;
  }
}


export async function encryptAES(str :any){
  return new Promise((resolve, reject) => {
    try {
      const encrypted = CryptoJS.AES.encrypt( str, import.meta.env.ENC_KEY|| "Worktual_Innovation").toString();
      resolve(encrypted); // Resolve the Promise with the encrypted string
    } catch(error :any){
      console.error("Encryption failed", error);
      reject(error); // Reject the Promise on error
    }
  });
}

export async function decryptAESCVM(str :any){
  return new Promise((resolve,reject)=>{
    try {
      const decrypted = CryptoJS.AES.decrypt( str, import.meta.env.CVM_DEC_KEY||"cvmCampaign@123").toString(CryptoJS.enc.Utf8);
      // const decrypted = CryptoJS.AES.decrypt( str,"cvmCampaign@123").toString();
      // const decrypted = CryptoJS.AES.decrypt( "U2FsdGVkX1+KQEKu1t12gkoZvn5esBTup1cPTMwnNbQ=","cvmCampaign@123").toString(CryptoJS.enc.Utf8);
      console.log("dcrypt",decrypted);
      resolve(decrypted); // Resolve the Promise with the encrypted string
    } catch (error) {
       console.error("Decryption failed", error);
      reject(error); // Reject the Promise on error
    }
  })
}

export function floatTo16BitPCM(float32Array: Float32Array) {
   const buffer = new ArrayBuffer(float32Array.length * 2);
   const view = new DataView(buffer);
   let offset = 0;
   for (let i = 0; i < float32Array.length; i++, offset += 2) {
       let s = Math.max(-1, Math.min(1, float32Array[i]));
       view.setInt16(offset, s < 0 ? s * 0x8000 : s * 0x7fff, true);
   }
   return new Int16Array(buffer);
}


export function downsampleBuffer(buffer: Float32Array, inputSampleRate: number, targetRate: number) {
   if (targetRate === inputSampleRate) return buffer;
   const sampleRateRatio = inputSampleRate / targetRate;
   const newLength = Math.round(buffer.length / sampleRateRatio);
   const result = new Float32Array(newLength);
   let offsetResult = 0, offsetBuffer = 0;
   while (offsetResult < result.length) {
       const nextOffsetBuffer = Math.round((offsetResult + 1) * sampleRateRatio);
       let accum = 0, count = 0;
       for (let i = offsetBuffer; i < nextOffsetBuffer && i < buffer.length; i++) {
           accum += buffer[i]; count++;
       }
       result[offsetResult] = accum / count;
       offsetResult++; offsetBuffer = nextOffsetBuffer;
   }
   return result;
}


export const updateDeviceDetails = async (email :any) => {
  try {

    const getDetils: any = await getClientData();
    console.log("<<<getDetilsgetDetils>>> ", getDetils)

    const body = {
      Device: getDetils.deviceName ?? '',
      Device_Model: getDetils.device ?? '',
      OS_Type: getDetils.os ?? '',
      Browser_Type: getDetils.browser ?? '',
      IP_Address: getDetils.ip ?? '',
      Location: `${getDetils?.location?.city || ''} ${getDetils?.country || ''}`.trim() ?? '',
      is_active: 1,
      product_type:"CAMPAIGN",
      User_Email:email

    }
    AICoreServices.post(body, '/insert_update_user_devices')
      .then((res)=> {
        console.log(res)
      })

  } catch (error) {
    console.log("insert update Device error:",error)
  }
}



export async function getClientData() {
    try {

        const ua = navigator?.userAgent;
        const platform = navigator?.platform || 'unknown';
        const browserName = (() => {
            // simple detection (use ua-parser-js for robust parsing)
            if (ua.includes("Firefox")) return "Firefox";
            if (ua.includes("Chrome") && !ua.includes("Edg")) return "Chrome";
            if (ua.includes("Edg")) return "Edge";
            if (ua.includes("Safari") && !ua.includes("Chrome")) return "Safari";
            return "Unknown";
        })();

        let deviceName = 'Unknown'
        if (/windows phone/i.test(ua)) deviceName = "Windows Phone";
        else if (/windows/i.test(ua)) deviceName = "Windows PC";
        else if (/macintosh|mac os x/i.test(ua)) deviceName = "Mac";
        else if (/android/i.test(ua)) deviceName = "Android Device";
        else if (/iphone/i.test(ua)) deviceName = "iPhone";
        else if (/ipad/i.test(ua)) deviceName = "iPad";
        else if (/linux/i.test(ua)) deviceName = "Linux Machine";

        let gps = null;
        if (navigator.geolocation) {
            try {
                gps = await new Promise((resolve, reject) => {
                    navigator.geolocation.getCurrentPosition(
                        pos => resolve({ lat: pos.coords.latitude, lon: pos.coords.longitude }),
                        err => resolve(null),
                        { timeout: 8000 }
                    );
                });
            } catch (e) { gps = null; }
        }

        // 3. Public IP (via public API)
        // Note: pick a provider; this example uses ipify and ipapi for geo
        const ipRes = await fetch('https://api.ipify.org?format=json');
        const ipJson = await ipRes.json(); // { ip: "203.0.113.45" }
        const ip = ipJson.ip;

        // 4. IP-based geolocation (fallback if GPS not allowed)
        const geoRes = await fetch(`https://ipapi.co/${ip}/json/`);
        const geo = await geoRes.json(); // contains city, region, country_name, etc.

        const client = {
            device: platform,
            os: platform,
            browser: browserName,
            userAgent: ua,
            ip,
            deviceName,
            location:  { source: 'ip', city: geo.city, region: geo.region, country: geo.country_name },
        };

        console.log(client);
        return client;
    } catch (error) {
        return null
    }
}


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
  };
}

export const getCurrencyByDomain = () => {
  try {
    const host = window.location.host;
    const parts = host.split('.');
    let res = ''
    if (parts.length > 2) {
      const loc = parts[parts.length - 1];

      if(loc === 'in')  res = '₹';
      else if(loc === 'uk')  res = '£';
      else if(loc === 'com')  res = '$';
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

export const stripHtmlTags = (html: string) => {
  return html.replace(/<[^>]*>/g, '').replace(/\n/g, ' ').trim();
};