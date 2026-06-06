/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { DID } from '../../helpers/constants';
import {
  deleteDidPurchaseHandler,
  deleteSipTrunkNumHandler,
  getDidDomainListHandler,
  getDidListHandler,
  getDidPurchaseDetailsHandler,
  getDidPurchaseHandler,
  indicateQaToNlp,
  sipTrunkNumCheckHandler,
  updateDIDforISSMS,
  updateDIDNumberSMS,
  updateDidPurchaseHandler,
  updateDIDStatusHandler
} from './handler';
import {
  deleteDidSchema,
  getDidSchema,
  insertDidSchema,
  updateDidSchema,
} from './schema';

export default function didPruchaseHandler(
  server: any,
  options: any,
  next: any
) {
  // purchase insert api for ccs
  // server.post(
  //   DID.PURCHASE_INSERT,
  //   {
  //     preValidation: [server.validateSession],
  //     schema: {
  //       summary: 'Insert did_pruchase',
  //       description: 'Insert did_pruchase api',
  //       tags: ['didPurchase'],
  //       body: insertDidSchema.body,
  //       response: insertDidSchema.response,
  //     },
  //   },
  //   didPurchaseHandler
  // );

  // get api for ccs
  server.get(
    `${DID.PURCHASE_GET}/:pid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get did_pruchase',
        description: 'Get did_pruchase api',
        tags: ['didPurchase'],
        response: getDidSchema.response,
      },
    },
    getDidPurchaseHandler
  );

  // update api for ccs
  server.put(
    `${DID.PURCHASE_UPDATE}/:phoneNumbar`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update did_pruchase',
        description: 'update did_pruchase api',
        tags: ['didPurchase'],
        body: updateDidSchema.body,
        params: updateDidSchema.response,
        response: updateDidSchema.response,
      },
    },
    updateDidPurchaseHandler
  );

  // delete api for ccs
  server.delete(
    `${DID.PURCHASE_DELETE}/:pid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete did_pruchase',
        description: 'delete did_pruchase api',
        tags: ['didPurchase'],
        params: deleteDidSchema.params,
      },
    },
    deleteDidPurchaseHandler
  );
  // get api for ccs
  server.get(
    DID.PURCHASE_GET_lIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get did_pruchase list',
        description: 'Get did_pruchase list api',
        tags: ['didPurchase'],
        // response: getDidListSchema.response,
      },
    },
    getDidListHandler
  );

  // get did purchase
  server.get(
    DID.GET_DID_PURCHASES,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get did_pruchase',
        description: 'Get did_pruchase api',
        tags: ['didPurchase'],
        // response: getDidListSchema.response,
      },
    },
    getDidPurchaseDetailsHandler
  );

  // post outBound SMS
  server.post(
    `${DID.UPDATE_IS_SMS}/:pid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'update outbound SMS',
        description: 'update DID number for outbound SMS',
        tags: ['didOutboundSMS']
      }
    },
    updateDIDforISSMS,
  );

  // didNumberEdit
  server.post(
    `${DID.UPDATE_DID_NUMBER_EDIT}/:pid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'edit DID number update',
        description: 'edit DID number Update for SMS',
        tags: ['didOutboundSMS']
      }
    },
    updateDIDNumberSMS,
  );

  // didNumberUpdate
  server.put(
    `${DID.UPDATE_DID_STATUS}/:pid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Update did status',
        description: 'Update did status',
        tags: ['didstatusUpdate']
      }
    },
    updateDIDStatusHandler,
  );

  server.delete(
    `${DID.SIP_TRUNK_NUM_DELETE}/:pid`,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete sip trunk number',
        description: 'delete the sip trunk number',
        tage: ['didPurchase']
      },
    },
    deleteSipTrunkNumHandler
  );

  server.post(
    DID.SIP_TRUNK_NUM_CHECK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'check sip trunk number',
        description: 'check the sip trunk number',
        tage: ['didPurchase']
      }
    },
    sipTrunkNumCheckHandler
  );

  server.post(
    DID.INDICATE_QA_TO_NLP,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'check sip trunk number',
        description: 'check  sip trunk number',
        tage: ['didPurchase']
      }
    },
    indicateQaToNlp
  );
  server.post(
    DID.GET_DOMAIN_DID_lIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get domain did_pruchase list',
        description: 'Get domain did_pruchase list api',
        tags: ['didPurchase'],
        // response: getDidListSchema.response,
      },
    },
    getDidDomainListHandler
  );
  next();
}
