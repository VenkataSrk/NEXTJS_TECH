import { useSelector } from "react-redux";
import styles from "./toast.module.scss";
import toastinfo from "../../../src/styles/images/toastinfoicon.svg"
import { useEffect } from "react";
import { useDispatcher } from "../../store/redux-store/dispatch";
import { UserSettingsActions } from "../../store/slice/settings_slice/user_settings_slice";
import store from "../../store/redux-store/root_store";

export const Toastbox = () => {
    const dispatch = useDispatcher()
    const notifications = useSelector((store: any) => store.settingsStore.notify)

    useEffect(() => {
        setTimeout(() => {
                dispatch(UserSettingsActions.removeNotification(notifications?.[0]?.id))
        }, notifications?.[0]?.time ?? 3000);
    }, [notifications])

    const removeNotification = (id: any) => {
        dispatch(UserSettingsActions.removeNotification(id))
    }


    return (<>
        <div className={styles.toastmessageArea}>
            
            {notifications?.map((item: any, i: any) => {
                if (item.type === 'error') {
                    return <div className={`${styles.toastmessage}`}>
                        <div className={`${styles.toasttxt}`}>
                            <div>
                                {/* <svg xmlns="http://www.w3.org/2000/svg" width="25" height="25" viewBox="0 0 25 25" fill="none">
                                    <path d="M12.5708 2.74219C18.0938 2.74219 22.5708 7.22019 22.5708 12.7422C22.5708 18.2642 18.0938 22.7422 12.5708 22.7422C7.0478 22.7422 2.5708 18.2642 2.5708 12.7422C2.5708 7.22019 7.0478 2.74219 12.5708 2.74219ZM12.5726 15.7459C12.0211 15.7459 11.5739 16.193 11.5739 16.7446C11.5739 17.2961 12.0211 17.7432 12.5726 17.7432C13.1241 17.7432 13.5713 17.2961 13.5713 16.7446C13.5713 16.193 13.1241 15.7459 12.5726 15.7459ZM12.5704 7.74219C12.0576 7.74237 11.6351 8.12856 11.5775 8.62592L11.5708 8.74255L11.5726 13.7434L11.5794 13.86C11.6373 14.3574 12.0601 14.7433 12.573 14.7431C13.0858 14.7429 13.5083 14.3567 13.5659 13.8593L13.5726 13.7427L13.5708 8.74183L13.564 8.62521C13.5061 8.12789 13.0833 7.742 12.5704 7.74219Z" fill="#DE003D" />
                                </svg> */}
                                {item.message}
                            </div>
                            <svg onClick={() => removeNotification(item.id)} xmlns="http://www.w3.org/2000/svg" width="11" height="11" viewBox="0 0 11 11" fill="none">
                                <path d="M6.30082 5.28575L10.2042 1.38208C10.4255 1.16135 10.4255 0.802017 10.2042 0.581283C9.98282 0.35995 9.62342 0.35995 9.40335 0.581283L5.50002 4.48488L1.59668 0.581283C1.37662 0.35995 1.01595 0.35995 0.795883 0.581283C0.574549 0.802017 0.574549 1.16135 0.795883 1.38208L4.69922 5.28575L0.795883 9.18942C0.574549 9.41015 0.574549 9.76948 0.795883 9.99022C0.906549 10.1009 1.05108 10.1562 1.19628 10.1562C1.34148 10.1562 1.48662 10.1009 1.59668 9.99022L5.50002 6.08662L9.40335 9.99022C9.51335 10.1009 9.65855 10.1562 9.80375 10.1562C9.94895 10.1562 10.0935 10.1009 10.2042 9.99022C10.4255 9.76948 10.4255 9.41015 10.2042 9.18942L6.30082 5.28575Z" fill="#fff" />
                            </svg>
                        </div>
                    </div>
                } else if (item.type === 'success') {
                    return <div className={styles.toastmessage}>
                        <div className={styles.toasttxt}>
                            <div>
                                {/* <svg xmlns="http://www.w3.org/2000/svg" width="25" height="25" viewBox="0 0 25 25" fill="none">
                                    <path d="M12.5 2.28564C18.0228 2.28564 22.5 6.7628 22.5 12.2856C22.5 17.8085 18.0228 22.2856 12.5 22.2856C6.97715 22.2856 2.5 17.8085 2.5 12.2856C2.5 6.7628 6.97715 2.28564 12.5 2.28564ZM15.7197 9.25531L11.25 13.725L9.28033 11.7553C8.98744 11.4624 8.51256 11.4624 8.21967 11.7553C7.92678 12.0482 7.92678 12.5231 8.21967 12.816L10.7197 15.316C11.0126 15.6089 11.4874 15.6089 11.7803 15.316L16.7803 10.316C17.0732 10.0231 17.0732 9.54821 16.7803 9.25531C16.4874 8.96242 16.0126 8.96242 15.7197 9.25531Z" fill="#237B4B" />
                                </svg> */}
                                {item.message}
                            </div>
                            <svg onClick={() => removeNotification(item.id)} xmlns="http://www.w3.org/2000/svg" width="11" height="11" viewBox="0 0 11 11" fill="none">
                                <path d="M6.30082 5.28575L10.2042 1.38208C10.4255 1.16135 10.4255 0.802017 10.2042 0.581283C9.98282 0.35995 9.62342 0.35995 9.40335 0.581283L5.50002 4.48488L1.59668 0.581283C1.37662 0.35995 1.01595 0.35995 0.795883 0.581283C0.574549 0.802017 0.574549 1.16135 0.795883 1.38208L4.69922 5.28575L0.795883 9.18942C0.574549 9.41015 0.574549 9.76948 0.795883 9.99022C0.906549 10.1009 1.05108 10.1562 1.19628 10.1562C1.34148 10.1562 1.48662 10.1009 1.59668 9.99022L5.50002 6.08662L9.40335 9.99022C9.51335 10.1009 9.65855 10.1562 9.80375 10.1562C9.94895 10.1562 10.0935 10.1009 10.2042 9.99022C10.4255 9.76948 10.4255 9.41015 10.2042 9.18942L6.30082 5.28575Z" fill="#fff" />
                            </svg>
                        </div>
                    </div>
                } else if (item.type === 'info') {
                    return <div className={`${styles.toastmessage} ${styles.toastmsginfo}`}>
                        <div className={`${styles.toasttxt} `}>
                            <div>
                                {/* <svg width="16" height="17" viewBox="0 0 16 17" fill="none" xmlns="http://www.w3.org/2000/svg">
                                        <g clip-path="url(#clip0_20051_22917)">
                                            <path d="M8.00065 1.83398C4.32065 1.83398 1.33398 4.82065 1.33398 8.50065C1.33398 12.1807 4.32065 15.1673 8.00065 15.1673C11.6807 15.1673 14.6673 12.1807 14.6673 8.50065C14.6673 4.82065 11.6807 1.83398 8.00065 1.83398ZM8.00065 5.78065C8.38065 5.78065 8.69398 6.08732 8.69398 6.47398C8.69398 6.86065 8.38732 7.16732 8.00065 7.16732C7.61398 7.16732 7.30732 6.86065 7.30732 6.47398C7.30732 6.08732 7.61398 5.78065 8.00065 5.78065ZM9.13398 10.6807C9.05398 10.894 8.62732 11.2007 8.13398 11.2207C7.64065 11.2407 7.02732 11.0273 7.09398 10.454C7.16065 9.88065 7.34732 8.96065 7.38065 8.75398C7.41398 8.54732 7.52732 8.14732 7.33398 8.10065C7.14065 8.05398 6.88065 8.07398 6.85398 7.90065C6.82732 7.76732 6.96065 7.67398 7.24065 7.66065C7.44732 7.64732 7.92065 7.64065 8.26065 7.72065C8.50065 7.78065 8.66732 8.00065 8.65398 8.25398C8.64065 8.44732 8.59398 8.69398 8.53398 8.97398C8.42732 9.45398 8.35398 9.95398 8.31398 10.194C8.27398 10.434 8.35398 10.8273 8.66065 10.674C8.96732 10.5207 9.24065 10.4207 9.14732 10.674L9.13398 10.6807Z" fill="black" />
                                        </g>
                                        <defs>
                                            <clipPath id="clip0_20051_22917">
                                                <rect width="16" height="16" fill="white" transform="translate(0 0.5)" />
                                            </clipPath>
                                        </defs>
                                    </svg> */}
                                {item?.message}
                            </div>
                            <svg onClick={() => removeNotification(item?.id)} xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 12 12" fill="none">
                                <path d="M6.70684 6.00003L11.8004 0.906301C11.9958 0.710968 11.9958 0.394568 11.8004 0.199301C11.6051 0.00403433 11.2887 0.00396764 11.0934 0.199301L5.99984 5.29303L0.906235 0.199234C0.710901 0.00390103 0.394501 0.00390103 0.199234 0.199234C0.00396771 0.394568 0.00390103 0.710968 0.199234 0.906234L5.29283 5.99997L0.199234 11.0937C0.00390103 11.289 0.00390103 11.6054 0.199234 11.8007C0.296901 11.8984 0.424834 11.9472 0.552768 11.9472C0.680701 11.9472 0.808634 11.8984 0.906301 11.8007L5.9999 6.70697L11.0935 11.8007C11.1912 11.8984 11.3191 11.9472 11.447 11.9472C11.575 11.9472 11.7029 11.8984 11.8006 11.8007C11.9959 11.6054 11.9959 11.289 11.8006 11.0937L6.70684 6.00003Z" fill="#D2D3D4" />
                            </svg>
                        </div>
                    </div>
                }
            })}
        </div>
    </>
    );
};

export const toastnotification = {
    success: (message: any, duration = 3) => store.dispatch(UserSettingsActions.setNotification({
        type: 'success',
        message: message || '',
        time: duration
    })),
    error: (message: any, duration = 3) => store.dispatch(UserSettingsActions.setNotification({
        type: 'error',
        message: message || '',
        time: duration
    })),
    info: (message: any, duration = 3) => store.dispatch(UserSettingsActions.setNotification({
        type: 'info',
        message: message || '',
        time: duration
    })),
}
