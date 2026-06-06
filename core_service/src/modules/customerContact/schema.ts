/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

// insertCustomerContactSchema
export const insertCustomerContactSchema = {
  body: {
    type: 'object',
    properties: {
      coid: { type: 'number' },
      firstName: { type: 'string' },
      lastName: { type: 'string' },
      phoneNumber: { type: 'array' },
      email: { type: 'array' },
      accountNumber: { type: 'string' },
      tags: { type: 'array' },
      location: { type: 'string' },
      address: { type: 'string' },
      country: { type: 'string' },
      state: { type: 'string' },
      city: { type: 'string' },
      postalCode: { type: 'string' },
      facebook: { type: 'string' },
      instagram: { type: 'string' },
      whatsapp: { type: 'string' },
      twitter: { type: 'string' },
      voiceBiometric: { type: 'string' },
      customerType: { type: 'string' },
      timeZone: { type: 'string' }
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        insertContactList: {
          type: 'object',
          properties: {
            coid: { type: 'number' },
            firstName: { type: 'string' },
            lastName: { type: 'string' },
            phoneNumber: { type: 'array' },
            email: { type: 'array' },
            accountNumber: { type: 'string' },
            tags: { type: 'string' },
            location: { type: 'string' },
            address: { type: 'string' },
            country: { type: 'string' },
            state: { type: 'string' },
            city: { type: 'string' },
            postalCode: { type: 'string' },
            facebook: { type: 'string' },
            instagram: { type: 'string' },
            whatsapp: { type: 'string' },
            twitter: { type: 'string' },
            voiceBiometric: { type: 'string' },
            mergeCustomer: { type: 'number' },
            blockCustomer: { type: 'number' },
            doNotDisturb: { type: 'number' },
            blockCustomerReq: { type: 'number' },
            doNotDisturbReq: { type: 'number' },
            createdAt: { type: 'string' },
            updatedAt: { type: 'string' },
            domainId: { type: 'number' },
            customerType: { type: 'string' },
            timeZone: { type: 'string' }
          },
        },
      },
    },
  },
};

