import { getGlobalShortcutKeysDao, getShortcutKeysDao, getShortcutKeysValuesDao, insertGlobalShortcutKeysDao, insertShortcutKeysDao, resetAllShortcutKeysByUuidDao, updateStatusShortcutKeysDao } from '../../dao/shortcutkeys';

import { v4 as uuidv4 } from 'uuid';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';
/**
 *
 * @param req
 * @param res
 * @param done
 * ccs skill api
 */

export async function insertGlobalShortcutKeys(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = [
      {
        uuid: uuidv4(),
        function: 'Global Search',
        keyValues: {
          key1: 'Ctrl',
          key2: 'S'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Go To Agent Tracker',
        keyValues: {
          key1: 'Alt',
          key2: 'T'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Decline Incoming call',
        keyValues: {
          key1: 'Shift',
          key2: 'D'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Mute and Unmute the call',
        keyValues: {
          key1: 'Ctrl',
          key2: 'M'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Ready State',
        keyValues: {
          key1: 'Alt',
          key2: 'R'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Transfer Only',
        keyValues: {
          key1: 'Shift',
          key2: 'T'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Offline',
        keyValues: {
          key1: 'Alt',
          key2: 'O'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Create New Customer',
        keyValues: {
          key1: 'Alt',
          key2: 'C'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Answer Incoming call',
        keyValues: {
          key1: 'Shift',
          key2: 'A'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Hang-up Call',
        keyValues: {
          key1: 'Shift',
          key2: 'H'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Logout',
        keyValues: {
          key1: 'Alt',
          key2: 'Q'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Not Ready',
        keyValues: {
          key1: 'Alt',
          key2: 'N'
        },
        status: 1
      }, {
        uuid: uuidv4(),
        function: 'Away',
        keyValues: {
          key1: 'Alt',
          key2: 'A'
        },
        status: 1
      }
    ];

    await insertGlobalShortcutKeysDao(data, auth);
    res.status(200).send({ statusCode: 200, message: RESPONSE.insert_shortcutkeys });
  } catch (err) {
    logger.error('insertGlobalShortcutKeys', req.headers , err);
  }
}

export async function getGlobalShortcutKeys(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const getShortcutKeysRes = await getGlobalShortcutKeysDao(auth);
    logger.info('getGlobalShortcutKeysDao', getShortcutKeysRes);
    if (Array.isArray(getShortcutKeysRes) && getShortcutKeysRes.length > 0) {
      res.status(200).send({ statusCode: 200, message: 'Success', result: getShortcutKeysRes });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.not_found });
    }

  } catch (err) {

    logger.error('getGlobalShortcutKeys', req.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 */
export async function insertShortcutKeys(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = [
      {
        uuid: uuidv4(),
        function: 'My Activity tab',
        shortcutKey: 'Alt + M + A',
        keyValues: {
          key1: 'Alt',
          key2: 'M',
          key3: 'A'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'M',
          key3: 'A'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'All Activity tab',
        shortcutKey: 'Alt + A+ A',
        keyValues: {
          key1: 'Alt',
          key2: 'A',
          key3: 'A'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'A',
          key3: 'A'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Contacts',
        shortcutKey: 'Alt + A + C',
        keyValues: {
          key1: 'Alt',
          key2: 'A',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'A',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Hold the call',
        shortcutKey: 'Alt + H',
        keyValues: {
          key1: 'Alt',
          key2: 'H'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'H'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Transfer the call',
        shortcutKey: 'Shift + R',
        keyValues: {
          key1: 'Shift',
          key2: 'R'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'R'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Callback',
        shortcutKey: 'Ctrl + B',
        keyValues: {
          key1: 'Ctrl',
          key2: 'B'
        },
        defaultKeys: {
          key1: 'Ctrl',
          key2: 'B'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Leave Conference Call',
        shortcutKey: 'Alt + L',
        keyValues: {
          key1: 'Alt',
          key2: 'L'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'L'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Custom Queue',
        shortcutKey: 'Ctrl + Q',
        keyValues: {
          key1: 'Ctrl',
          key2: 'Q'
        },
        defaultKeys: {
          key1: 'Ctrl',
          key2: 'Q'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Create Custom Queue',
        shortcutKey: 'Ctrl + Q +C',
        keyValues: {
          key1: 'Ctrl',
          key2: 'Q',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Ctrl',
          key2: 'Q',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Prompt Library',
        shortcutKey: 'Alt + P',
        keyValues: {
          key1: 'Alt',
          key2: 'P'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'P'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Create Prompt Library',
        shortcutKey: 'Alt + P +C',
        keyValues: {
          key1: 'Alt',
          key2: 'P',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'P',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Skill Management',
        shortcutKey: 'Alt + M',
        keyValues: {
          key1: 'Alt',
          key2: 'M'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'M'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Create Skill Management',
        shortcutKey: 'Alt + M +C',
        keyValues: {
          key1: 'Alt',
          key2: 'M',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'M',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Business Hours',
        shortcutKey: 'Alt + B',
        keyValues: {
          key1: 'Alt',
          key2: 'B'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'B'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'To Create Business Hours',
        shortcutKey: 'Alt + B + C',
        keyValues: {
          key1: 'Alt',
          key2: 'B',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'B',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Routing Profile',
        shortcutKey: 'Alt + U',
        keyValues: {
          key1: 'Alt',
          key2: 'U'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'U'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'To Create Routing Profile',
        shortcutKey: 'Alt + U + C',
        keyValues: {
          key1: 'Alt',
          key2: 'U',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'U',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Webchat Configuration',
        shortcutKey: 'Alt + W',
        keyValues: {
          key1: 'Alt',
          key2: 'W'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'W'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'To Create Chat widget',
        shortcutKey: 'Alt + W + C',
        keyValues: {
          key1: 'Alt',
          key2: 'W',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Alt',
          key2: 'W',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Agent Status',
        shortcutKey: 'Shift + S',
        keyValues: {
          key1: 'Shift',
          key2: 'S'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'S'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'To Create Agent Status',
        shortcutKey: 'Shift + S + C',
        keyValues: {
          key1: 'Shift',
          key2: 'S',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'S',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Disposition Status',
        shortcutKey: 'Shift + D',
        keyValues: {
          key1: 'Shift',
          key2: 'D'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'D'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'To Create Disposition Status',
        shortcutKey: 'Shift + D + C',
        keyValues: {
          key1: 'Shift',
          key2: 'D',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'D',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Customer Type',
        shortcutKey: 'Shift + T + T',
        keyValues: {
          key1: 'Shift',
          key2: 'T',
          key3: 'T'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'T',
          key3: 'T'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'To Create Customer Type',
        shortcutKey: 'Shift + T + C',
        keyValues: {
          key1: 'Shift',
          key2: 'T',
          key3: 'C'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'T',
          key3: 'C'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Analytics',
        shortcutKey: 'Shift  + Alt + A',
        keyValues: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'A'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'A'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Dashboard',
        shortcutKey: 'Shift  + Alt + D',
        keyValues: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'D'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'D'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Reports',
        shortcutKey: 'Shift  + Alt + R',
        keyValues: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'R'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'Alt',
          key3: 'R'
        },
        status: 1
      },
      {
        uuid: uuidv4(),
        function: 'Bell Notification',
        shortcutKey: 'Shift + B',
        keyValues: {
          key1: 'Shift',
          key2: 'B'
        },
        defaultKeys: {
          key1: 'Shift',
          key2: 'B',
        },
        status: 1
      }
    ];

    await insertShortcutKeysDao(data, auth);

    res.status(200).send({ statusCode: 200, message: RESPONSE.insert_shortcutkeys });

  } catch (err) {

    logger.error('insertShortcutKeys', req.headers , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateShortcutKeys(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      uuid: req.body.uuid,
      domainId: auth.domainId,
      keyValues: req.body.keyValues,
      ext: auth.ext,
      keyId: req.body.keyId
    };
    const shortCutKeyValues = await getShortcutKeysValuesDao(data);
    const resultData = JSON.parse(shortCutKeyValues[0].keyValues ?? 'null') ?? [];
    const mapResponse = resultData.map((list: any) => {
      if (list.uuid === data.uuid) {
        list.keyValues = data.keyValues;
      }
      return list;
    });
    if (mapResponse?.length > 0) {
      const response2 = await updateStatusShortcutKeysDao({
        keyId: data.keyId,
        response: mapResponse
      });
      res.status(200).send({ statusCode: 200, message: RESPONSE.shortcut_keys_updated, result: response2[0] });
    } else {
      res.status(200).send({ statusCode: 422, message: RESPONSE.failed });
    }
  } catch (err) {

    logger.error('updateShortcutKeys', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getShortcutKeys(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const getShortcutKeysRes = await getShortcutKeysDao(auth);
    logger.info('getShortcutKeysDao', getShortcutKeysRes);
    if (getShortcutKeysRes) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.shortcut_keys_updated, result: getShortcutKeysRes });
    } else {
      res.status(200).send({ statusCode: 200, message: RESPONSE.not_found });
    }

  } catch (err) {

    logger.error('getShortcutKeys', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function updateShortcutKeysStatus(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      uuid: req.body.uuid,
      status: req.body.status,
      domainId: auth.domainId,
      ext: auth.ext,
      keyId: req.body.keyId
    };
    data.status === 1 ? data.status = 0 : data.status = 1;

    const shortCutKeyValues = await getShortcutKeysValuesDao(data);
    const resultData = JSON.parse(shortCutKeyValues[0].keyValues ?? 'null') ?? [];
    const mapResponse = resultData.map((list: any) => {
      if (list.uuid === data.uuid) {
        list.status = data.status;
      }
      return list;
    });
    if (mapResponse?.length > 0) {
      const response2 = await updateStatusShortcutKeysDao({
        keyId: data.keyId,
        response: mapResponse
      });
      res.status(200).send({ statusCode: 200, message: RESPONSE.shortcut_keys_updated, result: response2[0] });
    } else {
      res.status(200).send({ statusCode: 422, message: RESPONSE.failed });
    }
  } catch (err) {

    logger.error('updateShortcutKeysStatus', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function resetAllShortcutKeys(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const keyId = req.body.keyId;
    await resetAllShortcutKeysByUuidDao(auth, keyId);
    await insertShortcutKeys(req, res);
    const getShortcutKeysRes = await getShortcutKeysDao(auth);
    logger.info('getShortcutKeysDao', getShortcutKeysRes);
    res.status(200).send({ statusCode: 200, message: RESPONSE.shortcut_keys_updated, result: getShortcutKeysRes });
  } catch (err) {

    logger.error('resetAllShortcutKeys', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function resetShortcutKeysByUuid(req: any, res: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      uuid: req.body.uuid,
      domainId: auth.domainId,
      ext: auth.ext,
      keyId: req.body.keyId
    };
    const shortCutKeyValues = await getShortcutKeysValuesDao(data);
    const resultData = JSON.parse(shortCutKeyValues[0]?.keyValues ?? 'null') ?? [];
    const mapResponse = resultData.map((list: any) => {
      if (list.uuid === data.uuid) {
        list.keyValues = list.defaultKeys;
      }
      return list;
    });

    if (mapResponse?.length > 0) {
      await updateStatusShortcutKeysDao({
        keyId: data.keyId,
        response: mapResponse
      });
      res.status(200).send({ statusCode: 200, message: 'Reset successfully' });
    } else {
      res.status(200).send({ statusCode: 422, message: RESPONSE.failed });
    }
  } catch (err) {
    logger.error('resetShortcutKeysByUuid', req.headers, req.body , err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
