import React, { useState, useRef } from "react";
import styles from "../../assets/styles/commoneditor.module.scss";
import addicon from "../../assets/images/editor/addicon.svg";
import voiceicon from "../../assets/images/editor/voiceicon.svg";
import sendwhite from "../../assets/images/editor/sendwhite.svg";
import { SpeachToText } from "../../services/speachToText";
import { excelFormats, getContactModuleByPath, imageFormat, pdfFormats, wordFormats } from "../../base/utils";
import { handleFileUpload } from "../../base/onboardConfig";
import Excelicon from "../../assets/images/xlsicon.svg";
import Wordicon from "../../assets/images/documentformate.svg";
import Pdficon from "../../assets/images/pdfFile.svg";
import { useSelector } from "react-redux";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { contactDataSliceActions } from "../../store/slice/contactSlice";

const MAX_ROWS = 6;
const LINE_HEIGHT = 24; // must match CSS line-height

interface CommonEditorProps {
  onChange: (value: string, files: any[]) => void;
  templateChat?: boolean;
  searchResponse?: string;
  searchStatus?: string;
  isWaitingResponse?: boolean;
  onCloseSearchResponse?: () => void;
}

const SetUpCommonEditor = ({
  onChange,
  templateChat = false,
  searchResponse = "",
  searchStatus = "",
  isWaitingResponse = false,
  onCloseSearchResponse,
}: CommonEditorProps) => {
  const fileInputRef = useRef<HTMLInputElement>(null);
  const dispatch = useDispatcher();

  const [value, setValue] = useState("");
  const textareaRef = useRef<any>(null);
  const [micOpen, setMicOpen] = useState(false);
  const placeholderText = "Ask your question"
  const [saveFiles, setSaveFiles] = useState<any>([]);
  const { contactModuleFollowUp } = useSelector((store: any) => store.contactStore);

  const contactModule = getContactModuleByPath();
  const contactFollowUp = contactModule ? contactModuleFollowUp?.[contactModule] : null;
  const isContactModule = !!contactModule;

  const effectiveSearchResponse = isContactModule
    ? (contactFollowUp?.response || "")
    : searchResponse;
  const effectiveSearchStatus = isContactModule
    ? (contactFollowUp?.searchStatus || "")
    : searchStatus;
  const effectiveIsWaitingResponse = isContactModule
    ? !!contactFollowUp?.isWaitingResponse
    : isWaitingResponse;

  const handleCloseSearchResponse = () => {
    if (isContactModule) {
      dispatch(contactDataSliceActions.resetContactModuleFollowUp({ module: contactModule }));
    }
    onCloseSearchResponse?.();
  };

  
  const handleChange = (e) => {
    setValue(e.target.value);

    const textarea = textareaRef.current;

    textarea.style.height = "auto";

    const maxHeight = LINE_HEIGHT * MAX_ROWS;
    textarea.style.height = Math.min(textarea.scrollHeight, maxHeight) + "px";
  };

  const handleSend = () => {
    const hasText = value?.trim();
    const hasFiles = saveFiles && saveFiles.length > 0;
    if (!hasText && !hasFiles) return;

    onChange(value, saveFiles);

    setValue("");
    setSaveFiles([]);
    textareaRef.current.style.height = "auto";
  };

  const handleKeyDown = (e) => {
    if (e.key === "Enter" && !e.shiftKey) {
      e.preventDefault();
      handleSend();
    }
  };

  const handleFileChange = (event: React.ChangeEvent<HTMLInputElement>) => {
    const file: any = event.target.files?.[0];

    if (file) {
      handleFileUpload(file)
        .then((res: any) => {
          setSaveFiles((prev: any) => [...prev, ...res]);
        })
        .catch((error) => {
          console.log(error);
        });
    }

    event.target.value = '';
  };

  const removeFile = (index: number) => {
    setSaveFiles((prev: any) => prev.filter((_: any, i: any) => i !== index));
  };

  return (
    <div className={`maincontainer  ${templateChat ? 'chatmaincontainer' : ""}`}>

      <div className={styles.editor}>
        {saveFiles?.length > 0 && (
          <div className={styles.previewContainer}>
            {saveFiles?.map((itm: any, index: any) => fileFormatPoster(itm, index, removeFile))}
          </div>
        )}
        <div className={styles.editorblock}
          style={{
            alignItems: textareaRef.current?.offsetHeight >= 48 ? "flex-end" : "center"
          }}
        >
          <div className={styles.lefts} style={{
            alignItems: textareaRef.current?.offsetHeight >= 48 ? "flex-end" : "center"
          }}>


            <div className={styles.editorblockicon}>
              <img src={addicon} alt="Attach File" onClick={() => fileInputRef.current?.click()} />
              <input
                type="file"
                ref={fileInputRef}
                style={{ display: 'none' }}
                accept=".csv, .xlsx, image/*"
                onChange={handleFileChange}
              />
            </div>

            <textarea
              ref={textareaRef}
              rows={1}
              value={value}
              onChange={handleChange}
              onKeyDown={handleKeyDown}
              placeholder={placeholderText}
              className={styles.textarea}
            />
          </div>
          <div className={styles.rights}>
            <div className={styles.editorblockicon} onClick={() => setMicOpen(true)}>
              <SpeachToText
                open={micOpen}
                setIsOpen={setMicOpen}
                textAreaRef={textareaRef}
                placeholderText={placeholderText}
                onText={(newText: string) => setValue(newText)}
              />
            </div>

            {effectiveIsWaitingResponse && (
              <>
                <div
                  style={{
                    width: 20,
                    height: 20,
                    border: "2.5px solid rgba(255,255,255,0.3)",
                    borderTop: "2.5px solid #fff",
                    borderRadius: "50%",
                    animation: "spin 0.7s linear infinite"
                  }}
                />

                <style>{`
                  @keyframes spin {
                    to { transform: rotate(360deg); }
                  }
                `}</style>
              </>
            )}

            {!effectiveIsWaitingResponse && (value?.trim() || saveFiles?.length > 0) && (
              <div className={styles.editorblockicon} onClick={handleSend}>
                <img src={sendwhite} alt="" />
              </div>
            )}
          </div>


        </div>
        {
          (!!effectiveSearchResponse || !!effectiveSearchStatus) &&
          <div className={styles.searchcontent}>
            <div className={styles.btn}>
              <img src={sendwhite} />
            </div>
            <div className={styles.body} dangerouslySetInnerHTML={{ __html: effectiveSearchResponse }} />
            <div className={styles.btn} onClick={handleCloseSearchResponse}>
              <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#FCFBFB" />
              </svg>
            </div>
          </div>

        }
      </div>
    </div>

  );
};

