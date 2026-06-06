import { NOTIFICATION } from '../../helpers/constants';
import { deleteNotification, getNotification, insertNotification, updateMarkAsReadAllNotification, updateMarkAsReadNotification } from './handler';
import { deleteNotificationSchema, getNotificationSchema, insertGetNotificationSchema, updatemarkasReadAllNotificationSchema, updatemarkasReadNotificationSchema } from './schema';

export default function notificationhandler(server: any, options: any, next: any) {
  server.post(
    NOTIFICATION.INSERT_NOTIFICATION,
    {
      // preValidation: [server.validateSession],
      schema: {
        description: 'Notificaton saved',
        tags: ['Notification'],
        // body: insertGetNotificationSchema.body,
        response: insertGetNotificationSchema.response
      },
    },
    insertNotification,
  );

  server.post(
    NOTIFICATION.INSERT_NOTIFICATION_CALLBACK,
    {
      schema: {
        description: 'Notificaton saved',
        tags: ['Notification'],
        // body: insertGetNotificationSchema.body,
        response: insertGetNotificationSchema.response
      },
    },
    insertNotification,
  );

  server.post(
    NOTIFICATION.GET_NOTIFICATION,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'Notificaton saved',
        tags: ['Notification'],
        response: getNotificationSchema.response
      },
    },
    getNotification,
  );

  server.post(
    NOTIFICATION.DELETE_NOTIFICATION,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'Notificaton saved',
        tags: ['Notification'],
        body: deleteNotificationSchema.body,
        response: deleteNotificationSchema.response
      },
    },
    deleteNotification,
  );

  server.post(
    NOTIFICATION.UPDATE_MARKAS_READ,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'Notificaton saved',
        tags: ['Notification'],
        body: updatemarkasReadNotificationSchema.body,
        response: updatemarkasReadNotificationSchema.response
      },
    },
    updateMarkAsReadNotification,
  );

  server.post(
    NOTIFICATION.UPDATE_MARKAS_READ_ALL,
    {
      preValidation: [server.validateSession],
      schema: {
        description: 'Notificaton saved',
        tags: ['Notification'],
        response: updatemarkasReadAllNotificationSchema.response
      },
    },
    updateMarkAsReadAllNotification,
  );

  next();
}
