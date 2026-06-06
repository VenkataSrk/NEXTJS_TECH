import { v4 as uuidv4 } from 'uuid';
import { getAgentCount, getHours, getHoursofOperationNew, getQueueSettings, getRoutingProfile } from './dao/callflow.dao';
import { logger } from './plugins/log';
import { loggerError } from './plugins/logger';

// INFO: UPDATE WAITING QUEUE LIST
export const transferUserInsideQueue = async (adaptor: any, channel: any, targetNode: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      console.log('TRANSFER TO QUEUE ID :::::::::', targetNode);
      const queueSettings: any = await getQueueSettings(targetNode.data?.information?.transferTo);
      console.log('QUEUE SETTINGS :::::::::', queueSettings);
      const agentCount: any = await getAgentCount(targetNode.data?.information?.transferTo);
      console.log('AGENT COUNT :::::::::', agentCount);
      const validateHours: any = await getHoursofOperationNew(targetNode.data?.information?.transferTo);
      const rp: any = await getRoutingProfile(queueSettings.domainId, targetNode.data?.information?.transferTo);
      console.log('VALIDATE BUSINESS HOURS ::::::::', validateHours, rp);
      // const validateHours: any = true;
      if (queueSettings) {
        const getTime: any = await getHours(targetNode.data?.information?.transferTo);
        const queueData = {
          ...queueSettings,
          settingAudio: JSON.parse(queueSettings.settingAudio ?? 'null'),
          announcement: JSON.parse(queueSettings.announcement ?? 'null'),
          maximumCallers: JSON.parse(queueSettings.maximumCallers ?? 'null'),
          maximumWaitTime: JSON.parse(queueSettings.maximumWaitTime ?? 'null'),
          outOfBusinessHours: JSON.parse(queueSettings.outOfBusinessHours ?? 'null'),
          businessStartTime: `${getTime?.startTimeStamp ?? ''}`,
          businessEndTime: `${getTime?.endTimeStamp ?? ''}`,
        };
        targetNode.data.information.routingProfile = rp;
        targetNode.data.information.queueSettings = queueData;
        targetNode.data.information.queueSettings.hoursOfOperation = validateHours ? 1 : 0;
        targetNode.data.information.queueSettings.agentCount = agentCount;
        const targetRestructured = { ...targetNode, session_id: channel?.session_id, data: { ...targetNode?.data?.information } };
        adaptor.publishMessage(JSON.stringify(targetRestructured));
        resolve(true);
      } else {
        reject(false);
      }
    } catch (error) {
      loggerError('transferUserInsideQueue', { channel, targetNode }, error);
      reject(false);
    }
  });
};

export const contextPattern = /\${(.*?)}/g;

export const replacePatternWithVariables = (availableContexts: any, text: any, context: any) => {
  try {
    let originalText = text;
    for (const ctx of availableContexts) {
      const ctxString = ctx.toString();
      const variableName = ctxString.slice(2, ctxString.length - 1);
      const variable = variableName.split('.')[1];
      const specialPattern = /[${}]/g;
      originalText = originalText.replace(specialPattern, '');
      const patternString = `\\bcontext.${variable}\\b`; // Creating pattern with a dynamic string
      const pattern = new RegExp(patternString, 'gi');
      originalText = originalText?.replace(pattern, context[variable]);
    }
    return originalText;
  } catch (error) {
    loggerError('replacePatternWithVariables', { availableContexts, text, context }, error);
  }
};

export const replaceContextWithVariables = (text: any, context: any) => {
  try {
    const originalText = text;
    const variableName = originalText?.slice(2, originalText?.length - 1);
    const variable = variableName.split('.')[1];
    if (context[variable]) {
      return context[variable];
    }
    return variable;
  } catch (error) {
    loggerError('replaceContextWithVariables', { text, context }, error);
  }
};

export const replaceContextWithRightVariables = (text: any, context: any) => {
  try {
    if (context[text]) {
      return context[text];
    }
    return text;
  } catch (error) {
    loggerError('replaceContextWithRightVariables', { text, context }, error);
  }
};
