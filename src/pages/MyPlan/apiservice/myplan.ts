import axios from "axios";
import { config } from "../../../base/constant";
import { CoreServices } from "../../../base/service/apiservices";
import countryList from "../countryList";


export async function getClientData() {
    try {
        type GeoPoint = { lat: number; lon: number } | null;

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

        let gps: GeoPoint = null;
        if (navigator.geolocation) {
            try {
                gps = await new Promise<GeoPoint>((resolve) => {
                    navigator.geolocation.getCurrentPosition(
                        pos => resolve({ lat: pos.coords.latitude, lon: pos.coords.longitude }),
                        () => resolve(null),
                        { timeout: 8000 }
                    );
                });
            } catch (e) { gps = null; }
        }

        // 3. Public IP (via public API)
        // Note: pick a provider; this example uses ipify and ipapi for geo
        // const ipRes = await fetch('https://api.ipify.org?format=json');
        // const ipJson = await ipRes.json(); // { ip: "203.0.113.45" }
        // const ip = ipJson.ip;

        // // 4. IP-based geolocation (fallback if GPS not allowed)
        // const geoRes = await fetch(`https://ipapi.co/${ip}/json/`);
        // const geo = await geoRes.json(); // contains city, region, country_name, etc.
 
        const ipRes = await fetch('https://ipinfo.io/json');
        const ipJson = await ipRes.json(); 
        const { city, region, ip, country } = ipJson;


        
        const client = {
            device: platform,
            os: platform,
            browser: browserName,
            userAgent: ua,
            ip,
            deviceName,
            location:  { 
                source: 'ip', 
                city: city, 
                region: region,
                country: countryList[country] || country 
            },
        };

        console.log(client);
        return client;
    } catch (error) {
        return null
    }
}


export const getDevices = async (emailId:any)=> {
    try {
        const body={
            userEmail: emailId,
        }
        const res = await CoreServices.post(body,'/get_user_devices_dtl').catch((e)=> console.log(e))
        return res
    } catch (error) {
        return []
    }
}

const headers = { headers: { "Content-Type": "application/json" } }

export const getMyplanDescription = async (data: any) => {
    try {
      const res = await axios.post(
        `${config().NLP_DNS}/myplan_dis`,
        data,
        headers
      );
      return res.data?.response ?? '';
    } catch (error) {
      console.log(error);
      return '';
    }
  };
