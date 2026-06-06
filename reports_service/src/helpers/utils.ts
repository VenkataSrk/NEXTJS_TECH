import excel from 'excel4node';
import ExcelJS from 'exceljs';
import * as fastcsv from 'fast-csv';
import fs from 'fs';
import moment from 'moment-timezone';
import cron from 'node-cron';
import PDFDocument from 'pdfkit';
import { RRule } from 'rrule';
import { PassThrough } from 'stream';
import { getAgentCallSummaryReportForSchedule, getAgentLoginAndLogoutReportDaoForSchedule, getAgentPerformanceHandlerDaoForSchedule, getAgentStatusReportReportDaoForSchedule, getQueryHandlingTimeReportDaoForSchedule } from '../dao/agentReport.dao';
import { getAiBotPerformancesBotAgentReportDaoForSchedule, getAiBotPerformancesReportDaoForSchedule, getAiCallQualityReportDaoForSchedule, getAiChatQualityReportDaoForSchedule } from '../dao/aiBotReports.dao';
import { getEmailAgentReportHandlerDaOForSchedule, getEmailInterationReportDaoForSchedule, getEmailReportHandlerDaoForSchedule } from '../dao/emailReportdao';
import { getChatACWAgentReportHandlerDaoForSchedule } from '../dao/reportChat.dao';
import { getEmailConfigDao, getSmsAgentReportHandlerDaOForShedule, getSmsInterationReportDaoForSchedule, getSmsProfileReportHandlerDaoForSchedule } from '../dao/smsReportdao';
import { getfacebookAgentACWReportHandlerDaoForSchedule, getfacebookAgentReportHandlerDaoForSchedule, getFacebookReportHandlerDaoForSchedule, getInstagramacwReportHandlerDaoForSchedule, getInstagramAgentacwReportHandlerDaoForSchedule, getInstagramReportHandlerDaoForSchedule, getSocialMediaAgentReportHandlerDaoForSchedule, getSocialMediaProfileReportHandlerDaoForSchedule, getWhatsappACWReportHandlerDaoForSchedule } from '../dao/socialMediadao';
import { getStandAloneBotChatReportDaoForSchedule, getStandAloneChatReportDaoForSchedule } from '../dao/standAloneReportDao';
import { getAnsweredCallReportForSchedule, getCallBackSummaryDaoForSchedule, getTranferCallReportForSchedule, interActionDaoForSchedule, missedCallListDaoForSchedule, outboundCallReportForSchedule } from '../dao/voiceReport.dao';
import EmailConfig from '../service/email';

export const CronJob = async (data: any) => {
  try {
    if (data?.rrule) {
      const rule = RRule.fromString(data.rrule);
      await scheduleRRule(rule, data);
    } else {
      console.error('No rrule found. Please check the recurrence rule.');
      return;
    }
  } catch (err) {
    console.error('Error creating RRule or scheduling:', err);
  }
};

