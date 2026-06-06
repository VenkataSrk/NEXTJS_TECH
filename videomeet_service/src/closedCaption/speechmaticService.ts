import { logger } from "../log";



const WebSocket = require('ws');

export class SpeechmaticsService {
    seqNoOut: any = 0;
    seqNoIn: any = 0;
    socket: any = ""
    engineContext: any = "";
    constructor(engine: any) {
        this.seqNoOut = 0;
        this.seqNoIn = 0;
        this.engineContext = engine
    }
    public async connect() {
        this.socket = new WebSocket('wss://rt-speech.unifiedring.co.uk:9000/v2', {
            rejectUnauthorized: false
        });
        this.seqNoOut = 0;
        this.seqNoIn = 0;
    }
    public async startStreaming(sipid:any) {

        let that: any = this;
        this.socket.binaryType = "arraybuffer";
        this.socket.onopen = function (event: any) {
            var msg = {
                "message": "StartRecognition",
                "type": "transcription",
                "transcription_config": {
                    "language": "en",
                    "output_locale": "en-GB",
                    "max_delay": 3.5,
                    "diarization": "speaker_change",
                    "enable_partials": true
                },
                "audio_format": {
                    "type": "raw",
                    "encoding": "pcm_s16le",
                    "sample_rate": 16000
                }
            };

            this.send(JSON.stringify(msg));

        };

        this.socket.onmessage = function (event: any) {

            // console.log("event >>>>>>", event);
            let data = JSON.parse(event.data)
           // console.info(data.seq_no, data.message)

            switch (data.message) {
                case "RecognitionStarted":
                    console.log("recognitionStarted");
                    logger.info("recognitionStarted >>>");
                   // console.log(data);
                    that.engineContext.recogonitionStart(data)
                    // TODO
                    break;

                case "AudioAdded":
                  // console.log(data.seq_no, that.seqNoIn);
                    if (data.seq_no === that.seqNoIn) {
                        that.seqNoIn++;
                    } else {
                      console.log('Unexpected sequence number from API', event)

                      logger.info("Unexpected sequence number from API", event);
                    }  // TODO
                    break;

                case "AddPartialTranscript":
                    console.log("AddPartialTranscript", data);
                    logger.info("AddPartialTranscript", data);
                    that.engineContext.broacastToClient(data)// TODO
                    break;
                case "AddTranscript":
                    console.log("transcriptOutput", data);
                   // console.log(data);
                   logger.info("transcriptOutput", data);
                    that.engineContext.broacastToClient(data)// TODO
                    break;

                case "EndOfTranscript":
                    console.log("EndOfTranscript");
                    that.engineContext.endOfTranscript();
                    this.close();// TODO
                    break;

                case "Info":
                case "Warning":
                case "Error":
                    console.log("SHOW MESSAGE");
                   console.log(data);  // TODO
                    break;

                default:
                    console.log("UNKNOWN MESSAGE: " + data.message);
            }
        };
        this.socket.onclose = function (event: any) { 
            
            console.log("CLOSE", event); 

            that.engineContext.endOfTranscript();
    
        };
        this.socket.onerror = function (event: any) { console.log("ERROR", event);
    
        that.engineContext.endOfTranscript();
    
        };
    }

    public async sendAudioBuffer(pcmData: any) {
      //  console.log(this.seqNoIn);
        //console.log(pcmData);

        try {


           // console.log("this.socket.readyState ", this.socket.readyState)
            if(this.socket.readyState == 1){
               /*  this.socket.send(
                    JSON.stringify({
                        message: 'AddData',
                        offset: 0,
                        seq_no: this.seqNoOut,
                        size: pcmData.length
                    })
                )
                this.seqNoOut++ */
                this.socket.send(pcmData)
            }
            
        } catch (error) {

            console.log("<<<<sendAudioBuffer >>>", error)
            
        }
       
    }


    public async stopRecognition() {

        console.log("MANUAL stopRecognition >>>>>");
        logger.info("MANUAL stopRecognition >>>>>");

        try {

            this.socket.send(
                JSON.stringify({
                    message: 'EndOfStream',
                    last_seq_no: this.seqNoOut
                })
            )
            
        } catch (error) {
            
        }
       

    }


}

