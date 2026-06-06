import _ from 'lodash';
import { ChatFlowAdapters } from '../chatflow2.0';
import { isJSON } from '../constants';
import { splitFirst } from '../helpers/constants';
import { B2cRedisClient, ioredisChat, NLPRedisClient, otherRedisClient, pubsubRedisClient, redisClient, redisClientAuth, subscriberEmitter } from '../plugins/db';

const RedisAdapter = require('socket.io-redis');

export class RedisAdapters {
  public static sessionInstances: any;
  private adaptor: any;
  private sub: any;
  private pub: any;
  private dub: any;
  private authpub: any;
  private socketadapter: any;
  private templateStreamClient: any;
  private sendtemplateStreamClient: any;
  private rulebaseStreamClient: any;
  private sendrulebasetemplateStreamClient: any;
  private litebaseStreamClient: any;
  private sendlitetemplateStreamClient: any;
  private agentAssistReqStreamClient: any;
  private agentAssistConvoReqStreamClient: any;
  private agentAssistVideoReqStreamClient: any;
  private nlpWBsub: any;
  private agentbaseStreamClient: any;
  private agentbaseConvoStreamClient: any;
  private b2csearchbasedStreamClient: any;
  private nlpVisitorInsightsStreamClient: any;
  constructor(socketadapter?: any) {
    this.pub = redisClient;
    this.sub = pubsubRedisClient;
    this.dub = otherRedisClient;
    this.authpub = redisClientAuth;
    this.templateStreamClient = NLPRedisClient.duplicate();
    this.sendtemplateStreamClient = NLPRedisClient.duplicate();
    this.rulebaseStreamClient = NLPRedisClient.duplicate();
    this.sendrulebasetemplateStreamClient = NLPRedisClient.duplicate();
    this.b2csearchbasedStreamClient = B2cRedisClient.duplicate();
    this.nlpVisitorInsightsStreamClient = NLPRedisClient.duplicate();
    this.litebaseStreamClient = NLPRedisClient.duplicate();
    this.sendlitetemplateStreamClient = NLPRedisClient.duplicate();
    this.agentAssistReqStreamClient = NLPRedisClient.duplicate();
    this.agentAssistConvoReqStreamClient = NLPRedisClient.duplicate();
    this.agentAssistVideoReqStreamClient = NLPRedisClient.duplicate();
    const subscriberConvo = NLPRedisClient.duplicate();
    const subscriber1 = NLPRedisClient.duplicate();
    this.nlpWBsub = NLPRedisClient.duplicate();
    const subscriberweb = NLPRedisClient.duplicate();

    this.agentbaseStreamClient = NLPRedisClient.duplicate();
    this.agentbaseConvoStreamClient = NLPRedisClient.duplicate();

    RedisAdapters.sessionInstances = {} as any;

    this.adaptor = RedisAdapter({ pubClient: this.dub, subClient: subscriberEmitter });
    this.socketadapter = socketadapter;
    // this.BLPOPPER();

    // this.BLPOP();
    /* tslint:disable */
    const self: any = this;
    // (async function responseAIblpop() {

    //   console.log("ruleBasereadStream >>>>>>")
    //   client2.xread("BLOCK", 0, "STREAMS", `${process.env.CONVER_STREAM_RESPONSE}`, "$").then((result) => {
    //     // Process the result, which is an array of messages
    //     // console.log("ruleBasereadStream >>>>>>> result", result)
    //     if (result) {
    //       const [stream, messages] = result[0];
    //       messages.forEach(([messageId, messageData]) => {
    //         //console.log(`  ruleBasereadStream: `, JSON.parse(messageData[1]));
    //         self.socketadapter.receiveMessageFromNLP(messageData[1]);
    //       });

    //     } else {
    //       console.log("No new messages.");
    //     }

    //     responseAIblpop();
    //   })
    //     .catch((error) => {
    //       console.error("Error reading from Redis:", error);
    //       responseAIblpop()
    //     });

    // })();


    // (function templateAIblpop() {
    //   console.log("templateAIblpop calling>>>>>", process.env.TEMPLATE_REDIS_CHANNEL_RES)
    //   client4.blpop(`${process.env.TEMPLATE_REDIS_CHANNEL_RES}`, 60000).then((data: any) => {
    //     try {
    //       console.log("templateAIblpop data >>>>>", data);
    //       if (data !== null || data !== undefined) {
    //         self.socketadapter.receiveMessageFromTemplateResponse(data?.at(1));
    //         templateAIblpop();
    //       } else {
    //         templateAIblpop();
    //       }
    //     } catch (error) {
    //       templateAIblpop();
    //     }
    //   })
    // })();

    (function agentAssistConvoblpop() {
      subscriberConvo.blpop(`${process.env.AGENT_ASSIT_CONVO_REDIS_CHANNEL_RES}`, 60000).then((data: any) => {
        try {
          if (data !== null || data !== undefined) {
            self.socketadapter.receiveMessageFromAgentAssist(data?.at(1));
            agentAssistConvoblpop();
          } else {
            agentAssistConvoblpop();
          }
        } catch (error) {
          agentAssistConvoblpop();
        }
      })
    })();

    // (function agentAssistblpop() {
    //   console.log("agentAssistblpop calling>>>>>", process.env.AGENT_ASSIT_REDIS_CHANNEL_RES)
    //   subscriber.blpop(`${process.env.AGENT_ASSIT_REDIS_CHANNEL_RES}`, 60000).then((data: any) => {
    //     try {
    //       console.log("agentAssistblpop data >>>>>", data);
    //       if (data !== null || data !== undefined) {
    //         self.socketadapter.receiveMessageFromAgentAssist(data?.at(1));
    //         agentAssistblpop();
    //       } else {
    //         agentAssistblpop();
    //       }
    //     } catch (error) {
    //       agentAssistblpop();
    //     }
    //   })
    // })();

    // (function agentStatus() {
    //   console.log("agent status>>>>>", process.env.PRESENCE_CHANNEL_REQUEST)
    //   subscriber2.blpop(`${process.env.PRESENCE_CHANNEL_REQUEST}`, 60000).then((data: any) => {
    //     try {
    //       if (data !== null || data !== undefined) {
    //         self.socketadapter.presenceResponse(data?.at(1));
    //         agentStatus();
    //       } else {
    //         agentStatus();
    //       }
    //     } catch (error) {
    //       agentStatus();
    //     }
    //   });
    // })();

    (function webScrapeblpop() {
      subscriberweb.blpop(`${process.env.WEBSITE_SCRAPE_RESPONSE}`, 60000).then((data: any) => {
        try {
          console.log("agentAssistblpop data >>>>>", data);
          if (data !== null || data !== undefined) {
            self.socketadapter.receiveMessageFromWebscrape(data?.at(1));
            webScrapeblpop();
          } else {
            webScrapeblpop();
          }
        } catch (error) {
          webScrapeblpop();
        }
      })
    })();

    (async function ruleBasereadStream() {
      subscriber1.xread("BLOCK", 0, "STREAMS", `${process.env.TEMPLATE_STREAM_RESPONSE}`, "$").then((result) => {
        // Process the result, which is an array of messages
        // console.log("ruleBasereadStream >>>>>>> result", result)
        if (result) {
          const [stream, messages] = result[0];
          messages.forEach(([messageId, messageData]) => {
            self.socketadapter.receiveMessageFromTemplateResponse(messageData[1]);
          });
        } else {
          console.log("No new messages.");
        }
        ruleBasereadStream();
      })
        .catch((error) => {
          console.error("Error reading from Redis:", error);
          ruleBasereadStream()
        });

    })();

    /**
     * DELETE Rule Based Stream
     */

    this.templateStreamClient.xtrim(process.env.CONVER_BASE_STREAM_NAME_RES, 'MAXLEN', 0)
      .then(() => {
        console.log(`Stream "${process.env.CONVER_BASE_STREAM_NAME_RES}" has been deleted.`);
        self.convBaseCreateGroup();
      })
      .catch((err) => {
        console.error('Error deleting the stream:', err);
      })
      .finally(() => {
        // client.quit();
      });


    this.rulebaseStreamClient.xtrim(process.env.TEMP_BASE_STREAM_NAME_RES, 'MAXLEN', 0)
      .then(() => {
        console.log(`Stream "${process.env.TEMP_BASE_STREAM_NAME_RES}" has been deleted.`);
        self.ruleBaseCreateGroup();
      })
      .catch((err: any) => {
        console.error('Error deleting the stream:', err);
      })
      .finally(() => {
        // client.quit();
      });
    //   let i = 0;
    //  setInterval(() => {
    //    self.sendRuleBasedSteam(i)
    //    i++;

    //  }, 6000) 

    this.litebaseStreamClient.xtrim(process.env.LITE_BASE_STREAM_NAME_RES, 'MAXLEN', 0)
      .then(() => {
        console.log(`litebaseStreamClientStream "${process.env.LITE_BASE_STREAM_NAME_RES}" has been deleted.`);
        self.liteBaseCreateGroup();
      })
      .catch((err) => {
        console.error('Error deleting the stream:', err);
      })
      .finally(() => {
        // client.quit();
      });

    this.agentbaseStreamClient.xtrim(process.env.AGENT_ASSIT_STREAM_NAME_RES, 'MAXLEN', 0)
      .then(() => {
        console.log(`litebaseStreamClientStream "${process.env.AGENT_ASSIT_STREAM_NAME_RES}" has been deleted.`);
        self.agentBaseCreateGroup();
      })
      .catch((err: any) => {
        console.error('Error deleting the stream:', err);
      })
    this.agentbaseConvoStreamClient.xtrim(process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, 'MAXLEN', 0)
      .then(() => {
        console.log(`litebaseStreamClientStream "${process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES}" has been deleted.`);
        self.agentBaseConvoAssistCreateGroup();
      })
      .catch((err: any) => {
        console.error('Error deleting the stream:', err);
      })
  }



