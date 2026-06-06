import { logger } from '../../src/plugins/log';
import { UpdateEmailSentForPlans, UpdateEmailSentForSessions } from '../dao/email.dao';
import { htmlContentData } from '../helpers/utils';
import { htmlPlanActive } from './../helpers/utils';

const nodemailer = require('nodemailer');

class SmtpConfigLeadConfig {
  public server: any;
  public imap: any;
  public simpleParser: any;

  constructor() {
    this.server = nodemailer.createTransport({
      host: process.env.SMTP_HOST,
      port: process.env.SMTP_PORT,
      secure: false,
      auth: {
        user: process.env.SMTP_USER,
        pass: process.env.SMTP_PASSWORD,
      },
      tls: {
        minVersion: 'TLSv1',
        rejectUnauthorized: false,
      },
    });
  }

  public async getServer() {
    return this.server;
  }
}

export const sendMailForLead = async (data: any) => {
  const ob = new SmtpConfigLeadConfig();
  const smtp = await ob.getServer();
  try {
    logger.info('data-=-=-=', data);
    let res: any = null;
    res = await smtp.sendMail({
      from: data.FromEmail,
      to: data.ToEmail,
      cc: data.cc,
      bcc: data.bcc,
      subject: data.EmailSubject,
      html: data.text,
      attachments: data?.attachments ?? []
    },                        (error: any, info: any) => {
      smtp.close();
      logger.info('smtp.close 1 error', error);
    });
  } catch (err) {
    smtp.close();
    logger.info(err);
  }
};

/**
 *
 * @param data
 */
