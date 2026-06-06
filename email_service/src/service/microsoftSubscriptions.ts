import { getMultipleEmail } from '../dao/email.dao';
import { ensureMicrosoftSubscription } from './microsoftGraph';

export async function bootstrapMicrosoftSubscriptions() {
  try {
    const users: any = await getMultipleEmail();
    const microsoftUsers = users.filter((user: any) => {
      if (user.is_smtp !== 1) return false;
      try {
        const jsonData = JSON.parse(user.smtp_json || '{}');
        return jsonData.provider === 'MICROSOFT';
      } catch {
        return false;
      }
    });

    // Run all subscription renewals in parallel so one slow/failed domain doesn't block others
    await Promise.allSettled(
      microsoftUsers.map(async (user: any) => {
        try {
          const jsonData = JSON.parse(user.smtp_json || '{}');
          await ensureMicrosoftSubscription(user, jsonData);
        } catch (err: any) {
          console.error('❌ MS subscription failed for', user?.emailAddress, err?.message);
        }
      })
    );
  } catch (err) {
    console.error('❌ Microsoft subscription bootstrap failed', err);
  }
}
