import React from "react";
import styles from './loginnew.module.scss'
import lockicon from "../../src/assets/images/locksvg.png"

const Newpassword = () =>{
    return(
        <>
            <div className={styles.loginmain}>
                  <div className={`${styles.loginmainblock} ${styles.loginmainblockstp}`}>
                                  <h2>Create New Password</h2>
                                     <div className={styles.loginblock}>
                                        <div className={styles.inputyourself}>
                                                 <div className={`${styles.passwordtext} ${styles.inputdiv}  ${styles.inputdivmail}`}>
                                                    <input
                                                       placeholder="Enter your new password"
                                                       className={`${styles.textpassword} ${styles.eroreword}`}
                                                       name="password"
                                                       autoComplete="off"
                                                       type="password"
                                                    />
                                                    <span className={styles.msgicon}>
                                                        <img src={lockicon} alt="Messageicon"/>
                                                    </span>
                                                    
                
                                                       <div
                                                          className={`${styles.placetold} ${styles.stepverifyfair} ${styles.stepverifymoderate} ${styles.stepverifystrong}`}
                                                       >
                                                          <span></span>
                                                          <span></span>
                                                          <span></span>
                                                          <span></span>
                                                       </div>
                
                
                                                    <div className={styles.contented}>
                                                          <span className={styles.alertweak}>Weak</span>
                                                          <span className={styles.alertweakfair}>Fair</span>
                                                          <span className={styles.alertweakmoder}>
                                                             Moderate
                                                          </span>
                                                          <span className={styles.alertweakstrong}>Strong</span>
                                                    </div>
                
                                                       <div className={styles.absolutedetails}>
                                                          <p>
                                                                <svg xmlns="http://www.w3.org/2000/svg" width="15" height="14" viewBox="0 0 15 14" fill="none">
                                                                   <path d="M7.50049 0.682617C10.9981 0.682793 13.8335 3.519 13.8335 7.0166C13.8333 10.5141 10.998 13.3494 7.50049 13.3496C4.00282 13.3496 1.16668 10.5143 1.1665 7.0166C1.1665 3.51876 4.00265 0.682617 7.50049 0.682617ZM10.7964 4.19434C10.4324 3.91463 9.90827 3.98341 9.62842 4.34766L6.61084 8.27832C6.57989 8.31869 6.53887 8.33986 6.48877 8.34277L6.48389 8.34375C6.43611 8.34717 6.39817 8.33327 6.35791 8.29199L5.30908 7.21094H5.30811C4.98809 6.88246 4.46069 6.87337 4.13037 7.19336C3.79978 7.51363 3.79254 8.04212 4.11279 8.37207L5.1626 9.4541C5.50947 9.81139 5.98474 10.0098 6.479 10.0098C6.51259 10.0098 6.54923 10.0084 6.5874 10.0059C7.11788 9.97426 7.60814 9.71598 7.93213 9.29395L10.9507 5.36328C11.2308 4.998 11.1617 4.47451 10.7964 4.19434Z" fill="#00B885" stroke="#00C851" /></svg>
                                                           
                                                             Cannot contain your name or e-mail</p>
                                                          <p>
                                                             <svg xmlns="http://www.w3.org/2000/svg" width="15" height="14" viewBox="0 0 15 14" fill="none">
                                                                <path d="M7.49983 0.351562C3.81949 0.351562 0.835938 3.33512 0.835938 7.01545C0.835938 10.6958 3.81949 13.6793 7.49983 13.6793C11.1802 13.6793 14.1637 10.6958 14.1637 7.01545C14.1637 3.33512 11.1802 0.351562 7.49983 0.351562ZM10.4206 9.15576C10.6159 9.35101 10.6159 9.66728 10.4206 9.86247C10.323 9.96009 10.1951 10.0089 10.0672 10.0089C9.93935 10.0089 9.81146 9.96009 9.71384 9.86247L7.49989 7.64852L5.28595 9.86247C5.18832 9.96009 5.06044 10.0089 4.93256 10.0089C4.80468 10.0089 4.6768 9.96009 4.57918 9.86247C4.38393 9.66721 4.38393 9.35095 4.57918 9.15576L6.79312 6.94182L4.57918 4.72787C4.38393 4.53262 4.38393 4.21635 4.57918 4.02117C4.77443 3.82598 5.0907 3.82592 5.28588 4.02117L7.49983 6.23511L9.71377 4.02117C9.90902 3.82592 10.2253 3.82592 10.4205 4.02117C10.6157 4.21642 10.6157 4.53269 10.4205 4.72787L8.20653 6.94182L10.4206 9.15576Z" fill="#FF4444" />
                                                             </svg>
                                                             At least 8 characters</p>
                                                          <p>
                                                             <svg xmlns="http://www.w3.org/2000/svg" width="15" height="14" viewBox="0 0 15 14" fill="none">
                                                                <path d="M7.49983 0.351562C3.81949 0.351562 0.835938 3.33512 0.835938 7.01545C0.835938 10.6958 3.81949 13.6793 7.49983 13.6793C11.1802 13.6793 14.1637 10.6958 14.1637 7.01545C14.1637 3.33512 11.1802 0.351562 7.49983 0.351562ZM10.4206 9.15576C10.6159 9.35101 10.6159 9.66728 10.4206 9.86247C10.323 9.96009 10.1951 10.0089 10.0672 10.0089C9.93935 10.0089 9.81146 9.96009 9.71384 9.86247L7.49989 7.64852L5.28595 9.86247C5.18832 9.96009 5.06044 10.0089 4.93256 10.0089C4.80468 10.0089 4.6768 9.96009 4.57918 9.86247C4.38393 9.66721 4.38393 9.35095 4.57918 9.15576L6.79312 6.94182L4.57918 4.72787C4.38393 4.53262 4.38393 4.21635 4.57918 4.02117C4.77443 3.82598 5.0907 3.82592 5.28588 4.02117L7.49983 6.23511L9.71377 4.02117C9.90902 3.82592 10.2253 3.82592 10.4205 4.02117C10.6157 4.21642 10.6157 4.53269 10.4205 4.72787L8.20653 6.94182L10.4206 9.15576Z" fill="#FF4444" />
                                                             </svg>
                                                             Contains a number or symbol</p>
                                                       </div>
                                                       <></>
                                                 </div>
                
                                                 <div className={`${styles.passwordtextverify} ${styles.inputdiv}  ${styles.inputdivmail}`}>
                                                    <input
                                                       placeholder="Confirm your password"
                                                       className={styles.textpassword}
                                                       name="confirmPassword"
                                                       autoComplete="off"
                                                    />
                                                    <span className={styles.msgicon}>
                                                                             <img src={lockicon} alt="Messageicon"/>
                                                                          </span>
                                                       <svg
                                                          className={`${styles.eyeicon} ${styles.eyeiconpassword}`}
                                                          xmlns="http://www.w3.org/2000/svg"
                                                          width="20"
                                                          height="11"
                                                          viewBox="0 0 20 11"
                                                          fill="none"
                                                       >
                                                          <path
                                                             d="M19.779 6.33692L17.6325 4.18992C18.4175 3.46582 19.119 2.63422 19.6936 1.68452C19.908 1.33002 19.7947 0.86912 19.4402 0.65422C19.0867 0.44032 18.6253 0.55362 18.4104 0.90712C16.6028 3.89442 13.4583 5.67762 9.99777 5.67762H9.99537C6.53737 5.67662 3.39527 3.89342 1.59057 0.90812C1.37527 0.55262 0.914271 0.44032 0.560271 0.65422C0.205771 0.86812 0.0924708 1.32902 0.306371 1.68352C0.880671 2.63372 1.58197 3.46572 2.36707 4.19032L0.220971 6.33682C-0.0720293 6.62982 -0.0720293 7.10442 0.220971 7.39732C0.367471 7.54382 0.559371 7.61702 0.751271 7.61702C0.943171 7.61702 1.13507 7.54382 1.28157 7.39732L3.54297 5.13562C4.39957 5.73572 5.33467 6.20642 6.32567 6.54672L5.66777 9.54382C5.57887 9.94912 5.83477 10.3485 6.23957 10.4374C6.29377 10.4491 6.34747 10.455 6.40067 10.455C6.74537 10.455 7.05597 10.2157 7.13257 9.86612L7.77557 6.93642C8.49897 7.08042 9.23687 7.17762 9.99487 7.17762H9.99777C10.7536 7.17762 11.4894 7.08142 12.2111 6.93832L12.8533 9.86602C12.93 10.2156 13.2405 10.4549 13.5852 10.4549C13.6384 10.4549 13.6921 10.449 13.7463 10.4373C14.1511 10.3484 14.4069 9.94902 14.3181 9.54372L13.6613 6.54962C14.6568 6.20932 15.5962 5.73712 16.4564 5.13462L18.7185 7.39732C18.865 7.54382 19.0569 7.61702 19.2488 7.61702C19.4407 7.61702 19.6326 7.54382 19.7791 7.39732C20.0721 7.10432 20.072 6.62982 19.779 6.33692Z"
                                                             fill="#788073"
                                                          />
                                                       </svg>
                                                 </div>
                                        </div>
                                     </div>
                                     <div className={styles.loginbutton}>
                                      <div  className={`${styles.btnSignin} `}>Create password</div> <div className="spinloader"></div>
                                      <div className={`${styles.loginbuttonst} ${styles.lognbtn}`}>
                                        <p>Once you've set your new password, you can use it to log in and reset it anytime in the settings.</p>
                                      </div>
                                    </div>
                                </div>
            </div>
        </>
    )
}
export default Newpassword;