// CustomModal.jsx
import styles from "./scss/worktualmodal.module.scss";
import Close from "../../assets/images/projectinsights/closeicon.svg";

const CustomModal = ({ open, onClose, title, children, footer ,headerBlock} :any) => {
  if (!open) return null;

  return (
    <div className={styles.backdrop} onClick={onClose}>
      <div
        className={styles.modal}
        onClick={(e) => e.stopPropagation()}
      >
        {/* Header */}
        <div className={styles.header}>
           {headerBlock}
          <button onClick={onClose}>
            <img src={Close} alt="Close" />
          </button>
        </div>

        {/* Body */}
        <div className={styles.body}>
          {children}
        </div>

        {/* Footer (optional) */}
        {footer && (
          <div className={styles.footer}>
            {footer}
          </div>
        )}
      </div>
    </div>
  );
};

export default CustomModal;
