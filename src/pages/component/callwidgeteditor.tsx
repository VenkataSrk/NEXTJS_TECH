import { useRef, useState } from "react";
import styles from "./scss/calleditor.module.scss";
import editoricon2 from "../../assets/images/callwidget/editoricon2.svg";
import editoricon3 from "../../assets/images/callwidget/editoricon3.svg";
import editoricon4 from "../../assets/images/callwidget/editoricon4.svg";
import editoricon5 from "../../assets/images/callwidget/editoricon5.svg";
import editoricon6 from "../../assets/images/callwidget/editoricon6.svg";
import editoricon7 from "../../assets/images/callwidget/editoricon7.svg";
import editoricon8 from "../../assets/images/callwidget/editoricon8.svg";
import editoricon9 from "../../assets/images/callwidget/editoricon9.svg";
import { insertUpdateNotes } from "../../base/service/core_serviceApi";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import { useSelector } from "react-redux";
import { generateAIMessage } from "../../base/service/nlp_service";
import Worktualdropdown from "./worktualdropdown";
import AIIcon from '../../assets/images/aiicon.svg'

const aiOptions = [
    {
        label: 'Formal', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="9" height="19" viewBox="0 0 9 19" fill="none">
            <path d="M5.21414 4.1518H3.03084C2.93684 4.1518 2.85584 4.2067 2.83674 4.2835L0.0174414 15.5523C-0.0467586 15.8089 0.0692413 16.0736 0.316541 16.2352L3.73594 18.4696C3.84944 18.5437 3.98574 18.5807 4.12194 18.5807C4.25864 18.5807 4.39534 18.5435 4.50894 18.4691L8.01434 16.1717C8.20134 16.049 8.28924 15.8483 8.24064 15.6539L5.40834 4.2837C5.38934 4.207 5.30814 4.1518 5.21414 4.1518ZM4.12134 16.9295L1.62674 15.2995L4.04054 5.6518H4.20314L6.60674 15.3006L4.12134 16.9295Z" fill="#404040" />
            <path d="M2.31844 3.434C2.36114 3.5728 2.51084 3.6698 2.68244 3.6698H5.57114C5.74274 3.6698 5.89264 3.5728 5.93544 3.434L6.87074 0.3918C6.93174 0.1933 6.75234 0 6.50674 0H1.74724C1.50164 0 1.32194 0.1933 1.38294 0.3918L2.31844 3.434ZM5.23154 1.2999L4.90244 2.3698H3.35114L3.02224 1.2999H5.23154Z" fill="#404040" />
        </svg> Formal</div>
    },
    {
        label:'Friendly' , value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="19" height="13" viewBox="0 0 19 13" fill="none">
            <path d="M0 3.50093V9.53973C0 9.93553 0.3208 10.2564 0.7168 10.2564H1.8008C2.1966 10.2564 2.5176 9.93553 2.5176 9.53973V2.78423H0.7168C0.3208 2.78423 0 3.10513 0 3.50093Z" fill="#404040" />
            <path d="M18.2575 2.78823H16.4567V9.54383C16.4567 9.93963 16.7775 10.2605 17.1735 10.2605H18.2575C18.6535 10.2605 18.9741 9.93963 18.9741 9.54383V3.50493C18.9741 3.10913 18.6535 2.78823 18.2575 2.78823Z" fill="#404040" />
            <path d="M15.7998 2.42653C15.7947 2.23373 15.6628 1.95733 15.5154 1.83153C12.7661 -0.528973 10.6048 0.0671275 10.6048 0.0671275C10.5914 0.0702275 10.5047 0.0970274 10.4925 0.101127C10.2974 0.164027 10.1397 0.253727 9.9942 0.344528L9.473 0.705227C7.849 -0.336573 5.9588 -0.0445728 3.8485 1.59743L3.7103 1.70463C3.2894 2.01403 3.2264 2.39343 3.2264 2.58533L3.1595 8.59103C3.1553 8.96333 3.4503 9.27153 3.8226 9.28393L4.4124 9.30453L6.8089 11.7814C7.2193 12.2269 7.7225 12.4713 8.2288 12.4713H8.2371C8.6929 12.4692 9.1084 12.2743 9.4385 11.9073C9.4663 11.8764 9.492 11.8454 9.5169 11.8134C9.8387 12.0609 10.221 12.1929 10.6233 12.1929H10.6336C11.1585 12.1898 11.6482 11.9619 12.0052 11.5566C12.034 11.5246 12.0599 11.4927 12.0845 11.4607C12.699 11.7535 13.387 11.6484 13.8479 11.4411C14.749 11.8175 15.176 11.1967 15.3171 10.9048C15.4431 10.647 15.3957 10.3377 15.1965 10.1293L9.904 4.58943L10.2592 4.34123L14.6252 9.01983C14.7583 9.16213 14.9409 9.23843 15.1276 9.23843C15.2121 9.23843 15.2975 9.22193 15.38 9.18993C15.642 9.08573 15.8141 8.83213 15.8141 8.55063L15.8009 2.44593C15.8009 2.43973 15.7998 2.43273 15.7998 2.42653ZM13.3323 10.1647C13.2664 10.1997 12.8064 10.4225 12.5166 10.0997L12.2185 9.76873C12.0298 9.56043 11.7358 9.48823 11.4709 9.58313C11.208 9.68003 11.0295 9.92753 11.0222 10.208L11.0193 10.3297C11.0161 10.5235 11.011 10.6092 10.9812 10.6432C10.8801 10.7567 10.7541 10.8195 10.6252 10.8206H10.6232C10.4943 10.8206 10.3715 10.7608 10.2704 10.6484L9.7951 10.1193C9.6105 9.91413 9.322 9.83783 9.0588 9.92963C8.7981 10.0213 8.6164 10.2606 8.6001 10.5359C8.5886 10.7122 8.5256 10.869 8.4175 10.9896C8.3196 11.0989 8.2547 11.0989 8.2298 11.0989C8.1422 11.0989 7.9866 11.0339 7.8069 10.839L5.2063 8.15063C5.0837 8.02273 4.9146 7.94853 4.7371 7.94233L4.5403 7.93503L4.5979 2.75223L4.6907 2.68103C5.6746 1.91563 6.9234 1.15123 8.1907 1.59243L6.6253 2.67583C6.1107 3.00993 5.8025 3.56063 5.8025 4.14943C5.8025 4.56403 5.9487 4.95073 6.218 5.26213C6.8264 5.98703 7.7678 6.05713 8.6868 5.43953L8.7654 5.38463L13.3323 10.1647ZM10.8562 2.97003C10.7212 2.82573 10.5388 2.75143 10.3542 2.75143C10.217 2.75143 10.0798 2.79273 9.9611 2.87523L7.9101 4.30753C7.4636 4.60963 7.3337 4.45603 7.2624 4.37143C7.2038 4.30443 7.176 4.23123 7.176 4.14973C7.176 4.02803 7.2492 3.90843 7.3894 3.81663L10.7449 1.49333C10.8181 1.44803 10.8697 1.42213 10.9646 1.39223C10.98 1.38813 12.4546 1.11073 14.4285 2.71113L14.4378 6.80703L10.8562 2.97003Z" fill="#404040" />
        </svg> Friendly</div>
    },
    {
        label: 'Concise', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="17" height="17" viewBox="0 0 17 17" fill="none">
            <path d="M12.868 9.32437C12.277 9.32437 11.7282 9.48257 11.2382 9.73847L3.442 0.274573C3.1783 -0.0467271 2.7052 -0.0916273 2.3858 0.172073C2.0664 0.435773 2.0206 0.908373 2.2837 1.22777L7.2345 7.23757L5.1741 9.73857C4.6842 9.48267 4.1355 9.32447 3.5446 9.32447C1.587 9.32447 0 10.9114 0 12.8689C0 14.8264 1.587 16.4133 3.5446 16.4133C5.5022 16.4133 7.0892 14.8264 7.0892 12.8689C7.0892 12.0466 6.7975 11.2991 6.3275 10.6975L8.2063 8.41717L10.0851 10.698C9.6151 11.2991 9.3236 12.0466 9.3236 12.8689C9.3236 14.8264 10.9105 16.4133 12.8681 16.4133C14.8257 16.4133 16.4127 14.8264 16.4127 12.8689C16.4127 10.9114 14.8257 9.32437 12.868 9.32437ZM3.5446 14.9133C2.4172 14.9133 1.5 13.9963 1.5 12.8689C1.5 11.7415 2.4172 10.8245 3.5446 10.8245C4.672 10.8245 5.5892 11.7415 5.5892 12.8689C5.5892 13.9963 4.672 14.9133 3.5446 14.9133ZM12.868 14.9133C11.7407 14.9133 10.8235 13.9963 10.8235 12.8689C10.8235 11.7415 11.7407 10.8245 12.868 10.8245C13.9953 10.8245 14.9126 11.7415 14.9126 12.8689C14.9126 13.9963 13.9954 14.9133 12.868 14.9133Z" fill="#404040" />
            <path d="M14.129 1.22767C14.3922 0.908373 14.3463 0.435673 14.0269 0.171973C13.7071 -0.0926272 13.2344 -0.0468272 12.9707 0.274473L8.9646 5.13727L9.9363 6.31697L14.129 1.22767Z" fill="#404040" />
        </svg> Concise</div>
    },
    {
        label:'Elaborate', value: <div className={styles.formalist}><svg xmlns="http://www.w3.org/2000/svg" width="17" height="17" viewBox="0 0 17 17" fill="none">
            <path d="M2.5606 1.5H5.1895C5.6036 1.5 5.9395 1.1641 5.9395 0.75C5.9395 0.3359 5.6036 0 5.1895 0H2.0069C0.9005 0 0.000100136 0.9004 0.000100136 2.0068V5.1904C0.000100136 5.6045 0.336 5.9404 0.7501 5.9404C1.1642 5.9404 1.5001 5.6045 1.5001 5.1904V2.5605L5.5812 6.6416C5.7277 6.7881 5.9191 6.8613 6.1115 6.8613C6.3039 6.8613 6.4953 6.7881 6.6418 6.6416C6.9348 6.3486 6.9348 5.874 6.6418 5.5811L2.5606 1.5Z" fill="#404040" />
            <path d="M5.5811 9.6749L1.5 13.756V11.1271C1.5 10.713 1.1641 10.3771 0.75 10.3771C0.3359 10.3771 0 10.713 0 11.1271V14.3097C0 15.4161 0.9004 16.3165 2.0068 16.3165H5.1894C5.6035 16.3165 5.9394 15.9806 5.9394 15.5665C5.9394 15.1524 5.6035 14.8165 5.1894 14.8165H2.5605L6.6416 10.7354C6.9346 10.4424 6.9346 9.9678 6.6416 9.6749C6.3486 9.382 5.874 9.3819 5.5811 9.6749Z" fill="#404040" />
            <path d="M14.2296 0H11.046C10.6319 0 10.296 0.3359 10.296 0.75C10.296 1.1641 10.6319 1.5 11.046 1.5H13.6759L9.5948 5.5811C9.3018 5.8741 9.3018 6.3487 9.5948 6.6416C9.7413 6.7881 9.9327 6.8613 10.1251 6.8613C10.3175 6.8613 10.5089 6.7881 10.6554 6.6416L14.7365 2.5605V5.1904C14.7365 5.6045 15.0724 5.9404 15.4865 5.9404C15.9006 5.9404 16.2365 5.6045 16.2365 5.1904V2.0068C16.2365 0.9004 15.336 0 14.2296 0Z" fill="#404040" />
            <path d="M15.4864 10.377C15.0723 10.377 14.7364 10.7129 14.7364 11.127V13.7559L10.6553 9.6748C10.3623 9.3818 9.8877 9.3818 9.5948 9.6748C9.3019 9.9678 9.3018 10.4424 9.5948 10.7353L13.6759 14.8164H11.046C10.6319 14.8164 10.296 15.1523 10.296 15.5664C10.296 15.9805 10.6319 16.3164 11.046 16.3164H14.2296C15.336 16.3164 16.2364 15.416 16.2364 14.3096V11.127C16.2364 10.7129 15.9005 10.377 15.4864 10.377Z" fill="#404040" />
        </svg> Elaborate</div>
    },
];


