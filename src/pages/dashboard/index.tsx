import React, {  useState } from "react";
import styles from "./scss/dashboard.module.scss";
import greenarrow from "./images/greenarow.svg";
import redarrow from "./images/redarrow.svg";
import yellowarrow from "./images/yellowarrow.svg";
import customizeicon from "./images/customizebtn.svg";
import tooltipicon from "./images/tooltipicon.svg";
import {
    AreaChart,
    Area,
    BarChart,
    Bar,
    XAxis,
    YAxis,
    Tooltip,
    ResponsiveContainer,
    ReferenceLine,
    CartesianGrid,
} from "recharts";
import SetUpCommonEditor from "../shared/commonEditor";
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage";
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys";
import jwtDecode from "jwt-decode";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { useNavigate } from "react-router-dom";
import { v4 as uuidv4 } from "uuid";
import aiChatAssistSocketservice from "../../services/aiChatService";
import { contactDataSliceActions } from "../../store/slice/contactSlice";
import { commonDataSliceActions } from "../../store/slice/commonSilce/commonSlice";
import { stripHtmlTags } from "../../base/utils";


const databar = [
    { name: "Value", value: 40 },
    { name: "Value", value: 32 },
    { name: "Value", value: 48 },
    { name: "Value", value: 36 },
    { name: "Value", value: 28 },
    { name: "Value", value: 45 },
    { name: "Value", value: 38 },
];

const data = [
    { value: 20 },
    { value: 25 },
    { value: 40 },
    { value: 55 },
    { value: 60 },
    { value: 58 },
    { value: 65 },
    { value: 75 },
    { value: 90 },
];

