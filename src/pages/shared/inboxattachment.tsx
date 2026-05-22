import { excelFormats, imageFormat, pdfFormats, wordFormats } from "../../base/utils";
import styles from "./inboxattachment.module.scss"
import Excelicon from "../../assets/images/xlsicon.svg";
import Wordicon from "../../assets/images/documentformate.svg";
import Pdficon from "../../assets/images/pdfFile.svg";

export const AttachmentFormatPoster = (file: any, index: number, removeFile?: any) => {
    const isnotImage = excelFormats.includes(file?.mimetype) || wordFormats.includes(file?.mimetype) || pdfFormats.includes(file?.mimetype);
    
    const sizeFormat = (size: number) => {
        if (size < 1024) {
            return `${size} B`;
        } else if (size < 1048576) {
            return `${(size / 1024).toFixed(2)} KB`;
        } else {
            return `${(size / 1048576).toFixed(2)} MB`;
        }
    }
    return (
        <>       
                <div style={{ display: "flex", flexWrap: "wrap", gap: "8px", marginBottom: "16px" }} className="attachmentcardwrapper">
                    {isnotImage && <div className={`${styles.card} attachmentcard`}>
                        <div className={styles.left}>
                            <div className={styles.icon}>
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
                            </div>

                            <div className={styles.meta}>
                                <p className={`${styles.name} truncate1`}>{file?.filename}</p>
                                <span className={styles.size}>{file?.size ? sizeFormat(file?.size) : ""}</span>
                            </div>
                        </div>

                        {!!removeFile ? (<div
                            className={styles.close}
                            onClick={() => removeFile(index)}

                        ><svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                                <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#23232B" />
                            </svg>
                        </div>) : <div></div>}
                    </div>}
                </div>

                {imageFormat.includes(file?.mimetype) && (
                        <div className={`${styles.filePreview} filepreview`}>
                            <div className={styles.previewImage}>
                                <img
                                    src={file?.url}
                                    alt={file?.name}
                                    className={styles.previewImage}
                                    onClick={() => window.open(file?.url, "_blank")}
                                />
                            </div>
                            {removeFile ? <div
                                className={styles.closeButton}
                                onClick={() => removeFile(index)}
                            >
                                <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                                    <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#23232B" />
                                </svg>
                            </div> : <div></div>}
                        </div>)}
         

            {/* Image Preview */}
            

        </>
    );
};