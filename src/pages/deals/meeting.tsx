import react from "react"
import styles from "./scss/dealslist.module.scss";

function Meeting(){
    return(<>
     <div className={styles.toplayer}>
      <div className={styles.lefts}>
       <div className={styles.svgspan}>
<svg xmlns="http://www.w3.org/2000/svg" width="8" height="16" viewBox="0 0 8 16" fill="none">
  <path d="M7.04593 15.9644C6.81453 15.9644 6.58302 15.8755 6.40723 15.6988L0.889625 10.147C-0.295875 8.95361 -0.296875 7.01131 0.888625 5.81791L6.40323 0.266115C6.75283 -0.0883855 7.32413 -0.0873855 7.67573 0.261215C8.02923 0.611814 8.03123 1.18111 7.68063 1.53371L2.16603 7.08551C1.67483 7.57961 1.67582 8.38342 2.16702 8.87751L7.68463 14.4293C8.03423 14.7818 8.03323 15.3522 7.68073 15.7027C7.50493 15.8765 7.27543 15.9644 7.04593 15.9644Z" fill="#404040"/>
</svg>
       </div>
       <div className={styles.content}>
         <div><h5>Discovery</h5> <span className={`${styles.badge} ${styles.green}`}>Approved</span></div>
         <p>Initial discovery call to understand their requirements</p>

       </div>
      </div>
      <div className={styles.rights}>
10:00 am - 11:00 am - Monday Jan 5, 2026
      </div>
   </div>
  <div className={`${styles.meetingoverallpage} maincontainer`}>
  
  </div>
    </>)
}export default Meeting