  /**
   * Rule based chat implementation
   */

  public convBaseDeleteGroup() {
    let self = this;
    this.templateStreamClient.xgroup('DESTROY', process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP)
      .then((response: any) => {
        console.log(`Stream "${process.env.CONVER_BASE_STREAM_NAME_RES}" and consumer group "${process.env.CONVER_BASE_CONSUMER_GROUP}" destroyed.`);
        self.convBaseCreateGroup();
      })
      .catch((err) => {
        console.error('Error destroying stream and consumer group:', err);
        self.convBaseCreateGroup();
      });
  };

  public ruleBaseDeleteGroup() {
    let self = this;
    this.rulebaseStreamClient.xgroup('DESTROY', process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP)
      .then((response) => {
        self.ruleBaseCreateGroup();

      })
      .catch((err) => {
        console.error('Error destroying stream and consumer group:', err);
        self.ruleBaseCreateGroup();
      });
  };


  public liteBaseDeleteGroup() {
    let self = this;
    this.litebaseStreamClient.xgroup('DESTROY', process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP)
      .then((response) => {
        self.liteBaseCreateGroup();

      })
      .catch((err) => {
        console.error('Error destroying stream and consumer group:', err);
        self.liteBaseCreateGroup();
      });
  };

  public agentBaseDeleteGroup() {
    let self = this;
    this.agentbaseStreamClient.xgroup('DESTROY', process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP)
      .then((response: any) => {
        self.agentBaseCreateGroup();

      })
      .catch((err: any) => {
        console.error('Error destroying stream and consumer group:', err);
        self.agentBaseCreateGroup();
      });
  };