// getCustomerContactchema
export const getCustomerContactchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getConatctListRes: {
          type: 'array',
          items: {
            properties: {
              coid: { type: 'number' },
              firstName: { type: 'string' },
              lastName: { type: 'string' },
              phoneNumber: { type: 'string' },
              email: { type: 'string' },
              accountNumber: { type: 'string' },
              tags: { type: 'string' },
              location: { type: 'string' },
              address: { type: 'string' },
              country: { type: 'string' },
              state: { type: 'string' },
              city: { type: 'string' },
              postalCode: { type: 'string' },
              facebook: { type: 'string' },
              instagram: { type: 'string' },
              whatsapp: { type: 'string' },
              twitter: { type: 'string' },
              voiceBiometric: { type: 'string' },
              mergeCustomer: { type: 'number' },
              blockCustomer: { type: 'number' },
              doNotDisturb: { type: 'number' },
              blockCustomerReq: { type: 'number' },
              doNotDisturbReq: { type: 'number' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              customerType: { type: 'string' },
              timeZone: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// getCustomerContactDetailschema
export const getCustomerContactDetailschema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getContactDetailsRes: {
          type: 'array',
          items: {
            properties: {
              coid: { type: 'number' },
              firstName: { type: 'string' },
              lastName: { type: 'string' },
              phoneNumber: { type: 'string' },
              email: { type: 'string' },
              accountNumber: { type: 'string' },
              tags: { type: 'string' },
              location: { type: 'string' },
              address: { type: 'string' },
              country: { type: 'string' },
              state: { type: 'string' },
              city: { type: 'string' },
              postalCode: { type: 'string' },
              facebook: { type: 'string' },
              instagram: { type: 'string' },
              whatsapp: { type: 'string' },
              twitter: { type: 'string' },
              voiceBiometric: { type: 'string' },
              mergeCustomer: { type: 'number' },
              blockCustomer: { type: 'number' },
              doNotDisturb: { type: 'number' },
              blockCustomerReq: { type: 'number' },
              doNotDisturbReq: { type: 'number' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              customerType: { type: 'string' },
              timeZone: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// updateContactSchema
export const updateContactSchema = {
  params: {
    type: 'object',
    properties: {
      coid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      firstName: { type: 'string' },
      lastName: { type: 'string' },
      phoneNumber: { type: 'array' },
      email: { type: 'string' },
      accountNumber: { type: 'string' },
      tags: { type: 'array' },
      location: { type: 'string' },
      address: { type: 'string' },
      country: { type: 'string' },
      state: { type: 'string' },
      city: { type: 'string' },
      postalCode: { type: 'string' },
      facebook: { type: 'string' },
      instagram: { type: 'string' },
      whatsapp: { type: 'string' },
      twitter: { type: 'string' },
      voiceBiometric: { type: 'string' },
      mergeCustomer: { type: 'number', enum: [0, 1] },
      blockCustomer: { type: 'number', enum: [0, 1] },
      doNotDisturb: { type: 'number', enum: [0, 1] },
      blockCustomerReq: { type: 'number', enum: [0, 1] },
      doNotDisturbReq: { type: 'number', enum: [0, 1] },
      customerType: { type: 'string' },
      timeZone: { type: 'string' },
      blockedBy: { type: 'number' },
      blockedReqBy: { type: 'number' },
      doNotDisturbBy: { type: 'number' },
      doNotDisturbReqBy: { type: 'number' }
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

// deleteContactSchema
export const deleteContactSchema = {
  params: {
    type: 'object',
    properties: {
      coid: { type: 'number' },
    },
    required: ['coid'],
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

// isValidSchema
export const isValidSchema = {
  body: {
    type: 'object',
    properties: {
      email: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        flag: { type: 'number' }
      },
    },
  },
};

// getBlockCustomerContactchema
export const getBlockCustomerContactchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getBlockCustomerListRes: {
          type: 'array',
          items: {
            properties: {
              coid: { type: 'number' },
              firstName: { type: 'string' },
              lastName: { type: 'string' },
              phoneNumber: { type: 'array' },
              email: { type: 'string' },
              accountNumber: { type: 'string' },
              tags: { type: 'string' },
              location: { type: 'string' },
              address: { type: 'string' },
              country: { type: 'string' },
              state: { type: 'string' },
              city: { type: 'string' },
              postalCode: { type: 'string' },
              facebook: { type: 'string' },
              instagram: { type: 'string' },
              whatsapp: { type: 'string' },
              twitter: { type: 'string' },
              voiceBiometric: { type: 'string' },
              mergeCustomer: { type: 'number', enum: [0, 1] },
              blockCustomer: { type: 'number', enum: [0, 1] },
              doNotDisturb: { type: 'number', enum: [0, 1] },
              blockCustomerReq: { type: 'number', enum: [0, 1] },
              doNotDisturbReq: { type: 'number', enum: [0, 1] },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              timeZone: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// getDoNotDisturbCustomerContactchema
export const getDoNotDisturbCustomerContactchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getDoNotDisturbCustomerListRes: {
          type: 'array',
          items: {
            properties: {
              coid: { type: 'number' },
              firstName: { type: 'string' },
              lastName: { type: 'string' },
              phoneNumber: { type: 'array' },
              email: { type: 'string' },
              accountNumber: { type: 'string' },
              tags: { type: 'string' },
              location: { type: 'string' },
              address: { type: 'string' },
              country: { type: 'string' },
              state: { type: 'string' },
              city: { type: 'string' },
              postalCode: { type: 'string' },
              facebook: { type: 'string' },
              instagram: { type: 'string' },
              whatsapp: { type: 'string' },
              twitter: { type: 'string' },
              voiceBiometric: { type: 'string' },
              mergeCustomer: { type: 'number', enum: [0, 1] },
              blockCustomer: { type: 'number', enum: [0, 1] },
              doNotDisturb: { type: 'number', enum: [0, 1] },
              blockCustomerReq: { type: 'number', enum: [0, 1] },
              doNotDisturbReq: { type: 'number', enum: [0, 1] },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              timeZone: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// getBlockCustomerContactReqchema
export const getBlockCustomerContactReqchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getBlockCustomerRequestListRes: {
          type: 'array',
          items: {
            properties: {
              coid: { type: 'number' },
              firstName: { type: 'string' },
              lastName: { type: 'string' },
              phoneNumber: { type: 'array' },
              email: { type: 'string' },
              accountNumber: { type: 'string' },
              tags: { type: 'string' },
              location: { type: 'string' },
              address: { type: 'string' },
              country: { type: 'string' },
              state: { type: 'string' },
              city: { type: 'string' },
              postalCode: { type: 'string' },
              facebook: { type: 'string' },
              instagram: { type: 'string' },
              whatsapp: { type: 'string' },
              twitter: { type: 'string' },
              voiceBiometric: { type: 'string' },
              mergeCustomer: { type: 'number', enum: [0, 1] },
              blockCustomer: { type: 'number', enum: [0, 1] },
              doNotDisturb: { type: 'number', enum: [0, 1] },
              blockCustomerReq: { type: 'number', enum: [0, 1] },
              doNotDisturbReq: { type: 'number', enum: [0, 1] },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              timeZone: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// getDoNotDisturbCustomerContactReqchema
export const getDoNotDisturbCustomerContactReqchema = {
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getDoNotDisturbCustomerReqListRes: {
          type: 'array',
          items: {
            properties: {
              coid: { type: 'number' },
              firstName: { type: 'string' },
              lastName: { type: 'string' },
              phoneNumber: { type: 'array' },
              email: { type: 'string' },
              accountNumber: { type: 'string' },
              tags: { type: 'string' },
              location: { type: 'string' },
              address: { type: 'string' },
              country: { type: 'string' },
              state: { type: 'string' },
              city: { type: 'string' },
              postalCode: { type: 'string' },
              facebook: { type: 'string' },
              instagram: { type: 'string' },
              whatsapp: { type: 'string' },
              twitter: { type: 'string' },
              voiceBiometric: { type: 'string' },
              mergeCustomer: { type: 'number', enum: [0, 1] },
              blockCustomer: { type: 'number', enum: [0, 1] },
              doNotDisturb: { type: 'number', enum: [0, 1] },
              blockCustomerReq: { type: 'number', enum: [0, 1] },
              doNotDisturbReq: { type: 'number', enum: [0, 1] },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              timeZone: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// getCustomerContactPhoneNumberSchema
export const getCustomerContactPhoneNumberSchema = {
  params: {
    type: 'object',
    properties: {
      phoneNumber: { type: 'string' },
    },
  },
  response: {
    200: {
      type: 'object',
      properties: {
        statusCode: { type: 'number' },
        message: { type: 'string' },
        getContactNumberRes: {
          type: 'array',
          items: {
            properties: {
              coid: { type: 'number' },
              firstName: { type: 'string' },
              lastName: { type: 'string' },
              phoneNumber: { type: 'array' },
              email: { type: 'string' },
              accountNumber: { type: 'string' },
              tags: { type: 'string' },
              location: { type: 'string' },
              address: { type: 'string' },
              country: { type: 'string' },
              state: { type: 'string' },
              city: { type: 'string' },
              postalCode: { type: 'string' },
              facebook: { type: 'string' },
              instagram: { type: 'string' },
              whatsapp: { type: 'string' },
              twitter: { type: 'string' },
              voiceBiometric: { type: 'string' },
              mergeCustomer: { type: 'number' },
              blockCustomer: { type: 'number' },
              doNotDisturb: { type: 'number' },
              blockCustomerReq: { type: 'number' },
              doNotDisturbReq: { type: 'number' },
              createdAt: { type: 'string' },
              updatedAt: { type: 'string' },
              domainId: { type: 'number' },
              timeZone: { type: 'string' }
            },
          },
        },
      },
    },
  },
};

// updateCustomerContactSchema
export const updateCustomerContactSchema = {
  params: {
    type: 'object',
    properties: {
      coid: { type: 'number' },
    },
  },
  body: {
    type: 'object',
    properties: {
      tags: { type: 'array' },
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
