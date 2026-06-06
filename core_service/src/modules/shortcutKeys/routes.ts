import { SHORTCUTKEYS } from '../../helpers/constants';
import { getGlobalShortcutKeys, getShortcutKeys, insertGlobalShortcutKeys, insertShortcutKeys, resetAllShortcutKeys, resetShortcutKeysByUuid, updateShortcutKeys, updateShortcutKeysStatus } from './handler';
import { updateShortcutKeysSchema } from './schema';

export default function shortcutKeyshandler(server: any, options: any, next: any) {
  // shortcutkeys insert
  server.post(
    SHORTCUTKEYS.INSERT_SHORTCUT_KEYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'insert shortcut keys',
        description: ' insert shortcut keys api',
        tags: ['Shortcut Keys'],
        // body: insertShortcutKeysSchema.body,
      },
    },
    insertShortcutKeys
  );

  server.put(
    SHORTCUTKEYS.UPDATE_SHORTCUT_KEYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update shortcut keys',
        description: ' update shortcut keys api',
        tags: ['Shortcut Keys'],
        body: updateShortcutKeysSchema.body,
      },
    },
    updateShortcutKeys
  );
  server.get(
    SHORTCUTKEYS.GET_SHORTCUT_KEYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get shortcut keys',
        description: ' get shortcut keys api',
        tags: ['Shortcut Keys'],
      },
    },
    getShortcutKeys
  );

  server.put(
    SHORTCUTKEYS.UPDATE_STATUS_SHORTCUT_KEYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update status shortcut keys',
        description: ' update status shortcut keys api',
        tags: ['Shortcut Keys'],
      },
    },
    updateShortcutKeysStatus
  );
  server.get(
    SHORTCUTKEYS.RESET_ALL_SHORTCUT_KEYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update status shortcut keys',
        description: ' update status shortcut keys api',
        tags: ['Shortcut Keys'],
      },
    },
    resetAllShortcutKeys
  );
  server.post(
    SHORTCUTKEYS.INSERT_GLOBAL_SHORTCUT_KEYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create global status shortcut keys',
        description: ' create global status shortcut keys api',
        tags: ['Shortcut Keys'],
      },
    },
    insertGlobalShortcutKeys
  );

  server.get(
    SHORTCUTKEYS.GET_GLOBAL_SHORTCUT_KEYS,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get global status shortcut keys',
        description: ' get global status shortcut keys api',
        tags: ['Shortcut Keys'],
      },
    },
    getGlobalShortcutKeys
  );

  server.put(
    SHORTCUTKEYS.RESET_GLOBAL_SHORTCUT_KEY_BYUUID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'reset the keys by uuid  shortcut keys',
        description: ' reset the keys by uuid shortcut keys api',
        tags: ['Shortcut Keys'],
      },
    },
    resetShortcutKeysByUuid
  );
  next();
}
