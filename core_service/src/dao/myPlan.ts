// import { companyUserOrderExtensionInterface } from '../../src/modules/myPlans/schema';
import { mysqlPoolConnection, mysqlPoolConnectionRole, mysqlPoolConnectionWorktual } from '../../src/plugins/db';
// import { usersGetUserExtensionInfoInterface } from '../modules/myPlans/interface';

export const usersGetUserExtensionInfoModel = (data: any) => {
  return new Promise((resolve, reject) => {
    mysqlPoolConnectionRole.query(
      'CALL UR_Myacc_Web_get_user_extension_info (?,?,?)', [
        data.company_id,
        data.orderid,
        data.extension_number
      ],
      (err: any, result: any) => {
        // console.log("result----->",result)
        if (err) {
          console.log('err:', err);
          return reject(err);
        }
        return resolve(result[0]);
      }
    );
  });
};

export const companyUserOrderExtensionModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL UR_Myacc_Web_user_order_extension (?,?,?,?,?,?,?,?,?)', [
        data.OrderId,
        data.Order_Type,
        data.user_status,
        data.user_role,
        data.user_dept,
        data.usertype,
        data.country_id,
        data.area_code,
        data.Number_Type
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {

          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const billingGetInvoiceSummaryModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL ur_bill_get_customer_invoice_summary(?,?,?,?,?,?,?)', [
        data.company_id,
        data.invoice_id,
        data.Search_type,
        data.status_type,
        data.datefilter,
        data.fromdate,
        data.todate
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          resolve(result);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getBillServicePlanModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL wtms_bill_get_service_plan (?)', [
        data.company_id,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
        } else {
          console.log('results:', result[0]);
          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getAddonListModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL worktual_myaccount_get_addon_list (?)', [
        data.company_id,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {

          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

// anbu

export const getUserPasswordInfoFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    console.log(`getUserPasswordInfoFromDb> ${data.company_id},${data.email}`);
    try {
      mysqlPoolConnectionRole.query('CALL ur_get_user_password_info (?,?)', [
        data.company_id,
        data.email
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          console.log('getUserPasswordInfoFromDb>>', rows);
          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const syncMyAccountDetailsFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {

    try {
      mysqlPoolConnectionRole.query('CALL worktual_myaccount_get_sync_up_account_details(?,?)', [
        data.company_id,
        data.extension
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('syncMyAccountDetailsFromDb>>', result);
          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const createMyAccountFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {

    try {

      mysqlPoolConnectionRole.query(
        'CALL worktual_myaccount_create_sync_up_account_details(?,?,?,?,?,?,?,?,?,?)',
        [
          data.company_id,
          data.myaccount_email,
          data.social_account_email,
          data.processtype,
          data.social_account_type,
          data.login_source,
          data.login_device_id,
          data.login_ipaddress,
          data.extension,
          data.address,

        ],
        (err, result) => {
          if (err) {
            console.error('❌ [DB ERROR] while executing stored procedure:', err);
            resolve(err);
          } else {
            console.log('createMyAccountFromDbcreateMyAccountFromDb', result);
            resolve(result[0]);
          }
        }
      );
    } catch (err) {
      console.error('🔥 [EXCEPTION] Unexpected error in createMyAccountFromDb:', err);
      reject(err);
    }
  });
};

export const getUserDataPrivacySettingFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {

    try {
      mysqlPoolConnectionRole.query('CALL UR_get_user_data_privacy_settings_dtl(?)', [
        data.company_id,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('getUserDataPrivacySettingFromDb>>', result);

          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const insertupdateDataPrivacySettingFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL UR_insert_update_user_data_privacy_settings_dtl(?,?,?,?)', [
        data.company_id,
        data.UserEmail,
        data.isTwoStep,
        data.isContentAnlysis,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('insertupdateDataPrivacySettingFromDb>>', result);

          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getUserDevicesFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL UR_get_user_devices_dtl_v2(?,?,?)', [
        data.company_id,
        data.user_email,
        data.product_type
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('getUserDevicesFromDb>>', result);

          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getManagementFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL UR_insert_update_user_devices_dtl_v2(?,?,?,?,?,?,?,?,?,?)', [
        data.company_id,
        data.UserEmail,
        data.Device,
        data.Device_Model,
        data.OS_Type,
        data.Browser_Type,
        data.IP_Address,
        data.Location,
        data.is_active,
        data.product_type
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('getManagementFromDb>>', result);

          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getUserDepartmentFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      console.log('companyIdgetUserDepartmentFromDb', data.company_id);

      mysqlPoolConnectionRole.query(
        'CALL ur_ma_user_get_department(?)',
        [data.company_id],
        (err, result) => {
          if (err) {
            console.log('err:', err);
            return reject(err);
          }

          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          console.log('getUserDepartmentFromDb>>', rows);
          resolve(rows);
        }
      );
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getUserRolesFromDB = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      console.log(`companyIdproduct_id ${data.company_id},${data.product_id}`);
      mysqlPoolConnectionRole.query('CALL ur_get_user_roles(?,?)', [
        data.company_id,
        data.product_id,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('getUserRolesFromDB>>', result);
          const rows = Array.isArray(result) && Array.isArray(result[0]) ? result[0] : [];
          console.log('getUserRolesFromDB>>', rows);
          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const userGetDetailsFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      console.log('company_id', data.company_id);
      mysqlPoolConnectionRole.query('CALL ur_get_user_details(?)', [data.company_id], (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('userGetDetailsFromDb>>', result);
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          console.log('userGetDetailsFromDb>>', rows);
          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getUserLoginLogFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      console.log('dataa>>>>', data);
      mysqlPoolConnectionRole.query('CALL ur_get_ccaas_user_activity(?,?)', [
        data?.email,
        data.company_id,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
            Array.isArray(result) && Array.isArray(result[0])
              ? result[0]
              : result || [];

          console.log('getUserLoginLogFromDb>>>', rows);
          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getAllAddonListModel = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL ur_myacc_GetPurchasedAddonListByCompany(?)', [
        data?.company_id    ,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('result', result);
          resolve(result);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getRolesInfoFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL Worktual_get_RoleInfo_myPlan(?,?,?)', [
        data.RoleCategoryId,
        data.company_id,
        data.productId],            (err, result) => {
          if (err) {
            console.log('err:', err);
            resolve(err);
          } else {
            console.log('getRolesInfoFromDb>>>', result);

            resolve(result[0]);
          }
        });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const insertUpdateTwoStepVerificationDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL UR_insert_updated_security_two_step_verification(?,?,?,?,?,?,?)', [
        data.email,
        data.company_id,
        data.PhoneNumber,
        data.app_2fa_enabled,
        data.email_2fa_enabled,
        data.phone_2fa_enabled,
        data.password,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('insertUpdateTwoStepVerificationDao>>>', result);
          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getsecuritytwostepverificationFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL UR_get_security_two_step_verification(?)', [
        data.email,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
          Array.isArray(result) && Array.isArray(result[0])
            ? result[0]
            : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getAllMembersRoleDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL UR_get_all_members_role_dtl(?,?)', [
        data.companyId,
        data.roleId,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          const rows =
          Array.isArray(result) && Array.isArray(result[0])
            ? result[0]
            : result || [];

          resolve(rows);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const insertUserActivityLogFromDb = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_insert_activity_log(?,?,?,?)', [
        data.userId,
        data.roleId,
        data.domainId,
        data.messagelog,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('getManagementFromDb>>', result);
          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getUserogActivityLogDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_activity_log(?,?,?,?,?)', [
        data?.userId ?? null,
        data?.roleId ?? null,
        data?.domainId ?? null,
        data?.limit ?? null,
        data?.offset ?? null
      ],                        (err, result) => {
        if (err) {
          console.log('ccaas_get_activity_log err:', err);
          resolve(err);
        } else {
          const rows = Array.isArray(result) && Array.isArray(result[0]) ? result : result || [];
          resolve(rows);
        }
      });
    } catch (err) {
      console.log('ccaas_get_activity_log err:', err);
      reject(err);
    }
  });
};

export const insertDevicesDtlDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_insert_update_user_devices_dtl(?,?,?,?,?,?,?,?,?,?,?)', [
        data.companyId,
        data.userEmail,
        data.device,
        data.deviceModal,
        data.osType,
        data.browserType,
        data.ipAddress,
        data.location,
        data.isActive,
        data.productType,
        data.deviceId

      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          console.log('insertDevicesDtlDao>>', result);
          resolve(result[0]);
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

export const getUserDevicesDao = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_user_devices_dtl(?)', [
        data?.userEmail ?? null,
      ],                        (err, result) => {
        if (err) {
          console.log('ccaas_get_user_devices_dtl err:', err);
          resolve(err);
        } else {
          const rows = Array.isArray(result) && Array.isArray(result[0]) ? result : result || [];
          resolve(rows);
        }
      });
    } catch (err) {
      console.log('ccaas_get_user_devices_dtl err:', err);
      reject(err);
    }
  });
};

export const getCompanyBilling = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnectionRole.query('CALL ur_myacc_get_billing_and_subscription_details(?)', [
        data.company_id,
      ],                            (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          if (result && result[0]?.length > 0) {
            resolve(result);
          } else {
            resolve([]);
          }
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

/**
 *
 * @param data
 * @returns
 */
export const getStorageData = (data: any) => {
  return new Promise(async (resolve, reject) => {
    try {
      mysqlPoolConnection.query('CALL ccaas_get_voice_storage_info(?)', [
        data.domainId,
      ],                        (err, result) => {
        if (err) {
          console.log('err:', err);
          resolve(err);
        } else {
          if (result && result[0]?.length > 0) {
            resolve(result[0]);
          } else {
            resolve([]);
          }
        }
      });
    } catch (err) {
      console.log(err);
      reject(err);
    }
  });
};

/**
 * Forget Password
 * @param data any
 * SP: ur_forget_password_user_dtl
 */
export const getEmailOtpFromDb = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnectionWorktual.query(
        'CALL ur_forget_password_user_dtl(?)',
        [data.email],
        (err, response) => {
          if (err) {
            console.log(err);
            return reject(err);
          }
          resolve(response[0]);
        },
      );
    } catch (err) {
      reject(err);
    }
  });
};

export const sendVerifyOtp = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnectionWorktual.query(
        'CALL ur_validate_signup_user_dtl(?,?)',
        [data.email, data.verify_code],
        (err, response) => {
          if (err) {
            console.log(err);
            return reject(err);
          }
          resolve(response[0]);
        },
      );
    } catch (err) {
      reject(err);
    }
  });
};

export const passwordUpdateFromDb = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnectionWorktual.query(
        'CALL ur_update_user_password_dtl(?,?)',
        [data.email, data.password],
        (err, response) => {
          if (err) {
            console.log(err);
            return reject(err);
          }
          console.log('passwordUpdateFromDb', response[0]);
          resolve(response[0]);
        },
      );
    } catch (err) {
      reject(err);
    }
  });
};

export const resendOtpToDb = (data: any) => {
  return new Promise((resolve, reject) => {
    try {
      mysqlPoolConnectionWorktual.query(
        'CALL ur_resend_create_signup_user_dtl(?)',
        [data.email],
        (err, response) => {
          if (err) {
            console.log(err);
            return reject(err);
          }
          console.log('passwordUpdateFromDb', response[0]);
          resolve(response[0]);
        },
      );
    } catch (err) {
      reject(err);
    }
  });
};
