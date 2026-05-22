'use client';

import { useRef, useState } from 'react';
import styles from '../assets/styles/simpleeditor.module.scss';
// import Aicon from '../../aiccaas/assets/images/agentworkspace/operationhubai.svg';

export default function SimpleCustomEditorPlan({ disableEditor }: any) {

    const editorRef = useRef<HTMLDivElement>(null);

    const [hideAI, setHideAI] = useState(true);
    const [isFocused, setIsFocused] = useState(false);
    const [isEmpty, setIsEmpty] = useState(true);

    const placeholderText = "Type your message";

    const handleInput = () => {
        const text = editorRef.current?.innerText || "";
        setIsEmpty(text.trim() === "");
    };

    return (
        <>
            <div className={`${styles.overalleditor} ${styles.overalleditormyplan}`}>
                
                {hideAI ? (
                    <div className={`${styles.editbox} ${styles.editwraper}`}>
                        <p onClick={() => setHideAI(false)}>
                            {/* <img src={Aicon} alt="" /> */}
                            Search or Make Changes
                        </p>
                    </div>
                ) : (
                    <>
                        {/* Static AI Response Block */}
                        <div
                            className={`${styles.mainblocks} ${styles.mainblocktrans}`}
                            style={{
                                borderBottom: 0,
                                padding: "12px 12px 24px",
                                margin: "0 0 -20px",
                                borderRadius: "12px 12px 0 0"
                            }}
                        >
                            <div className={styles.sentimentdes}>
                                {/* <img src={Aicon} alt="" /> */}
                                <div>AI response will appear here...</div>
                            </div>
                        </div>

                        <div className={`${styles.editorblock} ${styles.editorblockplan}`}>
                            
                            {/* Input Area */}
                            <div className={styles.fileformaterblock}>
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
                                        className={styles.editorblockcenter}
                                        style={{ outline: 'none' }}
                                        suppressContentEditableWarning={true}
                                        onFocus={() => setIsFocused(true)}
                                        onBlur={() => setIsFocused(false)}
                                        onInput={handleInput}
                                    />
                                </div>
                            </div>

                            {/* Right Icons */}
                            <div className={styles.editorblockright}>
                                
                                {/* Mic icon (static) */}
                                <div className={styles.editoricon}>
                                    🎤
                                </div>

                                {/* Send icon (static) */}
                                {!isEmpty && (
                                    <div className={styles.editoricon}>
                                        <svg
                                            xmlns="http://www.w3.org/2000/svg"
                                            width="21"
                                            height="18"
                                            viewBox="0 0 21 18"
                                            fill="none"
                                        >
                                            <path
                                                d="M19.826 1.36153C19.4237 0.95333 18.8172 0.81953 18.2796 1.01583L1.24889 7.18083C0.701992 7.36933 0.324592 7.84293 0.263492 8.41623C0.203392 8.97873 0.461692 9.50803 0.921692 9.78633L3.96659 11.7521C4.33279 11.9845 4.80839 11.9914 5.17849 11.7648L15.5178 4.99223L5.83769 12.6876C5.63699 12.8595 5.50179 13.0978 5.45679 13.3556L5.01049 15.8829C4.89869 16.5157 5.17459 17.1231 5.71409 17.4317C6.25119 17.7383 6.90449 17.6768 7.37959 17.2696L9.60229 15.3634L13.0159 17.5948C13.2517 17.7491 13.5286 17.8282 13.8084 17.8282C13.9764 17.8282 14.1458 17.7999 14.3079 17.7413C14.7425 17.5841 15.0696 17.2423 15.1971 16.8282L20.1346 2.86823C20.3402 2.34183 20.222 1.76383 19.826 1.36153Z"
                                                fill="#000"
                                            />
                                        </svg>
                                    </div>
                                )}
                            </div>
                        </div>
                    </>
                )}
            </div>
        </>
    );
}