"use-client"
import styles from './scss/contactproject.module.scss'
import Arrow from '../../assets/images/contacts/dropdown.svg'
import Kebab from '../../assets/images/contacts/kebab.svg'
import Imageattachment from '../../assets/images/contacts/imageattachment.svg'
import Avatar from '../../assets/images/contacts/profileavatar.svg'
import Reminder from '../../assets/images/contacts/reminder.svg'
import Target from '../../assets/images/contacts/targeticon.svg'
import Pdficon from '../../assets/images/contacts/pdficon.svg'
import Download from '../../assets/images/contacts/downloadicon.svg'
import Clickablebtn from '../../assets/images/contacts/clickablebtn.svg'
import { useEffect, useState } from 'react';
import Worktualdropdown from '../component/worktualdropdown'
import { useNavigate, useSearchParams } from 'react-router-dom'
import { get_Individual_projects } from '../../base/service/core_serviceApi'
import Nodata from '../component/nodata'
import { useSelector } from 'react-redux'
import { useDispatcher } from '../../store/redux-store/dispatch'
import { commonDataSliceActions } from '../../store/slice/commonSilce/commonSlice'
import { v4 as uuidv4 } from 'uuid';

const Contactproject = () => {

    const [params] = useSearchParams()
    const customerId = params.get('coid')
    const { contactProjectList, contactProjectLoading } = useSelector((store: any) => store.contactStore)
    const navigate = useNavigate();
    const dispatch = useDispatcher();
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
    const [open, setOpen] = useState('');

    const dropdownPdf = [
        { label: 'Download', value: 'Download' },
        { label: 'View', value: 'View' },
        { label: 'Delete ', value: 'Delete ' },
    ];

    const projectList = contactProjectList ?? [];
    const [individualProjectAllData, setIndividualProjectAll] = useState<any>({});
    const [timers, setTimers] = useState<any>([]);

    const individualProjectData = individualProjectAllData?.[open] ?? {}

    const getTimeDifference = (startTime: any, endTime: any) => {
        if(!endTime || !startTime){
            return
        }
        const diff: any = new Date(endTime) - new Date(startTime);
        if (diff < 0) {
            return "00:00:00";
        }
        const hours = Math.floor(diff / 3600000);
        const minutes = Math.floor((diff % 3600000) / 60000);
        const seconds = Math.floor((diff % 60000) / 1000);

        return (
            String(hours).padStart(2, "0") + ":" +
            String(minutes).padStart(2, "0") + ":" +
            String(seconds).padStart(2, "0")
        );
    }



    useEffect(() => {
        const next = (projectList || []).map((item: any) => getTimeDifference(item?.current_stage, item?.project_endtime));
        setTimers(next);
    }, [projectList]);

    const getIndividualProject = async (productRefId: any) => {
        try {
            const res = await get_Individual_projects({ customerId, productRefId: productRefId });
            if (res?.response?.length > 0 && Array.isArray(res?.response)) {
                setIndividualProjectAll((prev :any) => ({
                    ...prev, 
                    [productRefId]: res?.response?.[0] ?? {}
                }));
            } else {
                setIndividualProjectAll({});
            }
        } catch (error) {
            console.log("getIndividualProject error:", error)
        }
    }
    const reduceTime = (timeStr: any) => {
        if (!timeStr || typeof timeStr !== "string") return "00:00:00";
        const [h = 0, m = 0, s = 0] = timeStr.split(":").map(Number);
        let total = h * 3600 + m * 60 + s;

        if (!Number.isFinite(total) || total <= 0) return "00:00:00";
        total--;

        const hours = String(Math.floor(total / 3600)).padStart(2, "0");
        const mins = String(Math.floor((total % 3600) / 60)).padStart(2, "0");
        const secs = String(total % 60).padStart(2, "0");

        return `${hours}:${mins}:${secs}`;
    }


    useEffect(() => {
        const intervalId = setInterval(() => {
            setTimers((prev: any[]) => prev.map((t) => reduceTime(t)));
        }, 1000);

        return () => clearInterval(intervalId);
    }, []);

    const openIndividual = (item :any)=> {
        if(open === item?.project_name){
            setOpen('')
        }else {
            setOpen(item?.project_name); 
            getIndividualProject(item?.project_name)
        }
    }

    const updateActivityHandler =()=> {
        const sessionId: any = uuidv4() + 'contact';
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/contact/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "contact" }));
        navigate(`/contact/chat-conversation?sessionId=${sessionId}`);
    }

    return (
        <>
            <div className={styles.projectcontainer}>
                <div className={styles.projectopbar}>

                    {projectList?.length > 0
                        ? (
                            projectList?.map((item: any, index: any) => {
                                if(!item.project_name) return;
                                return (
                                    <div className={styles.projectblock}>
                                        <div className={styles.projectblockinnermain}>
                                            <div className={styles.projecttoplayer} key={item?.project_name || index}>
                                                <div className={styles.projectblockleft} >
                                                    <div className={styles.projectblockinner}>
                                                        <div className={styles.projectblocktitle}>{item?.project_name ?? "-"}</div>
                                                        <div className={styles.subtitleblock}>{item?.description ?? "-"}</div>
                                                        <div className={styles.priorityblock}>{item?.priority ?? "-"}</div>
                                                    </div>
                                                    <div className={styles.projectstatus}>{item?.status ?? "-"}</div>
                                                </div>
                                                <div className={styles.projectblockright}>
                                                    <div className={styles.projectblockremain}>Remaining time <span>{timers?.[index] || "00:00:00"}</span>
                                                    </div>
                                                    <img 
                                                        src={Arrow} 
                                                        alt="Arrow" 
                                                        className={`${styles.arrow} ${open ? styles.rotate : ""}`} 
                                                        onClick={() => { openIndividual(item)}}
                                                        />
                                                </div>
                                            </div>
                                        </div>

                                        {!!open && open === item?.project_name && (
                                            <div className={styles.projectInnerContent}>
                                                <div className={styles.projectblockinsight}>
                                                    <div className={styles.agenticinsight}>Agentic AI Insight</div>
                                                    <div dangerouslySetInnerHTML={{ __html: individualProjectData?.agentic_insight?.message || '' }} />
                                                    {/* {summaryList.map((item, index) => ( */}
                                                    {/* <div className={styles.summarizelist} key={index}>
                                                        <div className={styles.intentlist}>{item.intentNo}</div>

                                                        <div className={styles.agentblock}>
                                                            <div className={styles.agentblocktitle}>{item.title}</div>
                                                            <div className={styles.agentblocksubtitle}>
                                                                {item.description}
                                                            
                                                            </div>
                                                        </div>
                                                    </div> */}
                                                    {/* ))} */}
                                                    {/* <div className={styles.viewanalysis}>View full AI analysis and recommendations <img src={Clickablebtn} alt="Clickablebtn" /></div> */}
                                                </div>
                                                <div className={styles.requirementlist}>
                                                    <div className={styles.requireblock}>
                                                        <div className={styles.requireblocklist}>Requirement List</div>
                                                        <div dangerouslySetInnerHTML={{ __html: individualProjectData?.requirement_list?.[0] || '' }} />
                                                        {/* <div className={styles.requireblocksublist}>This section should display a consolidated list of all requirements associated with the project, clearly outlining each requirement’s.</div> */}
                                                    </div>
                                                    <div className={styles.requireblockbtn}> <div className='dynamiccontent' dangerouslySetInnerHTML={{ __html: individualProjectData?.requirement_list?.[1] || '' }} /><img src={Download} alt="Download" /></div>
                                                </div>
                                                <div className={styles.stepscontainer}>
                                                    <div className={styles.stepstitle}>Next Steps</div>
                                                    <div dangerouslySetInnerHTML={{ __html: individualProjectData?.next_steps || '' }} />
                                                    {/* <div className={styles.outlinetitle}>This section should outline the current status of upcoming actions, including feasibility review, development estimation, and any AI-recommended add-ons.</div> */}
                                                </div>
                                                <div className={styles.dealactivity}>
                                                    <div className={styles.dealblock}>

                                                        <div className={styles.dealblocktitle}>Product Stages</div>
                                                        <button onClick={updateActivityHandler}>Update Status</button>
                                                    </div>

                                                    <div className={styles.dealcreatedblock}>
                                                        {individualProjectData?.product_stages?.map((item, index) => (
                                                            <div className={styles.loopcontainer} key={index}>
                                                                <div
                                                                    className={`${styles.loopcount} ${item.status == "completed" ? styles.active : ''
                                                                        }`}
                                                                >
                                                                    {index + 1}
                                                                </div>

                                                                <div className={styles.loopblock}>
                                                                    <div className={styles.loopblocktitle}>{item?.team}<span className={styles.inprogress}>{item?.status}</span></div>
                                                                    <div className={styles.loopblocksubtitle}>{item.stage}</div>
                                                                    {/* <div className={styles.loopblocktitleline}>{item.lineLabel}</div> */}
                                                                </div>
                                                            </div>
                                                        ))}
                                                    </div>
                                                </div>
                                                <div className={styles.requestednamelist}>
                                                    <div className={styles.requestedtitle}>Requested requirements </div>
                                                    <div className={styles.requestmainblock}>
                                                        <div dangerouslySetInnerHTML={{ __html: individualProjectData?.requested_requirements?.summary || '' }} />
                                                        {individualProjectData?.requested_requirements?.domainList?.length > 0 &&
                                                            <>
                                                                <div className={styles.requestname}>API</div>
                                                                <div className={styles.requestdirectmain}>
                                                                    {individualProjectData?.requested_requirements?.domainList?.length > 0 && individualProjectData?.requested_requirements?.domainList.map((item, index) => (
                                                                        <>

                                                                            <div className={styles.requestblock}>
                                                                                <div className={styles.domainname} key={index}>
                                                                                    <div className={styles.domainnametitle}>{item.domainNameHeading}</div>
                                                                                    <div className={styles.testconnection}>{item.domainDesc}</div>
                                                                                    <div className={styles.domainnametitle}>{item.domainUrlHeading}</div>
                                                                                    <div className={styles.testconnection}>{item.domainURL}</div>
                                                                                </div>

                                                                            </div>

                                                                        </>
                                                                    ))}
                                                                    {/* <div className={styles.requestblock}>
                                                {domainList.map((item, index) => (
                                                    <div className={styles.domainname} key={index}>
                                                        <div className={styles.domainnametitle}>{item.title}</div>
                                                        <div className={styles.testconnection}>{item.value}</div>
                                                    </div>
                                                ))}
                                            </div> */}
                                                                </div>
                                                            </>
                                                        }
                                                    </div>
                                                    <div className={styles.requestmainblock}>
                                                        {individualProjectData?.requested_requirements?.documents?.length > 0 &&
                                                            <>
                                                                <div className={styles.requestname}>Document</div>
                                                                <div className={styles.documentlistid}>
                                                                    {individualProjectData?.requested_requirements?.documents?.length > 0 && individualProjectData?.requested_requirements?.documents?.map((doc) => (
                                                                        <div className={styles.documentblock} key={doc.uniqueid}>
                                                                            <div className={styles.documenttype}>
                                                                                <div className={styles.documentlist}>
                                                                                    <div className={styles.documentlistleft}>
                                                                                        <img src={doc.filetype == "pdf" ? Pdficon : Pdficon} alt="Pdficon" />
                                                                                    </div>

                                                                                    <div className={styles.officedetails}>
                                                                                        <div className={styles.officedetailstitle}>
                                                                                            {doc.fileName}
                                                                                            {/* <span>{doc.lang}</span> */}
                                                                                        </div>
                                                                                        <div className={styles.officedetailssubtitle}>{doc.size}kb</div>
                                                                                    </div>
                                                                                </div>

                                                                                <div className={`${styles.kebablist} activitydropdown`}>
                                                                                    <Worktualdropdown options={dropdownPdf} triggerIcon={Kebab} />
                                                                                </div>
                                                                            </div>
                                                                        </div>
                                                                    ))}
                                                                </div>
                                                            </>
                                                        }
                                                    </div>
                                                </div>
                                                <div className={styles.teamoverview}>
                                                    <div className={styles.teamoverviewblockone}>
                                                        <div className={styles.teamtitle}>Team Execution Overview</div>
                                                        {/* <div className={styles.teamsubtitle}>This section should clearly describe the overall project requirement, outlining the scope of work such as redesign goals, key focus areas, and the major phases involved from research and design to implementation.</div> */}
                                                        <div className={styles.teamsubtitle} dangerouslySetInnerHTML={{ __html: individualProjectData?.team_execution_overview?.summary || "" }} />

                                                    </div>
                                                    <div className={styles.teamoverviewblocktwo}>
                                                        {/* <div className={styles.workcompleted}><img src={Target} alt="Target" />Who Worked & What They Completed</div> */}
                                                        <div className={styles.teamprepared}>
                                                            {individualProjectData?.team_execution_overview?.teamWork?.length > 0 && individualProjectData?.team_execution_overview?.teamWork?.map((item, index) => (
                                                                <div className={styles.completedtask} key={index}>
                                                                    <div className={styles.teamleft}>
                                                                        <div className={styles.teamtitletask}>{item?.team ?? "-"}</div>

                                                                        <div className={styles.teamprofile}>
                                                                            <div className={styles.teamprofiletitle}>{item?.admin ? item?.admin?.charAt(0)?.toUpperCase() : "-"}</div>
                                                                            <div className={styles.teamprofilesubtitle}>
                                                                                {item.work_desc}
                                                                            </div>
                                                                        </div>
                                                                    </div>

                                                                    <div className={styles.teamright}>
                                                                        {item.participants?.length > 0 && item.participants?.map((avatar, i) => (
                                                                            <img src={Avatar} alt="Avatar" key={i} />

                                                                            // <p  >{avatar?avatar?.charAt(0)?.toUpperCase():"-"}</p>
                                                                        ))}
                                                                    </div>
                                                                </div>
                                                            ))}
                                                        </div>
                                                    </div>
                                                    <div className={styles.teamoverviewblocktwo}>
                                                        {individualProjectData?.team_execution_overview?.ongoingWork?.length > 0 &&
                                                            <>
                                                                <div className={styles.workcompleted}><img src={Reminder} alt="Reminder" />Ongoing Work</div>
                                                                <div className={styles.teamprepared}>
                                                                    {individualProjectData?.team_execution_overview?.ongoingWork?.length > 0 && individualProjectData?.team_execution_overview?.ongoingWork?.map((item, index) => (
                                                                        <div className={`${styles.completedtask} ${styles.completedtaskblock}`} key={index}>
                                                                            <div className={styles.completeleft}>

                                                                            </div>
                                                                            <div className={styles.completeright} dangerouslySetInnerHTML={{ __html: item || "" }} />


                                                                        </div>
                                                                    ))}
                                                                </div>
                                                            </>}
                                                    </div>

                                                    <div className={styles.teamoverviewblockthree}>

                                                        {
                                                            <>
                                                                <div className={styles.daystake}>
                                                                    <div className={styles.Remainingtake}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                                                                        <path d="M10 5V10L13.3333 11.6667" stroke="white" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                                                        <path d="M9.99984 18.3327C14.6022 18.3327 18.3332 14.6017 18.3332 9.99935C18.3332 5.39698 14.6022 1.66602 9.99984 1.66602C5.39746 1.66602 1.6665 5.39698 1.6665 9.99935C1.6665 14.6017 5.39746 18.3327 9.99984 18.3327Z" stroke="white" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                                                    </svg>Average Time Taken</div>
                                                                    <div className={styles.remaintitle}>{individualProjectData?.team_execution_overview?.avgTime} days</div>
                                                                </div>
                                                                <div className={styles.daystake}>
                                                                    <div className={styles.Remainingtake}><svg xmlns="http://www.w3.org/2000/svg" width="19" height="10" viewBox="0 0 19 10" fill="none">
                                                                        <path d="M17.4999 0.833984L10.4166 7.91732L6.24992 3.75065L0.833252 9.16732" stroke="white" stroke-width="1.66667" stroke-linecap="round" stroke-linejoin="round" />
                                                                    </svg>Estimated Time Remaining</div>
                                                                    <div className={styles.remaintitle}>{individualProjectData?.team_execution_overview?.estTime} days</div>
                                                                </div>
                                                            </>
                                                        }
                                                    </div>
                                                </div>
                                                <div className={styles.documentloader}>
                                                    {individualProjectData?.team_execution_overview?.files?.length > 0 && individualProjectData?.team_execution_overview?.files?.map((doc) => (
                                                        <div className={styles.documentblock} key={doc.uniqueid}>
                                                            <div className={styles.documenttype}>
                                                                <div className={styles.documentlist}>
                                                                    <div className={styles.documentlistleft}>
                                                                        <img src={doc.filetype == "pdf" ? Pdficon : Pdficon} alt="Pdficon" />
                                                                    </div>

                                                                    <div className={styles.officedetails}>
                                                                        <div className={styles.officedetailstitle}>
                                                                            {doc.fileName}
                                                                            {/* <span>{doc.lang}</span> */}
                                                                        </div>
                                                                        <div className={styles.officedetailssubtitle}>{doc.size}</div>
                                                                    </div>
                                                                </div>

                                                                <div className={`${styles.kebablist} activitydropdown`}>
                                                                    <Worktualdropdown options={dropdownPdf} triggerIcon={Kebab} />
                                                                </div>
                                                            </div>
                                                        </div>
                                                    ))}
                                                </div>
                                                {individualProjectData?.team_execution_overview?.refernceImg?.length > 0 &&
                                                    <div className={styles.imageuploader}>
                                                        <div className={styles.imagecontent}>

                                                            <img src={
                                                                individualProjectData?.team_execution_overview?.refernceImg ??
                                                                Imageattachment} alt="Imageattachment" />

                                                        </div>
                                                    </div>

                                                }
                                            </div>
                                        )}
                                    </div>
                                )
                            })
                        ) : (
                                    <div > <Nodata /></div>
                        )
                    }
                </div>
            </div>
        </>
    )
}
export default Contactproject;
