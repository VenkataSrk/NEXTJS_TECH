import React from "react";
import styles from "./scss/projectdrawer.module.scss"
import bluetick from "../../assets/images/project/blutick.svg"
import greytick from "../../assets/images/project/greytick.svg"
import avator1 from "../../assets/images/project/Avatar.png"
import { useState } from "react";
import list from "../../assets/images/product/list.svg"
import grid from "../../assets/images/product/grid.svg"
import threedot from "../../assets/images/inbox/threedot.svg";
import pdf from "../../assets/images/project/pdf.svg";
import editpencil from "../../assets/images/project/editpencil.svg"
import bellicon from "../../assets/images/project/notificationbell.svg"
import like from "../../assets/images/project/like.svg"
import eyeicon from "../../assets/images/project/eyeicon.svg"
import reply from "../../assets/images/project/reply.svg"

function Projectdrawer() {
    const [view, setView] = useState("list");
    const chatData = [
        {
            id: 1,
            name: "Common chat",
            message: "You: There's no way you'll be able",
            time: "8:35 AM",
            count: 12,
            color: "purple",
            img: bluetick
        },
        {
            id: 2,
            name: "Presales",
            message: "You: There's no way you'll be able",
            time: "8:35 AM",
            count: 12,
            color: "pink",
            img: greytick,
        },
        {
            id: 3,
            name: "BA",
            message: "You Created a Task",
            time: "3:30 PM",
            count: 6,
            color: "yellow"
        },
        {
            id: 4,
            name: "UI UX Designer",
            message: "There's no way you'll be able to motorcycle over that bus.",
            time: "8:35 AM",
            count: 4,
            color: "blue"
        },
        {
            id: 5,
            name: "Dev",
            message: "There's no way you'll be able to motorcycle over that bus.",
            time: "8:35 AM",
            count: 1,
            color: "sky"
        },
        {
            id: 6,
            name: "NLP",
            message: "There's no way you'll be able to motorcycle over that bus.",
            time: "6:45 PM",
            count: 1,
            color: "green"
        },
        {
            id: 7,
            name: "Sales",
            message: "Created a Meet",
            time: "Monday",
            count: 1,
            color: "orange"
        }
    ];

    const avatars = [
        { id: 1, img: avator1 },
        { id: 2, img: avator1 },
        { id: 3, img: avator1 }
    ];
    return (<>

        <div className={`${styles.drawer} ${styles.drawersmall}`}>
            <div className={styles.header}>
                <div className={styles.lefts}>
                    <div className={`${styles.title} ${styles.titlewithcancel}`}>
                        Inbound Interactions
                        <div className={styles.cancelbtn}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                                <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#2D2B2F" />
                            </svg>
                        </div>
                    </div>



                </div>
                <div className={styles.avatarStack}>
                    {avatars.map((item, index) => (
                        <div
                            key={item.id}
                            className={styles.avatar}
                            style={{ zIndex: avatars.length - index }}
                        >
                            <img src={item.img} alt="avatar" />
                        </div>
                    ))}
                </div>

            </div>
            <div className={styles.body}>
                <div className={styles.chatlist}>

                    {/* Tabs */}
                    <div className={styles.tabs}>
                        <span className={styles.active}>Updates</span>
                        <span>Files</span>
                        <span>Activity Log</span>
                    </div>
                    {true && <>
                        <div className={`${styles.tabinner} ${styles.updates}`}>

                            <div className={styles.search}>
                                <input type="text" placeholder="Search" />
                            </div>


                            <div className={styles.items}>
                                {chatData.map((chat) => (
                                    <div className={styles.item} key={chat.id}>

                                        <div className={`${styles.avatar} ${styles[chat.color]}`}>
                                            {chat.name.charAt(0)}
                                        </div>

                                        <div className={styles.content}>
                                            <h4>{chat.name}</h4>
                                            <p> <span>{<img src={chat.img} />}</span>{chat.message}</p>
                                        </div>

                                        <div className={styles.meta}>
                                            <span className={styles.time}>{chat.time}</span>
                                            <span className={styles.count}>{chat.count}</span>
                                        </div>

                                    </div>
                                ))}
                            </div>
                        </div>
                        {false &&<div className={styles.chatinterfaceafterclick}>
                            <div className={styles.header}>
                                <div className={styles.lefts}>
                                    <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
                                        <path d="M16.2724 5.33001H1.94332L5.99312 1.28021C6.28612 0.987213 6.28612 0.512613 5.99312 0.219713C5.70012 -0.0731875 5.22552 -0.0732875 4.93262 0.219713L0.662618 4.48921C0.232918 4.91891 -0.00238182 5.48871 1.81797e-05 6.09371C0.00291818 6.69331 0.239718 7.25481 0.662618 7.66991L4.93262 11.9404C5.07912 12.0869 5.27102 12.1601 5.46292 12.1601C5.65482 12.1601 5.84672 12.0869 5.99322 11.9404C6.28622 11.6474 6.28622 11.1728 5.99322 10.8799L1.94392 6.83011H16.2725C16.6866 6.83011 17.0225 6.49421 17.0225 6.08011C17.0225 5.66601 16.6865 5.33001 16.2724 5.33001Z" fill="#404040" />
                                    </svg></span>
                                    <div>
                                        <h5>Sales</h5>
                                        <p>Zain Korsgaard, John, William</p>
                                    </div>
                                </div>
                                <div className={styles.rights}>
 <div className={styles.avatarStack}>
                    {avatars.map((item, index) => (
                        <div
                            key={item.id}
                            className={styles.avatar}
                            style={{ zIndex: avatars.length - index }}
                        >
                            <img src={item.img} alt="avatar" />
                        </div>
                    ))}
                </div>
                                </div>
                            </div>
                            <div>
                                 <div className={styles.threadWrapper}>

      {/* Comment 1 */}
      <div className={styles.comment}>
        <div className={styles.headerofthread}>
          <div className={styles.user}>
            <img src={avator1} alt="user" />
            <span className={styles.name}>Zain Korsgaard</span>
            <span className={styles.time}>7:50 PM</span>
          </div>

          <div className={styles.actions}>
            <div>
                <img src={editpencil}/>
            </div>
            <div>
                <img src={bellicon}/>
            </div>
            <div>
                <img src={threedot}/>
            </div>
          </div>
        </div>

        <div className={styles.message}>
          <span className={styles.mention}>@Marley Torff</span>
        </div>

        <div className={styles.meta}>
          <span><img src={eyeicon}/>1</span> 
        </div>

        <div className={styles.footer}>
          <span><img src={like}/>Like </span> <span><span><img src={reply}/></span> Reply</span>
        </div>
      </div>

      {/* Comment 2 */}
      <div className={styles.comment}>
        <div className={styles.headerofthread}>
          <div className={styles.user}>
            <img src={avator1} alt="user" />
            <span className={styles.name}>John</span>
            <span className={styles.time}>7:50 PM</span>
          </div>
        </div>

        <div className={styles.message}>
          Yes, partial refunds are allowed, but only for Premium users.
        </div>

        <div className={styles.footer}>
           <span><img src={eyeicon}/>1</span> <span><img src={like}/> </span>  <span> <img src={reply}/>Reply</span>
        </div>
      </div>

      {/* Reply bubble */}
     
<div className={`${styles.comment} ${styles.replycomment}`}>
        <div className={styles.headerofthread}>
          <div className={styles.user}>
            <img src={avator1} alt="user" />
            <span className={styles.name}>John</span>
            <span className={styles.time}>7:50 PM</span>
          </div>
        </div>

        <div className={styles.message}>
          Yes, partial refunds are allowed, but only for Premium users.
        </div>

       
      </div>

    </div>
                            </div>
                        </div>}
                    </>
                    }
                    {
                        false && <div className={`${styles.tabinner} ${styles.files}`}>
                            <div className={styles.header}>
                                <div className={styles.lefts}>
                                    <button className={"secondarybtn"}> Add file</button>
                                </div>
                                <div className={styles.rights}>
                                    <div className={styles.viewToggle}>
                                        <button
                                            className={view === "grid" ? styles.active : ""}
                                            onClick={() => setView("grid")}
                                        >
                                            <img src={grid} alt="grid" />
                                        </button>

                                        <button
                                            className={view === "list" ? styles.active : ""}
                                            onClick={() => setView("list")}
                                        >
                                            <img src={list} alt="list" />
                                        </button>
                                    </div>
                                    <div>
                                        <svg xmlns="http://www.w3.org/2000/svg" width="13" height="18" viewBox="0 0 13 18" fill="none">
                                            <path d="M4.47174 13.3584C4.89894 13.7949 5.46974 14.0352 6.07864 14.0352C6.68804 14.0352 7.25884 13.795 7.68604 13.3584L11.9428 9.0127C12.2328 8.7168 12.228 8.2422 11.9321 7.9522C11.6362 7.6631 11.1616 7.667 10.8716 7.9639L6.83494 12.0847L6.82864 0.75C6.82864 0.3359 6.49274 0 6.07864 0C5.66454 0 5.32864 0.3359 5.32864 0.75L5.33494 12.0972L1.28564 7.9639C0.995639 7.668 0.521039 7.6641 0.225139 7.9522C-0.0707612 8.2422 -0.075661 8.7168 0.214439 9.0127L4.47174 13.3584Z" fill="#757676" />
                                            <path d="M11.4073 15.5313L0.749639 15.5372C0.335539 15.5372 0.000138764 15.8731 0.000138764 16.2872C0.000138764 16.7013 0.336039 17.0372 0.750139 17.0372L11.4078 17.0313C11.8219 17.0313 12.1573 16.6954 12.1573 16.2813C12.1573 15.8672 11.8214 15.5313 11.4073 15.5313Z" fill="#757676" />
                                        </svg>
                                    </div>
                                </div>
                            </div>

                            {
                                view !== "list" ?
                                    <>    <div className={styles.listcontainer}>


                                        <div className={`${styles.card} ${styles.prodcard}`}>
                                            <div className={styles.lefts}>
                                                <img src={pdf} />
                                            </div>
                                            <div className={styles.rights}>
                                                <div className={styles.top}>
                                                    <div>
                                                        <div className="boldtxt">File name</div>
                                                        <p>file size</p>
                                                    </div>
                                                    <div className="title">
                                                        <img src={threedot} />
                                                    </div>

                                                </div>
                                                <div className={`${styles.bottom} ${styles.bottomwithavator}`}>
                                                    <div>
                                                        <img src={avator1} />
                                                    </div>
                                                    Dec5, 2025
                                                </div>
                                            </div>
                                        </div>
                                        <div className={`${styles.card} ${styles.prodcard}`}>
                                            <div className={styles.lefts}>
                                                <img src={pdf} />
                                            </div>
                                            <div className={styles.rights}>
                                                <div className={styles.top}>
                                                    <div>
                                                        <div className="boldtxt">File name</div>
                                                        <p>file size</p>
                                                    </div>
                                                    <div className="title">
                                                        <img src={threedot} />
                                                    </div>

                                                </div>
                                                <div className={`${styles.bottom} ${styles.bottomwithavator}`}>
                                                    <div>
                                                        <img src={avator1} />
                                                    </div>
                                                    Dec5, 2025
                                                </div>
                                            </div>
                                        </div><div className={`${styles.card} ${styles.prodcard}`}>
                                            <div className={styles.lefts}>
                                                <img src={pdf} />
                                            </div>
                                            <div className={styles.rights}>
                                                <div className={styles.top}>
                                                    <div>
                                                        <div className="boldtxt">File name</div>
                                                        <p>file size</p>
                                                    </div>
                                                    <div className="title">
                                                        <img src={threedot} />
                                                    </div>

                                                </div>
                                                <div className={`${styles.bottom} ${styles.bottomwithavator}`}>
                                                    <div>
                                                        <img src={avator1} />
                                                    </div>
                                                    Dec5, 2025
                                                </div>
                                            </div>
                                        </div>



                                    </div> </> : <> <div className={styles.gridcontainer}>


                                        <div className={`${styles.card} ${styles.prodcard}`}>
                                            <div className={styles.lefts}>
                                                <img src={pdf} />
                                            </div>
                                            <div className={styles.rights}>
                                                <div className={styles.top}>
                                                    <div>
                                                        <div className="boldtxt">File name</div>
                                                        <p>file size</p>
                                                    </div>
                                                    <div style={{ display: `${view == "list" ? "none" : ""}` }} className="title">
                                                        <img src={threedot} />
                                                    </div>

                                                </div>
                                                <div className={`${styles.bottom} ${styles.bottomwithavator}`}>
                                                    <div>
                                                        <img src={avator1} />
                                                    </div>
                                                    Dec5, 2025
                                                </div>
                                            </div>
                                        </div>
                                        <div className={`${styles.card} ${styles.prodcard}`}>
                                            <div className={styles.lefts}>
                                                <img src={pdf} />
                                            </div>
                                            <div className={styles.rights}>
                                                <div className={styles.top}>
                                                    <div>
                                                        <div className="boldtxt">File name</div>
                                                        <p>file size</p>
                                                    </div>
                                                    <div style={{ display: `${view == "list" ? "none" : ""}` }} className="title">
                                                        <img src={threedot} />
                                                    </div>

                                                </div>
                                                <div className={`${styles.bottom} ${styles.bottomwithavator}`}>
                                                    <div>
                                                        <img src={avator1} />
                                                    </div>
                                                    Dec5, 2025
                                                </div>
                                            </div>
                                        </div>
                                        <div className={`${styles.card} ${styles.prodcard}`}>
                                            <div className={styles.lefts}>
                                                <img src={pdf} />
                                            </div>
                                            <div className={styles.rights}>
                                                <div className={styles.top}>
                                                    <div>
                                                        <div className="boldtxt">File name</div>
                                                        <p>file size</p>
                                                    </div>
                                                    <div style={{ display: `${view == "list" ? "none" : ""}` }} className="title">
                                                        <img src={threedot} />
                                                    </div>

                                                </div>
                                                <div className={`${styles.bottom} ${styles.bottomwithavator}`}>
                                                    <div>
                                                        <img src={avator1} />
                                                    </div>
                                                    Dec5, 2025
                                                </div>
                                            </div>
                                        </div>




                                    </div></>
                            }


                        </div>
                    }
                    {
                        false && <div className={`${styles.tabinner} ${styles.activitylog}`}>

                            <div className={styles.card}>
                                <div className={styles.top}>
                                    <div className={styles.lefts}>

                                        <img src={avator1} />

                                    </div>
                                    <div className={styles.rights}>
                                        <div className={styles.top}>
                                            <div className={styles.namewithtime}><h5>Zain Dokidis</h5> <span>10:00am</span></div>
                                            <p>Ux designer</p>
                                        </div>
                                        <div className={styles.bottom}>
                                            df
                                        </div>
                                    </div>
                                </div>

                                <div className={styles.greybgcard}>
                                    <div className={styles.dimtxt}>What Changed</div>
                                    <div className={styles.secondblock}>
                                        <label>Status:</label>
                                        <div className={styles.stschangeddiv}>
                                            <div className={`${styles.badge} ${styles.green}`}>Approved</div>
                                            <div><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
                                                <path d="M16.3604 4.48975L12.0899 0.21975C11.7969 -0.07325 11.3223 -0.07325 11.0294 0.21975C10.7365 0.51275 10.7364 0.98735 11.0294 1.28025L15.0797 5.33005H0.75C0.3359 5.33005 0 5.66595 0 6.08005C0 6.49415 0.3359 6.83005 0.75 6.83005H15.0791L11.0293 10.8799C10.7363 11.1729 10.7363 11.6475 11.0293 11.9404C11.1758 12.0869 11.3672 12.1601 11.5596 12.1601C11.752 12.1601 11.9434 12.0869 12.0899 11.9404L16.3555 7.67475C16.7832 7.25485 17.0196 6.69335 17.0225 6.09365C17.0254 5.48865 16.7901 4.91945 16.3604 4.48975Z" fill="black" />
                                            </svg></div>
                                            <div className={`${styles.badge} ${styles.green}`}>Approved</div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                            <div className={styles.card}>
                                <div className={styles.top}>
                                    <div className={styles.lefts}>

                                        <img src={avator1} />

                                    </div>
                                    <div className={styles.rights}>
                                        <div className={styles.top}>
                                            <div className={styles.namewithtime}><h5>Zain Dokidis</h5> <span>10:00am</span></div>
                                            <p>Ux designer</p>
                                        </div>
                                        <div className={styles.bottom}>
                                            df
                                        </div>
                                    </div>
                                </div>

                                <div className={styles.greybgcard}>
                                    <div className={styles.dimtxt}>What Changed</div>
                                    <div className={styles.secondblock}>
                                        <label>Status:</label>
                                        <div className={styles.stschangeddiv}>
                                            <div className={`${styles.badge} ${styles.green}`}>Approved</div>
                                            <div><svg xmlns="http://www.w3.org/2000/svg" width="18" height="13" viewBox="0 0 18 13" fill="none">
                                                <path d="M16.3604 4.48975L12.0899 0.21975C11.7969 -0.07325 11.3223 -0.07325 11.0294 0.21975C10.7365 0.51275 10.7364 0.98735 11.0294 1.28025L15.0797 5.33005H0.75C0.3359 5.33005 0 5.66595 0 6.08005C0 6.49415 0.3359 6.83005 0.75 6.83005H15.0791L11.0293 10.8799C10.7363 11.1729 10.7363 11.6475 11.0293 11.9404C11.1758 12.0869 11.3672 12.1601 11.5596 12.1601C11.752 12.1601 11.9434 12.0869 12.0899 11.9404L16.3555 7.67475C16.7832 7.25485 17.0196 6.69335 17.0225 6.09365C17.0254 5.48865 16.7901 4.91945 16.3604 4.48975Z" fill="black" />
                                            </svg></div>
                                            <div className={`${styles.badge} ${styles.green}`}>Approved</div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    }

                </div>
            </div>


        </div>
    </>)
} export default Projectdrawer