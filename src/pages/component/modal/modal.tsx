import React from "react";
import styles from "../modal/scss/modal.module.scss"
import closeIcon from '../../../assets/images/projectinsights/closeicon.svg'

function Modal({ header, children, btntxtprimary, onclosePrimary, btntxtsecondary ,btntxtsecondary2, oncloseSecondary, onClose} :any) {
    return (<>
        <div className={styles.modaloverlay}>
            <div className={styles.modal}>
                <div className={styles.modalheader}>
                    <div>{header}</div>
                    {!!onClose && <div onClick={()=> onClose?.()}> <img src={closeIcon} alt="" /></div>}
                </div>
                <div className={styles.body}>
                    {children}
                </div>
                <div className={styles.footer}>
                    <div className={styles.btncontainer}>
                        {!!btntxtsecondary && 
                            <button onClick={() => oncloseSecondary?.()}> {btntxtsecondary}</button>}
                             {!!btntxtsecondary2 && 
                            <button className={`${!!btntxtsecondary2 && styles.whitebtbn}`} onClick={() => oncloseSecondary?.()}> {btntxtsecondary2}</button>}
                        {!!btntxtprimary &&
                            <button className={`${!!btntxtsecondary2 && styles.blackbtbn}`} onClick={() => onclosePrimary?.()}>{btntxtprimary}</button>}
                    </div>
                </div>
            </div>
        </div>
    </>)
} export default Modal