const scheduleRRule = async (rrule: any, data: any) => {
  try {
    // const nextDate = rrule.after(new Date());
    // if (!nextDate) {
    //   console.error('No next date found for the given rule.');
    //   return;
    // }
    // const timeZoneAdjustedDate = convertToTimeZone(nextDate, data.timezone);
    // const minutes = timeZoneAdjustedDate.getMinutes();
    // const hours = timeZoneAdjustedDate.getHours();
    // const day = timeZoneAdjustedDate.getDate();
    // const month = timeZoneAdjustedDate.getMonth() + 1;
    // const cronExpression = `${minutes} ${hours} ${day} ${month} *`;
    // console.log('Scheduled cron job with expression:', cronExpression, nextDate);

    // const inputStartTime = new Date(nextDate);
    // const inputTimestamp = inputStartTime.getTime();
    // let timeBeforerecurrenceHours: any;

    // if (data?.recurrence === 'Once') {
    //   timeBeforerecurrenceHours = inputTimestamp - (24 * 60 * 60 * 1000);
    // } else if (data?.recurrence === 'daily') {
    //   timeBeforerecurrenceHours = inputTimestamp - (24 * 60 * 60 * 1000);
    // } else if (data?.recurrence === 'weekly') {
    //   timeBeforerecurrenceHours = inputTimestamp - (7 * 24 * 60 * 60 * 1000);
    // } else if (data?.recurrence === 'monthly') {
    //   inputStartTime.setMonth(inputStartTime.getMonth() - 1);
    //   timeBeforerecurrenceHours = inputStartTime.getTime();
    // }

    const reportData: any = {
      startTmpstmp: data?.startDataRange ? data?.startDataRange : Date.now() - (24 * 60 * 60 * 1000),
      endTmpstmp: data?.endDataRange ? data?.endDataRange : Date.now(),
      timeZone: data.timezone,
      domainId: data?.domainId
    };
    const selectedReportData: any = data?.reportId === 1 ? await getAnsweredCallReportForSchedule(reportData) : data?.reportId === 2 ? await missedCallListDaoForSchedule(reportData) : data?.reportId === 3 ? await outboundCallReportForSchedule(reportData) : data?.reportId === 4 ? await interActionDaoForSchedule(reportData) :
      data?.reportId === 5 ? await getTranferCallReportForSchedule(reportData) : data?.reportId === 6 ? await getCallBackSummaryDaoForSchedule(reportData) : data?.reportId === 7 ? await getStandAloneChatReportDaoForSchedule(reportData) : data?.reportId === 8 ? await getStandAloneBotChatReportDaoForSchedule(reportData) :
        data?.reportId === 9 ? await getChatACWAgentReportHandlerDaoForSchedule(reportData) : data?.reportId === 10 ? await getEmailReportHandlerDaoForSchedule(reportData) : data?.reportId === 11 ? await getEmailAgentReportHandlerDaOForSchedule(reportData) : data?.reportId === 12 ? await getEmailInterationReportDaoForSchedule(reportData) :
          data?.reportId === 13 ? await getSmsProfileReportHandlerDaoForSchedule(reportData) : data?.reportId === 14 ? await getSmsAgentReportHandlerDaOForShedule(reportData) : data?.reportId === 15 ? await getSmsInterationReportDaoForSchedule(reportData) : data?.reportId === 16 ? await getSocialMediaProfileReportHandlerDaoForSchedule(reportData) :
            data?.reportId === 17 ? await getSocialMediaAgentReportHandlerDaoForSchedule(reportData) : data?.reportId === 18 ? await getWhatsappACWReportHandlerDaoForSchedule(reportData) : data?.reportId === 19 ? await getFacebookReportHandlerDaoForSchedule(reportData) : data?.reportId === 20 ? await getfacebookAgentReportHandlerDaoForSchedule(reportData) :
              data?.reportId === 21 ? await getfacebookAgentACWReportHandlerDaoForSchedule(reportData) : data?.reportId === 22 ? await getInstagramReportHandlerDaoForSchedule(reportData) : data?.reportId === 23 ? await getInstagramAgentacwReportHandlerDaoForSchedule(reportData) : data?.reportId === 24 ? await getInstagramacwReportHandlerDaoForSchedule(reportData) :
                data?.reportId === 25 ? await getAgentCallSummaryReportForSchedule(reportData) : data?.reportId === 26 ? await getAgentPerformanceHandlerDaoForSchedule(reportData) : data?.reportId === 27 ? await getQueryHandlingTimeReportDaoForSchedule(reportData) : data?.reportId === 28 ? await getAgentLoginAndLogoutReportDaoForSchedule(reportData) :
                  data?.reportId === 29 ? await getAgentStatusReportReportDaoForSchedule(reportData) : data?.reportId === 30 ? await getAiCallQualityReportDaoForSchedule(reportData) : data?.reportId === 31 ? await getAiChatQualityReportDaoForSchedule(reportData) : data?.reportId === 32 ? await getAiBotPerformancesBotAgentReportDaoForSchedule(reportData) :
                    data?.reportId === 33 ? await getAiBotPerformancesReportDaoForSchedule(reportData) : [];
    let reportCsvFile: any;
    let reportPdfFile: any;
    let reportXlsFile: any;
    let reportXlslFile: any;
    if (JSON.stringify(data?.fileType).includes('CSV') && selectedReportData?.length > 0) {
      reportCsvFile = await getReportsInCSVDao(selectedReportData);
    }

    if (JSON.stringify(data?.fileType).includes('PDF') && selectedReportData?.length > 0) {
      reportPdfFile = await createPDFBuffer(selectedReportData);
    }

    if (JSON.stringify(data?.fileType).includes('XLS') && selectedReportData?.length > 0) {
      reportXlsFile = await createExcelBuffer(selectedReportData);
    }

    if (JSON.stringify(data?.fileType).includes('XLSX') && selectedReportData?.length > 0) {
      reportXlslFile = await createXLSXBuffer(selectedReportData);
    }

    const getDomainEmail: any = await getEmailConfigDao(data?.domainId);
    const emailData: any = {
      from: getDomainEmail?.length > 0 ? getDomainEmail[0]?.emailAddress : '',
      to: data?.toAddr,
      subject: data?.subject,
      content: data?.message
    };
    const mail = new EmailConfig(emailData.from);
    mail.testMail({
      from: emailData.from,
      to: emailData.to,
      subject: emailData.subject,
      content: emailData.content,
      attachmentsData: reportCsvFile,
      attachmentsPdf: reportPdfFile,
      attachmentsXls: reportXlsFile,
      attachmentsXlsl: reportXlslFile
    });
    return mail;
    // cron.schedule(cronExpression, async () => {
    //   const emailData: any = {
    //     from: getDomainEmail?.length > 0 ? getDomainEmail[0]?.emailAddress : '',
    //     to: data?.toAddr,
    //     subject: data?.subject,
    //     content: data?.message
    //   };
    //   console.log('reportCsvFile:::::', reportCsvFile, reportPdfFile, reportXlsFile, reportXlslFile);
    //   const mail = new EmailConfig(emailData.from);
    //   mail.testMail({
    //     from: emailData.from,
    //     to: emailData.to,
    //     subject: emailData.subject,
    //     content: emailData.content,
    //     attachmentsData: reportCsvFile,
    //     attachmentsPdf: reportPdfFile,
    //     attachmentsXls: reportXlsFile,
    //     attachmentsXlsl: reportXlslFile
    //   });
    // });
  } catch (err) {
    console.error('Error scheduling RRule job:', err);
    return;
  }
};

