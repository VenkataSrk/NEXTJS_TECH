import React, { useState, useRef, useEffect } from "react";
import styles from "./scss/onboardeditor.module.scss";
// import addicon from "../../assets/images/editor/addicon.svg";
// import voiceicon from "../../assets/images/editor/voiceicon.svg";
// import sendwhite from "../../assets/images/editor/sendwhite.svg";
import addicon from "./Archive/editor/addicon.svg";
import voiceicon from "./Archive/editor/voiceicon.svg";
import sendwhite from "./Archive/editor/sendwhite.svg";
import NlpWebSocketService from "../../../base/service/sttNlpSocket";
import { config } from "../../../base/constant";
import { v4 as uuidv4 } from "uuid";
import { downsampleBuffer, floatTo16BitPCM } from "../../../base/utils";

const MAX_ROWS = 6;
const LINE_HEIGHT = 24; // must match CSS line-height

const OnBoardEditor = ({handleSend,handleInput,knowledgeMessage,setKnowledgeMessage,knowledgeMessageData,showSend,textareaRef,uploadWhatsappTemplate}:any ) => {
  // const [value, setValue] = useState("");
  // const textareaRef = useRef(null);

  // const handleChange = (e) => {
  //   setValue(e.target.value);

  //   const textarea = textareaRef.current;
  //   textarea.style.height = "auto";

  //   const maxHeight = LINE_HEIGHT * MAX_ROWS;
  //   textarea.style.height = Math.min(textarea.scrollHeight, maxHeight) + "px";
  // };

  // const handleSend = () => {
  //   if (!value.trim()) return;

  //   console.log("Sent:", value);
  //   setValue("");

  //   textareaRef.current.style.height = "auto";
  // };

  // const handleKeyDown = (e) => {
  //   if (e.key === "Enter" && !e.shiftKey) {
  //     e.preventDefault();
  //     handleSend();
  //   }
  // };

    const nlpService = NlpWebSocketService.getInstance();
    const nlpSocketUrl: any = config().STT_NLP;
    const audioContextRef = useRef<AudioContext | null>(null);
    const sourceRef = useRef<MediaStreamAudioSourceNode | null>(null);
    const processorRef = useRef<ScriptProcessorNode | null>(null);
    const streamRef = useRef<MediaStream | null>(null);
    const textareaId = useRef(uuidv4()).current;

    const fileInputRef = useRef<HTMLInputElement | null>(null);
      const textDataRef = useRef('');


      const [isRecording, setIsRecording] = useState(false);



            useEffect(() => {
        if (!nlpService.isConnected()) {
          nlpService.connect(nlpSocketUrl);
        }
      }, [nlpService.isConnected()])

      useEffect(() => {
        const unsubscribe = nlpService.subscribe((msg) => {
          if (msg.text && msg.session_id === textareaId) {
            console.log("insertAtCursor beforeee", msg.text)
            let text :any= insertAtCursor(msg.text)
            handelTextArea(text)
          }
        });
        return () => unsubscribe();
      }, []);


      
      const normalTextBoxFocus = () => {
        setTimeout(() => {
          if (textareaRef.current) {
            textareaRef.current?.focus()
            textareaRef.current.selectionStart = textareaRef.current.selectionEnd = (textareaRef.current.value?.length || 0);
          }
        }, 100);
      }
    
      const startVoiceToText = async () => {
        try {
          normalTextBoxFocus()
          nlpService.sendMessage(JSON.stringify({ session_id: textareaId }));
          const stream = await navigator.mediaDevices.getUserMedia({ audio: true });
          streamRef.current = stream;
          const audioCtx = new AudioContext();
          audioContextRef.current = audioCtx;
          const source = audioCtx.createMediaStreamSource(stream);
          sourceRef.current = source;
          const processor = audioCtx.createScriptProcessor(4096, 1, 1);
          processorRef.current = processor;
          source.connect(processor);
          processor.connect(audioCtx.destination);
          let pcmBuffer: number[] = [];
          processor.onaudioprocess = (e) => {
            const input = e.inputBuffer.getChannelData(0);
            const downsampled = downsampleBuffer(input, audioCtx.sampleRate, 16000);
            const pcm16: any = floatTo16BitPCM(downsampled);
            pcmBuffer.push(...pcm16);
            while (pcmBuffer.length >= 320) {
              const chunk = pcmBuffer.slice(0, 320);
              pcmBuffer = pcmBuffer.slice(320);
              nlpService.sendMessage(Int16Array.from(chunk).buffer);
            }
          };
          setIsRecording(true);
        } catch (err) {
          alert(err);
    
        }
      };
    
      const stopRawRecording = async () => {
        try {
          processorRef.current?.disconnect();
          if (processorRef.current) processorRef.current.onaudioprocess = null;
          sourceRef.current?.disconnect();
          streamRef.current?.getTracks().forEach((t) => t.stop());
          await audioContextRef.current?.close();
        } finally {
          processorRef.current = null;
          sourceRef.current = null;
          streamRef.current = null;
          audioContextRef.current = null;
          setIsRecording(false);
        }
      };

        const micToggle = () => {
        isRecording ? stopRawRecording() : startVoiceToText()
      }
 
      const handelTextArea = (text: any) => {
        console.log("text????",text)
        setKnowledgeMessage(text);
        textareaRef.current = text
      }

      const insertAtCursor = (newText: any) => {
     const textarea = textareaRef.current;
    if (!textarea) return;
    const start = textarea.selectionStart;
    const end = textarea.selectionEnd;
    const updated = (textDataRef.current
      ? textDataRef.current.slice(0, start) + " " + newText + " " + textDataRef.current.slice(end)
      : newText
    )?.trim();

    setTimeout(() => {
      textarea.selectionStart = textarea.selectionEnd = start + newText.length + 1;
    }, 1000);

    return updated
      };



  return (
    <div className={styles.editor}>
      <div className={styles.editorblock}>
        <div className={styles.editorblockicon}>
          <input
            type="file"
            // accept="application/pdf"
            accept=".pdf,.doc,.docx,.png,.jpg,.jpeg"

            ref={fileInputRef}
            style={{ display: "none" }}
            onChange={uploadWhatsappTemplate}
          />
          <img src={addicon} alt=""
            onClick={() => fileInputRef.current?.click()}
            style={{ cursor: "pointer" }}
          />
        </div>

        <textarea
          ref={textareaRef}
          rows={1}
          style={{ minHeight: "40px", maxHeight: "120px", height: "40px" }}
          // value={value}
          // onChange={handleChange}
          // onKeyDown={handleKeyDown}
          // placeholder="How may I help you configure your Campaign Management"
          // className={styles.textarea}
          onInput={handleInput}
          value={knowledgeMessage}
          onChange={(e) => setKnowledgeMessage(e.target.value)}
          onKeyDown={(e) => {
            if (!knowledgeMessageData?.isTyping && !knowledgeMessageData?.progress && e.key === "Enter" && !e.shiftKey) {
              e.preventDefault();
              handleSend();
            }
          }}
                        
        />

        <div className={styles.editorblockicon} onClick={()=>micToggle()}>
          <img src={voiceicon} alt="" />
        </div>

        {showSend && (
          <div className={styles.editorblockicon} onClick={()=>handleSend()}>
            <img src={sendwhite} alt="" />
          </div>
        )}
      </div>
    </div>
  );
};

export default OnBoardEditor;
