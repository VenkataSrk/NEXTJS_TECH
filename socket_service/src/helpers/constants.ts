import { createChatSession } from '../../src/dao/webChat.dao';

export const ROLES = {
  OWNER_ROLE_ID: '1',
  SUB_OWNER_ROLE_ID: '2',
  ADMIN_ROLE_ID: '3',
  MEMBER_ROLE_ID: '4',
  SUPERVISOR_ROLE_ID: '5',
  AGENT_ROLE_ID: '6',
};

export const channelTypeCode = {
  Call: 2003001,
  Chat: 2003002,
  Email: 2003003,
  SMS: 2003004,
  Whatsapp: 2003005,
  Facebook: 2003006,
  Instagram: 2003007
};

export const insertQueuedTimeForChat = (data: any) => {
  try {
    const currentTime = Math.floor(Date.now() / 1000);
    const queuedTime = data?.[0]?.queuedTime ?? 0;
    const queueDurationSeconds = currentTime - queuedTime;
    const bodydata = {
      domainId: data[0]?.channelId?.domain_id,
      ChatHistoryId: data[0]?.chatHistoryId ?? 0,
      queueDuration: queueDurationSeconds ?? 0,
    };
    console.log('<<<<<bodydata>>>>>', bodydata);
    bodydata?.ChatHistoryId ? createChatSession(bodydata) : '';

  } catch (err) {
    console.log(err);
  }
};

export const splitFirst = (str: string, sep = ''): string[] => {
  try {
    if (!str) return ['', ''];
    if (str?.indexOf(sep) === 0) {
      return ['', str.slice(sep?.length)];
    }
    return ['', str];
  } catch (error) {
    return ['', str];
  }
};
