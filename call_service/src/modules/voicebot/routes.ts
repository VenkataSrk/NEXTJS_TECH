import { VOICEBOT } from '../../helpers/constants';
import { createGuestHandler } from './handler';
import { guestSchema } from './schema';

export default function voiceBotHandler(server: any, options: any, next: any) {
  server.post(
    VOICEBOT.CREATE_GUEST_USER,
    {
      schema: {
        description: 'Create voicebot call register details',
        tags: ['VOICEBOT'],
        body: guestSchema.body,
      },
    },
    createGuestHandler,
  );

  next();
}