  public agentBaseConvoAssistDeleteGroup() {
    let self = this;
    this.agentbaseConvoStreamClient.xgroup('DESTROY', process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP)
      .then((response: any) => {
        self.agentBaseConvoAssistCreateGroup();

      })
      .catch((err: any) => {
        console.error('Error destroying stream and consumer group:', err);
        self.agentBaseConvoAssistCreateGroup();
      });
  };

    public videoChannelAssistDeleteGroup() {
    let self = this;
    this.agentAssistVideoReqStreamClient.xgroup('DESTROY', process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP)
      .then((response: any) => {
        self.videoChannelAssistCreateGroup();

      })
      .catch((err: any) => {
        console.error('Error destroying stream and consumer group:', err);
        self.videoChannelAssistCreateGroup();
      });
  };


  public convBaseDeleteConsumer() {
    let self = this;
    this.templateStreamClient.xgroup('DELCONSUMER', process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP, process.env.CONVER_BASE_CONSUMER_NAME)
      .then((response) => {
        if (response === 1) {
          self.convBaseDeleteGroup();
        } else {
          self.convBaseDeleteGroup();
        }
      })
      .catch((err) => {
        console.error('Error removing consumer:', err);
        self.convBaseDeleteGroup();
      });
  };

  public ruleBaseDeleteConsumer() {
    let self = this;
    this.rulebaseStreamClient.xgroup('DELCONSUMER', process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP, process.env.TEMP_BASE_CONSUMER_NAME)
      .then((response) => {
        if (response === 1) {
          self.ruleBaseDeleteGroup();
        } else {
          self.ruleBaseDeleteGroup();
        }
      })
      .catch((err) => {
        console.error('Error removing consumer:', err);
        self.ruleBaseDeleteGroup();
      });
  };

  public liteBaseDeleteConsumer() {
    let self = this;
    this.litebaseStreamClient.xgroup('DELCONSUMER', process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP, process.env.LITE_BASE_CONSUMER_NAME)
      .then((response) => {
        if (response === 1) {
          self.liteBaseDeleteGroup();
        } else {
          self.liteBaseDeleteGroup();
        }
      })
      .catch((err) => {
        console.error('Error removing consumer:', err);
        self.liteBaseDeleteGroup();
      });
  };

  public agentBaseDeleteConsumer() {
    let self = this;
    this.agentbaseStreamClient.xgroup('DELCONSUMER', process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONSUMER_NAME)
      .then((response: any) => {
        if (response === 1) {
          self.agentBaseDeleteGroup();
        } else {
          self.agentBaseDeleteGroup();
        }
      })
      .catch((err: any) => {
        console.error('Error removing consumer:', err);
        self.agentBaseDeleteGroup();
      });
  };


  public agentBaseConvoAssistDeleteConsumer() {
    let self = this;
    this.agentbaseConvoStreamClient.xgroup('DELCONSUMER', process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME)
      .then((response: any) => {
        if (response === 1) {
          self.agentBaseConvoAssistDeleteGroup();
        } else {
          self.agentBaseConvoAssistDeleteGroup();
        }
      })
      .catch((err: any) => {
        self.agentBaseConvoAssistDeleteGroup();
      });
  };

    public videoChannelAssistDeleteConsumer() {
    let self = this;
    this.agentAssistVideoReqStreamClient.xgroup('DELCONSUMER', process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME)
      .then((response: any) => {
        if (response === 1) {
          self.videoChannelAssistDeleteGroup();
        } else {
          self.videoChannelAssistDeleteGroup();
        }
      })
      .catch((err: any) => {
        self.videoChannelAssistDeleteGroup();
      });
  };



