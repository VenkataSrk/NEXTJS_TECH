// /**
//  * @createdBy <team@vectone.com>
//  * @createdOn
//  */

// import { avamo } from '../../dao/avamo.dao';

// /**
//  * avamo webhook
//  * @param req
//  * @param res
//  * @param done
//  */
// export async function avamoHandler(req: any, res: any, done: any) {
//   try {
//     const data: any = req.body;
//     const avamoRes: any = await avamo(data);
//     if (avamoRes) {
//       res.status(200).send({ statusCode: 200, message: 'inserted successfully' });
//     } else {
//       res.status(424).send({ statusCode: 424, message: 'failed' });
//     }
//   } catch (err) {
//     req.log.error(err);
//     res.status(500).send({ statusCode: 500, message: 'internal server error' });
//   }
// }
