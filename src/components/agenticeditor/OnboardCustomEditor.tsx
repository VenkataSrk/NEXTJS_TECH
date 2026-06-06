'use client';

import styles from '../agenticeditor/scss/onboardeditor.module.scss';

import Excelicon from '../../myplans-new/assets/images/agentworkspace/xlsicon.svg'
import Wordicon from '../../myplans-new/assets/images/workspace/wordicon.jpg'
import Pdficon from '../../myplans-new/assets/images/workspace/pdfFile.svg'

import { wordFormats, excelFormats, imageFormat } from '../../base/utils';

export const fileFormatPoster = (file: any, index: number, removeFile: any) => {
    const pdfFormats = ["pdf", "application/pdf"];
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