import { pubsubRedisClient } from './plugins/db';

export function isJSON(str: any) {
  try {
    return JSON.parse(str) && !!str;
  } catch (e) {
    return false;
  }
}

export function updateInactivity(domainId: any, ext: any, inActive: boolean) {
  const key = `${domainId}_inactivity`;
  const field = `${domainId}_${ext}`;
  pubsubRedisClient.hget(key, field)
    .then((res: any) => {
      if (res) {
        const presenceRes = isJSON(res) ? JSON.parse(res) : res;
        const data = {
          ...presenceRes,
          ccaasInactive: inActive,
        };
        pubsubRedisClient.hset(key, field, JSON.stringify(data));
      } else {
        const data = {
          ccaasInactive: inActive,
        };
        pubsubRedisClient.hset(key, field, JSON.stringify(data));
      }
    })
    .catch((err: any) => console.log('update_inactivity_presence err>>>>', err));
}
