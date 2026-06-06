import React from 'react';
// import styles from '../../../../assets/styles/user.module.scss';
import styles from '../../../../assets/styles/usermanagement.module.scss';
const Adduser = () => {
    return (
        <>
            <div className={styles.addusercontainer}>
                <div className={styles.breadcrumb}>
                    <p>User <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                        <path d="M0.847363 11.6653C0.654963 11.6653 0.462564 11.5921 0.316164 11.4446C0.0236636 11.1516 0.0246635 10.677 0.318163 10.3841L4.18246 6.53165C4.32456 6.39005 4.40266 6.20055 4.40266 5.99945C4.40266 5.79925 4.32456 5.61075 4.18246 5.46915L0.317764 1.61565C0.0247639 1.32265 0.0238637 0.848048 0.316264 0.555148C0.608264 0.261248 1.08336 0.261248 1.37726 0.553148L5.24156 4.40665C5.66736 4.83145 5.90216 5.39685 5.90266 5.99945C5.90316 6.60205 5.66826 7.16745 5.24156 7.59415L1.37686 11.4466C1.23086 11.5921 1.03876 11.6653 0.847363 11.6653Z" fill="black" />
                    </svg></p>
                    <h3>Add User</h3>
                </div>
                <div className={styles.headerole}>
                    <h3>Invite people to Worktual</h3>
                    <p>Create and Manage users of your Worktual account</p>
                </div>
                <div className={styles.createroleblock}>
                    <div className={styles.rolename}>
                        <label>Email Address<span>*</span></label>
                        <p className={styles.emailadres}>Separate emails using a comma. Note, we can’t send invitations to distribution lists.</p>
                        <div style={{position: 'relative'}}>
                        <input type="text" placeholder='Enter Role name' />
                        <span className={styles.clrole}><svg xmlns="http://www.w3.org/2000/svg" width="20" height="20" viewBox="0 0 20 20" fill="none">
                            <path d="M11.0602 10.0001L14.3812 6.67922C14.674 6.38635 14.674 5.91194 14.3812 5.61917C14.0883 5.32639 13.6139 5.32629 13.3211 5.61917L10.0002 8.94008L6.67927 5.61917C6.38639 5.32629 5.91199 5.32629 5.61921 5.61917C5.32643 5.91204 5.32633 6.38645 5.61921 6.67922L8.94013 10.0001L5.61921 13.3211C5.32633 13.6139 5.32633 14.0883 5.61921 14.3811C5.76565 14.5276 5.95747 14.6007 6.14929 14.6007C6.34111 14.6007 6.53293 14.5276 6.67937 14.3811L10.0003 11.0602L13.3212 14.3811C13.4676 14.5276 13.6595 14.6007 13.8513 14.6007C14.0431 14.6007 14.2349 14.5276 14.3814 14.3811C14.6742 14.0882 14.6742 13.6138 14.3814 13.3211L11.0602 10.0001Z" fill="#757676" />
                            <path d="M9.99999 1.50328C14.6849 1.50328 18.4963 5.31479 18.4963 9.99974C18.4963 14.6847 14.6849 18.4962 9.99999 18.4962C5.31504 18.4962 1.50353 14.6847 1.50353 9.99974C1.50353 5.31479 5.31514 1.50328 9.99999 1.50328ZM9.99999 0.00390625C4.47939 0.00390625 0.00415039 4.47924 0.00415039 9.99974C0.00415039 15.5202 4.47939 19.9956 9.99999 19.9956C15.5206 19.9956 19.9957 15.5202 19.9957 9.99974C19.9957 4.47924 15.5206 0.00390625 9.99999 0.00390625Z" fill="#757676" />
                        </svg></span>
                        </div>
                    </div>
                    <div className={styles.rolename}>
                        <label>Select Role<span>*</span></label>
                        <input type="text" placeholder='Enter Role name' />
                    </div>
                    <div className={styles.btnblock}>
                        <button>Cancel</button>
                        <button>Invite User</button>
                    </div>
                </div>
            </div>
        </>
    )
}
export default Adduser
