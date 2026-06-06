/**
 * @createdBy Jayaselva
 * @createdOn 31th oct 2023
 */

import axios from 'axios';
import fs from 'fs';
import { google } from 'googleapis';
import http2 from 'http2';
import jwt from 'jsonwebtoken';
import { v4 as uuidv4 } from 'uuid';
import { DEVICE_TYPE, isJSON } from './config/constant';
import { getDeviceInfo, getUserListByRole } from './dao/subscription';
import { ioredisChat, pubsubRedisClient, tokenRedisStore } from './plugin/db';
import { logger } from './plugin/log';

// CCAAS IOS & ANDROID CALL
export async function IOSANDROIDCALL(req: any) {
  try {
    if (req?.channelType === 'Chat') {
      const target = req.agentId.split('_');
      console.log('reqqq>>>>', req);
      const redisVal: any = await tokenRedisStore.get(`${target[1]}@${target[0]}@presence_chat`);
      const parsedRedisVal = isJSON(redisVal) ? JSON.parse(redisVal) : redisVal;
      if (parsedRedisVal?.presence === 0) {
        const deviceData = {
          domainId: target[0],
          ext: target[1],
          deviceType: parsedRedisVal.deviceType
        };
        console.log('parsedRedisVal>>>>', parsedRedisVal);
        const deviceInfo = await getDeviceInfo(deviceData);
        const getRoleId: any = await getUserListByRole(deviceData?.domainId);
        let userList = {};
        if (Array.isArray(getRoleId) && getRoleId.length > 0) {
          getRoleId.filter((list) => {
            if (`${list.ext}` === deviceData.ext && `${list.domainId}` === deviceData?.domainId) {
              const userData = {
                isChat: list?.isChat,
                isChatTransfer: list?.isChatTransfer,
                isEmail: list?.isEmail,
                isEmailTransfer: list?.isEmailTransfer,
                isVoice: list?.isVoice,
                isVoiceCallTransfer: list?.isVoiceCallTransfer,
                roleId: list?.roleId,
                routingProfileId: list?.routingProfileId,
                UserName: list?.UserName
              };
              userList = userData;
            }
          });
        }

        console.log(`${deviceData.domainId}_${deviceData.ext}_${deviceData.deviceType} Response:`, deviceInfo);

        if (Array.isArray(deviceInfo) && deviceInfo.length > 0) {
          const registrationTokens = deviceInfo[0]?.deviceToken;
          const authToken = deviceInfo[0]?.authToken;
          const customerNumber = 'Web Chat';
          const customerName = req?.customer_name ? req?.customer_name : 'New Customer';
          const queueName = req?.queueName ? req?.queue_name : 'Default queue';
          if (registrationTokens) {
            if (parsedRedisVal.deviceType === DEVICE_TYPE.AND) {
              const payload = {
                message: {
                  token: registrationTokens,
                  data: {
                    queueName,
                    customerNumber,
                    webChatConnectedData: JSON.stringify(req),
                    priority: 'high',
                    type: 'accept',
                    call_uuid: uuidv4(),
                    caller: customerName,
                    iuid: req?.iuid,
                    screen: 'ChatWorkspace',
                  },
                  android: {
                    priority: 'high',
                    ttl: '2s'
                  }
                }
              };
              const accessToken = await getAndroidTestAccessToken();
              const config = {
                method: 'post',
                maxBodyLength: Infinity,
                url: 'https://fcm.googleapis.com/v1/projects/ccaas-328c6/messages:send',
                headers: {
                  'Content-Type': 'application/json',
                  Authorization: `Bearer ${accessToken}`,
                },
                data: JSON.stringify(payload),
              };
              console.log('config>>>>', config);
              axios.request(config).then((response: any) => {
                console.log('FCM send Successfully------->', JSON.stringify(response.data));
              }).catch((error) => {
                console.log(JSON.stringify(error.response.data));
              });
            } else {
              sendIosPush(deviceInfo, userList, req, customerName);
            }
          }
        }
      }
    } else {
      if (!req || !req.device_type) {
        return;
      }

      if (req?.iscancel && req?.device_type === DEVICE_TYPE.AND) {
        ioredisChat.to(`${req.domain_id}_${req.ext}`).emit('call_disconnect', req);
        return;
      }

      const target = req.target.split('_');
      const deviceData = {
        domainId: target[0],
        ext: target[1],
        deviceType: req.device_type
      };

      const deviceInfo = await getDeviceInfo(deviceData);
      const getRoleId: any = await getUserListByRole(deviceData?.domainId);

      let userList = {};
      if (Array.isArray(getRoleId) && getRoleId.length > 0) {
        getRoleId.filter((list) => {
          if (`${list.ext}` === deviceData.ext && `${list.domainId}` === deviceData?.domainId) {
            const userData = {
              isChat: list?.isChat,
              isChatTransfer: list?.isChatTransfer,
              isEmail: list?.isEmail,
              isEmailTransfer: list?.isEmailTransfer,
              isVoice: list?.isVoice,
              isVoiceCallTransfer: list?.isVoiceCallTransfer,
              roleId: list?.roleId,
              routingProfileId: list?.routingProfileId,
              UserName: list?.UserName
            };
            userList = userData;
          }
        });
      }

      console.log(`${deviceData.domainId}_${deviceData.ext}_${deviceData.deviceType} Response:`, deviceInfo);

      if (Array.isArray(deviceInfo) && deviceInfo.length > 0) {
        const registrationTokens = deviceInfo[0]?.deviceToken;
        const authToken = deviceInfo[0]?.authToken;
        const customerNumber = req?.origin?.split('_')?.[1] ?? '';
        const customerName = (req?.customer_name === '' || !req?.customer_name) ? 'New Customer' : `${req?.customer_name}`;
        const queueName = (req?.queueName === '' || !req?.queueName) ? 'Default Queue' : `${req?.queue_name}`;
        if (registrationTokens) {
          if (req?.device_type === DEVICE_TYPE.AND) {
            const payload = {
              message: {
                token: registrationTokens,
                data: {
                  queueName,
                  customerNumber,
                  priority: 'high',
                  type: 'accept',
                  call_uuid: uuidv4(),
                  caller: customerName,
                  iuid: req?.iuid,
                  screen: 'AcceptCall'
                },
                android: {
                  priority: 'high',
                  ttl: '2s'
                }
              }
            };
            const accessToken = await getAndroidTestAccessToken();
            const config = {
              method: 'post',
              maxBodyLength: Infinity,
              url: 'https://fcm.googleapis.com/v1/projects/ccaas-328c6/messages:send',
              headers: {
                'Content-Type': 'application/json',
                Authorization: `Bearer ${accessToken}`,
              },
              data: JSON.stringify(payload),
            };

            axios.request(config).then((response: any) => {
              console.log('FCM send Successfully------->', JSON.stringify(response.data));
            }).catch((error) => {
              console.log(JSON.stringify(error.response.data));
            });
          } else {
            sendIosPush(deviceInfo, userList, req, customerName);
          }

        }
      }
    }
  } catch (err) {
    logger.info(`[${req?.iuid}] ::: IOSANDROIDCALL ::::: error >>>`, err);
  }
}

