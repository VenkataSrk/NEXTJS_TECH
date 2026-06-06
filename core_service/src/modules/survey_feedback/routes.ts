import { bulkUpload } from '../../config/upload';
import { BOTSETTINGS } from '../../helpers/constants';
import {
  createSurveyFeedbackHandler,
  deleteSurveyFeedbackHandler,
  getSurveyFeedbackByIdHandler,
  getSurveyFeedbackListHandler
} from './handler';

export default function surveyFeedbackHandler(server: any, options: any, next: any) {

  server.post(
    BOTSETTINGS.CREATE_SURVEY_FEEDBACK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'create survey_feedbacks',
        description: 'create survey_feedbacks',
        tags: ['BOT_Settings'],
      },
    },
    createSurveyFeedbackHandler
  );

  server.delete(
    BOTSETTINGS.DELETE_SURVEY_FEEDBACK,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'delete survey_feedbacks',
        description: 'delete survey_feedbacks',
        tags: ['BOT_Settings'],
      },
    },
    deleteSurveyFeedbackHandler
  );
  server.get(
    BOTSETTINGS.GET_SURVEY_FEEDBACK_BY_ID,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get survey_feedbacks',
        description: 'get survey_feedbacks',
        tags: ['BOT_Settings'],
      },
    },
    getSurveyFeedbackByIdHandler
  );
  server.get(
    BOTSETTINGS.GET_SURVEY_FEEDBACK_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get survey_feedbacks',
        description: 'get survey_feedbacks',
        tags: ['BOT_Settings'],
      },
    },
    getSurveyFeedbackListHandler
  );
  next();
}
