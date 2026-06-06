import { logger } from "../log";

const schedule = require('node-schedule');


export class cronJob {
  socket: any
  constructor(socket: any) {
    this.socket = socket;
  }


  public async startCron(meetingdetails: any, participant: any) {
    let that = this
    const startTime = new Date(Date.now() + 60000);
    const endTime = new Date(startTime.getTime() + 1000);
    const job = schedule.scheduleJob({ start: startTime, end: endTime, rule: '*/1 * * * * *' }, function () {
      console.log('Cron started');

      that.socket.cronExecution(meetingdetails, participant)
    });
  }

  public async startHostLeaveCron(meetingdetails: any, participant: any) {
    let that = this
    const startTime = new Date(Date.now() + 60000);
    const endTime = new Date(startTime.getTime() + 1000);
    const job = schedule.scheduleJob({ start: startTime, end: endTime, rule: '*/1 * * * * *' }, function () {
      console.log('Cron started');

      that.socket.cronExecutionHostLeave(meetingdetails, participant)
    });
  }

  public async transportErrorCron(meetingdetails: any, participant: any, delay?: any) {

    console.log("transportErrorCron started", delay);
    logger.info("transportErrorCron started", delay);
    let that = this
    const startTime = new Date(Date.now() + delay);
    const endTime = new Date(startTime.getTime() + 1000);
    const job = schedule.scheduleJob({ start: startTime, end: endTime, rule: '*/1 * * * * *' }, function () {
      console.log('transport Cron Executed', participant);
      logger.info("transport Cron Executed", participant);

      that.socket.transportErrorCronExecution(meetingdetails, participant)
    });
  }
}