export const sendMailRouteMobile = async (data: any) => {
  const ob = new SmtpConfigLeadConfig();
  const smtp = await ob.getServer();
  try {
    const htmlContent = `<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional //EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xmlns:o="urn:schemas-microsoft-com:office:office" xmlns:v="urn:schemas-microsoft-com:vml" lang="en"><head><meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
    <title>Company whats-app Integration</title>
    <meta property="og:title" content="Company whats-app Integration">
    <meta name="twitter:title" content="Company whats-app Integration">
<meta name="x-apple-disable-message-reformatting">
<meta http-equiv="X-UA-Compatible" content="IE=edge">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
    <!--[if gte mso 9]>
  <xml>
      <o:OfficeDocumentSettings>
      <o:AllowPNG/>
      <o:PixelsPerInch>96</o:PixelsPerInch>
      </o:OfficeDocumentSettings>
  </xml>
  <style>
    ul > li {
      text-indent: -1em;
    }
  </style>
<![endif]-->
<!--[if mso]>
<style type="text/css">
 body, td {font-family: Arial, Helvetica, sans-serif;}
 .hse-body-wrapper-table {background-color: #EAF0F6;}
</style>
<![endif]-->
<style type="text/css">.moz-text-html .hse-column-container{max-width:600px !important;width:600px !important}
.moz-text-html .hse-column{display:table-cell;vertical-align:top}.moz-text-html .hse-section .hse-size-12{max-width:600px !important;width:600px !important}
@media only screen and (min-width:640px){.hse-column-container{max-width:600px !important;width:600px !important}
.hse-column{display:table-cell;vertical-align:top}.hse-section .hse-size-12{max-width:600px !important;width:600px !important}
}@media only screen and (max-width:639px){img.stretch-on-mobile,.hs_rss_email_entries_table img,.hs-stretch-cta .hs-cta-img{height:auto !important;width:100% !important}
.display_block_on_small_screens{display:block}.hs_padded{padding-left:20px !important;padding-right:20px !important}
.hs-hm,table.hs-hm{display:none}.hs-hd{display:block !important}table.hs-hd{display:table !important}
}</style><!--<![endif]--><style type="text/css">#hs_body #hs_cos_wrapper_main a[x-apple-data-detectors]{color:inherit !important;text-decoration:none !important;font-size:inherit !important;font-family:inherit !important;font-weight:inherit !important;line-height:inherit !important}
a{text-decoration:underline}p{margin:0}body{-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%;-webkit-font-smoothing:antialiased;moz-osx-font-smoothing:grayscale}</style></head>
  <body id="hs_body" bgcolor="#EAF0F6" style="margin:0 !important; padding:0 !important; font-family:Arial, sans-serif; font-size:15px; color:#23496d; word-break:break-word">
<!--[if gte mso 9]>
<v:background xmlns:v="urn:schemas-microsoft-com:vml" fill="t">
    <v:fill type="tile" size="100%,100%" color="#ffffff"/>
</v:background>
<![endif]-->
    <div class="hse-body-background" lang="en" style="background-color:#eaf0f6" bgcolor="#eaf0f6">
      <table role="presentation" class="hse-body-wrapper-table" cellpadding="0" cellspacing="0" style="border-spacing:0 !important; border-collapse:collapse; mso-table-lspace:0pt; mso-table-rspace:0pt; margin:0; padding:0; width:100% !important; min-width:320px !important; height:100% !important" width="100%" height="100%">
        <tbody><tr>
          <td class="hse-body-wrapper-td" valign="top" style="border-collapse:collapse; mso-line-height-rule:exactly; font-family:Arial, sans-serif; font-size:15px; color:#23496d; word-break:break-word; padding-top:20px">
            <div id="hs_cos_wrapper_main" class="hs_cos_wrapper hs_cos_wrapper_widget hs_cos_wrapper_type_dnd_area" style="color: inherit; font-size: inherit; line-height: inherit;" data-hs-cos-general-type="widget" data-hs-cos-type="dnd_area">
  <div id="section-0" class="hse-section hse-section-first" style="padding-left:10px; padding-right:10px">
    <!--[if !((mso)|(IE))]><!-- -->
      <div class="hse-column-container" style="min-width:280px; max-width:600px; Margin-left:auto; Margin-right:auto; border-collapse:collapse; border-spacing:0; background-color:#ffffff" bgcolor="#ffffff">
    <!--<![endif]-->

    <!--[if (mso)|(IE)]>
      <div class="hse-column-container" style="min-width:280px;max-width:600px;width:100%;Margin-left:auto;Margin-right:auto;border-collapse:collapse;border-spacing:0;">
      <table align="center" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px;" cellpadding="0" cellspacing="0" role="presentation" width="600" bgcolor="#ffffff">
      <tr style="background-color:#ffffff;">
    <![endif]-->
    <!--[if (mso)|(IE)]>
  <td valign="top" style="width:600px;">
<![endif]-->
<!--[if gte mso 9]>
  <table role="presentation" width="600" cellpadding="0" cellspacing="0" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px">
<![endif]-->
<div id="column-0-0" class="hse-column hse-size-12">
  <div id="hs_cos_wrapper_module-0-0-0" class="hs_cos_wrapper hs_cos_wrapper_widget hs_cos_wrapper_type_module" style="color: inherit; font-size: inherit; line-height: inherit;" data-hs-cos-general-type="widget" data-hs-cos-type="module"><table style="border-spacing:0 !important; border-collapse:collapse; mso-table-lspace:0pt; mso-table-rspace:0pt; WIDTH:100%; MIN-WIDTH:100%" cellspacing="0" cellpadding="0" width="100%" border="0"><tbody>
<tr>
<td style="border-collapse:collapse; mso-line-height-rule:exactly; font-family:Arial, sans-serif; font-size:15px; color:#23496d; word-break:break-word; FONT-SIZE:4px; WIDTH:100%; MIN-WIDTH:100%; LINE-HEIGHT:6px; BACKGROUND-COLOR:#00e79f" valign="middle" width="100%" align="center" cellpadding="0"><span style="FONT-SIZE: 0px; WIDTH: 100%; BACKGROUND: none transparent scroll repeat 0% 0%; MIN-WIDTH: 100%; TEXT-OVERFLOW: ellipsis; COLOR: transparent; PADDING-BOTTOM: 0px; -MS-TEXT-SIZE-ADJUST: 100%; PADDING-TOP: 0px; PADDING-LEFT: 0px; MARGIN: -1px 0px 0px; PADDING-RIGHT: 0px; -webkit-text-size-adjust: 100%; opacity: 0; user-select: none; -moz-user-select: none; -ms-user-select: none; -webkit-user-select: none; overlfow: hidden">&nbsp;</span></td></tr>
</tbody></table></div>
</div>
<!--[if gte mso 9]></table><![endif]-->
<!--[if (mso)|(IE)]></td><![endif]-->
    <!--[if (mso)|(IE)]></tr></table><![endif]-->
    </div>
  </div>
  <div id="section_16830151099893" class="hse-section" style="padding-left:10px; padding-right:10px">
    <!--[if !((mso)|(IE))]><!-- -->
      <div class="hse-column-container" style="min-width:280px; max-width:600px; Margin-left:auto; Margin-right:auto; border-collapse:collapse; border-spacing:0; background-color:#000000" bgcolor="#000000">
    <!--<![endif]-->
    <!--[if (mso)|(IE)]>
      <div class="hse-column-container" style="min-width:280px;max-width:600px;width:100%;Margin-left:auto;Margin-right:auto;border-collapse:collapse;border-spacing:0;">
      <table align="center" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px;" cellpadding="0" cellspacing="0" role="presentation" width="600" bgcolor="#000000">
      <tr style="background-color:#000000;">
    <![endif]-->
    <!--[if (mso)|(IE)]>
  <td valign="top" style="width:600px;">
<![endif]-->
<!--[if gte mso 9]>
  <table role="presentation" width="600" cellpadding="0" cellspacing="0" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px">
<![endif]-->
<div id="column-0-0" class="hse-column hse-size-12">
  <div id="hs_cos_wrapper_module_16830151099882" class="hs_cos_wrapper hs_cos_wrapper_widget hs_cos_wrapper_type_module" style="color: inherit; font-size: inherit; line-height: inherit;" data-hs-cos-general-type="widget" data-hs-cos-type="module"><table class="hse-image-wrapper" role="presentation" width="100%" cellpadding="0" cellspacing="0" style="border-spacing:0 !important; border-collapse:collapse; mso-table-lspace:0pt; mso-table-rspace:0pt">
  <tbody>
    <tr>
      <td class="hs_padded" align="center" valign="top" style="border-collapse:collapse; mso-line-height-rule:exactly; font-family:Arial, sans-serif; color:#23496d; word-break:break-word; text-align:center; padding:20px 20px 16px; font-size:0px">
        <a href="http://www.worktual.co.uk/" target="_blank" style="color:#00a4bd; mso-line-height-rule:exactly" data-hs-link-id="0">
        <img alt="Worktual_Logo" src="https://qaurstorage.worktual.co.uk/userdetails_myaccount/cbd06eab-5968-44c9-aac2-8f55c65b8251-white logo.png" style="outline:none; text-decoration:none; -ms-interpolation-mode:bicubic; border:none; max-width:100%; font-size:16px" width="180" align="middle">
        </a>
      </td>
    </tr>
  </tbody>
</table></div>
</div>
<!--[if gte mso 9]></table><![endif]-->
<!--[if (mso)|(IE)]></td><![endif]-->
    <!--[if (mso)|(IE)]></tr></table><![endif]-->
    </div>
  </div>
  <div id="section-2" class="hse-section" style="padding-left:10px; padding-right:10px">
    <!--[if !((mso)|(IE))]><!-- -->
      <div class="hse-column-container" style="min-width:280px; max-width:600px; Margin-left:auto; Margin-right:auto; border-collapse:collapse; border-spacing:0; background-color:#ffffff; padding-bottom:30px" bgcolor="#ffffff">
    <!--<![endif]-->
    <!--[if (mso)|(IE)]>
      <div class="hse-column-container" style="min-width:280px;max-width:600px;width:100%;Margin-left:auto;Margin-right:auto;border-collapse:collapse;border-spacing:0;">
      <table align="center" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px;" cellpadding="0" cellspacing="0" role="presentation" width="600" bgcolor="#ffffff">
      <tr style="background-color:#ffffff;">
    <![endif]-->
    <!--[if (mso)|(IE)]>
  <td valign="top" style="width:600px;padding-bottom:30px;">
<![endif]-->
<!--[if gte mso 9]>
  <table role="presentation" width="600" cellpadding="0" cellspacing="0" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px">
<![endif]-->
<div id="column-2-0" class="hse-column hse-size-12">
<table role="presentation" cellpadding="0" cellspacing="0" width="100%" style="border-spacing:0 !important; border-collapse:collapse; mso-table-lspace:0pt; mso-table-rspace:0pt"><tbody><tr><td class="hs_padded" style="border-collapse:collapse; mso-line-height-rule:exactly; font-family:Arial, sans-serif; font-size:15px; color:#23496d; word-break:break-word; padding:10px 40px"><div id="hs_cos_wrapper_module_16776916590521" class="hs_cos_wrapper hs_cos_wrapper_widget hs_cos_wrapper_type_module" style="color: inherit; font-size: inherit; line-height: inherit;" data-hs-cos-general-type="widget" data-hs-cos-type="module"><div id="hs_cos_wrapper_module_16776916590521_" class="hs_cos_wrapper hs_cos_wrapper_widget hs_cos_wrapper_type_rich_text" style="color: inherit; font-size: inherit; line-height: inherit;" data-hs-cos-general-type="widget" data-hs-cos-type="rich_text"><p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%"><span style="font-family: Helvetica, Arial, sans-serif; color: #000000;">Dear Team,</span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:175%"><span style="color: #000000;">Please find the details for onboarding Worktual.</span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:100%">&nbsp;</p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:175%; font-weight:bold"><span style="color: #000000;">Company Details</span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Registered company name&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;: ${data?.businessName}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Business vertical&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;: ${data?.industry}</span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Company website&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;: ${data?.websiteURL1}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;"><span style="font-weight: normal; color: #000000;">&nbsp;<span style="font-weight: bold;">Contact Details</span></span></span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Business Unit&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; : ${data?.businessUnits}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Contact Name&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;: ${data?.contactName}</span> <span style="font-weight: normal;">   </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Contact Designation&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; : ${data?.ContactDesignation}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Contact Email Address&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; : ${data?.contactEmailAddress}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Callback URL&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;: ${data?.callbackURL}</span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;">Whatsapp Deployment Details</span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Launch Regions&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; : ${data?.launchRegions}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Whatsapp phone number&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; : ${data?.whatsappPhoneNumber}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Whatsapp Conversation volume&nbsp; &nbsp;: ${data?.whatsappConversationVolume}</span> <span style="font-weight: normal;"> </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:100%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">&nbsp; </span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">KYC Document&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;:   &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;&nbsp;</span></span></p>
<p style="mso-line-height-rule:exactly; font-size:15px; line-height:300%; font-weight:bold"><span style="color: #000000; font-family: Helvetica, Arial, sans-serif;"><span style="font-weight: normal;">Download link for signed Exhibit B document : ${data?.downloadLinkForSignedExhibitBdocument}</span> <span style="font-weight: normal;"> </span></span></p></div></div></td></tr></tbody></table>
</div>
<!--[if gte mso 9]></table><![endif]-->
<!--[if (mso)|(IE)]></td><![endif]-->
    <!--[if (mso)|(IE)]></tr></table><![endif]-->
    </div>
  </div>
  <div id="section-3" class="hse-section" style="padding-left:10px; padding-right:10px">
    <!--[if !((mso)|(IE))]><!-- -->
      <div class="hse-column-container" style="min-width:280px; max-width:600px; Margin-left:auto; Margin-right:auto; border-collapse:collapse; border-spacing:0; background-color:#ffffff" bgcolor="#ffffff">
    <!--<![endif]-->
    <!--[if (mso)|(IE)]>
      <div class="hse-column-container" style="min-width:280px;max-width:600px;width:100%;Margin-left:auto;Margin-right:auto;border-collapse:collapse;border-spacing:0;">
      <table align="center" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px;" cellpadding="0" cellspacing="0" role="presentation" width="600" bgcolor="#ffffff">
      <tr style="background-color:#ffffff;">
    <![endif]-->
    <!--[if (mso)|(IE)]>
  <td valign="top" style="width:600px;">
<![endif]-->
<!--[if gte mso 9]>
  <table role="presentation" width="600" cellpadding="0" cellspacing="0" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;width:600px">
<![endif]-->
<div id="column-3-0" class="hse-column hse-size-12">
  <table role="presentation" cellpadding="0" cellspacing="0" width="100%" style="border-spacing:0 !important; border-collapse:collapse; mso-table-lspace:0pt; mso-table-rspace:0pt"><tbody><tr><td class="hs_padded" style="border-collapse:collapse; mso-line-height-rule:exactly; font-family:Arial, sans-serif; font-size:15px; color:#23496d; word-break:break-word; padding:10px 20px 30px 40px"><div id="hs_cos_wrapper_module-3-0-0" class="hs_cos_wrapper hs_cos_wrapper_widget hs_cos_wrapper_type_module" style="color: inherit; font-size: inherit; line-height: inherit;" data-hs-cos-general-type="widget" data-hs-cos-type="module"><div id="hs_cos_wrapper_module-3-0-0_" class="hs_cos_wrapper hs_cos_wrapper_widget hs_cos_wrapper_type_rich_text" style="color: inherit; font-size: inherit; line-height: inherit;" data-hs-cos-general-type="widget" data-hs-cos-type="rich_text"><h2 style="margin:0; mso-line-height-rule:exactly; font-size:14px; line-height:100%; font-weight:bold"><span style="color: #1a1a1a;">Thank You!</span></h2>
<p style="mso-line-height-rule:exactly; font-size:13px; line-height:115%">&nbsp;</p>
<h2 style="margin:0; mso-line-height-rule:exactly; font-size:13px; line-height:175%; font-weight:normal"><span style="font-family: Helvetica, Arial, sans-serif; color: #1a1a1a;">Regards,</span></h2>
<h2 style="margin:0; mso-line-height-rule:exactly; font-size:13px; line-height:175%; font-weight:normal"><span style="color: #1a1a1a;">Team Worktual</span></h2></div></div></td></tr></tbody></table>
</div>
<!--[if gte mso 9]></table><![endif]-->
<!--[if (mso)|(IE)]></td><![endif]-->
    <!--[if (mso)|(IE)]></tr></table><![endif]-->
    </div>
  </div>
<div id="section_hubspot_brand" class="hse-section branding-design-1 branding-box-design " style="padding-left:10px; padding-right:10px">
    <!--[if !((mso)|(IE))]><!-- -->
      <div class="hse-column-container" style="min-width:280px; max-width:600px; Margin-left:auto; Margin-right:auto; border-collapse:collapse; border-spacing:0; padding-bottom:20px; padding-top:20px">
    <!--<![endif]-->
    <!--[if (mso)|(IE)]>
      <div class="hse-column-container" style="min-width:280px;max-width:600px;border-collapse:collapse;border-spacing:0;">
      <table align="center" style="border-collapse:collapse;mso-table-lspace:0pt;mso-table-rspace:0pt;" cellpadding="0" cellspacing="0" role="presentation">
      <tr>
      <td valign="top" style="width:600px;padding-bottom:20px;padding-top:20px">
    <![endif]-->
<div id="column_hubspot_brand" class="hse-column hse-size-12">
  <div id="hs_cos_wrapper_module_hubspot_brand">
  <table role="presentation" width="100%" cellpadding="0" cellspacing="0" style="border-spacing:0 !important; border-collapse:collapse; mso-table-lspace:0pt; mso-table-rspace:0pt">
      <tbody>
      <tr><td class="moduleAlignmentCell" align="center" style="border-collapse:collapse; mso-line-height-rule:exactly; font-family:Arial, sans-serif; font-size:15px; color:#23496d; word-break:break-word">
        <img src="./Whatsapp Integration New_files/tracking" style="outline:none; text-decoration:none; -ms-interpolation-mode:bicubic; display:none">
      </td></tr>
      </tbody>
  </table>
</div>
</div>
<!--[if (mso)|(IE)]></td></tr></table><![endif]-->
</div>
</div>
</div>
          </td>
        </tr>
      </tbody></table>
    </div>
</body></html>`;
    let res: any = null;
    res = await smtp.sendMail({
      from: data.FromEmail,
      to: data.ToEmail,
      subject: 'Whatsapp integration email template',
      html: htmlContent,
      attachments: data?.attachments ?? []
    },                        (error: any, info: any) => {
      smtp.close();
      logger.info('smtp.close 1 error', error);
    });
  } catch (err) {
    smtp.close();
    logger.info(err);
  }
};