export async function sendIosPush(deviceInfo: any, userList: any, callStatus: any, customerName) {
  try {
    if (callStatus?.channelType === 'Chat') {
      const payload = {
        aps: {
          alert: 'Incoming Call',
          sound: 'default',
          'content-available': 1
        },
        action: 'call_connected',
        call_uuid: uuidv4(),
        caller: customerName,
        iuid: callStatus?.iuid,
        screen: 'ChatWorkspace',
        webChatConnectedData: JSON.stringify(callStatus),
      };
      console.log('payload--->', payload);
      const devicetoken = deviceInfo[0]?.deviceToken;

      let apnsHost: any;

      // if (process.env.NODE_ENV === 'qatesting') {
      //   //   apnsHost = 'api.sandbox.push.apple.com';
      apnsHost = 'api.development.push.apple.com';
      // } else {
      // apnsHost = 'api.push.apple.com';
      // }

      console.log('apnsHost------>', apnsHost);

      const apnsPort = 443;
      console.log(Date.now(), 'APNS CONNECTED');
      const client = http2.connect(`https://${apnsHost}:${apnsPort}`);
      const apnstokennewvoip: any = await pubsubRedisClient.get('ccaas_token_voip');
      const apnsLivevoip = `/3/device/${devicetoken}`;

      console.log('apnsLivevoip----->', apnsLivevoip);

      const headers1 = {
        ':method': 'POST',
        ':path': apnsLivevoip,
        'apns-priority': 10,
        'apns-expiration': 30,
        'apns-push-type': 'voip',
        'apns-topic': 'com.worktual.ccaas.voip',
        authorization: `bearer ${apnstokennewvoip}`
      };
      const request = client.request(headers1);

      request.setEncoding('utf8');

      request.on('response', async (result) => {
        console.log('Status:', result);
        console.log(result[':status']);

        let resolveResult: any = '';
        if (result[':status'] === 200) {
          if (result[':status'] === 200) {
            return 'Push notification sent successfully';
          }
        } else if (result[':status'] === 403) {
          const newtoken: any = await getToken();
          await pubsubRedisClient.set('ccaas_token_voip', newtoken);
          sendIosPush(deviceInfo, userList, callStatus, customerName);
        } else {
          resolveResult = 'Error';
        }
      });

      request.on('data', (chunk) => {
        console.log(`Received data: ${chunk}`);
      });

      request.on('end', () => {
        console.log('No more data in response.');
        client.close();
      });

      client.on('error', (err) => {
        console.error(err);
      });

      request.write(JSON.stringify(payload));
      request.end();
    } else {
      const payload = !callStatus?.iscancel ? {
        aps: {
          alert: 'Incoming Call',
          sound: 'default',
          'content-available': 1
        },
        action: 'call_connected',
        call_uuid: uuidv4(),
        caller: customerName,
        iuid: callStatus?.iuid,
        screen: 'AcceptCall'
      } : {
        aps: {
          alert: '',
          'content-available': 1
        },
        action: 'call_cancelled',
        call_uuid: uuidv4(),
      };
      console.log('payload--->', payload);
      const devicetoken = deviceInfo[0]?.deviceToken;

      let apnsHost: any;

      // if (process.env.NODE_ENV === 'qatesting') {
      //   //   apnsHost = 'api.sandbox.push.apple.com';
        // apnsHost = 'api.development.push.apple.com';
      // } else {
      apnsHost = 'api.push.apple.com';
      // }

      console.log('apnsHost------>', apnsHost);

      const apnsPort = 443;
      console.log(Date.now(), 'APNS CONNECTED');
      const client = http2.connect(`https://${apnsHost}:${apnsPort}`);
      const apnstokennewvoip: any = await pubsubRedisClient.get('ccaas_token_voip');
      const apnsLivevoip = `/3/device/${devicetoken}`;

      console.log('apnsLivevoip----->', apnsLivevoip);

      const headers1 = {
        ':method': 'POST',
        ':path': apnsLivevoip,
        'apns-priority': 10,
        'apns-expiration': 30,
        'apns-push-type': 'voip',
        'apns-topic': 'com.worktual.ccaas.voip',
        authorization: `bearer ${apnstokennewvoip}`
      };
      const request = client.request(headers1);

      request.setEncoding('utf8');

      request.on('response', async (result) => {
        console.log('Status:', result);
        console.log(result[':status']);

        let resolveResult: any = '';
        if (result[':status'] === 200) {
          if (result[':status'] === 200) {
            return 'Push notification sent successfully';
          }
        } else if (result[':status'] === 403) {
          const newtoken: any = await getToken();
          await pubsubRedisClient.set('ccaas_token_voip', newtoken);
          sendIosPush(deviceInfo, userList, callStatus, customerName);
        } else {
          resolveResult = 'Error';
        }
      });

      request.on('data', (chunk) => {
        console.log(`Received data: ${chunk}`);
      });

      request.on('end', () => {
        console.log('No more data in response.');
        client.close();
      });

      client.on('error', (err) => {
        console.error(err);
      });

      request.write(JSON.stringify(payload));
      request.end();
    }
  } catch (error) {
    console.log('error--->', error);
  }
}