export default SetUpCommonEditor;

export const fileFormatPoster = (file: any, index: number, removeFile: any) => {
  return (
    <div key={index} className={styles.filePreview}>
      {/* Image Preview */}
      {imageFormat.includes(file?.mimetype) && (
        <img
          src={file?.url}
          alt={file?.name}
          className={styles.previewImage}
          onClick={() => window.open(file?.url, "_blank")}
        />
      )}

      {/* Excel Preview */}
      {excelFormats.includes(file?.mimetype) && (
        <img
          src={Excelicon}
          alt="excel"
          className={styles.previewImage}
          onClick={() => window.open(file?.url, "_blank")}
        />
      )}

      {/* Word Preview */}
      {wordFormats.includes(file?.mimetype) && (
        <img
          src={Wordicon}
          alt="word"
          className={styles.previewImage}
          onClick={() => window.open(file?.url, "_blank")}
        />
      )}

      {/* PDF Preview */}
      {pdfFormats.includes(file?.mimetype) && (
        <img
          src={Pdficon}
          alt="pdf"
          className={styles.previewImage}
          onClick={() => window.open(file?.url, "_blank")}
        />
      )}

      {/* Remove Button */}
      <span onClick={() => removeFile(index)} className={styles.closeButton}>
        ✖
      </span>
    </div>
  );
};
