import React from 'react';
import style from './scss/knowledgebase.module.scss'
import { assets } from '../../styles/assets/assets';

// import micIcon from '../../styles/images/customai/mic.svg'

const SipTrunk = () => {
  return (
    <div  className='overallscrollnone'>
     <div className={style.header}>
              <div className={style.lefts}>
                <h3>SIP Trunk</h3>
              </div>
              <div className={style.rights}>
                <span><svg xmlns="http://www.w3.org/2000/svg" width="6" height="18" viewBox="0 0 6 18" fill="none">
                  <path fill-rule="evenodd" clip-rule="evenodd" d="M3.00009 4.513C4.23399 4.513 5.24359 3.5034 5.24359 2.2695C5.24359 1.0356 4.23399 0.026001 3.00009 0.026001C1.76619 0.026001 0.756592 1.0356 0.756592 2.2695C0.756592 3.5034 1.76619 4.513 3.00009 4.513ZM3.00009 6.7565C1.76619 6.7565 0.756592 7.7661 0.756592 9C0.756592 10.2339 1.76619 11.2435 3.00009 11.2435C4.23399 11.2435 5.24359 10.2339 5.24359 9C5.24359 7.7661 4.23399 6.7565 3.00009 6.7565ZM0.756592 15.7305C0.756592 14.4966 1.76619 13.487 3.00009 13.487C4.23399 13.487 5.24359 14.4966 5.24359 15.7305C5.24359 16.9644 4.23399 17.974 3.00009 17.974C1.76619 17.974 0.756592 16.9644 0.756592 15.7305Z" fill="#404040" />
                </svg></span>
              </div>
            </div>
            <div className={`${style.sipscroller} padnone `}>
                    <div className={`${style.ticketright} ${style.ticketrightnew} ${style.htnone}`}>
        <div className={style.constactright}>
          
          <div className={`${style.elevatebrandbody} ${style.evelatelefts}`}>
           
            <div className={style.innerbody}>
              <div className={style.promtabmain}>
                <div className={style.bubblechatright}>
                  <div className={style.msgchatright}>
                    {/* <img src={assets.Pdfup} alt="" /> */}
                    <div className={style.msgchatleft}>
                      <div className={style.siptrunkleft}>
                        <h3>SIP Trunk<span>EN</span></h3>
                        <svg xmlns="http://www.w3.org/2000/svg" width="18" height="6" viewBox="0 0 18 6" fill="none">
                          <path fill-rule="evenodd" clip-rule="evenodd" d="M13.4869 3.00009C13.4869 4.23399 14.4965 5.24359 15.7304 5.24359C16.9643 5.24359 17.9739 4.23399 17.9739 3.00009C17.9739 1.76619 16.9643 0.756592 15.7304 0.756592C14.4965 0.756592 13.4869 1.76619 13.4869 3.00009ZM11.2434 3.00009C11.2434 1.76619 10.2338 0.756592 8.99988 0.756592C7.76598 0.756592 6.75638 1.76619 6.75638 3.00009C6.75638 4.23399 7.76598 5.24359 8.99988 5.24359C10.2338 5.24359 11.2434 4.23399 11.2434 3.00009ZM2.26938 0.756592C3.50328 0.756592 4.51288 1.76619 4.51288 3.00009C4.51288 4.23399 3.50328 5.24359 2.26938 5.24359C1.03548 5.24359 0.0258789 4.23399 0.0258789 3.00009C0.0258789 1.76619 1.03548 0.756592 2.26938 0.756592Z" fill="#252526" />
                        </svg>
                      </div>
                      <p>Please enter your website url so we can scrape your website.</p>
                    </div>
                  </div>
                </div>
                <div className={style.bubblechatleft}>
                  <p>✅ Confirmed → goes into Agents KB.</p>
                </div>
                <div className={style.bubblechatleft}>
                  <p><a href='/'>https://worktual.co.uk/ me?</a></p>
                  <div className={style.preparingli}>
                    <span className={style.loader}></span>
                    <p>Preparing your file</p>
                  </div>

                </div>
                <div className={style.updatefile}>
                  <p><svg xmlns="http://www.w3.org/2000/svg" width="16" height="17" viewBox="0 0 16 17" fill="none">
                    <path d="M9.67311 2.5356H12.0238C11.9968 2.48829 11.9627 2.44533 11.9228 2.40819L9.48667 0.145654C9.42791 0.091137 9.35786 0.0502418 9.28149 0.0258789L9.32315 2.18947C9.32505 2.28134 9.36264 2.36886 9.42797 2.43347C9.4933 2.49809 9.58123 2.53472 9.67311 2.5356Z" fill="#757676" />
                    <path d="M12.3867 10.6694C10.7384 10.6694 9.39722 12.014 9.39722 13.6666C9.39722 15.3191 10.7384 16.6637 12.3867 16.6637C14.0351 16.6637 15.3763 15.3191 15.3763 13.6666C15.3763 12.014 14.0351 10.6694 12.3867 10.6694ZM13.9997 12.6744L12.4458 14.9692L12.4242 14.99C12.3172 15.0929 12.1749 15.151 12.0264 15.1524C11.878 15.1537 11.7346 15.0983 11.6257 14.9973L10.9487 14.3675C10.8947 14.3153 10.8634 14.244 10.8613 14.1689C10.8593 14.0938 10.8868 14.0209 10.9379 13.9658C10.989 13.9108 11.0597 13.8779 11.1347 13.8744C11.2098 13.8708 11.2832 13.8968 11.3393 13.9468L12.0163 14.5762H12.0191L13.5258 12.3511C13.5466 12.3191 13.5736 12.2915 13.6052 12.27C13.6368 12.2485 13.6724 12.2335 13.7098 12.2259C13.7473 12.2183 13.7859 12.2183 13.8234 12.2259C13.8608 12.2334 13.8964 12.2483 13.928 12.2698C13.9597 12.2913 13.9867 12.3189 14.0075 12.3509C14.0283 12.383 14.0425 12.4188 14.0493 12.4565C14.0561 12.4941 14.0553 12.5327 14.0469 12.57C14.0386 12.6073 14.0229 12.6425 14.0008 12.6737L13.9997 12.6744Z" fill="#757676" />
                    <path d="M12.0999 10.1065V3.11054H9.67313C9.43111 3.10889 9.19927 3.01295 9.02684 2.84311C8.85441 2.67328 8.75497 2.44291 8.74965 2.20095L8.70695 -0.000488281H1.37427C1.05399 0.000798453 0.747305 0.129098 0.521515 0.356255C0.295725 0.583412 0.169278 0.890868 0.169924 1.21115V14.5184C0.16937 14.8386 0.295857 15.1459 0.521637 15.373C0.747418 15.6001 1.05405 15.7284 1.37427 15.7296H9.47941C9.1153 15.2155 8.89367 14.6141 8.837 13.9866C8.78032 13.3591 8.89063 12.7278 9.15672 12.1567C9.42281 11.5856 9.83522 11.095 10.3521 10.7348C10.869 10.3745 11.472 10.1574 12.0999 10.1054V10.1065Z" fill="#757676" />
                  </svg>Knowledge Base updated</p>
                </div>
                <div className={`${style.bubblechatright} `}>
                  <div className={`${style.bubblechatrightupdate}`}>
                    <a href="#">https://worktual.co.uk/ me?</a>
                  </div>
                </div>
              </div>
          

            </div>
          </div>
        </div>
                     </div>
            </div>
                 <div className={style.searchblockouter}>
            
                            <div style={{ marginTop: "16px" }} className={`${style.searchblock}`}>
                              <div className={style.searchmessage}>
                                <div className={style.searchmessageText}>
                                  {/* <input
                                type="file"
                                accept="application/pdf"
                                ref={fileInputRef}
                                style={{ display: "none" }}
                                onChange={uploadWhatsappTemplate}
                              />
                              <svg
                                xmlns="http://www.w3.org/2000/svg"
                                width="16"
                                height="16"
                                viewBox="0 0 16 16"
                                fill="none"
                                onClick={() => fileInputRef.current?.click()}
                                style={{ cursor: "pointer" }}
                              >
                                <path
                                  d="M15.2515 7.24538L8.74468 7.24987V0.748017C8.74468 0.333985 8.40902 -0.00167847 7.99499 -0.00167847C7.58096 -0.00167847 7.2453 0.333985 7.2453 0.748017V7.25097L0.747509 7.25547C0.333482 7.25577 -0.00197814 7.59163 -0.00167827 8.00567C-0.00137839 8.4197 0.334482 8.75516 0.748509 8.75486L7.2453 8.75036V15.2522C7.2453 15.6663 7.58096 16.0019 7.99499 16.0019C8.40902 16.0019 8.74468 15.6663 8.74468 15.2522V8.74936L15.2525 8.74487C15.6665 8.74457 16.002 8.4087 16.0017 7.99467C16.0014 7.58064 15.6655 7.24508 15.2515 7.24538Z"
                                  fill="#404040"
                                />
                              </svg> */}
                                  <textarea
                                  
                                    placeholder="Write your Goal & Objective"
                                    className="textareamainsnd"
                                    
                                    
                                    
                              
                                    style={{ minHeight: "40px", maxHeight: "120px", height: "40px" }}
                                  />
                                  
                                    <img src={assets.passicon} alt="typing" />
                                
                                    <img
                                      src={assets.sndarrowchat}
                                      alt="send"
                                     
                                      style={{ cursor: "pointer" }}
                                    />
                                
                                    <svg
                                      width="38"
                                      height="38"
                                      viewBox="0 0 38 38"
                                      fill="none"
                                      xmlns="http://www.w3.org/2000/svg"
                                    
                                    >
                                      <path
                                        d="M24.9911 20.2275C24.5907 20.1162 24.1776 20.3427 24.0634 20.7412C23.3202 23.3271 20.9189 25.1338 18.2236 25.1338C15.5507 25.1338 13.1543 23.3457 12.3965 20.7861C12.2803 20.3896 11.8662 20.1601 11.4649 20.2802C11.0674 20.3974 10.8409 20.8144 10.959 21.2118C11.832 24.1635 14.4568 26.268 17.4815 26.5722V28.3261C17.4815 28.7402 17.8174 29.0761 18.2315 29.0761C18.6456 29.0761 18.9815 28.7402 18.9815 28.3261V26.5707C22.0242 26.2595 24.649 24.1343 25.5049 21.1552C25.6192 20.7577 25.3896 20.3417 24.9911 20.2275Z"
                                        fill="#12C3B3"
                                      />
                                      <g filter="url(#filter0_i_273_2583)">
                                        <path
                                          d="M21.9571 17.1995L21.0801 15.1556C21.0332 15.0482 20.9463 14.9603 20.8399 14.9154L18.8399 14.058C18.3741 13.8266 18.1221 13.4291 18.1221 12.973C18.1221 12.5169 18.3741 12.1195 18.795 11.9085L20.8389 11.0315C20.9473 10.9846 21.0332 10.8987 21.0801 10.7893L21.3471 10.1665C20.5066 9.55758 19.4817 9.18848 18.3646 9.18848H18.0982C15.2792 9.18848 12.9941 11.4737 12.9941 14.2926V18.528C12.9941 21.347 15.2793 23.6321 18.0982 23.6321H18.3646C21.1834 23.6321 23.4686 21.3469 23.4686 18.528V17.7708C23.3282 17.8309 23.1805 17.8724 23.0216 17.8724C22.5655 17.8724 22.1681 17.6204 21.9571 17.1995Z"
                                          fill="url(#paint0_linear_273_2583)"
                                        />
                                      </g>
                                      <path
                                        d="M19.175 13.2766C19.0398 13.2095 18.9727 13.1081 18.9727 12.9729C18.9727 12.8384 19.0398 12.7372 19.175 12.6692L21.174 11.8126C21.4829 11.6802 21.7291 11.4341 21.8615 11.1251L22.7181 9.12613C22.7855 8.99163 22.8866 8.92383 23.0218 8.92383C23.1563 8.92383 23.2575 8.99163 23.3255 9.12613L24.1823 11.1252C24.3147 11.4341 24.5608 11.6802 24.8697 11.8126L26.8686 12.6692C27.0031 12.7373 27.0712 12.8384 27.0712 12.9729C27.0712 13.1081 27.0031 13.2095 26.8686 13.2766L24.8697 14.1332C24.5608 14.2656 24.3147 14.5117 24.1823 14.8206L23.3255 16.8197C23.2574 16.9549 23.1563 17.0223 23.0218 17.0223C22.8866 17.0223 22.7854 16.9549 22.7181 16.8197L21.8615 14.8207C21.7291 14.5118 21.483 14.2656 21.174 14.1332L19.175 13.2766Z"
                                        fill="url(#paint1_linear_273_2583)"
                                      />
                                      <defs>
                                        <filter
                                          id="filter0_i_273_2583"
                                          x="12.9941"
                                          y="9.18848"
                                          width="10.4745"
                                          height="16.4436"
                                          filterUnits="userSpaceOnUse"
                                          colorInterpolationFilters="sRGB"
                                        >
                                          <feFlood floodOpacity="0" result="BackgroundImageFix" />
                                          <feBlend
                                            mode="normal"
                                            in="SourceGraphic"
                                            in2="BackgroundImageFix"
                                            result="shape"
                                          />
                                          <feColorMatrix
                                            in="SourceAlpha"
                                            type="matrix"
                                            values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0"
                                            result="hardAlpha"
                                          />
                                          <feOffset dy="2" />
                                          <feGaussianBlur stdDeviation="2" />
                                          <feComposite in2="hardAlpha" operator="arithmetic" k2="-1" k3="1" />
                                          <feColorMatrix
                                            type="matrix"
                                            values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.1 0"
                                          />
                                          <feBlend
                                            mode="normal"
                                            in2="shape"
                                            result="effect1_innerShadow_273_2583"
                                          />
                                        </filter>
                                        <linearGradient
                                          id="paint0_linear_273_2583"
                                          x1="13.9726"
                                          y1="9.18848"
                                          x2="36.7322"
                                          y2="27.1426"
                                          gradientUnits="userSpaceOnUse"
                                        >
                                          <stop offset="0.0337" stopColor="#00E2A4" />
                                          <stop offset="0.1157" stopColor="#00E2A4" />
                                          <stop offset="0.5389" stopColor="#575CFF" />
                                          <stop offset="0.8687" stopColor="#FF793D" />
                                          <stop offset="1" stopColor="#00B885" />
                                        </linearGradient>
                                        <linearGradient
                                          id="paint1_linear_273_2583"
                                          x1="18.3027"
                                          y1="12.9068"
                                          x2="27.2121"
                                          y2="9.92262"
                                          gradientUnits="userSpaceOnUse"
                                        >
                                          <stop stopColor="#EA57FF" />
                                          <stop offset="0.945" stopColor="#FF5300" />
                                        </linearGradient>
                                      </defs>
                                    </svg>
                                
                                </div>
                              </div>
                            </div>
                          </div>
     
    </div>
  )
}
export default SipTrunk;
