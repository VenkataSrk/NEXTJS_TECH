import { useSelector } from "react-redux"
import styles from "../project/scss/projectdrawer.module.scss"
import { formatDate, formatDateTimeLable } from "../../base/utils"
import deleteIcon from "../../assets/images/inbox/deletered.svg"
import { useMemo, useState } from "react"
import { deleteAIChathistroyData, getAIChathistroyData } from "../../base/service/core_serviceApi"
import SkeletonBox from "../marketing/skeletonbox"
import { toastnotification } from "../../base/shared/toastmsg"
import { inboxDataSliceActions } from "../../store/slice/inbox/inboxSlice"
import { useDispatcher } from "../../store/redux-store/dispatch"
import { GetItemFromStorage } from "../../base/custom_hooks/useStorage"
import { LocalStorage } from "../../base/custom_hooks/localStorageKeys"
import Nodata from "../component/nodata"
import Modal from "../component/modal/modal"

const InboxHistory = ({ getInitialHistory, goBack }: any) => {

    const { aiChatHistoryData, inboxChatSessionId } = useSelector((state: any) => state.inboxChatStore)
    const [search, setsearch] = useState("")
    const [loading, setloading] = useState(false)
    const dispatch = useDispatcher()
    const LoginCcaasUserDetails: any = GetItemFromStorage(LocalStorage.USER_DETAILS);
    const [openMoveTrashDelete, setOpenMoveTrashDelete] = useState({
        open: false,
        data: {}
    })

    const filteredData = useMemo(() => {
        if (search) {
            return aiChatHistoryData.filter((item: any) => item?.title?.toLowerCase().includes(search.toLowerCase()))
        }
        return aiChatHistoryData
    }, [aiChatHistoryData, search])

    const deleteChat = async (item: any) => {
        try {
            const body = {
                sessionId: item?.session_id
            }
            setloading(true)
            const res = await deleteAIChathistroyData(body)
            if (res?.statusCode === 200) {
                await getInitialHistory()
                if (inboxChatSessionId === item?.session_id) {
                    dispatch(inboxDataSliceActions.resetInboxChat());
                }
            } else {
                toastnotification.error('Something went wrong')
            }
            setloading(false)
        } catch (error) {
            console.log(error)
        }
    }

    const openChat = async (item: any) => {
        const body = {
            sessionId: item?.session_id,
            agentId: LoginCcaasUserDetails?.userId ?? null,
        };
        setloading(true)
        await getAIChathistroyData(body)
            .then((res: any) => {
                const data: any = res?.[0]?.history_message ?? [];
                dispatch(inboxDataSliceActions.setInboxAichatMessageOld({
                    data,
                    sessionId: res?.[0]?.session_id
                }));
                goBack?.();
            })
        setloading(false)

    }

    return (<>

        <div className={`${styles.drawer} ${styles.drawersmall} ${styles.aichatdrawer}`}>
            <div className={`${styles.body} ${styles.bodyeditor}`}>
                <div style={{ height: "100%" }} className={`${styles.chatlist} ${styles.chatlistmaincontainer}`}>
                    <div className={`${styles.instructcenter}`}>
                        <div className={`${styles.chatlist} maincontainer`} style={{ minHeight: "300px" }}>
                            <div className={styles.Historyinterface}>
                                {loading && (<SkeletonBox count={3} />)}
                                {(!loading && aiChatHistoryData?.length > 0) && (
                                    <>
                                        <div className={styles.inputBlock}>
                                            <svg width="15" height="15" viewBox="0 0 15 15" fill="none" className={styles.searchIcon}>
                                                <circle cx="6.5" cy="6.5" r="4.5" stroke="currentColor" strokeWidth="1.3" />
                                                <line x1="10" y1="10" x2="13.5" y2="13.5" stroke="currentColor" strokeWidth="1.3" strokeLinecap="round" />
                                            </svg>
                                            <input type="text" placeholder="Search" onChange={(e) => setsearch(e.target.value)} />
                                        </div>
                                        <div className={styles.historylists}>
                                            {filteredData?.length === 0 && <Nodata content="No history found" />}

                                            {filteredData.map((item: any, index: number) => {
                                                return (
                                                    <div className={styles.list}>
                                                        <div className={`${styles.lefts} truncate1`} onClick={() => openChat(item)}>
                                                            {item?.title || "New Conversation"}
                                                        </div>
                                                        <div className={styles.rights}>
                                                            {formatDateTimeLable(item?.created_at)}
                                                            <div className={styles.delicon} onClick={() => setOpenMoveTrashDelete({
                                                                open: true,
                                                                data: item
                                                            })}>
                                                                <img src={deleteIcon} alt="del" />
                                                            </div>
                                                        </div>
                                                    </div>)
                                            }
                                            )}
                                        </div>
                                    </>
                                )}
                                {(filteredData?.length === 0 || aiChatHistoryData?.length === 0) 
                                    && <Nodata content="No history found" />}
                            </div>

                        </div>

                    </div>
                </div>
            </div>


        </div>

        {openMoveTrashDelete?.open && <Modal
            header={<div className={"modalheader"}><div>Delete</div></div>}
            children={<div className={"modalcontent"}>Are you sure you want to Delete this conversation?</div>}
            btntxtsecondary={"Cancel"}
            btntxtprimary={"Move to Trash"}
            onclosePrimary={() => {
                deleteChat(openMoveTrashDelete?.data)
                setOpenMoveTrashDelete({
                    open: false,
                    data: {}
                });
            }}
            oncloseSecondary={() => {
                setOpenMoveTrashDelete({
                    open: false,
                    data: {}
                });
            }}
        />}
    </>)
}

export default InboxHistory
