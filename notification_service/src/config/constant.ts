export const DEVICE_TYPE = {
  IOS: 'CCAAS:IOS',
  AND: 'CCAAS:AND'
};

export function isJSON(str: any) {
  try {
    return JSON.parse(str) && !!str;
  } catch (e) {
    return false;
  }
}