  public ruleBasedacknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    // Use XACK to acknowledge the message
    this.rulebaseStreamClient.xack(stream, group, messageId, (err, result) => {
      if (err) {
      } else if (result === 1) {
        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
      } else {
        console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
        const inputData = {
          stream,
          group,
          messageId,
          message: messageData,
          isAck: false
        };
      }
    });
  }

  public liteBasedacknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    // Use XACK to acknowledge the message
    this.litebaseStreamClient.xack(stream, group, messageId, (err, result) => {
      if (err) {
        console.error('Error Androidacknowledging message:', err);
      } else if (result === 1) {
        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
      } else {
        console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
        const inputData = {
          stream,
          group,
          messageId,
          message: messageData,
          isAck: false
        };
      }
    });
  }

  public agentBasedacknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    this.agentbaseStreamClient.xack(stream, group, messageId, (err, result) => {
      if (err) {
        console.error('Error Androidacknowledging message:', err);
      } else if (result === 1) {
        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
      } else {
        console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
        const inputData = {
          stream,
          group,
          messageId,
          message: messageData,
          isAck: false
        };
      }
    });
  }


  public agentBasedConvoAssistacknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    this.agentbaseConvoStreamClient.xack(stream, group, messageId, (err: any, result: any) => {
      if (err) {
        console.error('Error Androidacknowledging message:', err);
      } else if (result === 1) {
        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);
      } else {
        console.log(`${stream}_${group}  Message ${messageId}_ACK not found in consumer group`);
        const inputData = {
          stream,
          group,
          messageId,
          message: messageData,
          isAck: false
        };
      }
    });
  }


  public convBasedacknowledgeMessage(stream: any, group: any, messageId: any, messageData: any) {
    // Use XACK to acknowledge the message
    this.templateStreamClient.xack(stream, group, messageId, (err, result) => {
      if (err) {
        console.error('Error Androidacknowledging message:', err);
      } else if (result === 1) {

        console.log(`${stream}_${group}  Acknowledged message ${messageId}_ACK`);

      } else {
        console.log(`${stream}_${group}  Message ${messageData.iuid}_${messageId}_ACK not found in consumer group`);
        const inputData = {
          stream,
          group,
          messageId,
          message: messageData,
          isAck: false
        };
      }
    });
  }

  public sendB2cSearchSteam = (jsonData: any) => {
    const streamKey = process.env.B2C_STREAM_BASE_NAME_REQ;
    this.b2csearchbasedStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      } else {
        // console.log('Added entry with message ID:', messageId);
        // ack(messageId)
      }
    });
  };

  public sendwebsiteInsightsSteam = (jsonData: any) => {
    const streamKey = process.env.NLP_VISITORS_INSIGHTS_REQ;
    this.nlpVisitorInsightsStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      }
    });
  };

  public sendTempBasedSteam = (jsonData: any) => {

    const streamKey = process.env.CONVER_BASE_STREAM_NAME_REQ;
    //const jsonData = { 'question': 'Hello', 'sessionId': `${new Date().getTime()}`, 'bot_id': 2, 'botType': 'convo', 'domainId': 10300, 'websiteId': '', 'ipaddress': '194.66.175.90', 'isClose': 0 }
    //{ foo: 'Selva', baz: ie, iuid: `274d7496f5f-ccda-123c-11b0-00505690f345_10.150.0.182_${new Date().getTime()}` }; // JSON data to send

    //const streamData = [['data', JSON.stringify(jsonData)]]; // Format the JSON data as a stream entry

    // const that = this;
    console.log('sendSteam >>>>>>', jsonData);

    this.sendtemplateStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      } else {
        // console.log('Added entry with message ID:', messageId);
        // ack(messageId)
      }
    });

  };

  public sendRuleBasedSteam = (jsonData: any) => {

    const streamKey = process.env.TEMP_BASE_STREAM_NAME_REQ;
    this.sendrulebasetemplateStreamClient.xadd(streamKey, '*', 'message', (jsonData), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      } else {
        // console.log('Added entry with message ID:', messageId);
        // ack(messageId)
      }
    });

  };

  public convBaseReadMessages(stream: any, group: any, consumer: any) {
    // Use XREADGROUP to read messages
    let self = this;
    this.templateStreamClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
        // 'GROUP', group, consumer, 'STREAMS', stream, '$', (err, streams) => {
        if (err) {
          console.error('Error reading messages:', err);
          // redis.quit();
        } else {
          // Process the messages
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              messages.forEach(([messageId, messageData]) => {
                // const rawMessage = `${messageData}`.split('message,');
                const rawMessage = splitFirst(`${messageData}`, 'message,')
                console.log("rawMessage >>>>>>", rawMessage);
                self.convBasedacknowledgeMessage(stream, group, messageId, isJSON(rawMessage[1]) ? JSON.parse(rawMessage[1]) : rawMessage[1]);
                self.socketadapter.receiveMessageFromNLP(messageData[1]);
                self.templateStreamClient.xtrim(stream, 'MAXLEN', 100)
              });
            });
          }

          // Continue reading messages
          self.convBaseReadMessages(stream, group, consumer);
        }
      }
    );
  }


  public ruleBaseReadMessages(stream: any, group: any, consumer: any) {
    // Use XREADGROUP to read messages
    let self = this;
    this.rulebaseStreamClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
        // 'GROUP', group, consumer, 'STREAMS', stream, '$', (err, streams) => {
        if (err) {
          console.error('Error reading messages:', err);
          // redis.quit();
        } else {
          // Process the messages
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              // console.log(`Received messages from ${streamName}:`);
              messages.forEach(([messageId, messageData]) => {
                //console.log(`readMessages >>>>> ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                // const rawMessage = `${messageData}`.split('message,');
                // console.log("rule base rawMessage >>>>>>", rawMessage);
                self.ruleBasedacknowledgeMessage(stream, group, messageId, JSON.parse(messageData[1]));
                self.socketadapter.receiveMessageFromTemplateResponse(messageData[1]);
                self.rulebaseStreamClient.xtrim(stream, 'MAXLEN', 100)
              });
            });
          }

          // Continue reading messages
          self.ruleBaseReadMessages(stream, group, consumer);
        }
      }
    );
  }

  public liteBaseReadMessages(stream: any, group: any, consumer: any) {
    // Use XREADGROUP to read messages
    console.log(stream, group, consumer, "consumer")
    let self = this;
    this.litebaseStreamClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err, streams) => {
        // 'GROUP', group, consumer, 'STREAMS', stream, '$', (err, streams) => {
        if (err) {
          console.error('Error reading messages:', err);
          // redis.quit();
        } else {
          // Process the messages
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              // console.log(`Received messages from ${streamName}:`);
              messages.forEach(([messageId, messageData]) => {
                //console.log(`readMessages >>>>> ${stream}_${group} Message ID: ${messageId}, Message Data: ${messageData}`);
                // const rawMessage = `${messageData}`.split('message,');
                const rawMessage = splitFirst(`${messageData}`, 'message,')
                console.log("rule base rawMessage >>>>>>", rawMessage);
                self.socketadapter.receiveMessageFromLiteResponse(messageData[1]);
                self.liteBasedacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                self.rulebaseStreamClient.xtrim(stream, 'MAXLEN', 100)
              });
            });
          }

          // Continue reading messages
          self.liteBaseReadMessages(stream, group, consumer);
        }
      }
    );
  }


  public agentBaseReadMessages(stream: any, group: any, consumer: any) {
    console.log(stream, group, consumer, "consumer")
    let self = this;
    this.agentbaseStreamClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err: any, streams: any) => {
        if (err) {
          console.error('Error reading messages:', err);
        } else {
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              messages.forEach(([messageId, messageData]) => {
                // const rawMessage = `${messageData}`.split('message,');
                const rawMessage = splitFirst(`${messageData}`, 'message,')
                console.log("rule base rawMessage >>>>>>", rawMessage);
                self.agentBasedacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                self.socketadapter.receiveMessageFromAgentAssitResponse(messageData[1]);
                self.agentbaseStreamClient.xtrim(stream, 'MAXLEN', 100)
              });
            });
          }
          self.agentBaseReadMessages(stream, group, consumer);
        }
      }
    );
  }



  public agentBaseConvoAssistReadMessages(stream: any, group: any, consumer: any) {
    let self = this;
    this.agentbaseConvoStreamClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err: any, streams: any) => {
        if (err) {
          console.error('Error reading messages:', err);
        } else {
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              messages.forEach(([messageId, messageData]) => {
                // const rawMessage = `${messageData}`.split('message,');
                const rawMessage = splitFirst(`${messageData}`, 'message,')
                console.log("rule base rawMessage >>>>>>", rawMessage);
                self.agentBasedConvoAssistacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                self.socketadapter.receiveMessageFromAgentAssitResponse(messageData[1]);
                self.agentbaseStreamClient.xtrim(stream, 'MAXLEN', 100)
              });
            });
          }
          self.agentBaseConvoAssistReadMessages(stream, group, consumer);
        }
      }
    );
  }

  public videoChannelAssistReadMessages(stream: any, group: any, consumer: any) {
    let self = this;
    this.agentAssistVideoReqStreamClient.xreadgroup(
      'GROUP', group, consumer, 'BLOCK', 0, 'STREAMS', stream, '>', (err: any, streams: any) => {
        if (err) {
          console.error('Error reading messages:', err);
        } else {
          if (streams) {
            streams.forEach((entry: any) => {
              const [streamName, messages] = entry;
              messages.forEach(([messageId, messageData]) => {
                // const rawMessage = `${messageData}`.split('message,');
                const rawMessage = splitFirst(`${messageData}`, 'message,')
                console.log("rule base rawMessage >>>>>>", rawMessage);
                self.agentBasedConvoAssistacknowledgeMessage(stream, group, messageId, JSON.parse(rawMessage[1]));
                self.socketadapter.receiveMessageFromAgentAssitResponse(messageData[1]);
                self.agentbaseStreamClient.xtrim(stream, 'MAXLEN', 100);
              });
            });
          }
          self.videoChannelAssistReadMessages(stream, group, consumer);
        }
      }
    );
  }


  public convBaseCreateGroup() {
    let self = this;
    this.templateStreamClient.xgroup('CREATE', process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);

        self.convBaseDeleteConsumer();
      } else {
        self.convBaseReadMessages(process.env.CONVER_BASE_STREAM_NAME_RES, process.env.CONVER_BASE_CONSUMER_GROUP, process.env.CONVER_BASE_CONSUMER_NAME);
      }
    });
  };

  public ruleBaseCreateGroup() {
    let self = this;
    this.rulebaseStreamClient.xgroup('CREATE', process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);

        self.ruleBaseDeleteConsumer();
      } else {
        self.ruleBaseReadMessages(process.env.TEMP_BASE_STREAM_NAME_RES, process.env.TEMP_BASE_CONSUMER_GROUP, process.env.TEMP_BASE_CONSUMER_NAME);
      }
    });
  };

  public liteBaseCreateGroup() {
    let self = this;
    this.litebaseStreamClient.xgroup('CREATE', process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);
        self.liteBaseDeleteConsumer();
      } else {
        self.liteBaseReadMessages(process.env.LITE_BASE_STREAM_NAME_RES, process.env.LITE_BASE_CONSUMER_GROUP, process.env.LITE_BASE_CONSUMER_NAME);
      }
    });
  };

  public agentBaseCreateGroup() {
    let self = this;
    this.agentbaseStreamClient.xgroup('CREATE', process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP, '0', 'MKSTREAM', (err) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);
        self.agentBaseDeleteConsumer();
      } else {
        console.log('Start read readMessage >>>>>>>>');
        self.agentBaseReadMessages(process.env.AGENT_ASSIT_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONSUMER_NAME);
      }
    });
  };


  public agentBaseConvoAssistCreateGroup() {
    let self = this;
    this.agentbaseConvoStreamClient.xgroup('CREATE', process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, '0', 'MKSTREAM', (err: any) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);
        self.agentBaseConvoAssistDeleteConsumer();
      } else {
        self.agentBaseConvoAssistReadMessages(process.env.AGENT_ASSIT_CONVO_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME);
      }
    });
  };

    public videoChannelAssistCreateGroup() {
    let self = this;
    this.agentAssistVideoReqStreamClient.xgroup('CREATE', process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, '0', 'MKSTREAM', (err: any) => {
      if (err && !err.message.includes('BUSYGROUP Consumer Group name already exists')) {
        console.error('Error creating consumer group:', err);
        self.videoChannelAssistDeleteConsumer();
      } else {
        self.videoChannelAssistReadMessages(process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_RES, process.env.AGENT_ASSIT_CONVO_CONSUMER_GROUP, process.env.AGENT_ASSIT_CONVO_CONSUMER_NAME);
      }
    });
  };


  /** End of Rule base chat stream implementation   */


  public push(channel: any, msg: any): void {
    this.dub.rpush(channel, JSON.stringify({ msg }));
  }

  // public presencePushToUmn(channel: any, msg: any): void {
  //   this.pub.rpush(channel, JSON.stringify(msg));
  // }
  public createSet(key: any, data: any) {
    this.dub.sadd(key, data);
  }

  public setVariableValue(name: any, keyName: any, keyValue: any) {
    this.dub.hset(name, keyName, keyValue);
  }

  // public presenceAllChannel(data: any) {
  //   this.dub.hset(`${data.domainId}_presence`, `${data?.domainId}_${data?.ext}`, JSON.stringify({ ...data, currentTime: Date.now() }));
  // }

  public getapiData() {
    return new Promise((resolve, reject) => {
      try {
        this.dub.hgetall('apiValues', (err, results) => {
          if (results) {
            resolve(results);
          } else {
            reject(err);
          }
        });
      } catch (error) {
        reject(error);
      }
    });
  }

  public getValuefromSet(key: any) {
    return new Promise((resolve, reject) => {
      this.dub.smembers(key, (err, data) => {
        resolve(data);
      });
    });
  }

  public getFlow(key: any) {
    return new Promise((resolve, reject) => {
      this.sub.get(key).then((result) => {
        resolve(result);
      });
    });
  }

  // DELETE THIS KEY ONCE TRANSFER / MERGE SESSION IS COMPLETED
  public publishMessage(key: any) {
    console.log(key);
    this.sub.publish(process.env.RESPONSECHANNEL, key);
    // this.BLPOP();
  }

  public publishVideoMessage(key: any) {
    console.log(key, "publishVideoMessage===", process.env.RESPONSEVIDEOCHANNEL);
    this.pub.publish(process.env.RESPONSEVIDEOCHANNEL, key);
  }

  public publishMessageForAgent(key: any) {
    console.log(key);

    // this.BLPOP();
  }

  public sendLiteSteam(inputdata: any) {

    console.log("send lite stream callig", inputdata);
    // const streamKey = `${process.env.TEMPLATE_STREAM_REQUEST}` //`${process.env.TEMPLATE_STREAM_RESPONSE}`//`${process.env.TEMPLATE_STREAM_REQUEST}`
    //const jsonData = inputdata; // JSON data to send
    //this.nlpsub.xadd(streamKey, "*", "data", inputdata)
    const streamKey = process.env.LITE_BASE_STREAM_NAME_REQ;
    console.log('sendLiteSteam >>>>>>', inputdata, streamKey);

    this.sendlitetemplateStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      } else {
        console.log('Added entry with message ID:', messageId);
        // ack(messageId)
      }
    });
    // this.sendRuleBasedSteam(inputdata)
  }

  public sendSteam(inputdata: any) {
    // const streamKey = `${process.env.TEMPLATE_STREAM_REQUEST}` //`${process.env.TEMPLATE_STREAM_RESPONSE}`//`${process.env.TEMPLATE_STREAM_REQUEST}`
    //const jsonData = inputdata; // JSON data to send
    //this.nlpsub.xadd(streamKey, "*", "data", inputdata)
    const streamKey = process.env.TEMP_BASE_STREAM_NAME_REQ;
    this.sendrulebasetemplateStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      } else {
        // console.log('Added entry with message ID:', messageId);
        // ack(messageId)
      }
    });
    // this.sendRuleBasedSteam(inputdata)
  }

  public sendConvSteam(inputdata: any) {

    // console.log("send stream callig conv", inputdata);
    const streamKey = `${process.env.CONVER_STREAM_REQUEST}` //`${process.env.TEMPLATE_STREAM_RESPONSE}`//`${process.env.TEMPLATE_STREAM_REQUEST}`
    //const jsonData = inputdata; // JSON data to send
    // this.nlpsub.xadd(streamKey, "*", "data", inputdata)
    this.sendTempBasedSteam(inputdata)

  }


  public sendB2cStream(inputdata: any) {
    this.sendB2cSearchSteam(inputdata)
  }

  /*   public sendCcaasDynamicDashRequest(inputdata: any) {
      this.sendCcaasDynamicDashSteam(inputdata)
    } */

  /*   public sendCcaasAIChatRequest(inputdata: any) {
      this.sendCcaasAiChatSteam(inputdata)
    } */

  public agentAssistXadd(inputdata: any) {
    const streamKey = process.env.AGENT_ASSIT_STREAM_NAME_REQ;
    console.log('sendSteam >>>>>>', inputdata, streamKey);
    this.agentAssistReqStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      }
    });
  }


  // For Convo agent assist

  public agentAssistForConvoXadd(inputdata: any) {
    const streamKey = process.env.AGENT_ASSIT_CONVO_STREAM_NAME_REQ;
    this.agentAssistConvoReqStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      }
    });
  }

  // For Video channel agent assist
  public agentAssistForVideoChannelXadd(inputdata: any) {
    const streamKey = process.env.VIDEO_CHANNEL_AGENT_ASSIST_STREAM_NAME_REQ;
    this.agentAssistVideoReqStreamClient.xadd(streamKey, '*', 'message', (inputdata), (error: any, messageId: any) => {
      if (error) {
        console.error('Error adding entry to stream:', error);
      }
    });
  }


  public publishMessageToWebChat(domainId: any, key: any) {
    console.log('CHAT: ', JSON.parse(key));
    this.dub.rpush(`${domainId}_${process.env.CHATRESPONSECHANNEL}`, key);
  }
  public publishLiteMessageToNLP(key: any) {
    console.log('publishLiteMessageToNLP: ', process.env.TEMPLATE_STREAM_REQUEST, JSON.parse(key));
    this.sendLiteSteam(key)
    //this.nlpsub.rpush(`${process.env.TEMPLATE_REDIS_CHANNEL_REQ}`, key);
  }

  public publishMessageToNLP(key: any) {
    console.log('publishMessageToNLP: ', process.env.TEMPLATE_STREAM_REQUEST, JSON.parse(key));
    this.sendSteam(key)
    //this.nlpsub.rpush(`${process.env.TEMPLATE_REDIS_CHANNEL_REQ}`, key);
  }
  public publishMessageToNLPAI(key: any) {
    console.log('publishMessageToNLPAI: ', process.env.CONVER_REDIS_CHANNEL_REQ, JSON.parse(key));
    this.sendConvSteam(key)
    // this.nlpsub.rpush(`${process.env.CONVER_REDIS_CHANNEL_REQ}`, key);
  }

  public publishMessageToB2c(key: any) {
    console.log('publishMessageToB2c: ', process.env.CONVER_REDIS_CHANNEL_REQ, JSON.parse(key));
    this.sendB2cStream(key)
  }

  public publishWebscrapeToNLP(key: any) {
    console.log('publishMessageToNLP: ', process.env.WEBSITE_SCRAPE_REQUEST, JSON.parse(key));
    this.nlpWBsub.rpush(`${process.env.WEBSITE_SCRAPE_REQUEST}`, key);
  }

  public publishMessageToAgentAssist(key: any) {
    console.log('publishMessageToAgentAssist: ', process.env.AGENT_ASSIT_REDIS_CHANNEL_REQ, key);
    const subscriber = NLPRedisClient.duplicate();
    subscriber.rpush(`${process.env.AGENT_ASSIT_REDIS_CHANNEL_REQ}`, key);
  }
  public publishMessageToAgentAssistForConvo(key: any) {
    console.log('publishMessageToAgentAssistForConvo: ', process.env.AGENT_ASSIT_CONVO_REDIS_CHANNEL_REQ, key);
    const subscriber = NLPRedisClient.duplicate();
    subscriber.rpush(`${process.env.AGENT_ASSIT_CONVO_REDIS_CHANNEL_REQ}`, key);
  }
  public publishMessageChat(data: any) {
    console.log('publishMessageChat >>>>>', data);
    const parsedData = JSON.parse(data);
    if (parsedData?.call_state === 'init') {
      const newObject = new ChatFlowAdapters(this);
      RedisAdapters.sessionInstances[parsedData?.session_id] = newObject;
      newObject?.ChatFlowHandler(data);
    } else if (parsedData?.disconnectedBy) {
      RedisAdapters.sessionInstances[parsedData?.session_id]?.ChatFlowHandler(data);
      delete RedisAdapters.sessionInstances[parsedData?.session_id];
    } else {
      RedisAdapters.sessionInstances[parsedData?.session_id]?.ChatFlowHandler(data);
    }
    // this.pub.rpush('chatRequestChannel', key);

    // this.BLPOP();
  }
  public getredisAdapter(): any {
    return this.adaptor;
  }

  public removeValuebyKey(key: any) {
    return new Promise((resolve, reject) => {
      this.dub.del(key, (err: any, reply: any) => {
        resolve(1);
      });
    });
  }

  public async BLPOP(res: any, domainId: any, data: any) {
    // console.log("BLPOP >>>>>", `${domainId}_${process.env.CHATRESPONSECHANNEL}`)
    this.dub.blpop(`${domainId}_${process.env.CHATRESPONSECHANNEL}`, 1, async (err: any, resp: any) => {
      const time = new Date().getTime();
      if (resp !== null || time > data?.timestamp) {
        console.log(resp, 'pop response');
        res.status(200).send({ statusCode: 200, message: 'Available agent', Result: resp });
        return;
      }
      this.BLPOP(res, domainId, data);
    });
  }

  public handleSessionBasedInstances(data: any) {
    const parsedData = JSON.parse(data);
    if (parsedData?.call_state === 'init') {
      const newObject = new ChatFlowAdapters(this);
      RedisAdapters.sessionInstances[parsedData?.session_id] = newObject;
      newObject?.ChatFlowHandler(data);
    } else if (parsedData?.disconnectedBy) {
      RedisAdapters.sessionInstances[parsedData?.session_id]?.ChatFlowHandler(data);
      delete RedisAdapters.sessionInstances[parsedData?.session_id];
    } else {
      RedisAdapters.sessionInstances[parsedData?.session_id]?.ChatFlowHandler(data);
    }
  }

  public async BLPOPPER() {
    console.log(process.env.CHATRESPONSECHANNEL, 'process.env.CHATRESPONSECHANNEL', this.dub?.blpop);
    this.dub?.blpop('chatRequestChannel', 60000, async (data: any) => {
      console.log('Data >>>>>>.', data);
      if (data !== null && data?.length > 1) {
        const resultData = data?.at(1);
        this.handleSessionBasedInstances(resultData);
      }
      this.BLPOPPER();
    });
  }

  public getValuefromredis(key: any) {
    return new Promise((resolve, reject) => {
      this.authpub.get(key, (err: any, data: any) => {
        resolve(data);
      });
    });
  }

  public emailRpush(data: any, sessionId: any) {
    const emailData = {
      domain_id: data.domainId,
      channelType: 'Email',
      queueName: data.queueName,
      session_id: sessionId,
    };
    this.dub.rpush(process.env.REQUESTCHANNEL, emailData);
  }

  public emailBlpop(data: any) {
    return new Promise((resolve, reject) => {
      this.dub.blpop('CCAAS_email_response', 1, async (err: any, res: any) => {
        if (res !== null) {
          const agentRes: any = res[0];
          ioredisChat.to(`${data.domainId}_${data.ext}`).emit('email_channel_emit', res);
          return res;
        }
        this.emailBlpop(data);
      });
    });
  }

  public resetQueue(domainId: any) {
    this.dub.del(`${domainId}_${process.env.AGENTTRACKERCHANNEL}`);
    // this.newclinet.rpush(process.env.AGENTTRACKERCHANNEL, null);
  }
}