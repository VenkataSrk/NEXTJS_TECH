import { SOCIALMEDIA } from '../../../helpers/constants';
import {
  getFacebookAgentACWReportHandler,
  getFacebookAgentReportHandler,
  getFacebookReportHandler,
  getInstagramacwReportHandler,
  getInstagramAgentacwReportHandler,
  getInstagramReportHandler,
  getSocialMediaAgentReportHandler,
  getSocialMediaProfileReportHandler,
  getWhatsappACWReportHandler,
} from './handler';

export default function socialMediaHandler(server: any, options: any, next: any) {

  // 1st
  server.post(
    SOCIALMEDIA.GET_SOCIALMEDIA_PROFILE_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get social Media profile summary',
        description: 'get social Media profile summary api',
        tags: ['socialMediaReport'],
      },
    },
    getSocialMediaProfileReportHandler
  );

  // 2nd
  server.post(
    SOCIALMEDIA.GET_SOCIALMEDIA_AGENT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get social Media agent summary',
        description: 'get social Media agent summary api',
        tags: ['socialMediaReport'],
      },
    },
    getSocialMediaAgentReportHandler
  );

  // 3rd
  server.post(
    SOCIALMEDIA.GET_WHATSAPP_ACW_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get whatsapp ACW summary',
        description: 'get whatsapp ACW summary api',
        tags: ['socialMediaReport'],
      },
    },
    getWhatsappACWReportHandler
  );

  server.post(
    SOCIALMEDIA.GET_INSTAGRAM_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get instagram summary',
        description: 'get instagram ACW summary api',
        tags: ['socialMediaReport'],
      },
    },
    getInstagramReportHandler
  );
  server.post(
    SOCIALMEDIA.GET_INSTAGRAM_ACW_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get instagram ACW summary',
        description: 'get instagram ACW summary api',
        tags: ['socialMediaReport'],
      },
    },
    getInstagramacwReportHandler
  );
  server.post(
    SOCIALMEDIA.GET_INSTAGRAM_AGENT_ACW_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get instagram Agent ACW summary',
        description: 'get instagram Agent ACW summary api',
        tags: ['socialMediaReport'],
      },
    },
    getInstagramAgentacwReportHandler
  );
  server.post(
    SOCIALMEDIA.GET_FACEBOOK_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get facebook summary',
        description: 'get facebook summary api',
        tags: ['socialMediaReport'],
      },
    },
    getFacebookReportHandler
  );
  server.post(
    SOCIALMEDIA.GET_FACEBOOK_AGENT_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get facebook Agent summary',
        description: 'get facebook Agent summary api',
        tags: ['socialMediaReport'],
      },
    },
    getFacebookAgentReportHandler

  );
  server.post(
    SOCIALMEDIA.GET_FACEBOOK_AGENT_ACW_REPORT,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get facebook Agent ACW summary',
        description: 'get facebook Agent ACW summary api',
        tags: ['socialMediaReport'],
      },
    },
    getFacebookAgentACWReportHandler
  );

  next();
}
