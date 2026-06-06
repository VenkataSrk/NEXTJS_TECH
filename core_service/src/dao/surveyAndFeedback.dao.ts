// import { IsurveyFeedback } from '../modules/survey_feedback/survey_feedback.model';
import moment from 'moment';
import { mysqlPoolConnection } from '../plugins/db';
import { logger } from '../plugins/winston';

export const createSurveyFeedback = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_insert_survey_feedback(?,?,?,?,?,?,?)',
        [
          null,
          data?.domainId,
          data?.questionType,
          data?.question_details,
          data?.uuid,
          data?.status,
          data?.version
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_insert_survey_feedback - Unexpected Error:', err);
          } else {
            resolve(results?.[0]?.[0]);
          }
        });
    } catch (err) {
      reject(err);
      logger.error('ccaas_insert_survey_feedback - Unexpected Error:', err);
    }
  });
};

export const deleteSurveyFeedback = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_delete_survey_feedback(?)',
        [
          data?.feedbackId
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_delete_survey_feedback - Unexpected Error:', err);
          } else {
            resolve(results?.[0]?.[0]);
          }
        });
    } catch (error) {
      logger.error('ccaas_delete_survey_feedback - Unexpected Error:', error);
      reject(error);
    }
  });
};

export const getSurveyFeedbackById = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_survey_feedback_by_id(?)',
        [
          data?.feedbackId
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_survey_feedback_by_id - Unexpected Error:', err);
          } else {
            resolve(results?.[0]);
          }
        });
    } catch (error) {
      reject(error);
      logger.error('ccaas_get_survey_feedback_by_id - Unexpected Error:', error);
    }
  });
};

export const getAllSurveyFeedback = async (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query(
        'call ccaas_get_all_survey_feedback(?,?,?)',
        [
          data?.domainId,
          data?.offset,
          data?.limit
        ],
        (err, results) => {
          if (err) {
            reject(err);
            logger.error('ccaas_get_all_survey_feedback - Unexpected Error:', err);
          } else {
            resolve(results?.[0]);
          }
        });
    } catch (error) {
      logger.error('ccaas_get_all_survey_feedback - Unexpected Error:', error);
      reject(error);
    }
  });
};