export const formatLabel = (key: string): string => {
  return key
    .replace(/([a-z])([A-Z])/g, '$1 $2') // camelCase to spaced
    .replace(/^./, str => str.toUpperCase()); // Capitalize first letter
};

export const formatValue = (key: string, value: any): string => {
  if (value === null || value === undefined || value === '') return 'N/A';

  const lowerKey = key.toLowerCase();

  if ((lowerKey.includes('time') || lowerKey.includes('date')) && typeof value === 'number') {
    const date = new Date(value);
    return isNaN(date.getTime()) ? 'N/A' : date.toLocaleString();
  }

  if (typeof value === 'string' && value.endsWith('Z')) {
    const date = new Date(value);
    return isNaN(date.getTime()) ? value : date.toLocaleString();
  }

  return value.toString();
};

export const getReportsInCSVDao = async (data: any[]): Promise<Buffer> => {
  return new Promise((resolve, reject) => {
    const stream = new PassThrough();
    const chunks: Buffer[] = [];

    stream.on('data', chunk => chunks.push(chunk));
    stream.on('end', () => resolve(Buffer.concat(chunks)));
    stream.on('error', reject);

    if (data.length === 0) {
      fastcsv.writeToStream(stream, [], { headers: true }).on('error', reject);
      return;
    }

    const headers = Object.keys(data[0]);
    const csvHeaders = headers.map(formatLabel);
    const rows = data.map(row => headers.map(key => formatValue(key, row[key])));

    fastcsv.writeToStream(stream, [csvHeaders, ...rows], { headers: false }).on('error', reject);
  });
};

export const createPDFBuffer = async (data: any[]): Promise<Buffer> => {
  return new Promise((resolve, reject) => {
    try {
      if (!data || data.length === 0) {
        console.log('No data found for PDF generation.');
        return resolve(Buffer.from([]));
      }

      const doc = new PDFDocument({ margin: 50 });
      const buffers: Buffer[] = [];

      doc.on('data', chunk => buffers.push(chunk));
      doc.on('end', () => resolve(Buffer.concat(buffers)));

      doc.fontSize(18).text('Report Data', { align: 'center' }).moveDown(2);

      const keys = Object.keys(data[0]);

      data.forEach((item, index) => {
        doc.fontSize(12).text(`Entry ${index + 1}`, { underline: true }).moveDown(0.5);

        for (const key of keys) {
          const label = formatLabel(key);
          const value = formatValue(key, item[key]);
          doc.fontSize(11).text(`${label}: ${value}`);
        }

        doc.moveDown(1);
      });

      doc.end();
    } catch (err) {
      console.error('PDF generation failed:', err);
      reject(err);
    }
  });
};

export const createExcelBuffer = async (data: any[]): Promise<Buffer> => {
  const workbook = new ExcelJS.Workbook();
  const worksheet = workbook.addWorksheet('Report');

  if (data.length > 0) {
    const keys = Object.keys(data[0]);
    worksheet.addRow(keys.map(formatLabel));

    data.forEach((item:any) => {
      const row = keys.map(key => formatValue(key, item[key]));
      worksheet.addRow(row);
    });
  }
  const uint8array = await workbook.xlsx.writeBuffer();
  return Buffer.from(uint8array);
};

export const createXLSXBuffer = async (data: any[]): Promise<Buffer> => {

  const workbook = new ExcelJS.Workbook();
  const worksheet = workbook.addWorksheet('Report');

  if (data.length > 0) {
    const keys = Object.keys(data[0]);
    worksheet.addRow(keys.map(formatLabel));

    data.forEach((item:any) => {
      const row = keys.map(key => formatValue(key, item[key]));
      worksheet.addRow(row);
    });
  }

  const uint8Array = await workbook.xlsx.writeBuffer();
  return Buffer.from(uint8Array); // Ensure Buffer type for Node.js
};
