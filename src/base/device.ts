import { UAParser } from 'ua-parser-js';
import { v4 as uuidv4 } from 'uuid';
import countryList from './countryList';

export const DEVICE_ID_KEY = 'BROWSER_DEVICE_ID';
export const DEVICE_REGISTRY_KEY = 'BROWSER_DEVICE_REGISTRY';

type DeviceLocation = {
    gps: {
        latitude: number;
        longitude: number;
        accuracy: number;
    } | null;
    ipBased: {
        city?: string;
        region?: string;
        country?: string;
        latitude?: number;
        longitude?: number;
    };
};

export type DeviceDetails = {
    deviceId: string;
    deviceSignature: string;
    browserName?: string;
    browserVersion?: string;
    osName?: string;
    osVersion?: string;
    deviceType: string;
    deviceVendor: string;
    deviceModel: string;
    platform?: string;
    screenResolution: string;
    timezone: string;
    language: string;
    userAgent: string;
    ipAddress: string;
    connectionType?: string;
    effectiveConnectionType?: string;
    downlinkMbps?: number;
    rttMs?: number;
    online: boolean;
    cookiesEnabled: boolean;
    hardwareConcurrency?: number;
    deviceMemoryGB?: number;
    touchSupport: boolean;
    location: DeviceLocation;
    firstSeenAt: string;
    lastSeenAt: string;
};

type DeviceRegistry = {
    devices: DeviceDetails[];
};

function readRegistry(): DeviceRegistry {
    try {
        const raw = localStorage.getItem(DEVICE_REGISTRY_KEY);
        if (!raw) return { devices: [] };
        const parsed = JSON.parse(raw) as DeviceRegistry;
        return parsed?.devices ? parsed : { devices: [] };
    } catch {
        return { devices: [] };
    }
}

function writeRegistry(registry: DeviceRegistry) {
    try {
        localStorage.setItem(DEVICE_REGISTRY_KEY, JSON.stringify(registry));
    } catch {
        // Ignore write errors (storage full or blocked)
    }
}

async function hashString(input: string) {
    if (window.crypto?.subtle) {
        const encoder = new TextEncoder();
        const data = encoder.encode(input);
        const digest = await window.crypto.subtle.digest('SHA-256', data);
        const bytes = Array.from(new Uint8Array(digest));
        return bytes.map((b) => b.toString(16).padStart(2, '0')).join('');
    }

    // Fallback: non-crypto hash for older browsers
    let hash = 0;
    for (let i = 0; i < input.length; i++) {
        hash = (hash << 5) - hash + input.charCodeAt(i);
        hash |= 0;
    }
    return `legacy-${Math.abs(hash)}`;
}

export async function getDeviceDetails() {
    // 🔐 Stable Device ID
    let deviceId = localStorage.getItem(DEVICE_ID_KEY);
    if (!deviceId) {
        deviceId = uuidv4();
        localStorage.setItem(DEVICE_ID_KEY, deviceId);
    }

    // ✅ UA Parser (TypeScript-safe)
    const ua = UAParser();

    // 🌐 IP + Location
    let ipAddress = '';
    let locationFromIP: DeviceLocation['ipBased'] = {};

    try {
        const res = await fetch('https://ipapi.co/json/');
        const data = await res.json();

        ipAddress = data.ip;
        locationFromIP = {
            city: data.city,
            region: data.region,
            country: data.country_name,
            latitude: data.latitude,
            longitude: data.longitude,
        };
    } catch {
        ipAddress = 'UNKNOWN';
    }

    // 📍 Browser GPS (permission-based)
    const gpsLocation = await new Promise<DeviceLocation['gps']>((resolve) => {
        if (!navigator.geolocation) return resolve(null);

        navigator.geolocation.getCurrentPosition(
            (pos) =>
                resolve({
                    latitude: pos.coords.latitude,
                    longitude: pos.coords.longitude,
                    accuracy: pos.coords.accuracy,
                }),
            () => resolve(null),
            { timeout: 5000 }
        );
    });

    const now = new Date().toISOString();
    const connection =
        (navigator as Navigator & { connection?: any }).connection ||
        (navigator as Navigator & { mozConnection?: any }).mozConnection ||
        (navigator as Navigator & { webkitConnection?: any }).webkitConnection;

    const signatureSource = [
        ua.browser.name,
        ua.browser.version,
        ua.os.name,
        ua.os.version,
        ua.device.type || 'Desktop',
        ua.device.vendor || 'Unknown',
        ua.device.model || 'Web',
        (navigator as Navigator & { platform?: string }).platform,
        navigator.userAgent,
        navigator.language,
        Intl.DateTimeFormat().resolvedOptions().timeZone,
        `${window.screen.width}x${window.screen.height}`,
        navigator.hardwareConcurrency,
        (navigator as Navigator & { deviceMemory?: number }).deviceMemory,
        'maxTouchPoints' in navigator ? navigator.maxTouchPoints : 0,
    ]
        .map((v) => (v === undefined || v === null ? '' : String(v)))
        .join('|');

    const deviceSignature = await hashString(signatureSource);

    const details: DeviceDetails = {
        deviceId,
        deviceSignature,

        // Browser / Device
        browserName: ua.browser.name,
        browserVersion: ua.browser.version,
        osName: ua.os.name,
        osVersion: ua.os.version,
        deviceType: ua.device.type || 'Desktop',
        deviceVendor: ua.device.vendor || 'Unknown',
        deviceModel: ua.device.model || 'Web',

        // System
        screenResolution: `${window.screen.width}x${window.screen.height}`,
        timezone: Intl.DateTimeFormat().resolvedOptions().timeZone,
        language: navigator.language,
        userAgent: navigator.userAgent,
        platform: (navigator as Navigator & { platform?: string }).platform,

        // Network
        ipAddress,
        connectionType: connection?.type,
        effectiveConnectionType: connection?.effectiveType,
        downlinkMbps: typeof connection?.downlink === 'number' ? connection.downlink : undefined,
        rttMs: typeof connection?.rtt === 'number' ? connection.rtt : undefined,
        online: navigator.onLine,
        cookiesEnabled: navigator.cookieEnabled,
        hardwareConcurrency: navigator.hardwareConcurrency,
        deviceMemoryGB: (navigator as Navigator & { deviceMemory?: number }).deviceMemory,
        touchSupport: 'maxTouchPoints' in navigator ? navigator.maxTouchPoints > 0 : false,

        // Location
        location: {
            gps: gpsLocation,
            ipBased: locationFromIP,
        },
        firstSeenAt: now,
        lastSeenAt: now,
    };

    const registry = readRegistry();
    const existingIndex = registry.devices.findIndex(
        (d) => d.deviceId === deviceId || d.deviceSignature === deviceSignature
    );

    if (existingIndex >= 0) {
        const existing = registry.devices[existingIndex];
        registry.devices[existingIndex] = {
            ...existing,
            ...details,
            firstSeenAt: existing.firstSeenAt,
            lastSeenAt: now,
        };
    } else {
        registry.devices.push(details);
    }

    writeRegistry(registry);

    return {
        currentDevice: details,
        // allDevices: registry.devices,
    };
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