const CallWidgetEditor = () => {
    const userDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const editorRef = useRef<HTMLDivElement | null>(null);
    const [noteData, setNoteData] = useState("");
    const isNoteEmpty = noteData?.replace(/<br\s*\/?>/gi, "")?.replace(/&nbsp;/gi, " ")?.replace(/<[^>]*>/g, "")?.replace(/\u00a0/g, " ")?.trim()?.length === 0;
    const { incomingCallDetail, callDetails, sessionIDStore }: any = useSelector((state: any) => state.callPopUpDetails);
    const sessionId = `${incomingCallDetail?.sessionId ?? incomingCallDetail?.session_id ?? sessionIDStore ?? ""}`.trim();
    const customerId = incomingCallDetail?.contactDetails?.customerId ?? incomingCallDetail?.contactDetails?.customer_id ?? incomingCallDetail?.contactDetails?.id ?? null;
    const { customerDealId } = useSelector((store: any) => store.contactStore)

    const exec = (command: string, value: any | number | null = null) => {
        if (!editorRef.current) return;
        editorRef.current.focus();
        document.execCommand(command, false, value);
    };

    const addLink = () => {
        const url = prompt("Enter URL");
        if (url) exec("createLink", url);
    };

    const changeNote = (event: any) => {
        setNoteData(event.currentTarget.innerHTML);
    };

    const saveNoteData = async () => {
        const latestHtml = editorRef.current?.innerHTML ?? noteData ?? "";
        const latestIsEmpty = latestHtml?.replace(/<br\s*\/?>/gi, "")?.replace(/&nbsp;/gi, " ")?.replace(/<[^>]*>/g, "")?.replace(/\u00a0/g, " ")?.trim()?.length === 0;
        if (latestIsEmpty) return;
        const data = {
            sessionId: sessionId,
            customerId: customerId,
            agentId: userDetails?.userId,
            channelType: "Call",
            notes: latestHtml,
            noteImages: "",
            dealId : customerDealId ?? null,
        };
        const response = await insertUpdateNotes(data);
        if (response) {
            setNoteData("");
            if (editorRef.current) {
                editorRef.current.innerHTML = "";
            }
        }
    };



    // const aiNotes = async () => {
    //     const latestHtml = editorRef.current?.innerHTML ?? noteData ?? "";
    //     const plainText = latestHtml?.replace(/<br\s*\/?>/gi, "\n")?.replace(/&nbsp;/gi, " ")?.replace(/<[^>]*>/g, "")?.replace(/\u00a0/g, " ")?.trim()
    //     if (!plainText) return;
    //     const response: any = await generateAIMessage({ text: plainText });
    //     const aiText = response ;
    //     if (!aiText) return;
    //     setNoteData(aiText);
    //     if (editorRef.current) {
    //         editorRef.current.innerHTML = aiText;
    //     }
    // };

    const aiNotes = async (tone?: string) => {

        const latestHtml = editorRef.current?.innerHTML ?? noteData ?? "";

        const plainText = latestHtml
            ?.replace(/<br\s*\/?>/gi, "\n")
            ?.replace(/&nbsp;/gi, " ")
            ?.replace(/<[^>]*>/g, "")
            ?.replace(/\u00a0/g, " ")
            ?.trim();

        if (!plainText) return;

        const body = {
            text: plainText,
            tone: tone
        };

        const response: any = await generateAIMessage(body);

        const aiText = response ?? "";
        if (!aiText) return;
        setNoteData(aiText);
        if (editorRef.current) {
            editorRef.current.innerHTML = aiText;
        }
    };

    const handlechange = (value: any) => {
        if (!value) return;
        aiNotes(value?.label);
    };

    return (
        <div className={styles.notes}>
            <div
                style={{ display: "flex", width: "100%", justifyContent: "space-between" }}
                className={styles.title}>
                Note{" "}
                <span
                    className={`${styles.savetxt} ${isNoteEmpty ? styles.dimtxt : ""}`}
                    onClick={saveNoteData}>
                    Save
                </span>
            </div>
            <div
                ref={editorRef}
                contentEditable
                className={styles.editor}
                suppressContentEditableWarning
                role="textbox"
                data-placeholder="Take note on this call"
                onInput={changeNote}
            />
            <div className={styles.toolbar}>
                <div className={styles.editorbottom}>
                    <div onClick={() => exec("bold")} className={`${styles.editorbottominner}`}>
                        <img src={editoricon2} alt="" />
                    </div>
                    <div onClick={() => exec("italic")} className={styles.editorbottominner}>
                        <img src={editoricon3} alt="" />
                    </div>
                    <div onClick={() => exec("underline")} className={styles.editorbottominner}>
                        <img src={editoricon4} alt="" />
                    </div>
                    <div onClick={addLink} className={styles.editorbottominner}>
                        <img src={editoricon5} alt="" />
                    </div>
                    <div onClick={() => exec("insertUnorderedList")} className={styles.editorbottominner}>
                        <img src={editoricon6} alt="" />
                    </div>
                    <div onClick={() => exec("insertOrderedList")} className={styles.editorbottominner}>
                        <img src={editoricon7} alt="" />
                    </div>
                    <div onClick={() => exec("strikeThrough")} className={styles.editorbottominner}>
                        <img src={editoricon8} alt="" />
                    </div>
                    <div className={`activitydropdown aiactive dropdownfromwidget `}>
                        <Worktualdropdown options={aiOptions} onChange={handlechange} triggerIcon={AIIcon}  right={true}/>
                    </div>
                    {/* <div onClick={aiNotes} className={styles.editorbottominner}>
                        <img src={editoricon9} alt="" />
                        ⤫
                    </div> */}
                </div>
            </div>
        </div>
    );
};

export default CallWidgetEditor;