export async function sendIosPushTest(deviceInfo: any, userList: any, callStatus: any) {
  try {
    console.log('callStatus--->', callStatus);
    const payload = callStatus === 'connected' ? {
      aps: {
        alert: 'Incoming Call',
        sound: 'default',
        'content-available': 1
      },
      action: 'call_connected',
      call_uuid: uuidv4(),
      caller: 'Jayaselva M',
      type: 'EXTERNAL'
    } : {
      aps: {
        alert: '',
        'content-available': 1
      },
      action: 'call_cancelled',
      call_uuid: '550e8400-e29b-41d4-a716-446655440000'
    };
    // const devicetoken = '2cd9945964aa771aa598398f4858f3dad2dfce44d74b8a528ea47f6427f86c1f';
    const devicetoken = '67253870005bb5744c3f47c1fc8260baa78e69cef9ead4f4f872ff8f21c12482';

    let apnsHost: any;

    if (process.env.NODE_ENV === 'qatesting') {
      // apnsHost = 'api.sandbox.push.apple.com';
      apnsHost = 'api.development.push.apple.com';
    } else {
      apnsHost = 'api.push.apple.com';
    }

    console.log('apnsHost------>', apnsHost);

    const apnsPort = 443;
    console.log(Date.now(), 'APNS CONNECTED');
    const client = http2.connect(`https://${apnsHost}:${apnsPort}`);
    const apnstokennewvoip: any = await pubsubRedisClient.get('ccaas_token_voip');
    const apnsLivevoip = `/3/device/${devicetoken}`;
    console.log('apnsLivevoip----->', apnsLivevoip);

    console.log('inside voip new code');
    const headers1 = {
      ':method': 'POST',
      ':path': apnsLivevoip,
      'apns-priority': 10,
      'apns-expiration': 30,
      'apns-push-type': 'voip',
      'apns-topic': 'com.worktual.ccaas.voip',
      authorization: `bearer ${apnstokennewvoip}`
    };
    const request = client.request(headers1);

    request.setEncoding('utf8');

    request.on('response', async (result) => {
      console.log('Status:', result);
      console.log(result[':status']);

      let resolveResult: any = '';
      if (result[':status'] === 200) {
        if (result[':status'] === 200) {
          return 'Push notification sent successfully';
        }
      } else if (result[':status'] === 403) {
        const newtoken: any = await getToken();
        await pubsubRedisClient.set('ccaas_token_voip', newtoken);
        sendIosPushTest(deviceInfo, userList, callStatus);
      } else {
        resolveResult = 'Error';
      }
    });

    request.on('data', (chunk) => {
      console.log(`Received data: ${chunk}`);
    });

    request.on('end', () => {
      console.log('No more data in response.');
      client.close();
    });

    client.on('error', (err) => {
      console.error(err);
    });

    request.write(JSON.stringify(payload));
    request.end();
  } catch (error) {
    console.log('error--->', error);
  }
}