/**
 * @param data
 */
export const sendMailTemplate = async (data: any, html: any) => {
  const ob = new SmtpConfigLeadConfig();
  const smtp = await ob.getServer();
  try {
    const htmlContent = `
      <!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
      <html xmlns="http://www.w3.org/1999/xhtml" xmlns:o="urn:schemas-microsoft-com:office:office" xmlns:v="urn:schemas-microsoft-com:vml" lang="en">
              <head>
                <meta charset="UTF-8">
                <meta name="x-apple-disable-message-reformatting">
                <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
                <meta http-equiv="X-UA-Compatible" content="IE=edge">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <!--[if gte mso 9]>
                <xml>
                <o:OfficeDocumentSettings>
                <o:AllowPNG/>
                <o:PixelsPerInch>96</o:PixelsPerInch>
                </o:OfficeDocumentSettings>
                </xml>
                <![endif]-->
    <style>
          ${data?.text || ''}
    </style>
  </head>
     ${html || ''}
</html>`;
    let res: any = null;
    res = await smtp.sendMail({
      from: data?.FromEmail ?? null,
      to: data?.ToEmail ?? null,
      subject: data?.EmailSubject ?? null,
      html: htmlContent,
    });
    smtp.close();
    logger.info('Email sent successfully');
  } catch (err) {
    smtp.close();
    logger.error(err);
  }
};

