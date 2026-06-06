import { cpUploaded } from '../../config/upload';
import { CALL_BLAST } from '../../helpers/constants';
import { deleteCallblastHandler, deleteOutboundCampaignDraftHandler, getCallBlastAutoAnswerHandler, getCallBlastHistoryListHandler, getCallBlastListHandler, getCallBlastOverallScheduledCampaigns, getCallBlastParticularHistory, getCallBlastPerticularChildScheduledCampaigns, getCustomerContactFilterValueHandler, getCustomerContactHandler, getOverallCallBlastListHandler, insertUpdatCallBlastAutoAnswerHandler, insertUpdateCallBlastConfig, updateCallBlastStutasHandler } from './handler';

export default function callBlastHandler(server: any, option: any, next: any) {
  server.post(
    CALL_BLAST.INSERT_UPDATE_CALLBLAST, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Insert update Call Blast',
        description: 'Insert update Call Blast api',
        tags: ['Call Blast'],
      }

    },
    insertUpdateCallBlastConfig
  );
  server.post(
    CALL_BLAST.GET_CUSTOMER_CONTACT_CALLBLAST, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'GET_CUSTOMER_CONTACT_CALLBLAST',
        description: 'GET_CUSTOMER_CONTACT_CALLBLAST API',
        tags: ['Call Blast'],
      }

    },
    getCustomerContactHandler
  );
  server.post(
    CALL_BLAST.GET_CALLBLAST_LIST, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'GET_CALLBLAST_LIST',
        description: 'GET_CALLBLAST_LIST API',
        tags: ['Call Blast'],
      }
    },
    getCallBlastListHandler
  );
  server.post(
    CALL_BLAST.GET_CALLBLAST_OVERALL_CAMPAIGNS, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'GET_CALLBLAST_OVERALL_CAMPAIGNS',
        description: 'GET_CALLBLAST_OVERALL_CAMPAIGNS API',
        tags: ['Call Blast'],
      }
    },
    getOverallCallBlastListHandler
  );
  server.post(
    CALL_BLAST.UPDATE_CALLBLAST_STUTAS, {
      // preValidation: [server.validateSession], // nlp also will use
      schema: {
        summary: 'UPDATE_CALLBLAST_STUTAS',
        description: 'UPDATE_CALLBLAST_STUTAS API',
        tags: ['Call Blast'],
      }
    },
    updateCallBlastStutasHandler
  );
  server.post(
    CALL_BLAST.DELETE_CALLBLAST, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'UPDATE_CALLBLAST_STUTAS',
        description: 'UPDATE_CALLBLAST_STUTAS API',
        tags: ['Call Blast'],
      }
    },
    deleteCallblastHandler
  );
  server.post(
    CALL_BLAST.HISTORY_CALLBLAST, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'HISTORY_CALLBLAST',
        description: 'HISTORY_CALLBLAST API',
        tags: ['Call Blast'],
      }
    },
    getCallBlastHistoryListHandler
  );
  server.post(
    CALL_BLAST.GET_CALLBLAST_PARTICULAR_HISTORY, {
    // preValidation: [server.validateSession],
      schema: {
        summary: 'GET_CALLBLAST_PARTICULAR_HISTORY',
        description: 'GET_CALLBLAST_PARTICULAR_HISTORYs API',
        tags: ['Call Blast'],
      }
    },
    getCallBlastParticularHistory
  );
  server.get(
    CALL_BLAST.GET_CALLBLAST_AUTO_ANSWER, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'GET_CALLBLAST_AUTO_ANSWER',
        description: 'GET_CALLBLAST_AUTO_ANSWER API',
        tags: ['Call Blast'],
      }

    },
    getCallBlastAutoAnswerHandler
  );
  server.post(
    CALL_BLAST.INSERT_UPDATE_CALLBLAST_AUTO_ANSWER, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'INSERT_UPDATE_CALLBLAST_AUTO_ANSWER',
        description: 'INSERT_UPDATE_CALLBLAST_AUTO_ANSWER API',
        tags: ['Call Blast'],
      }
    },
    insertUpdatCallBlastAutoAnswerHandler
  );
  server.get(
    CALL_BLAST.GET_CUSTOMER_CONTACT_CALLBLAST_FILTER_VALUE, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'GET_CUSTOMER_CONTACT_CALLBLAST_FILTER_VALUE',
        description: 'GET_CUSTOMER_CONTACT_CALLBLAST_FILTER_VALUE API',
        tags: ['Call Blast'],
      }
    },
    getCustomerContactFilterValueHandler
  );
  server.post(
    CALL_BLAST.DELETE_OUTBOUND_CAMPAIGN_DRAFT, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Delete Outbound Campaign draft',
        description: 'Delete Outbound Campaign draft API',
        tags: ['Call Blast'],
      }
    },
    deleteOutboundCampaignDraftHandler
  );
  server.get(
    CALL_BLAST.GET_CALLBLAST_OVERALL_SCHEDULED_CAMPAIGNS, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get callblast overall scheduled campaigns',
        description: 'get callblast overall scheduled campaigns API',
        tags: ['Call Blast'],
      }
    },
    getCallBlastOverallScheduledCampaigns
  );
  server.post(
    CALL_BLAST.GET_CHILD_BLASTID_HISTORY_DTL, {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get callblast perticular children scheduled campaigns',
        description: 'get callblast perticular children scheduled campaigns API',
        tags: ['Call Blast'],
      }
    },
    getCallBlastPerticularChildScheduledCampaigns
  );
  next();
}
