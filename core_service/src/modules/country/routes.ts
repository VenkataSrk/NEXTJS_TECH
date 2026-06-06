/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */

import { COUNTRY } from '../../helpers/constants';
import { getCityConfigHandler, getCountryConfigHandler, getCountryHandler, getMultipleEntryNodes, getStateConfigHandler, } from './handler';
import { getCountrySchema } from './schema';
export default function CountryHandler(server: any, options: any, next: any) {
  // get api for role
  server.get(
    COUNTRY.GET_ROLE_LIST,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'Get country ',
        description: 'Get country api',
        tags: ['country'],
        response: getCountrySchema.response,
      },
    },
    getCountryHandler
  );

  server.get(
    COUNTRY.GET_COUNTRY_CONFIG,
    {
      preValidation: [server.validateSession],
      schema: {
        summary: 'get_country_config ',
        description: 'get country config api',
        tags: ['country'],
        // response: getCountrySchema.response,
      },
    },
    getCountryConfigHandler
  );

  server.post(
    COUNTRY.GET_NODES_ENTRY_POINT,
    {
      schema: {
        summary: 'get nodes entry point ',
        description: 'get nodes entry point api',
        tags: ['ChatFlow'],
      },
    },
    getMultipleEntryNodes
  );

  server.post(
    COUNTRY.GET_STATE_CONFIG,
    {
      schema: {
        summary: 'get state config',
        description: 'get state config api',
        tags: ['state'],
      },
    },
    getStateConfigHandler
  );

  server.post(
    COUNTRY.GET_CITY_CONFIG,
    {
      schema: {
        summary: 'get city config',
        description: 'get city config api',
        tags: ['city'],
      },
    },
    getCityConfigHandler
  );

  next();
}
