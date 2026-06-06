'use client';

import { useEffect, useRef, useState } from 'react';
// import styles from '../assets/styles/simpleeditor.module.scss';
import styles from '../agenticeditor/scss/agenteditor.module.scss';

import { useDispatch, useSelector } from 'react-redux';

import { SpeachToText } from './speachToText';

import Aicon from '../../myplans-new/assets/images/agentworkspace/aicon.svg'
import { MyplanProfileActions } from '../../store/slice/myplan/profile';
import Ailoadericon from './ailoadericon';
import aiMyPlanSocketService from '../../base/service/aiMypPanSocketService';
import { fileFormatPoster } from './OnboardCustomEditor';
export default function SimpleCustomEditorPlan({ disableEditor }: any) {

    // const { currentSession }: any = useSelector((state: any) => state.chatReducer);
    const botResponse: any = useSelector((state: any) => state.userExtension.botResponse);
    const [showBotResponse, setShowBotResponse] = useState<any>(false);
    console.log("botres", botResponse)

    let editorRef = useRef<HTMLDivElement>(null);

    const handlePaste = (e: React.ClipboardEvent<HTMLDivElement>) => {
        e.preventDefault(); // Stop the browser from pasting styled content

        const text = e.clipboardData.getData('text/plain'); // Only get plain text

        // Insert plain text at cursor position
        const selection = window.getSelection();
        if (!selection?.rangeCount) return;

        selection.deleteFromDocument();
        selection.getRangeAt(0).insertNode(document.createTextNode(text));
        selection.collapseToEnd();
    };
    const [hideAI, setHideAI] = useState(true);
    // const agentUserList: any = useSelector((state: any) => state.userList);
    // const { users } = agentUserList;

    const mentionRef = useRef<HTMLDivElement>(null);
    const [micOpen, setMicOpen] = useState(false);
    // const [displayedText, setDisplayedText] = useState("");
    const placeholderText = "Type your message"

    const setSpeechText = (newText: string) => {
        setIsEmpty(false)
        const el = editorRef.current;
        if (!el) return;
        el.innerText = newText
        el.focus();

        const range = document.createRange();
        const selection = window.getSelection();

        range.selectNodeContents(el);
        range.collapse(false);

        if (selection) {
            selection.removeAllRanges();
            selection.addRange(range);
        }

    };



    const dispatch = useDispatch()
    const handleSend = async () => {

        try {
            let html = editorRef.current?.innerText || '';
            if (html?.trim()?.length <= 0) {
                return;
            }
          
            setShowBotResponse(true);
            if (editorRef.current) {
                editorRef.current.innerHTML = '';
                editorRef.current.focus();
            }
            // dispatch(MyplanProfileActions.setMyplanBotResponse(null));
            const editor = editorRef.current;
            if (!editor) return;

            const text = editor.innerText.trim();
            const hasText = !!text;
            const hasFile = saveFiles?.length > 0;

            if (!hasText) return;
            aiMyPlanSocketService.io.emit("agent_chat_request", html)

        } catch (error) {
            console.log('error in handleSend:', error);
        }
    };

    const [saveFiles, setSaveFiles] = useState<any>([]);





    const removeFile = (index: number) => {
        setSaveFiles((prev: any) => prev.filter((_: any, i: any) => i !== index));
    };

    const [isFocused, setIsFocused] = useState(false);
    const [isEmpty, setIsEmpty] = useState(true);
    const editorWrapperRef = useRef<HTMLDivElement>(null);




    const handleKeyDown = (e: React.KeyboardEvent<HTMLDivElement>) => {
        if (e.key === 'Enter' && !e.shiftKey) {
            e.preventDefault();
            handleSend()
        }
    };




    const handleInput = (e: React.FormEvent<HTMLDivElement>) => {
        const editor = editorRef.current;
        if (!editor) return;

        const text = editor.innerText || "";

        setIsEmpty(text.trim() === "");


    };



//   useEffect(() => {
//     if (!botResponse) return;

//     setDisplayedText(""); 
//     let index = 0;

//     const interval = setInterval(() => {
//       setDisplayedText((prev) => prev + botResponse[index]);
//       index++;
//       if (index >= botResponse.length) clearInterval(interval);
//     }, 30); 

//     return () => clearInterval(interval);
//   }, [botResponse]);
   




    useEffect(() => {
        setHideAI(true);
        dispatch(MyplanProfileActions.setMyplanBotResponse(null));
        setShowBotResponse(false);
    }, [disableEditor])



    return (
        <>
            <div className={styles.overalleditor} ref={editorWrapperRef} style={{ width: "800px", padding: '0 0 24px' }}>
                {hideAI ? <div className={styles.editbox}>
                    <p onClick={() => setHideAI(false)}> <img src={Aicon} alt="" />Search or Make Changes</p></div>
                    : <>{
                        showBotResponse &&
                        <div className={`${styles.mainblocks} ${styles.mainblocktrans}`} style={{
                            borderBottom: 0,
                            padding: "12px 12px 24px",
                            margin: "0 0 -20px",
                            borderRadius: "12px 12px 0 0"
                        }}>
                            {/* <div className={styles.sentimenttop}><img src={sentiment} alt="" />Sentiment analysis</div> */}
                            <div className={styles.sentimentdes}>
                               {botResponse ?  <img src={Aicon} alt="" /> :
                                <div style={{width: '50px',height: '50px'}}><Ailoadericon /></div>}
                               {/* {botResponse&& <p>{displayedText}</p> }*/}
                               {botResponse&& <p>{botResponse}</p>}
                              
                      
                            </div>
                        </div>
                    }
                        <div className={`${styles.editorblock} ${styles.editorblockplan}`}>
                            <div className={styles.previewContainer}>
                                {saveFiles.map((itm: any, index: any) => fileFormatPoster(itm, index, removeFile))}
                            </div>
                            <div className={styles.fileformaterblock}>
                                {/* <div className={styles.editorblockleft}>
                                    <img src={editor2} alt="Attach File" onClick={() => fileInputRef.current?.click()} />
                                    <input type="file" accept={excelFormats.join(',')} ref={fileInputRef} style={{ display: 'none' }} onChange={handleFileChange} />
                                </div> */}

                                <div style={{ position: 'relative', width: '100%' }}>
                                    {isEmpty && !isFocused && (
                                        <div
                                            className={styles.placeholder}
                                            style={{
                                                position: 'absolute',
                                                pointerEvents: 'none',
                                                top: '-2px',
                                                left: 0,
                                                color: 'gray',
                                                userSelect: 'none',
                                                width: '100%',
                                            }}
                                        >
                                            {placeholderText}
                                        </div>
                                    )}

                                    <div
                                        ref={editorRef}
                                        contentEditable
                                        style={{
                                            color: 'black',
                                            position: 'relative',
                                            outline: 'none',
                                        }}
                                        className={styles.editorblockcenter}
                                        suppressContentEditableWarning={true}
                                        onFocus={() => setIsFocused(true)}
                                        onBlur={() => setIsFocused(false)}
                                        onInput={handleInput}
                                        onKeyDown={handleKeyDown}
                                        onPaste={handlePaste}
                                    />
                                </div>
                            </div>


                            {/* {showMention && (
                                <div className={styles.miconhandle} ref={mentionRef}>
                                    <div className={styles.micdrop}>
                                        <div className={styles.groumicdrop}>
                                            <h3>Agents</h3>
                                            <ul>
                                                {filteredAgentList.length > 0 ? (
                                                    filteredAgentList.map((agentData, agentIndex) => {
                                                        const user_data = users.find((user: any) => user.UserName === agentData.UserName);
                                                        if (!user_data) return null;

                                                        const statusClass = ({
                                                            Ready: styles.ready,
                                                            "Not Ready": styles.notready,
                                                            Offline: styles.offline,
                                                            Away: styles.away,
                                                            Busy: styles.busy,
                                                            "Transfer Only": styles.transferonly,
                                                            vacation: styles.vacation,
                                                        } as any)[user_data.statusName] || styles.offline;

                                                        return (
                                                            <li
                                                                key={agentIndex}
                                                                className={`${styles.assignnflexx} ${styles.assigneesrow} ${styles.userassign} ${user_data.statusName !== 'Ready' && styles.disable}`}
                                                                onMouseDown={(e) => {
                                                                    if (user_data.statusName !== "Ready") {
                                                                        e.preventDefault();
                                                                        e.stopPropagation();
                                                                        return;
                                                                    }
                                                                    e.preventDefault();
                                                                    // insertMention(agentData);
                                                                }}
                                                            >
                                                                <div className={`${styles.dot} ${statusClass}`}></div>
                                                                <div className={styles.tranfernamelists}>
                                                                    {agentData.UserName}
                                                                    <span>{user_data.statusName || 'Offline'}</span>
                                                                </div>
                                                            </li>
                                                        );
                                                    })
                                                ) : (
                                                    <li style={{ color: "#999" }}>No agents are ready</li>
                                                )}
                                            </ul>
                                        </div>
                                    </div>
                                </div>
                            )} */}
                            <div className={styles.editorblockright}>
                                <div className={styles.editoricon} onClick={() => setMicOpen(true)}>
                                    <SpeachToText
                                        open={micOpen}
                                        setIsOpen={setMicOpen}
                                        textAreaRef={editorRef}
                                        placeholderText={placeholderText}
                                        onText={(newText: string) => setSpeechText(newText)}
                                    />
                                </div>
                                {(!isEmpty) &&
                                    <div className={styles.editoricon}>
                                        <svg xmlns="http://www.w3.org/2000/svg" width="21" height="18" viewBox="0 0 21 18" fill="none" onClick={handleSend}>
                                            <path d="M19.826 1.36153C19.4237 0.95333 18.8172 0.81953 18.2796 1.01583L1.24889 7.18083C0.701992 7.36933 0.324592 7.84293 0.263492 8.41623C0.203392 8.97873 0.461692 9.50803 0.921692 9.78633L3.96659 11.7521C4.33279 11.9845 4.80839 11.9914 5.17849 11.7648L15.5178 4.99223L5.83769 12.6876C5.63699 12.8595 5.50179 13.0978 5.45679 13.3556L5.01049 15.8829C4.89869 16.5157 5.17459 17.1231 5.71409 17.4317C6.25119 17.7383 6.90449 17.6768 7.37959 17.2696L9.60229 15.3634L13.0159 17.5948C13.2517 17.7491 13.5286 17.8282 13.8084 17.8282C13.9764 17.8282 14.1458 17.7999 14.3079 17.7413C14.7425 17.5841 15.0696 17.2423 15.1971 16.8282L20.1346 2.86823C20.3402 2.34183 20.222 1.76383 19.826 1.36153Z" fill="#00E2A4" />
                                        </svg>
                                    </div>
                                }
                            </div>
                        </div>
                    </>
                }
            </div>
        </>
    );
}