export async function sendAndPushTest() {
  try {
    const payload = {
      message: {
        token: 'eF2Fl7ZlQFGZbravqEK8aQ:APA91bFr9BGZwQ9mWwzoYD-wBNyd6rt1wlg3QFrg5lPZ4QGZm3-vCYlntStvu5e4l2GeXRW7iBP8Yob9g30BLF67auTvkFfmPP1rf8OXxUcvev8ZIo1NROs',
        notification: {
          title: 'Incoming call',
          body: 'You have an incoming call'
        },
        data: {
          priority: 'high',
          type: 'accept',
          call_uuid: uuidv4(),
          caller: 'Jayaselva M',
          iuid: '123',
          screen: 'AcceptCall'
        },
        android: {
          ttl: '2s',
          priority: 'high',
          notification: {
            sound: 'default',
          }
        }
      }
    };
    const accessToken = await getAndroidTestAccessToken();
    console.log('accessToken====>', accessToken);
    const config = {
      method: 'post',
      maxBodyLength: Infinity,
      url: 'https://fcm.googleapis.com/v1/projects/ccaas-328c6/messages:send',
      headers: {
        'Content-Type': 'application/json',
        Authorization: `Bearer ${accessToken}`,
      },
      data: JSON.stringify(payload),
    };

    axios.request(config).then((response: any) => {
      console.log('FCM send Successfully------->', JSON.stringify(response.data));
    }).catch((error) => {
      console.log(error.response ? error.response.data : error.message);
    });

  } catch (err) {
    console.log('android catch err:', err);
  }
}