const Dashboard = () => {
    const [open, setopen] = useState(false)
    const [openbigdrawer, setopenbigdrawer] = useState(false)
    const activeIndex = 5;

    const token = GetItemFromStorage(LocalStorage.ACCESS_TOKEN)
    const LoginCcaasUserDetails = GetItemFromStorage(LocalStorage.USER_DETAILS);

    const dispatch = useDispatcher()
    const navigate = useNavigate()

    const handleConvoStart = (value :any, files :any) => {
        const sessionId: any = uuidv4() + 'home';
        let body: any = {
          query: stripHtmlTags(value?.trim()),
          sessionId: sessionId,
          roleId: LoginCcaasUserDetails?.roleId?.toString() ?? "",
          org_role_id_crm: LoginCcaasUserDetails?.org_role_id_crm?.toString() ?? "",
          userId: `${LoginCcaasUserDetails?.userId || ''}`,
          domainId: `${LoginCcaasUserDetails?.domainId || ''}`,
          module: "",
          ext: `${LoginCcaasUserDetails?.ext || ''}`,
          agent: 'home',
          isBot: 0,
          files: files ?? []
        }

        aiChatAssistSocketservice.io.emit('crm_chat', body);
        dispatch(contactDataSliceActions.setcontactAiAgentChat({ ...body }))
        dispatch(contactDataSliceActions.setcontactAiAgentChatTyping({sessionId:sessionId,typing:true}));
        dispatch(commonDataSliceActions.setTabMenuPush({ path: `/home/chat-conversation?sessionId=${sessionId}`, menuName: "New Conversation", close: 1, type: "contact" }));
        navigate(`/home/chat-conversation?sessionId=${sessionId}`);
      }


    return (
        <>
        <div>

       
            <div className={`${styles.dashboard} maincontainer`}>
                {/* Top Stats */}
                <div className={styles.stswrapperwithbtn}>


                    <div className={styles.stats}>

                        <div className={styles.stscard} >
                            <p onClick={() => setopenbigdrawer(!openbigdrawer)} className={styles.label}>{"Overall Calls"}</p>
                            <div className={styles.valuendsts}>

                                <div className={styles.value}>44</div>
                                <div className={styles.status}>+2</div><span><img src={greenarrow} /></span>
                            </div>
                        </div>
                        <div className={styles.stscard} >
                            <p onClick={() => setopenbigdrawer(!openbigdrawer)} className={styles.label}>{"Overall Calls"}</p>
                            <div className={styles.valuendsts}>

                                <div className={styles.value}>44</div>
                                <div className={styles.status}>+2</div><span><img src={greenarrow} /></span>
                            </div>
                        </div>
                        <div className={styles.stscard} >
                            <p onClick={() => setopenbigdrawer(!openbigdrawer)} className={styles.label}>{"Overall Calls"}</p>
                            <div className={styles.valuendsts}>

                                <div className={styles.value}>44</div>
                                <div className={`${styles.status} ${styles.red}`}>+2</div><span><img src={redarrow} /></span>
                            </div>
                        </div>
                        <div className={styles.stscard} >
                            <p onClick={() => setopenbigdrawer(!openbigdrawer)} className={styles.label}>{"Overall Calls"}</p>
                            <div className={styles.valuendsts}>

                                <div className={styles.value}>44</div>
                                <div className={`${styles.status} ${styles.yellow}`}>+2</div><span><img src={yellowarrow} /></span>
                            </div>
                        </div>
                        <div className={styles.stscard} >
                            <p onClick={() => setopenbigdrawer(!openbigdrawer)} className={styles.label}>{"Overall Calls"}</p>
                            <div className={styles.valuendsts}>

                                <div className={styles.value}>44</div>
                                <div className={styles.status}>+2</div><span><img src={greenarrow} /></span>
                            </div>
                        </div>
                        <div className={styles.stscard} >
                            <p onClick={() => setopenbigdrawer(!openbigdrawer)} className={styles.label}>{"Overall Calls"}</p>
                            <div className={styles.valuendsts}>

                                <div className={styles.value}>44</div>
                                <div className={styles.status}>+2</div><span><img src={greenarrow} /></span>
                            </div>
                        </div>




                    </div>
                    <div onClick={() => setopen(!open)} className={styles.customizeicon}>
                        <img src={customizeicon} />
                    </div>
                </div>

                {/* Main Content */}
                <div className={styles.content}>
                    {/* Left Chart */}
                    <div>
                        <div className={`subtitle ${styles.subtitletotal} `}>Total Interactions (Volume) </div>
                        <div className={styles.chartcard}>
                            <div >
                                <ResponsiveContainer width="100%" height={220}>
                                    <AreaChart data={data}>
                                        {/* Gradient */}
                                        <defs>
                                            <linearGradient id="greenGradient" x1="0" y1="0" x2="0" y2="1">
                                                <stop offset="0%" stopColor="#2e7d32" stopOpacity={0.5} />
                                                <stop offset="100%" stopColor="#2e7d32" stopOpacity={0.05} />
                                            </linearGradient>
                                        </defs>

                                        <XAxis hide />
                                        <YAxis hide />
                                        <Tooltip />

                                        {/* Vertical reference line */}
                                        <ReferenceLine
                                            x={activeIndex}
                                            stroke="#4caf50"
                                            strokeDasharray="3 3"
                                        />

                                        {/* Area */}
                                        <Area
                                            type="monotone"
                                            dataKey="value"
                                            stroke="#2e7d32"
                                            strokeWidth={2}
                                            fill="url(#greenGradient)"
                                            dot={false}
                                            activeDot={{
                                                r: 6,
                                                fill: "#2e7d32",
                                                stroke: "#2e7d32",
                                            }}
                                        />
                                    </AreaChart>
                                </ResponsiveContainer>
                            </div>
                            <div className={styles.contentunderchart}>
                                <p className="mb16">Definition: The aggregate measure of customer demand and system load across all channels. </p>
                                <span className={styles.positive}>+12.4%</span>
                            </div>

                        </div>
                    </div>


                    {/* Right Metrics */}
                    <div className={styles.metrics}>

                        <div className={styles.metriccard} >
                            <div className={styles.top}>
                                <h5>{"Inbound Interactions"}</h5>
                                <div className={styles.metricvalue}>
                                    <strong>{"170%"}</strong>
                                    <span className={styles.positive}>{"+2.1%"}</span> vs.last period <span><img src={tooltipicon} /></span>
                                </div>
                            </div>

                            <p >
                                Metrics insights (max 200 characters) summarise performance.
                            </p>
                        </div>
                        <div className={styles.metriccard} >
                            <div className={styles.top}>
                                <h5>{"Inbound Interactions"}</h5>
                                <div className={styles.metricvalue}>
                                    <strong>{"170%"}</strong>
                                    <span className={styles.positive}>{"+2.1%"}</span> vs.last period <span><img src={tooltipicon} /></span>
                                </div>
                            </div>

                            <p >
                                Metrics insights (max 200 characters) summarise performance.
                            </p>
                        </div>
                        <div className={styles.metriccard} >
                            <div className={styles.top}>
                                <h5>{"Inbound Interactions"}</h5>
                                <div className={styles.metricvalue}>
                                    <strong>{"170%"}</strong>
                                    <span className={styles.positive}>{"+2.1%"}</span> vs.last period <span><img src={tooltipicon} /></span>
                                </div>
                            </div>

                            <p >
                                Metrics insights (max 200 characters) summarise performance.
                            </p>
                        </div>
                        <div className={styles.metriccard} >
                            <div className={styles.top}>
                                <h5>{"Inbound Interactions"}</h5>
                                <div className={styles.metricvalue}>
                                    <strong>{"170%"}</strong>
                                    <span className={styles.positive}>{"+2.1%"}</span> vs.last period <span><img src={tooltipicon} /></span>
                                </div>
                            </div>

                            <p >
                                Metrics insights (max 200 characters) summarise performance.
                            </p>
                        </div>
                        <div className={styles.metriccard} >
                            <div className={styles.top}>
                                <h5>{"Inbound Interactions"}</h5>
                                <div className={styles.metricvalue}>
                                    <strong>{"170%"}</strong>
                                    <span className={styles.positive}>{"+2.1%"}</span> vs.last period <span><img src={tooltipicon} /></span>
                                </div>
                            </div>

                            <p >
                                Metrics insights (max 200 characters) summarise performance.
                            </p>
                        </div>


                    </div>
                </div>

            
            </div>
            {
                openbigdrawer &&
                 <div className={styles.drawer}>
                    <div className={styles.header}>
                        <div className={styles.lefts}>
                            <div className={styles.title}>
                                Inbound Interactions
                            </div>
                            <div className={styles.desc}>

                                Inbound Interactions microcopy
                            </div>

                        </div>
                        <div onClick={() => setopenbigdrawer(!openbigdrawer)} className={styles.cancelbtn}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                                <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#2D2B2F" />
                            </svg>
                        </div>
                    </div>
                    <div className={styles.body}>
                        <div className={styles.commonbox}>
                            <div className={styles.headerr}>
                                <div className={styles.heading}>Title of the visual chart</div>
                                <p>Brief explanation of what this chart is about</p>
                            </div>
                            <ResponsiveContainer width="100%" height={220}>
                                <BarChart data={databar} barCategoryGap={20}>
                                    <CartesianGrid
                                        strokeDasharray="3 3"
                                        vertical={false}
                                    />
                                    <XAxis
                                        dataKey="name"
                                        tickLine={false}
                                        axisLine={false}
                                    />
                                    <YAxis
                                        tickLine={false}
                                        axisLine={false}
                                        tickFormatter={() => "value"}
                                    />
                                    <Bar
                                        dataKey="value"
                                        fill="#5B61F6"
                                        radius={[6, 6, 0, 0]}
                                    />
                                </BarChart>
                            </ResponsiveContainer>
                        </div>
                        <div className={styles.titleofcontext}>
                            <div className={styles.titlewithchevron}>
                                <h4>Title of the context</h4> <span><svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
                                    <path d="M10.5809 5.8065C10.3885 5.8065 10.1961 5.7333 10.0497 5.5858L6.19621 1.7206C6.05461 1.579 5.86611 1.5009 5.66541 1.4999C5.46471 1.4999 5.27581 1.579 5.13371 1.7206L1.28121 5.5858C0.989214 5.8797 0.513614 5.8797 0.220714 5.5878C-0.0727862 5.2948 -0.0736861 4.8202 0.218714 4.5273L4.07171 0.662101C4.49751 0.235301 5.06341 0 5.66551 0C6.26761 0.001 6.83351 0.235401 7.25881 0.662101L11.1123 4.5273C11.4048 4.8203 11.4038 5.2949 11.1103 5.5878C10.9643 5.7333 10.7723 5.8065 10.5809 5.8065Z" fill="#2D2B2F" />
                                </svg></span>
                            </div>

                            <p>Title of the context frames the core idea, setting scope and intent. It guides readers, aligns expectations, and provides clarity about purpose, relevance, and direction of the discussion. overall. up</p>
                        </div>
                    </div>


                </div>
            }

            {
                open && <div className={`${styles.drawer} ${styles.drawersmall}  `}>
                    <div className={styles.header}>
                        <div className={styles.lefts}>
                            <div className={styles.title}>
                                Customize
                            </div>
                            <div className={styles.desc}>
                                Choose the metrics you want to display on your dashboard.
                            </div>

                        </div>
                        <div onClick={() => setopen(!open)} className={styles.cancelbtn}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="15" height="15" viewBox="0 0 15 15" fill="none">
                                <path d="M8.5064 7.3057L14.3614 1.4502C14.6934 1.1191 14.6934 0.5801 14.3614 0.249C14.0294 -0.083 13.4903 -0.083 13.1602 0.249L7.3052 6.1044L1.4502 0.249C1.1201 -0.083 0.5791 -0.083 0.249 0.249C-0.083 0.5801 -0.083 1.1191 0.249 1.4502L6.104 7.3057L0.249 13.1612C-0.083 13.4923 -0.083 14.0313 0.249 14.3624C0.415 14.5284 0.6318 14.6114 0.8496 14.6114C1.0674 14.6114 1.2851 14.5284 1.4502 14.3624L7.3052 8.507L13.1602 14.3624C13.3252 14.5284 13.543 14.6114 13.7608 14.6114C13.9786 14.6114 14.1954 14.5284 14.3614 14.3624C14.6934 14.0313 14.6934 13.4923 14.3614 13.1612L8.5064 7.3057Z" fill="#2D2B2F" />
                            </svg>
                        </div>
                    </div>
                    <div className={styles.body}>
                        <div className={styles.checkboxblock}>
                            <input     className="custom-checkbox" type="checkbox" id="subscribe" name="subscribe" value="newsletter" />
                            <label htmlFor="subscribe">Overall Calls</label>
                        </div>
                        <div className={styles.checkboxblock}>
                            <input      className="custom-checkbox" type="checkbox" id="subscribe" name="subscribe" value="newsletter" />
                            <label htmlFor="subscribe">Overall Calls</label>
                        </div>
                        <div className={styles.checkboxblock}>
                            <input     className="custom-checkbox" type="checkbox" id="subscribe" name="subscribe" value="newsletter" />
                            <label htmlFor="subscribe">Overall Calls</label>
                        </div>
                        <div className={styles.checkboxblock}>
                            <input     className="custom-checkbox"  type="checkbox" id="subscribe" name="subscribe" value="newsletter" />
                            <label htmlFor="subscribe">Overall Calls</label>
                        </div>
                        <div className={styles.btncontainer}>
                            <button>Add</button>
                            <button className="secondarybtn">Add</button>
                        </div>
                    </div>


                </div>
            }


             </div>
              {/* <div className={`${styles.editorbottom} maincontainer`}>
                <SetUpCommonEditor onChange={handleConvoStart}/>
              </div> */}
        </>
    );
};

export default Dashboard;
