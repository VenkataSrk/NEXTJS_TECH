import { insertGuestUserDetails } from './dao';

export const createGuestHandler = async (req: any, res: any) => {
  try {
    const response: any = await insertGuestUserDetails(req.body);
    console.log('response :', response);
    if (response && response === 'OK') {
      res.status(200).send({
        statusCode: 200,
        message: 'success'
      });
    } else {
      res.status(200).send({
        statusCode: 422,
        message: 'Failed'
      });
    }
  } catch (err) {
    console.error('guestHandler err :::::', err);
    res.status(500).send({ statusCode: 500, message: 'internal server error' });
  }
};
