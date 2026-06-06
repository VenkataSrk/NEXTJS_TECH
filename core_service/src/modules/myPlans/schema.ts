// /**
//   * @createdBy <phpteam@vectone.com>
//   * @createdOn 30th May 2022
// */

// export const usersGetUserExtensionInfoSchema = {
//   body: {
//     type: 'object',
//     required: ['company_id'],
//     properties: {
//       company_id: { type: 'number' },
//       orderid: { type: 'number' },
//       extension_number: { type: 'number' },
//     }
//   },
//   response: {
//     200: {
//       type: 'object',
//       properties: {
//         status_code: { type: 'number' },
//         err_code: { type: 'number' },
//         affected_rows: { type: 'number' },
//         message: { type: 'string' },
//         finalresponse: {
//           type: 'array',
//           items: {
//             properties: {
//               Basket_id: { type: 'number' },
//               Company_id: { type: 'number' },
//               Extension_Number: { type: 'string' },
//               Firstname: { type: 'string' },
//               Surname: { type: 'string' },
//               Department: { type: 'string' },
//               Email: { type: 'string' },
//               Mobileno: { type: 'string' },
//               is_mapped: { type: 'number' },
//               Direct_Number: { type: 'string' },
//               User_Language: { type: 'string' },
//               Greeting_Language: { type: 'string' },
//               Regional_Language: { type: 'string' },
//               Role_Name: { type: 'string' },
//               Status: { type: 'number' },
//               Record_usename_filepath: { type: 'string' },
//               Home_Country_Code: { type: 'string' },
//               Time_format: { type: 'number' },
//               Time_zone: { type: 'string' },
//               Recorded_by: { type: 'number' },
//               Recorded_type: { type: 'number' },
//               Recorded_greeting: { type: 'string' },
//               extn_audio_file: { type: 'string' },
//               extn_name_audio_file: { type: 'string' },
//               working_business_type: { type: 'number' },
//               app_login_user_name: { type: 'string' },
//               emp_id: { type: 'string' },
//               app_login_pwd: { type: 'string' },
//               company_name: { type: 'string' },
//               site_id: { type: 'number' },
//               site_name: { type: 'string' },
//               Hubspot_contactid: { type: 'number' },
//               user_type_desc: { type: 'number' },
//               user_type: { type: 'number' },
//               phone_id: { type: 'string' },
//               direct_no_display: { type: 'string' },
//               directno_info: { type: 'string' },
//               UR_Mobileno: { type: 'string' },
//               UR_Mobileno_display: { type: 'string' },
//               Role_Id: { type: 'number' },
//               errcode: { type: 'number' },
//               errmsg: { type: 'string' },
//             }
//           }
//         }
//       }
//     }
//   }
// };

// // companyUserOrderExtensionSchema
// export const companyUserOrderExtensionSchema = {
//   body: {
//     type: 'object',
//     required: ['OrderId', 'Order_Type'],
//     properties: {
//       OrderId: { type: 'number' },
//       Order_Type: { type: 'number' },
//       user_status: { type: 'string' },
//       user_role: { type: 'string' },
//       user_dept: { type: 'string' },
//       usertype: { type: 'string' },
//       country_id: { type: 'number' },
//       area_code: { type: 'string' },
//       Number_Type: { type: 'string' },
//     }
//   },
//   response: {
//     200: {
//       type: 'object',
//       properties: {
//         status_code: { type: 'number' },
//         err_code: { type: 'number' },
//         affected_rows: { type: 'number' },
//         message: { type: 'string' },
//         finalresponse: {
//           type: 'array',
//           items: {
//             properties: {
//               status: { type: 'number' },
//               position: { type: 'string' },
//               direct_number_display: { type: 'string' },
//               did_number_display: { type: 'string' },
//               name: { type: 'string' },
//               orderId: { type: 'string' },
//               domain_id: { type: 'string' },
//               email: { type: 'string' },
//               ext_number: { type: 'string' },
//               user_role: { type: 'string' },
//               department: { type: 'string' },
//               order_item_status_idx: { type: 'number' },
//               order_item_status: { type: 'number' },
//               mac_address: { type: 'string' },
//               provision_file: { type: 'string' },
//               account_plan_id: { type: 'string' },
//               mobileno: { type: 'string' },
//               direct_number: { type: 'string' },
//               emp_id: { type: 'string' },
//               is_mapped: { type: 'string' },
//               dp_password: { type: 'string' },
//               did_number: { type: 'string' },
//               phone_name: { type: 'string' },
//               user_type: { type: 'string' },
//               site_id: { type: 'string' },
//               site_name: { type: 'string' },
//               Hubspot_contact_id: { type: 'string' },
//               onboarding_flag: { type: 'string' },
//               User_Type: { type: 'string' },
//               country: { type: 'string' },
//               did_number_diplay: { type: 'string' },
//               SIP_ID: { type: 'string' },
//               Hubspot_contactid: { type: 'string' },
//               errcode: { type: 'number' },
//               errmsg: { type: 'string' },

