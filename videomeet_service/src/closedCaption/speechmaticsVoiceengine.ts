import { io } from 'socket.io-client';
import { SpeechmaticsService } from './speechmaticService'

export class SpeechmaticsVoiceEngine {
    speechService: any = new SpeechmaticsService(this)

    meetinfo: any = "";
    sio: any = ""
    recognizeStarted: boolean = false
    constructor(sio: any) {
        this.sio = sio
    }

    public async startConnection(meetInfo: any) {

        this.meetinfo = meetInfo

        console.log("Start connection calling");
        this.speechService.connect();

        this.speechService.startStreaming(parseInt(this.meetinfo.sipLoginId))
    }

    public async receiveVoicedata(data: any) {

        //await this.sleep(2000);
        try{

           // console.log("this.recognizeStarted >>>>.", this.recognizeStarted)

            if (this.recognizeStarted) {
                await this.speechService.sendAudioBuffer(data);
            }


        }catch(e){
        console.log("receiveVoicedata", e)
        }
       
    }


    public async broacastToClient(data: any) {
 

    try {
        // await this.sio.broadcastTranscript(this.meetinfo, data)
        
    } catch (error) {
        
    }
       
    }
    public async recogonitionStart() {

        console.log("recogonitionStart >>>>>>>.")
        this.recognizeStarted = true

        await this.sio.reconizationStarted(this.meetinfo, "Started");

    }


    public async sleep(ms: any) {
        return new Promise(
            resolve => setTimeout(resolve, ms)
        );
    }


    public async stopClosedcaption() {

        console.log("stopClosedcaption Calling >>>>");
        this.speechService.stopRecognition()
        this.recognizeStarted = false
    }

    public async endOfTranscript() {

        console.log("endOfTranscript >>>>>>>")
        this.recognizeStarted = false

        await this.sio.endOfTranscriptSucces(this.meetinfo)
       
    }

}