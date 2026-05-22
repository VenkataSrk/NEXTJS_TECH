import React, { useState, useRef } from "react";
import styles from "../../assets/styles/commoneditor.module.scss";
import addicon from "../../assets/images/editor/addicon.svg";
import voiceicon from "../../assets/images/editor/voiceicon.svg";
import sendwhite from "../../assets/images/editor/sendwhite.svg";
import { SpeachToText } from "../../services/speachToText";
import { excelFormats, imageFormat, pdfFormats, wordFormats } from "../../base/utils";
import { handleFileUpload } from "../../base/onboardConfig";
import Excelicon from "../../assets/images/xlsicon.svg";
import Wordicon from "../../assets/images/documentformate.svg";
import Pdficon from "../../assets/images/pdfFile.svg";

const MAX_ROWS = 6;
const LINE_HEIGHT = 24; // must match CSS line-height

const SetUpCommonEditor = ({
  onChange
}: any) => {
  const fileInputRef = useRef<HTMLInputElement>(null);

  const [value, setValue] = useState("");
  const textareaRef = useRef<any>(null);
  const [micOpen, setMicOpen] = useState(false);
  const placeholderText = "Ask your question"
  const [saveFiles, setSaveFiles] = useState<any>([]);

  const handleChange = (e) => {
    setValue(e.target.value);

    const textarea = textareaRef.current;
    textarea.style.height = "auto";

    const maxHeight = LINE_HEIGHT * MAX_ROWS;
    textarea.style.height = Math.min(textarea.scrollHeight, maxHeight) + "px";
  };

  const handleSend = () => {
    if (!value.trim()) return;

    onChange(value);
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
    <div className={styles.editor}>
       {saveFiles?.length > 0 && (
          <div className={styles.previewContainer}>
            {saveFiles?.map((itm: any, index: any) => fileFormatPoster(itm, index, removeFile))}
          </div>
        )}
      <div className={styles.editorblock}>
       
        <div className={styles.editorblockicon}>
          <img src={addicon} alt="Attach File" onClick={() => fileInputRef.current?.click()} />
          <input
            type="file"
            ref={fileInputRef}
            style={{ display: 'none' }}
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

        <div className={styles.editorblockicon} onClick={() => setMicOpen(true)}>
          <SpeachToText
            open={micOpen}
            setIsOpen={setMicOpen}
            textAreaRef={textareaRef}
            placeholderText={placeholderText}
            onText={(newText: string) => setValue(newText)}
          />
        </div>

        {(value?.trim() || saveFiles?.length > 0) && (
          <div className={styles.editorblockicon} onClick={handleSend}>
            <img src={sendwhite} alt="" />
          </div>
        )}
      </div>
    </div>
  );
};

export default SetUpCommonEditor;

export const fileFormatPoster = (file: any, index: number, removeFile: any) => {
  return (
    <div key={index} className={styles.filePreview}>
      {/* Image Preview */}
      {(imageFormat.includes(file?.mimetype) || (file?.mimetype || "").startsWith("image/")) && (
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
