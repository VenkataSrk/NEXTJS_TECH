import { TokenClient } from '../../plugins/db';

export const insertGuestUserDetails = async (data: any) => {
  try {
    const sipDetails = {
      user_id: `${data?.ext ?? ''}`,
      domain_id: `${data?.domainId ?? ''}`,
      domain_name: `${data?.domainName ?? ''}`,
      password: `${data?.password ?? ''}`,
      callerName: `${data?.callerName ?? ''}`,
    };
    return await TokenClient.set(`${sipDetails.user_id}@${sipDetails?.domain_name}`, JSON.stringify(sipDetails));
  } catch (error) {
    return false;
  }
};
