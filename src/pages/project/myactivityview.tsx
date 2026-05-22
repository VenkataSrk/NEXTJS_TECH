"use-client"
import { useState } from 'react';
import styles from './scss/activityview.module.scss'
import Arrow from '../../assets/images/contacts/dropdown.svg'
import Kebab from '../../assets/images/contacts/kebab.svg'
import Clouds from '../../assets/images/projectinsights/clouds.svg'
import Fileformate from '../../assets/images/projectinsights/fileformate.svg'
import Chevron from '../../assets/images/projectinsights/chevron.svg'
import People from '../../assets/images/projectinsights/contactlist.svg'
import Addicon from '../../assets/images/projectinsights/addicon.svg'
import Delete from '../../assets/images/projectinsights/delete.svg'
import Messageicon from '../../assets/images/projectinsights/messageicon.svg'
import Kebabvertical from '../../assets/images/projectinsights/verticalkebab.svg'
import Imageattachment from '../../assets/images/contacts/imageattachment.svg'
import Avatar from '../../assets/images/contacts/profileavatar.svg'
import Reminder from '../../assets/images/contacts/reminder.svg'
import Target from '../../assets/images/contacts/targeticon.svg'
import Pdficon from '../../assets/images/contacts/pdficon.svg'
import Download from '../../assets/images/contacts/downloadicon.svg'
import Clickablebtn from '../../assets/images/contacts/clickablebtn.svg'
import Worktualdropdown from '../component/worktualdropdown';
import CustomModal from '../component/worktualmodal';
const Myactivityview = () => {
    const domainList = [
        {
            title: "Domain Name",
            value: "Test_API Connection"
        },
        {
            title: "Auth Type",
            value: "OAuth 2.0"
        }
    ];
    const [open, setOpen] = useState(false);
    const summaryList = [
        {
            intentNo: 1,
            title: "🧠 Intent Detected:",
            description:
                "This section should summarize the customer’s detected intent, showing that they want deeper insights into usage patterns and more control over user-level reporting."
        },
        {
            intentNo: 2,
            title: "🧩 AI Suggested Requirement:",
            description:
                "Customer is looking for detailed analytics to understand feature adoption across teams."
        },
        {
            intentNo: 3,
            title: "📊 Reasoning:",
            description:
                "Customer is looking for detailed analytics to understand feature adoption across teams."
        },
    ];
    const files = [
        { id: 1, name: "Invoice.pdf", icon: Fileformate },
        { id: 2, name: "Agreement.docx", icon: Fileformate },
        { id: 3, name: "Report.xlsx", icon: Fileformate },
    ];

    const handleDelete = (id :any)=> {}


    const dealTimeline = [
        {
            step: 1,
            title: 'Design Phase',
            subtitle:
                'Shows the design stage status, key milestones, and overall progress handled by the design team.',
            lineLabel: 'Timeline',
            active: true
        },
        {
            step: 2,
            title: 'Deal created',
            subtitle:
                'Shows the design stage status, key milestones, and overall progress handled by the design team.',
            lineLabel: 'Timeline',
            active: true
        },
        {
            step: 3,
            title: 'Deal created',
            subtitle:
                'Shows the design stage status, key milestones, and overall progress handled by the design team.',
            lineLabel: 'Timeline',
            active: true
        }
    ];
    const documentList = [
        {
            id: 1,
            icon: Pdficon,
            title: "Office details",
            lang: "EN",
            size: "2 mb"
        },
        {
            id: 2,
            icon: Pdficon,
            title: "Company policy",
            lang: "EN",
            size: "1.4 mb"
        }
    ];
    const completedTasks = [
        {
            team: 'UX Team',
            initial: 'S',
            description: 'Shows completed work by team and individual contributors.',
            avatars: [Avatar, Avatar, Avatar]
        },
        {
            team: 'Dev Team',
            initial: 'D',
            description: 'Backend & frontend tasks completed.',
            avatars: [Avatar, Avatar]
        }
    ];
    const dropdownOptions = [
        { label: 'Internal Chat', value: 'Internal Chat' },
        { label: 'Files ', value: 'Files ' },
        { label: 'Activity Log', value: 'Activity Log' },
    ];
    const dropdownPdf = [
        { label: 'Download', value: 'Download'},
        { label: 'View', value: 'View'},
        { label: 'Delete ', value: 'Delete '},
    ];
    return (
        <>
            <div className={styles.projectcontainer}>
                <div className={styles.projectopbar}>
                    <div className={styles.projectblock}>
                        <div className={styles.projectblockinnermain}>
                            <div className={styles.projectblockleft}>
                                <div className={styles.projectblockinner}>
                                    <div className={styles.projectblocktitle}>REQ-041</div>
                                    <div className={styles.subtitleblock}>Setup process</div>
                                    <div className={styles.priorityblock}>P2</div>
                                </div>
                                <div className={styles.projectstatus}>In Progress</div>
                            </div>
                            <div className={`${styles.projectblockright} activitydropdown activityviewdropdown`}>
                                <img src={Messageicon} alt="Messageicon" onClick={() => setOpen(true)} />
                                <div className={styles.projectblockremain}>Remaining time <span>01:27:47</span></div>
                                {/* <img src={Kebabvertical} alt="Arrow" /> */}
                                <Worktualdropdown options={dropdownOptions} />
                            </div>
                        </div>
                        <div className={styles.projectInnerContent}>
                            <div className={styles.projectblockinsight}>
                                <div className={styles.agenticinsight}>Agentic AI Insight</div>
                                {summaryList.map((item, index) => (
                                    <div className={styles.summarizelist} key={index}>
                                        <div className={styles.intentlist}>{item.intentNo}</div>

                                        <div className={styles.agentblock}>
                                            <div className={styles.agentblocktitle}>{item.title}</div>
                                            <div className={styles.agentblocksubtitle}>
                                                {item.description}
                                            </div>
                                        </div>
                                    </div>
                                ))}
                                <div className={styles.viewanalysis}>View full AI analysis and recommendations <img src={Clickablebtn} alt="Clickablebtn" /></div>
                            </div>
                            <div className={styles.requirementlist}>
                                <div className={styles.requireblock}>
                                    <div className={styles.requireblocklist}>Requirement List</div>
                                    <div className={styles.requireblocksublist}>This section should display a consolidated list of all requirements associated with the project, clearly outlining each requirement’s.</div>
                                </div>
                                <div className={styles.requireblockbtn}>Download Full List <img src={Download} alt="Download" /></div>
                            </div>
                            <div className={styles.stepscontainer}>
                                <div className={styles.stepstitle}>Next Steps</div>
                                <div className={styles.outlinetitle}>This section should outline the current status of upcoming actions, including feasibility review, development estimation, and any AI-recommended add-ons.</div>
                            </div>
                            <div className={styles.dealactivity}>
                                <div className={styles.dealblock}>
                                    <div className={styles.dealblocktitle}>Product Stages</div>
                                    <button>Update Status</button>
                                </div>

                                <div className={styles.dealcreatedblock}>
                                    {dealTimeline.map((item, index) => (
                                        <div className={styles.loopcontainer} key={index}>
                                            <div
                                                className={`${styles.loopcount} ${item.active ? styles.active : ''
                                                    }`}
                                            >
                                                {item.step}
                                            </div>

                                            <div className={styles.loopblock}>
                                                <div className={styles.loopblocktitle}>{item.title}<span className={styles.inprogress}>Completed</span></div>
                                                <div className={styles.loopblocksubtitle}>{item.subtitle}</div>
                                                <div className={styles.loopblocktitleline}>{item.lineLabel}</div>
                                            </div>
                                        </div>
                                    ))}
                                </div>
                            </div>
                            <div className={styles.requestednamelist}>
                                <div className={styles.requestedtitle}>Requested requirements </div>
                                <div className={styles.requestmainblock}>
                                    <div className={styles.requestname}>API</div>
                                    <div className={styles.requestdirectmain}>
                                        <div className={styles.requestblock}>
                                            {domainList.map((item, index) => (
                                                <div className={styles.domainname} key={index}>
                                                    <div className={styles.domainnametitle}>{item.title}</div>
                                                    <div className={styles.testconnection}>{item.value}</div>
                                                </div>
                                            ))}
                                        </div>
                                        <div className={styles.requestblock}>
                                            {domainList.map((item, index) => (
                                                <div className={styles.domainname} key={index}>
                                                    <div className={styles.domainnametitle}>{item.title}</div>
                                                    <div className={styles.testconnection}>{item.value}</div>
                                                </div>
                                            ))}
                                        </div>
                                    </div>
                                </div>
                                <div className={styles.requestmainblock}>
                                    <div className={styles.requestname}>Document</div>
                                    <div className={styles.documentlistid}>
                                        {documentList.map((doc) => (
                                            <div className={styles.documentblock} key={doc.id}>
                                                <div className={styles.documenttype}>
                                                    <div className={styles.documentlist}>
                                                        <div className={styles.documentlistleft}>
                                                            <img src={doc.icon} alt="Pdficon" />
                                                        </div>

                                                        <div className={styles.officedetails}>
                                                            <div className={styles.officedetailstitle}>
                                                                {doc.title} <span>{doc.lang}</span>
                                                            </div>
                                                            <div className={styles.officedetailssubtitle}>{doc.size}</div>
                                                        </div>
                                                    </div>

                                                    <div className={`${styles.kebablist} activitydropdown`}>
                                                       <Worktualdropdown options={dropdownPdf} triggerIcon={Kebab}/>
                                                    </div>
                                                </div>
                                            </div>
                                        ))}
                                    </div>
                                </div>
                            </div>
                            <div className={styles.teamoverview}>
                                <div className={styles.teamoverviewblockone}>
                                    <div className={styles.teamtitle}>Team Execution Overview</div>
                                    <div className={styles.teamsubtitle}>This section should clearly describe the overall project requirement, outlining the scope of work such as redesign goals, key focus areas, and the major phases involved from research and design to implementation.</div>
                                </div>
                                <div className={styles.teamoverviewblocktwo}>
                                    <div className={styles.workcompleted}><img src={Target} alt="Target" />Who Worked & What They Completed</div>
                                    <div className={styles.teamprepared}>
                                        {completedTasks.map((item, index) => (
                                            <div className={styles.completedtask} key={index}>
                                                <div className={styles.teamleft}>
                                                    <div className={styles.teamtitletask}>{item.team}</div>

                                                    <div className={styles.teamprofile}>
                                                        <div className={styles.teamprofiletitle}>{item.initial}</div>
                                                        <div className={styles.teamprofilesubtitle}>
                                                            {item.description}
                                                        </div>
                                                    </div>
                                                </div>

                                                <div className={styles.teamright}>
                                                    {item.avatars.map((avatar, i) => (
                                                        <img src={avatar} alt="Avatar" key={i} />
                                                    ))}
                                                </div>
                                            </div>
                                        ))}
                                    </div>
                                </div>
                                <div className={styles.teamoverviewblocktwo}>
                                    <div className={styles.workcompleted}><img src={Reminder} alt="Reminder" />Ongoing Work</div>
                                    <div className={styles.teamprepared}>
                                        {completedTasks.map((item, index) => (
                                            <div className={`${styles.completedtask} ${styles.completedtaskblock}`} key={index}>
                                                <div className={styles.completeleft}>
                                                </div>
                                                <div className={styles.completeright}>
                                                    Each item should show the role or contributor involved and a short description of the work currently in progress, helping teams understand what is actively being developed or implemented.
                                                </div>
                                            </div>
                                        ))}
                                    </div>
                                </div>
                                <div className={styles.teamoverviewblockthree}>
                                    <div className={styles.daystake}>
                                        <div className={styles.Remainingtake}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                            <path d="M10 5V10L13.3333 11.6667" stroke="white" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                            <path d="M9.99984 18.3327C14.6022 18.3327 18.3332 14.6017 18.3332 9.99935C18.3332 5.39698 14.6022 1.66602 9.99984 1.66602C5.39746 1.66602 1.6665 5.39698 1.6665 9.99935C1.6665 14.6017 5.39746 18.3327 9.99984 18.3327Z" stroke="white" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                        </svg>Average Time Taken</div>
                                        <div className={styles.remaintitle}>2.4 days</div>
                                    </div>
                                    <div className={styles.daystake}>
                                        <div className={styles.Remainingtake}><svg xmlns="http://www.w3.org/2000/svg" width="19" height="10" viewBox="0 0 19 10" fill="none">
                                            <path d="M17.4999 0.833984L10.4166 7.91732L6.24992 3.75065L0.833252 9.16732" stroke="white" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                        </svg>Estimated Time Remaining</div>
                                        <div className={styles.remaintitle}>1.5 days</div>
                                    </div>
                                </div>
                            </div>
                            <div className={styles.documentloader}>
                                {documentList.map((doc) => (
                                    <div className={styles.documentblock} key={doc.id}>
                                        <div className={styles.documenttype}>
                                            <div className={styles.documentlist}>
                                                <div className={styles.documentlistleft}>
                                                    <img src={doc.icon} alt="Pdficon" />
                                                </div>

                                                <div className={styles.officedetails}>
                                                    <div className={styles.officedetailstitle}>
                                                        {doc.title} <span>{doc.lang}</span>
                                                    </div>
                                                    <div className={styles.officedetailssubtitle}>{doc.size}</div>
                                                </div>
                                            </div>

                                            <div className={`${styles.kebablist} activitydropdown`}>
                                                <Worktualdropdown options={dropdownPdf} triggerIcon={Kebab}/>
                                            </div>
                                        </div>
                                    </div>
                                ))}
                            </div>
                            <div className={styles.imageuploader}>
                                <div className={styles.imagecontent}>
                                    <img src={Imageattachment} alt="Imageattachment" />
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
            <CustomModal
                open={open}
                onClose={() => setOpen(false)}
                headerBlock={<div className={styles.modalheader}>
                    <div className={styles.modalheadertitle}>Share this file</div>
                    <div className={styles.modalheadersubtitle}>Content needed</div>
                </div>}
                footer={
                    <>
                        <button onClick={() => setOpen(false)}>
                            Cancel
                        </button>
                        <button>
                            Share
                        </button>
                    </>
                }
            >
                <div className={styles.modalcontainerblock}>
                    <div className={styles.uploaddocument}>
                        <img src={Clouds} alt="Clouds" />
                        <div className={styles.cloudlist}>
                            <div className={styles.cloudtitle}>Click to Upload <span>or Drag and drop here</span></div>
                            <div className={styles.cloudname}>JPEG or PNG format, up to 5 MB.</div>
                            <div className={styles.cloudname}>Recommended size: 192×192 px for best display.</div>
                        </div>
                    </div>
                    <div className={styles.filesadded}>
                        <div className={styles.filesaddedtitle}>7 files added</div>
                        <div className={styles.filesaddedblock}>
                            {files.map((file) => (
                                <div key={file.id} className={styles.filesaddedblockcontainer}>
                                    <div className={styles.filesaddedleft}>
                                        <img src={file.icon} alt="Fileformate" />
                                        <span>{file.name}</span>
                                    </div>

                                    <div
                                        className={styles.filesaddedright}
                                        onClick={() => handleDelete(file.id)}
                                    >
                                        <img src={Delete} alt="Delete" />
                                    </div>
                                </div>
                            ))}

                        </div>
                    </div>
                    <div className={styles.permissionfile}>
                         <div className={styles.viewfiletitle}>Permission to view files</div>
                         <div className={styles.inviteblock}>
                            <input type="text" placeholder='Invite teams '/>
                            <button>Invite</button>
                         </div>
                         <div className={styles.viewdetailsmaincontainer}>
                            <div className={styles.viewdetailsteam}> <img src={Addicon} alt="Addicon" />Ui UX designer</div>
                         </div>
                    </div>
                    <div className={styles.permissionfile}>
                         <div className={styles.viewfiletitle}>Who has access</div>
                         <div className={styles.contactloopcontainer}>
                            <div className={styles.contactlooplist}>
                                <div className={styles.contactlooplistleft}>
                                    <img src={People} alt="People" /><span>Presales team</span>
                                </div>
                                <div className={styles.contactlooplistright}>
                                  <span>9 </span> people
                                  <img src={Chevron} alt="Chevron" />
                                </div>
                            </div>
                            <div className={styles.contactlooplist}>
                                <div className={styles.contactlooplistleft}>
                                    <img src={People} alt="People" /><span>Presales team</span>
                                </div>
                                <div className={styles.contactlooplistright}>
                                  <span>9 </span> people
                                  <img src={Chevron} alt="Chevron" />
                                </div>
                            </div>
                         </div>
                    </div>
                </div>
            </CustomModal>
        </>
    )
}
export default Myactivityview;