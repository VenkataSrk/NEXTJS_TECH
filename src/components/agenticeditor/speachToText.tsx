import { useEffect, useRef, useState } from "react";
import styles from '../agenticeditor/scss/onboardeditor.module.scss';
import editorvoicegreen from "../../myplans-new/assets/images/workspace/editorvoice.svg";
import editorvoicewhite from "../../myplans-new/assets/images/workspace/editorvoiceb.svg";
import { v4 as uuid } from 'uuid'
import { config } from "../../base/constant";
import NlpWebSocketService from "../../base/service/sttNlpSocket";



export const SpeachToText = ({ textAreaRef, textareaId, onText, placeholderText, page, open, setIsOpen }: any) => {
    console.log('open, setIsOpen>>>>>>>>>>>>>>>', open, setIsOpen)

    const nlpSocketUrl: any = config().STT_NLP;
    const [micOn, setMicOn] = useState(false);
    const audioContextRef = useRef<AudioContext | null>(null);
    const sourceRef = useRef<MediaStreamAudioSourceNode | null>(null);
    const processorRef = useRef<ScriptProcessorNode | null>(null);
    const streamRef = useRef<MediaStream | null>(null);
    const textareaId_ = useRef(textareaId || uuid()).current

    const nlpService = NlpWebSocketService.getInstance();

    useEffect(() => {
        if (!nlpService.isConnected()) {
            nlpService.connect(nlpSocketUrl);
        }
    }, [nlpService.isConnected()])

    useEffect(() => {
        if (!open && micOn) {
            stopRawRecording();
        }
        setMicOn(open);
    }, [open]);

    useEffect(() => {
        return () => {
            stopRawRecording();
        };
    }, []);

    const micToggle = () => {
        if (micOn) {
            stopRawRecording();
        } else {
            checkConnectionToStart();
        }
    };

    const checkConnectionToStart = () => {
        try {
            if (!nlpService.isConnected()) {

            } else {
                startRawRecording()
            }
        } catch (error) {
            console.log(error)
        }
    }

    const insertAtCursor = (newText: string) => {
        try {
            let el :any;

            if(textAreaRef){
                el = textAreaRef.current;
            }else {
                el = document.getElementById('textedited');
            }
            
            if (!el) return;

            const currentTextRaw = el.innerText.trim();
            const currentText = currentTextRaw === placeholderText ? "" : currentTextRaw; 
            
            if (!currentText) {
                onText?.(newText.trim());
                return newText.trim()
            }

            const updatedText = currentText + " " + newText;
            onText?.(updatedText);
            return updatedText

        } catch (error) {
            console.log(error)
        }
    }

    // const insertAtCursor = (newText: string) => {
    //     try {
    //         const el = textAreaRef.current;
    //         if (!el) return;

    //         const currentTextRaw = el.innerText.trim();
    //         const currentText = currentTextRaw === placeholderText ? "" : currentTextRaw;

    //         const selection = window.getSelection();
    //         const range = selection && selection.rangeCount > 0 ? selection.getRangeAt(0) : null;

    //         if (!currentText) {
    //             el.innerText = newText;
    //             onText?.(newText.trim());

    //             const r = document.createRange();
    //             r.selectNodeContents(el);
    //             r.collapse(false);
    //             const s = window.getSelection();
    //             s?.removeAllRanges();
    //             s?.addRange(r);

    //             el.dispatchEvent(new Event("input", { bubbles: true }));
    //             return newText.trim();
    //         }

    //         if (range) {
    //             range.deleteContents();
    //             const textNode = document.createTextNode(newText + " ");
    //             range.insertNode(textNode);

    //             range.setStartAfter(textNode);
    //             range.setEndAfter(textNode);
    //             selection?.removeAllRanges();
    //             selection?.addRange(range);
    //         } else {
    //             el.innerText = currentText + " " + newText;
    //         }

    //         const updatedText = el.innerText.trim();
    //         onText?.(updatedText);
    //         el.dispatchEvent(new Event("input", { bubbles: true }));
    //         return updatedText;
    //     } catch (error) {
    //         console.log('insertAtCursor Error', error)
    //     }
    // };


    useEffect(() => {
        const unsubscribe = nlpService.subscribe((msg) => {
            if (msg.text && msg.session_id === textareaId_) {
                insertAtCursor(msg.text)
            }
        });

        return () => unsubscribe();
    }, []);

    const startRawRecording = async () => {
        try {
            setIsOpen(true);
            setMicOn(true);
            nlpService.sendMessage(JSON.stringify({ session_id: textareaId_ }));
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
            setIsOpen(false);
            setMicOn(false);
        }
    };



    return (
        <div className={`${styles.micIcon} ${
            micOn ? (page === "onboarding" ? styles.activeonboard : styles.active ) 
            : ""}`
        } >
            <img
                src={page === "onboarding" ? editorvoicewhite : editorvoicegreen}
                alt="Voice"
                onClick={micToggle}
            />
        </div>
    )
}




function floatTo16BitPCM(float32Array: Float32Array) {
    const buffer = new ArrayBuffer(float32Array.length * 2);
    const view = new DataView(buffer);
    let offset = 0;
    for (let i = 0; i < float32Array.length; i++, offset += 2) {
        let s = Math.max(-1, Math.min(1, float32Array[i]));
        view.setInt16(offset, s < 0 ? s * 0x8000 : s * 0x7fff, true);
    }
    return new Int16Array(buffer);
}


function downsampleBuffer(buffer: Float32Array, inputSampleRate: number, targetRate: number) {
    if (targetRate === inputSampleRate) return buffer;
    const sampleRateRatio = inputSampleRate / targetRate;
    const newLength = Math.round(buffer.length / sampleRateRatio);
    const result = new Float32Array(newLength);
    let offsetResult = 0, offsetBuffer = 0;
    while (offsetResult < result.length) {
        const nextOffsetBuffer = Math.round((offsetResult + 1) * sampleRateRatio);
        let accum = 0, count = 0;
        for (let i = offsetBuffer; i < nextOffsetBuffer && i < buffer.length; i++) {
            accum += buffer[i]; count++;
        }
        result[offsetResult] = accum / count;
        offsetResult++; offsetBuffer = nextOffsetBuffer;
    }
    return result;
}
