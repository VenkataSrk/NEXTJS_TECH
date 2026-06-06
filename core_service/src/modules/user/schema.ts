/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertUserSchema
export const insertUserSchema = {
  body: {
    type: 'object',
    properties: {
      emailId: { type: 'string' },
      directNumber: { type: 'string' },
      address: { type: 'string' },
      routing_profile: { type: 'number' },
      report_to: { type: 'string' },
      phone_type: { type: 'string' },
      phone_number: { type: 'string' },
      desk_phone_number: { type: 'string' },
      after_call_work_timeout: { type: 'string' },
      is_first_time: { type: 'number' },
      initial_setup_menu: { type: 'string' },
      initial_setup_flag: { type: 'string' },
      add_skill: { type: 'object' },
      country_code: { type: 'string' },
      initial_selection_flag: { type: 'string' },
      state: { type: 'string' },
      active_channel: { type: 'string' },
      stateTimer: { type: 'string' },
      callType: { type: 'string' },
      customer: { type: 'string' },
      queue: { type: 'string' },
      name: { type: 'string' },
      roleid: { type: 'number' },
      status: { type: 'string' },
      localization: { type: 'string' },
      routing_profile_name: { type: 'string' },
      active_status: { type: 'number' },
      currentSession: { type: 'string' },
      interactions: { type: 'string' },
      called_time: { type: 'string' },
      outBound_call: { type: 'number' },
      report_to_name: { type: 'string' },
      isVoice: { type: 'number' },
      isVoiceCallTransfer: { type: 'number' },
      isChat: { type: 'number' },
      isChatTransfer: { type: 'number' },
      isEmail: { type: 'number' },
      isEmailTransfer: { type: 'number' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};
// updateUserSchema
export const updateUserSchema = {
  body: {
    type: 'object',
    properties: {
      emailId: { type: 'string' },
      directNumber: { type: 'string' },
      address: { type: 'string' },
      routing_profile: { type: 'string' },
      report_to: { type: 'string' },
      phone_type: { type: 'string' },
      phone_number: { type: 'string' },
      desk_phone_number: { type: 'string' },
      after_call_work_timeout: { type: 'string' },
      initial_setup_menu: { type: 'string' },
      initial_setup_flag: { type: 'string' },
      add_skill: { type: 'string' },
      initial_selection_flag: { type: 'string' },
      country_code: { type: 'string' },
      state: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};
// getUserSchema
export const getUserSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getUserListNumber: {
          type: 'array',
          items: {
            properties: {
              uid: { type: 'number' },
              emailId: { type: 'string' },
              directNumber: { type: 'string' },
              address: { type: 'string' },
              routing_profile: { type: 'string' },
              report_to: { type: 'string' },
              phone_type: { type: 'string' },
              phone_number: { type: 'string' },
              desk_phone_number: { type: 'string' },
              after_call_work_timeout: { type: 'string' },
              initial_setup_menu: { type: 'string' },
              initial_setup_flag: { type: 'string' },
              add_skill: { type: 'string' },
              initial_selection_flag: { type: 'number' },
              country_code: { type: 'string' },
              sipLoginId: { type: 'number' },
              domainId: { type: 'number' },
              ext: { type: 'number' },
              companyId: { type: 'number' },
              state: { type: 'string' },
              stateTimer: { type: 'string' },
              callType: { type: 'string' },
              customer: { type: 'string' },
            },
          },
        },
      },
    },
  },
};
// deleteUserSchema
export const deleteUserSchema = {
  params: {
    type: 'object',
    properties: {
      uid: { type: 'number' },
    },
    required: ['uid'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};

// loginSchema
export const loginSchema = {
  body: {
    type: 'object',
    properties: {
      emailId: { type: 'string' },
      password: { type: 'string' },
      source: { type: 'string' },
      ipaddress: { type: 'string' },
    },
    required: ['emailId', 'password', 'source', 'ipaddress'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        accesstoken: { type: 'string' },
      },
    },
  },
};

// forgotPasswordSchema
export const forgotPasswordSchema = {
  body: {
    type: 'object',
    properties: {
      emailId: { type: 'string' },
    },
    required: ['emailId'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};

// resetPasswordSchema
export const resetPasswordSchema = {
  querystring: {
    type: 'object',
    properties: {
      accesstoken: { type: 'string' },
    },
  },
  body: {
    type: 'object',
    properties: {
      updatePass: { type: 'string' },
    },
    required: ['updatePass'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};
// getUserListSchema
export const getUserListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getUserListNumber: {
          type: 'object',
          properties: {
            list: {
              type: 'array',
              items: {
                properties: {
                  uid: { type: 'number' },
                  emailId: { type: 'string' },
                  directNumber: { type: 'string' },
                  address: { type: 'string' },
                  routing_profile: { type: 'string' },
                  report_to: { type: 'string' },
                  phone_type: { type: 'string' },
                  phone_number: { type: 'string' },
                  desk_phone_number: { type: 'string' },
                  after_call_work_timeout: { type: 'string' },
                  initial_setup_menu: { type: 'string' },
                  initial_setup_flag: { type: 'string' },
                  add_skill: { type: 'string' },
                  initial_selection_flag: { type: 'number' },
                  country_code: { type: 'string' },
                  sipLoginId: { type: 'number' },
                  domainId: { type: 'number' },
                  ext: { type: 'number' },
                  companyId: { type: 'number' },
                  state: { type: 'string' },
                  stateTimer: { type: 'string' },
                  callType: { type: 'string' },
                  customer: { type: 'string' },
                },
              },
            },
            total_count: { type: 'number' },
          },
        },
      },
    },
  },
};

// getUserListSchema:
export const getUserListByroleSchema = {
  params: {
    type: 'object',
    properties: {
      roleId: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getUserListByRoleId: {
          type: 'array',
          items: {
            properties: {
              uid: { type: 'number' },
              emailId: { type: 'string' },
              directNumber: { type: 'string' },
              address: { type: 'string' },
              routing_profile: { type: 'string' },
              report_to: { type: 'string' },
              phone_type: { type: 'string' },
              phone_number: { type: 'string' },
              desk_phone_number: { type: 'string' },
              after_call_work_timeout: { type: 'string' },
              initial_setup_menu: { type: 'string' },
              initial_setup_flag: { type: 'string' },
              add_skill: { type: 'string' },
              initial_selection_flag: { type: 'number' },
              country_code: { type: 'string' },
              state: { type: 'string' },
              stateTimer: { type: 'string' },
              callType: { type: 'string' },
              customer: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// getUserDetailsSchema
export const getUserDetailsSchema = {
  params: {
    type: 'object',
    properties: {
      uid: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getUserDetails: {
          type: 'array',
          items: {
            properties: {
              uid: { type: 'number' },
              emailId: { type: 'string' },
              directNumber: { type: 'string' },
              address: { type: 'string' },
              routing_profile: { type: 'string' },
              report_to: { type: 'string' },
              phone_type: { type: 'string' },
              phone_number: { type: 'string' },
              desk_phone_number: { type: 'string' },
              after_call_work_timeout: { type: 'string' },
              initial_setup_menu: { type: 'string' },
              initial_setup_flag: { type: 'string' },
              add_skill: { type: 'string' },
              initial_selection_flag: { type: 'number' },
              state: { type: 'string' },
              stateTimer: { type: 'string' },
              callType: { type: 'string' },
              customer: { type: 'string' },
              queue: { type: 'string' },
              roleId: { type: 'string' },

            },
          },
        },
      },
    },
  },
};

export const initialRes = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        result: {
          type: 'object',
          properties: {
            is_first_time: { type: 'number' },
            initial_setup_menu: { type: 'string' },
            initial_setup_flag: { type: 'string' },
            initial_selection_flag: { type: 'string' },
            uid: { type: 'number' },
          },
        },
      },
    },
  },
};

// getDirUserSchema
export const getDirUserSchema = {
  params: {
    type: 'object',
    properties: {
      companyId: { type: 'number' },
      ext: { type: 'number' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getDirUserList: {
          type: 'array',
          items: {
            properties: {
              Idx: { type: 'number' },
              Createdate: { type: 'string' },
              Basket_id: { type: 'number' },
              Company_id: { type: 'number' },
              Firstname: { type: 'string' },
              Surname: { type: 'string' },
              Department: { type: 'string' },
              Email: { type: 'string' },
              Mobileno: { type: 'string' },
              Direct_Number: { type: 'string' },
              Extension_Number: { type: 'string' },
              User_Language: { type: 'string' },
              Greeting_Language: { type: 'string' },
              Regional_Language: { type: 'string' },
              Role_Id: { type: 'number' },
              Status: { type: 'number' },
              Recorded_usename_filepath: { type: 'string' },
              Home_Country_Code: { type: 'string' },
              Time_format: { type: 'number' },
              Time_zone: { type: 'string' },
              Recorded_by: { type: 'string' },
              Recorded_type: { type: 'string' },
              Recorded_greeting: { type: 'string' },
              MyAcc_Password: { type: 'string' },
              dialby_dir_enable: { type: 'number' },
              provision_file: { type: 'number' },
              extn_audio_file: { type: 'string' },
              extn_name_audio_file: { type: 'string' },
              is_mapped: { type: 'number' },
              extn_last_login: { type: 'string' },
              account_plan_id: { type: 'string' },
              emp_id: { type: 'string' },
              desktop_admin: { type: 'string' },
              compliance_export: { type: 'string' },
              updated_at: { type: 'string' },
              device_type: { type: 'string' },
              browser: { type: 'string' },
              login_type: { type: 'number' },
              last_login: { type: 'string' },
              phone_name: { type: 'string' },
              site_id: { type: 'number' },
              is_callerid: { type: 'number' },
              callerid_number: { type: 'string' },
              host_code: { type: 'number' },
              partipation_code: { type: 'number' },
              bc_no_caller_id: { type: 'number' },
              updated_conference_number: { type: 'string' },
              country_code: { type: 'number' },
              Hubspot_contactid: { type: 'string' },
              onboarding_link: { type: 'string' },
              phone_id: { type: 'string' },
              pwd_reset_by: { type: 'string' },
              pwd_rest_ip: { type: 'string' },
              Pay_reference: { type: 'string' },
              Payment_Amount: { type: 'number' },
              Paystatus: { type: 'string' },
              Meeting_Id: { type: 'string' },
              Is_Operator: { type: 'number' },
              user_conv_type: { type: 'string' },
              User_conv_payref: { type: 'string' },
              User_conv_amount: { type: 'number' },
              user_conv_date: { type: 'string' },
              UR_Mobileno: { type: 'string' },
              is_cancel: { type: 'string' },
              position: { type: 'string' },
              Is_asr_access: { type: 'string' },
            },
          },
        },
      },
    },
  },
};

// getAllUserListSchema
export const getAllUserListSchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getAllUserList: {
          type: 'array',
          items: {
            properties: {
              uid: { type: 'number' },
              emailId: { type: 'string' },
              directNumber: { type: 'string' },
              address: { type: 'string' },
              routing_profile: { type: 'string' },
              report_to: { type: 'string' },
              phone_type: { type: 'string' },
              phone_number: { type: 'string' },
              desk_phone_number: { type: 'string' },
              after_call_work_timeout: { type: 'string' },
              initial_setup_menu: { type: 'string' },
              initial_setup_flag: { type: 'string' },
              add_skill: { type: 'string' },
              initial_selection_flag: { type: 'number' },
              country_code: { type: 'string' },
              sipLoginId: { type: 'number' },
              domainId: { type: 'number' },
              ext: { type: 'number' },
              companyId: { type: 'number' },
              roleid: { type: 'number' },
              state: { type: 'string' },
              stateTimer: { type: 'string' },
              callType: { type: 'string' },
              customer: { type: 'string' },
              name: { type: 'string' },
              status: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// insertOnboardUserSchema
export const insertOnboardUserSchema = {
  body: {
    type: 'object',
    properties: {
      ext: { type: 'number' },
      domainId: { type: 'number' },
      mobileNumbarType: { type: 'string' },
      city: { type: 'string' },
      didNumber: { type: 'number' },
      callFlowId: { type: 'object' },
      recording: { type: 'number', enum: [0, 1] },
      callFlowName: { type: 'string' },
    }
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};

// getCcasRoleInfoSchema
export const getCcasRoleInfoSchema = {
  body: {
    type: 'object',
    properties: {
      username: { type: 'string' },
      password: { type: 'string' },
      source: { type: 'string', enum: ['Web', 'Android', 'IOS', 'CCAAS:BROWSER', 'CCAAS:TPI'] },
      deviceId: { type: 'string' },
      ipAddress: { type: 'string' },
      isForceLogin: { type: 'boolean', default: false },
    },
    required: ['username', 'password', 'source', 'deviceId', 'ipAddress'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getCCaasRoleIdInfoRes: {
          type: 'array',
          items: {
            properties: {
              is_cas_user: { type: 'number' },
              ccas_role_info: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// deleteOnboardSchema
export const deleteOnboardSchema = {
  params: {
    type: 'object',
    properties: {
      emailId: { type: 'string' },
    },
    required: ['emailId'],
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
      },
    },
  },
};

// Otp generate schema
export const generateOtpSchema = {
  body: {
    type: 'object',
    required: ['domainId', 'toId', 'expiryTime', 'businessName'],
    properties: {
      domainId: { type: 'string' },
      toId: { type: 'string' },
      expiryTime: { type: 'number' },
      businessName: { type: 'string' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        // result: {
        //   type: 'object',
        //   properties: {
        //     verificationCode: { type: 'string' }
        //   },
        // }
      },
    },
  },
};

// validate otp
export const validateOtpSchema = {
  body: {
    type: 'object',
    required: ['domainId', 'toId', 'verificationCode'],
    properties: {
      domainId: { type: 'string' },
      toId: { type: 'string' },
      verificationCode: { type: 'number' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        // result: {
        //   type: 'object',
        //   properties: {
        //     verificationCode: { type: 'string' }
        //   },
        // }
      },
    },
  },
};
