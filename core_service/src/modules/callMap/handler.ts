import { callflowMap, callflowSetting, deletecallFlow, getcallflowList, getCallflowMap, getcallflowSetting, updateCallFlowMap } from '../../dao/callFlow';

/**
 * @createdBy <team@vectone.com>
 * @createdOn
 */
import shortid from 'shortid';
import { RESPONSE } from '../../helpers/constants';
import { logger } from '../../plugins/winston';

/**
 *
 * @param req
 * @param res
 * @param done
 * callflow api
 */
export async function callflowMapHandler(req: any, res: any, done: any) {
  const sourceId = shortid.generate();
  try {
    const auth: any = req.headers;
    const data: any = {
      sourceId,
      callFlowId: req.body.callFlowId,
      domainId: auth.domainId,
      description: req.body.description ? req.body.description : null,
      codeSnippet: `<script type="text/javascript" style={{position:"absolute"}}></script> <div><iframe src="http://localhost:3001/?webchatid=${sourceId}" title="avomo UI" scrolling="no" frameBorder="0" style={{borderStyle: "none", width: "400px",right:0,  height: "600px", position:"fixed"}}></iframe></div>`,
    };
    const insertCallFlow: any = await callflowMap(data);
    logger.info('callflowMap', insertCallFlow);
    if (insertCallFlow) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.callflow_insert_successfully, insertCallFlowList: insertCallFlow ?? {} });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('callflowMapHandler - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getcallflowMapHandler(req: any, res: any, done: any) {
  try {
    const cidq = req.params.cid;
    if (cidq !== '') {
      const getcallflowMap: any = await getCallflowMap(cidq);
      logger.info('getCallflowMap', getcallflowMap);
      if (Array.isArray(getcallflowMap) && getcallflowMap.length > 0) {
        res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getcallflowMapList: getcallflowMap });
      } else {
        res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getcallflowMapList: [] });
      }
    } else {
      res.status(200).send({ statusCode: 424, message: 'Cid no should not be empty' });
    }
  } catch (err) {
    logger.error('getcallflowMapHandler - Unexpected Error:', req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
/**
 *
 * @param req
 * @param res
 * @param done
 * ccs login api
 */
export async function updatecallFlowMapHandler(req: any, res: any, done: any) {
  try {
    const cidp = req.params.cid;
    const datau: any = {
      callFlowId: req.body.callFlowId ? req.body.callFlowId : null,
    };
    const updatecallFlow: any = await updateCallFlowMap(datau, cidp);
    logger.info('updateCallFlowMap', updatecallFlow);
    if (updatecallFlow.affectedRows === 1) {
      res.status(200).send({
        statusCode: 200,
        message: RESPONSE.callflow_updates_successfully,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_update });
    }
  } catch (err) {
    logger.error('updatecallFlowMapHandler - Unexpected Error:', req.params, req.body, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function deletecallFlowMapHandler(req: any, res: any, done: any) {
  try {
    const cidd: any = req.params.cid;
    const deleteCallFlow: any = await deletecallFlow(cidd);
    logger.info('deletecallFlow', deleteCallFlow);
    if (deleteCallFlow) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.callflow_deleted_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('deletecallFlowMapHandler - Unexpected Error:', req.params, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
export async function getCallFlowMapListHandler(req: any, res: any, done: any) {
  try {
    const getCallFlowMapList: any = await getcallflowList();
    logger.info('getcallflowList', getCallFlowMapList);
    if (Array.isArray(getCallFlowMapList) && getCallFlowMapList.length > 0) {
      res.status(200).send({
        getCallFlowMapList,
        statusCode: 200,
        message: RESPONSE.success_message,
      });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getCallFlowMapList: [] });
    }
  } catch (err) {
    logger.error('deletecallFlowMapHandler - Unexpected Error:', err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

/**
 *
 * @param req
 * @param res
 * @param done
 * callflow api
 */
export async function callflowSettingHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const data: any = {
      callFlow: req.body.callFlow,
      sourceId: req.body.sourceId,
      domainId: auth.domainId,
      status: req.body.status ? req.body.status : null,
      description: req.body.description ? req.body.description : null,
    };
    const insertCallFlow: any = await callflowSetting(data);
    logger.info('callflowSetting', insertCallFlow);
    if (insertCallFlow) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.callflow_insert_successfully });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found });
    }
  } catch (err) {
    logger.error('callflowSettingHandler - Unexpected Error:', req.body, req.headers, err);

    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}

export async function getcallflowSettingHandler(req: any, res: any, done: any) {
  try {
    const auth: any = req.headers;
    const domainId = auth.domainId;
    const getcallflow: any = await getcallflowSetting(domainId);
    logger.info('getcallflowSetting', getcallflow);
    if (Array.isArray(getcallflow) && getcallflow.length > 0) {
      res.status(200).send({ statusCode: 200, message: RESPONSE.success_message, getcallflowSettingList: getcallflow });
    } else {
      res.status(200).send({ statusCode: 404, message: RESPONSE.not_found, getcallflowSettingList: [] });
    }
  } catch (err) {

    logger.error('getcallflowSettingHandler - Unexpected Error:', req.headers, err);
    res.status(500).send({ statusCode: 500, message: RESPONSE.internal_error });
  }
}
