import React from "react";
import styles from "./scss/drawer.module.scss"

function Drawer({title, children,  onClose }){
    return(
    <>
      <div className={`${styles.drawer} ${styles.drawersmall} ${styles.aichatdrawer}`}>
            <div className={styles.header}>
                <div className={styles.lefts}>
                    <div className={`${styles.title} ${styles.titlewithcancel}`}>
                  {title}
                       
                    </div>



                </div>
                <div className={styles.rights}>
                {title === "Transcript" && <div className={styles.downloadbtn}>
<svg xmlns="http://www.w3.org/2000/svg" width="13" height="18" viewBox="0 0 13 18" fill="none">
  <path d="M4.47174 13.3584C4.89894 13.7949 5.46974 14.0352 6.07864 14.0352C6.68804 14.0352 7.25884 13.795 7.68604 13.3584L11.9428 9.0127C12.2328 8.7168 12.228 8.2422 11.9321 7.9522C11.6362 7.6631 11.1616 7.667 10.8716 7.9639L6.83494 12.0847L6.82864 0.75C6.82864 0.3359 6.49274 0 6.07864 0C5.66454 0 5.32864 0.3359 5.32864 0.75L5.33494 12.0972L1.28564 7.9639C0.995639 7.668 0.521039 7.6641 0.225139 7.9522C-0.0707612 8.2422 -0.075661 8.7168 0.214439 9.0127L4.47174 13.3584Z" fill="#23232B"/>
  <path d="M11.4073 15.5313L0.749639 15.5372C0.335539 15.5372 0.000138764 15.8731 0.000138764 16.2872C0.000138764 16.7013 0.336039 17.0372 0.750139 17.0372L11.4078 17.0313C11.8219 17.0313 12.1573 16.6954 12.1573 16.2813C12.1573 15.8672 11.8214 15.5313 11.4073 15.5313Z" fill="#23232B"/>
</svg>
                </div>}
                 <div onClick={onClose} className={styles.cancelbtn}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                                <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#2D2B2F" />
                            </svg>
                        </div>
                </div>
               
               

            </div>
            <div className={styles.body}>
                {children}
            </div>


        </div>
    </>
    
)
}export default Drawer;