//             }
//           }
//         }
//       }
//     }
//   }
// };

// /**
//  * UR_Myacc_Web_user_order_extension
//  */
// export interface companyUserOrderExtensionInterface {
//   OrderId: number;
//   Order_Type: number;
//   user_status: string;
//   user_role: string;
//   user_dept: string;
//   usertype: string;
//   country_id: number;
//   area_code: string;
//   Number_Type: string;
// }

// // billingGetInvoiceSummarySchema
// export const billingGetInvoiceSummarySchema = {
//   body: {
//     type: 'object',
//     properties: {
//       company_id: { type: 'number' },
//       Search_type: { type: 'string' },
//       status_type: { type: 'number' },
//       invoice_id: { type: 'number' }
//     }
//   },
//   response: {
//     200: {
//       type: 'object',
//       properties: {
//         statusCode: { type: 'number' },
//         message: { type: 'string' },
//         finalresponse: {
//           type: 'array',
//           items: {
//             properties: {
//               postcode: { type: 'string' },
//               last4digit_cc: { type: 'string' },
//               invoice_id: { type: 'string' },
//               bill_date: { type: 'string' },
//               bill_duration: { type: 'string' },
//               bill_due_date: { type: 'string' },
//               reference_no: { type: 'string' },
//               description: { type: 'string' },
//               charge: { type: 'string' },
//               adjustment: { type: 'string' },
//               tax_percentage: { type: 'string' },
//               tax_fee: { type: 'string' },
//               total_bill: { type: 'string' },
//               total_paid: { type: 'string' },
//               pay_type: { type: 'string' },
//               payment_date: { type: 'string' },
//               payment_reference: { type: 'string' },
//               comp_name: { type: 'string' },
//               comp_regno: { type: 'string' },
//               comp_address: { type: 'string' },
//               comp_building: { type: 'string' },
//               comp_street: { type: 'string' },
//               comp_city: { type: 'string' },
//               comp_country: { type: 'string' },
//               service_plan: { type: 'string' },
//               total_delivery_fee: { type: 'string' },
//               type: { type: 'string' },
//               billing_type: { type: 'string' },
//               status: { type: 'string' },
//               current_plan_price: { type: 'string' }
//             }
//           }
//         }
//       }
//     }
//   }
// };

// export const getBillServicePlanSchema = {

//   body: {
//     type: 'object',
//     properties: {
//       company_id: { type: 'number' }
//     },
//   },
//   response: {
//     200: {
//       type: 'object',
//       properties: {
//         statusCode: { type: 'number' },
//         message: { type: 'string' },
//         result: {
//           type: 'array',
//           items: {
//             properties: {
//               plan_duration: { type: 'string' },
//               plan_duration_desc: { type: 'string' },
//               total_bill_tax: { type: 'string' },
//               prd_name: { type: 'string' },
//               pln_name: { type: 'string' },
//               pc_price: { type: 'string' },
//               price_per_user: { type: 'string' },
//               ord_user_range: { type: 'string' },
//               account_credit: { type: 'string' },
//               bill_cycle: { type: 'string' },
//               tot_localno: { type: 'string' },
//               localno_price: { type: 'string' },
//               contract_period: { type: 'string' },
//               bill_date: { type: 'string' },
//               bill_due_date: { type: 'string' },
//               bundle_plan: { type: 'string' },
//               bundle_mins_usage: { type: 'string' },
//               tot_inter_nat_no: { type: 'string' },
//               inter_nat_no_price: { type: 'string' },
//               tot_non_geo_no: { type: 'string' },
//               non_geo_no_price: { type: 'string' },
//               calling_credit: { type: 'string' },
//               threshold_limit: { type: 'string' },
//               bill_cycle_type: { type: 'string' },
//               Is_free_trail: { type: 'string' },
//               new_price_combined: { type: 'string' },
//               device_phone_count: { type: 'string' },
//               Calling_UK_mins: { type: 'string' },
//               Unlimited_inbound_conf: { type: 'string' },
//               Unlimited_inbound_conf_value: { type: 'string' },
//               next_billing_date: { type: 'string' },
//               payment_method: { type: 'string' },
//               cc_no: { type: 'string' },
//               next_month_bill: { type: 'string' },
//               auto_topup_flag: { type: 'string' },
//               plan_id: { type: 'string' },
//               auto_topup_amount: { type: 'string' },
//               freetrial_days: { type: 'string' },
//               prev_paid_bill: { type: 'string' },
//               current_plan_price: { type: 'string' },
//               addon_bun_min_avail: { type: 'string' },
//               is_multi_ivr: { type: 'string' },
//               min_user: { type: 'number' },
//               errcode: { type: 'number' },
//               errmsg: { type: 'string' },

//             }
//           }
//         }
//       }
//     }
//   },
// };

// export const getAddonListSchema = {
//   body: {
//     type: 'object',
//     properties: {
//       company_id: { type: 'number' },

//     }
//   }
// };