async function getAndroidTestAccessToken() {
  try {
    return new Promise(async (resolve, reject) => {

      const MESSAGING_SCOPE = 'https://www.googleapis.com/auth/firebase.messaging';
      const scopes = [MESSAGING_SCOPE];
      const rootDir = require('path').resolve('./');
      const key = require(`${rootDir}/ccaas-328c6-firebase-adminsdk-fbsvc-5906cf377e.json`);
      const jwtClient = new google.auth.JWT(
        key.client_email,
        null,
        key.private_key,
        scopes,
        null
      );

      jwtClient.authorize((err, tokens) => {
        if (err) {
          reject(err);
          return;
        }
        resolve(tokens.access_token);
      });
    });
  } catch (error) {
    console.log('GetAccessToken error:', error);
  }
}

async function getAccessToken() {
  try {
    return new Promise(async (resolve, reject) => {

      const MESSAGING_SCOPE = 'https://www.googleapis.com/auth/firebase.messaging';
      const scopes = [MESSAGING_SCOPE];
      const rootDir = require('path').resolve('./');
      const key = require(`${rootDir}/worktual-ccaas-firebase-adminsdk-mn9bn-75e5de2654.json`);
      const jwtClient = new google.auth.JWT(
        key.client_email,
        null,
        key.private_key,
        scopes,
        null
      );

      jwtClient.authorize((err, tokens) => {
        if (err) {
          reject(err);
          return;
        }
        resolve(tokens.access_token);
      });
    });
  } catch (error) {
    console.log('GetAccessToken error:', error);
  }
}

async function getToken() {
  const teamId = '7ZY4PU2HAT';
  const now = Math.floor(Date.now() / 1000);
  const expiresIn = Math.floor(Date.now() / 1000) + 1800;
  const exp = now + expiresIn;

  const payload = {
    iss: teamId,
    iat: now,
    exp: now + expiresIn
  };

  const keyId = 'ZN9UCZ5MAP';

  const signOptions: any = {
    algorithm: 'ES256',
    header: {
      alg: 'ES256',
      kid: keyId,
    },
  };
  const privateKey = fs.readFileSync('./AuthKey_ZN9UCZ5MAP.p8');
  const jd: any = {
    expiresIn: '24h',
    algorithm: 'ES256',
  };
  const accessToken = jwt.sign(payload, privateKey, signOptions, jd);
  console.log('accessToken-------->', accessToken);
  return accessToken;
}

/*     const apnProvider = new apn.Provider({
      token: {
        key: './AuthKey_ZN9UCZ5MAP.p8',
        keyId: 'ZN9UCZ5MAP',
        teamId: '7ZY4PU2HAT',
      },
      production: false,
    });

    const notification = new apn.Notification();
    notification.alert = 'Hello, this is a push notification!';
    notification.sound = 'default';
    notification.badge = 1;
    notification.topic = 'com.worktual.ccaas.voip';
    const deviceToken = '8e3b9d48929eb73ffae754f6020773af1d2213ca310072db9a4f1c0b8f702297';

    // Send the notification
    apnProvider.send(notification, deviceToken)
      .then((response) => {
        console.log('Notification sent:', JSON.stringify(response));
      })
      .catch((error) => {
        console.error('Error sending notification:', JSON.stringify(error));
      }); */

/* if (registrationTokens) {
  const message = {
    registration_ids: [registrationTokens],
    priority: 'high',
    notification: {
      title: `${customerNumber}`,
      body: 'Customer Support UK',
      sound: 'default',
    },
    data: {
      title: "Incoming call",
      body: "Tammy",
      type: "accept",
      avatarUrl: "",
      actionUrl: `${req.iuid}/${customerNumber}/${'New Customer'}/callQueueName`
    },
    collapse_key: 'green',
    topic: 'com.unitedfone.unifiedring',
    time_to_live: 10,
  };
  fcm.send(message, (err: any, response: any) => {
    const res = JSON.parse(response ?? false);
    if (res?.success === 1) {
      console.log('Notificaiton send successfully:::::', res);
    } else {
      console.log('ERROR:::::::', err);
    }
  });
} */