/**
 * @param data
 */
export const sendEmailSessionExpiry = async (data: any, countData: any, getDatas: any) => {
  const ob = new SmtpConfigLeadConfig();
  const smtp = await ob.getServer();
  try {
    const subject = getDatas?.planActiveStatus === 1 ? getDatas?.is_reminder_for_date === 1
      ? 'Your Subscription Plan Has Expired – 7 Day Reminder'
      : 'Your Subscription Plan Has Expired'
      : data?.EmailSubject ?? null;

    let htmlContent: any;
    if (getDatas?.planActiveStatus === 1) {
      htmlContent = htmlPlanActive(countData, getDatas);
      UpdateEmailSentForPlans(countData);
    } else {
      htmlContent = htmlContentData(data, countData, getDatas);
      UpdateEmailSentForSessions(data, countData);
    }
    let res: any = null;
    res = await smtp.sendMail({
      from: data?.FromEmail,
      to: data?.ToEmail ?? null,
      cc: countData?.domain_id === 20598 ? ['genoj.c@pure-chemicals.com', 'anand@techfruitspl.com', 'k.jayashankar@worktual.com', 'technical.support@worktual.com'] : ['k.jayashankar@worktual.com', 'technical.support@worktual.com'],
      subject: subject ?? null,
      html: htmlContent,
    });
    smtp.close();
    logger.info('Email sent successfully');
  } catch (err) {
    smtp.close();
    logger.error(err);
  }
};

/**
 * @param data
 */
export const sendMailPlainText = async (data: any) => {
  const ob = new SmtpConfigLeadConfig();
  const smtp = await ob.getServer();
  try {

    let res: any = null;
    res = await smtp.sendMail({
      from: data?.FromEmail ?? null,
      to: data?.ToEmail ?? null,
      subject: data?.EmailSubject ?? null,
      html: data?.text ?? null,
    });
    smtp.close();
    logger.info('Email sent successfully');
  } catch (err) {
    smtp.close();
    logger.error(err);
  }
};
