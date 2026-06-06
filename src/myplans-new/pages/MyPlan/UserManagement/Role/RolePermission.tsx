import React, { useState } from 'react';
import styles from '../../../../assets/styles/usermanagement.module.scss';
const Rolepermission = () => {
    // state to track if accordion is open
    const [isOpen, setIsOpen] = useState(false);
    const [isOpen1, setIsOpen1] = useState(false);
    const users = [
        {
            id: 1, name: 'Contact Centre number', role: 'Users: 01', content: 'Lorem ipsum dolor sit amet, consectetur  et velit interdum, ac aliquet odio mattis.'
        },
        {
            id: 2, name: 'Contact Centre number', role: 'Users: 01', content: 'Lorem ipsum dolor sit amet, consectetur  et velit interdum, ac aliquet odio mattis.'
        },
        {
            id: 3, name: 'Contact Centre number', role: 'Users: 01', content: 'Lorem ipsum dolor sit amet, consectetur  et velit interdum, ac aliquet odio mattis.',
        },
        {
            id: 3, name: 'Contact Centre number', role: 'Users: 01', content: 'Lorem ipsum dolor sit amet, consectetur  et velit interdum, ac aliquet odio mattis.',
        },
        {
            id: 3, name: 'Contact Centre number', role: 'Users: 01', content: 'Lorem ipsum dolor sit amet, consectetur  et velit interdum, ac aliquet odio mattis.',
        },
    ];
    const toggleAccordion = () => {
        setIsOpen(!isOpen);
    };
    const [checked, setChecked] = useState(false);

    const handleChange = () => setChecked(!checked);
    return (
        <>
            <div className={styles.rolesandpermission}>
                <div className={styles.headerolemission}>
                    <h3>Roles</h3>
                    <p>Create and Manage roles for the Worktual products</p>
                </div>
                <div className={styles.productdetails}>
                    <div className={styles.roleproduct}>
                        <p>Role Name</p>
                        <h3>Test 01</h3>
                    </div>
                    <div className={styles.roleproduct}>
                        <p>Product</p>
                        <h3>Campaign Management</h3>
                    </div>
                    <div className={styles.roleproduct}>
                        <p>Reporting to</p>
                        <h3>Admin</h3>
                    </div>
                    <div className={styles.roleproduct}>
                        <button onClick={() => setIsOpen1(true)}>Edit</button>
                    </div>
                    {/* {isOpen1 && (
                        <div className={styles.modaloverlay} onClick={() => setIsOpen(false)}>
                            <div
                                className={styles.modalcontent}
                                onClick={(e) => e.stopPropagation()} // prevent closing when clicking inside
                            >
                                <div className={styles.header}>
                                    <h3>Role</h3>
                                    <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                        <path d="M10.0603 9.00005L17.7007 1.35945C17.9937 1.06645 17.9937 0.591851 17.7007 0.298951C17.4077 0.00605148 16.9331 0.00595146 16.6402 0.298951L8.99975 7.93955L1.35935 0.298852C1.06635 0.00585157 0.591752 0.00585157 0.298852 0.298852C0.00595158 0.591852 0.00585157 1.06645 0.298852 1.35935L7.93925 8.99995L0.298852 16.6406C0.00585157 16.9335 0.00585157 17.4082 0.298852 17.7011C0.445352 17.8476 0.637251 17.9208 0.829151 17.9208C1.02105 17.9208 1.21295 17.8476 1.35945 17.7011L8.99985 10.0605L16.6403 17.7011C16.7868 17.8476 16.9787 17.9208 17.1706 17.9208C17.3625 17.9208 17.5544 17.8476 17.7009 17.7011C17.9939 17.4081 17.9939 16.9335 17.7009 16.6406L10.0603 9.00005Z" fill="#404040" />
                                    </svg></span>
                                </div>
                                <div className={styles.containerblockin}>
                                    <div className={styles.rolename}>
                                        <label>Role name<span>*</span></label>
                                        <input placeholder='Enter' />
                                    </div>
                                    <div className={styles.rolename}>
                                        <label>Reporting to<span>*</span></label>
                                        <input placeholder='Enter' />
                                    </div>
                                    <div className={styles.rolename}>
                                        <label>Role Description</label>
                                        <textarea placeholder='Enter Role description'></textarea>
                                    </div>
                                </div>
                                <div className={styles.btn}>
                                    <button className={styles.canelbtn}>Cancel</button>
                                    <button className={styles.confirmbtn}>Confirm</button>
                                </div>
                            </div>
                        </div>
                    )} */}

                    {/* {isOpen1 && (
                        <div className={styles.modaloverlay} onClick={() => setIsOpen(false)}>
                            <div
                                className={styles.modalcontent}
                                onClick={(e) => e.stopPropagation()} // prevent closing when clicking inside
                            >
                                <p className={styles.csvupload}>CSV file is validating</p>
                                <div className={styles.containerblockin}>
                                    <div className={styles.progressContainer}>
                                        <div
                                            className={styles.progressFill}
                                            style={{ width: `${onprogress}%` }}
                                        ></div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    )} */}
                    {/* {isOpen1 && (
                        <div className={styles.modaloverlay} onClick={() => setIsOpen(false)}>
                            <div
                                className={styles.modalcontent}
                                onClick={(e) => e.stopPropagation()} // prevent closing when clicking inside
                            >
                                <div className={styles.header}>
                                    <h3>CSV Validating</h3>
                                    <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                        <path d="M10.0603 9.00005L17.7007 1.35945C17.9937 1.06645 17.9937 0.591851 17.7007 0.298951C17.4077 0.00605148 16.9331 0.00595146 16.6402 0.298951L8.99975 7.93955L1.35935 0.298852C1.06635 0.00585157 0.591752 0.00585157 0.298852 0.298852C0.00595158 0.591852 0.00585157 1.06645 0.298852 1.35935L7.93925 8.99995L0.298852 16.6406C0.00585157 16.9335 0.00585157 17.4082 0.298852 17.7011C0.445352 17.8476 0.637251 17.9208 0.829151 17.9208C1.02105 17.9208 1.21295 17.8476 1.35945 17.7011L8.99985 10.0605L16.6403 17.7011C16.7868 17.8476 16.9787 17.9208 17.1706 17.9208C17.3625 17.9208 17.5544 17.8476 17.7009 17.7011C17.9939 17.4081 17.9939 16.9335 17.7009 16.6406L10.0603 9.00005Z" fill="#404040" />
                                    </svg></span>
                                </div>
                                <p className={styles.csvupload}>Unable to import 2 out of 100 user records.
                                    Do you want to continue importing the remaining 98 user records?</p>
                                <div className={styles.btn}>
                                    <button className={styles.canelbtn}>Cancel</button>
                                    <button className={styles.btndel}>Confirm</button>
                                </div>
                            </div>
                        </div>
                    )} */}
                    {isOpen1 && (
                        <div className={styles.modaloverlay} onClick={() => setIsOpen(false)}>
                            <div
                                className={styles.modalcontent}
                                onClick={(e) => e.stopPropagation()} // prevent closing when clicking inside
                            >
                                <div className={styles.header}>
                                    <h3>Delete</h3>
                                    <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                        <path d="M10.0603 9.00005L17.7007 1.35945C17.9937 1.06645 17.9937 0.591851 17.7007 0.298951C17.4077 0.00605148 16.9331 0.00595146 16.6402 0.298951L8.99975 7.93955L1.35935 0.298852C1.06635 0.00585157 0.591752 0.00585157 0.298852 0.298852C0.00595158 0.591852 0.00585157 1.06645 0.298852 1.35935L7.93925 8.99995L0.298852 16.6406C0.00585157 16.9335 0.00585157 17.4082 0.298852 17.7011C0.445352 17.8476 0.637251 17.9208 0.829151 17.9208C1.02105 17.9208 1.21295 17.8476 1.35945 17.7011L8.99985 10.0605L16.6403 17.7011C16.7868 17.8476 16.9787 17.9208 17.1706 17.9208C17.3625 17.9208 17.5544 17.8476 17.7009 17.7011C17.9939 17.4081 17.9939 16.9335 17.7009 16.6406L10.0603 9.00005Z" fill="#404040" />
                                    </svg></span>
                                </div>
                                <div className={styles.containerblockindel}>
                                    <p>Are you sure to delete the role 'Test 01'?</p>
                                </div>
                                <div className={styles.btn}>
                                    <button className={styles.canelbtn}>Cancel</button>
                                    <button className={styles.btndel}>Confirm</button>
                                </div>
                            </div>
                        </div>
                    )}
                    {/* {isOpen1 && (
                        <div className={styles.modaloverlay} onClick={() => setIsOpen(false)}>
                            <div
                                className={styles.modalcontent}
                                onClick={(e) => e.stopPropagation()} // prevent closing when clicking inside
                            >
                                <div className={styles.header}>
                                    <h3>Can’t Delete role</h3>
                                    <span><svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
                                        <path d="M10.0603 9.00005L17.7007 1.35945C17.9937 1.06645 17.9937 0.591851 17.7007 0.298951C17.4077 0.00605148 16.9331 0.00595146 16.6402 0.298951L8.99975 7.93955L1.35935 0.298852C1.06635 0.00585157 0.591752 0.00585157 0.298852 0.298852C0.00595158 0.591852 0.00585157 1.06645 0.298852 1.35935L7.93925 8.99995L0.298852 16.6406C0.00585157 16.9335 0.00585157 17.4082 0.298852 17.7011C0.445352 17.8476 0.637251 17.9208 0.829151 17.9208C1.02105 17.9208 1.21295 17.8476 1.35945 17.7011L8.99985 10.0605L16.6403 17.7011C16.7868 17.8476 16.9787 17.9208 17.1706 17.9208C17.3625 17.9208 17.5544 17.8476 17.7009 17.7011C17.9939 17.4081 17.9939 16.9335 17.7009 16.6406L10.0603 9.00005Z" fill="#404040" />
                                    </svg></span>
                                </div>
                                <div className={styles.containerblockindel}>
                                    <p>Users are mapped to this role and can't be deleted. Re-assign the users to another role to proceed</p>
                                </div>
                                <div className={styles.btn}>
                                    <button className={styles.confirmbtn}>Ok</button>
                                </div>
                            </div>
                        </div>
                    )} */}
                </div>
                <div className={styles.productdetailssearch}>
                    <input type="text" placeholder='search' />
                    <div className={styles.configurations}>
                        <ul>
                            <li>View</li>
                            <li>Create</li>
                            <li>Edit</li>
                        </ul>
                    </div>
                </div>
                <div className={styles.accordinblockcontainer}>
                    <div className={styles.blockcontainer}>
                        <div className={styles.accordinheader} onClick={toggleAccordion}>
                            <h3>Basic Configurations</h3>
                            <span className={`${styles.arrow} ${isOpen ? styles.open : ""}`}><svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
                                <path d="M6.00009 5.90231C5.39759 5.90231 4.83209 5.66791 4.40679 5.24121L0.553286 1.37691C0.260786 1.08391 0.261786 0.609307 0.554786 0.316407C0.848686 0.024407 1.32379 0.0244068 1.61579 0.318407L5.46879 4.18271C5.75249 4.46591 6.24809 4.46591 6.53079 4.18271L10.3843 0.318407C10.6763 0.0245068 11.1514 0.024507 11.4453 0.316407C11.7383 0.609407 11.7392 1.08401 11.4468 1.37691L7.59329 5.24121C7.16799 5.66801 6.60219 5.90231 6.00009 5.90231Z" fill="#404040" />
                            </svg></span>
                        </div>
                        {isOpen && (
                            <div className={styles.accordinblockset}>
                                {users.map(user => (
                                    <div className={styles.accordinwrap} key={user.id}>
                                        <div className={styles.accordinleft}>
                                            <h3>{user.name}</h3>
                                            <p>{user.content}</p>
                                        </div>
                                        <div className={styles.accordinright}>
                                            <label className={styles.checkboxContainer}>
                                                <input
                                                    type="checkbox"
                                                    checked={checked}
                                                    onChange={handleChange}
                                                />
                                                <span className={checked ? styles.checkmark + " " + styles.checked : styles.checkmark}></span>
                                            </label>
                                            <label className={styles.checkboxContainer}>
                                                <input
                                                    type="checkbox"
                                                    checked={checked}
                                                    onChange={handleChange}
                                                />
                                                <span className={checked ? styles.checkmark + " " + styles.checked : styles.checkmark}></span>
                                            </label>
                                            <label className={styles.checkboxContainer}>
                                                <input
                                                    type="checkbox"
                                                    checked={checked}
                                                    onChange={handleChange}
                                                />
                                                <span className={checked ? styles.checkmark + " " + styles.checked : styles.checkmark}></span>
                                            </label>
                                        </div>
                                    </div>
                                ))}
                            </div>
                        )}
                    </div>
                    <div className={styles.blockcontainer}>
                        <div className={styles.accordinheader}>
                            <h3>Agent Workspace</h3>
                            <span><svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
                                <path d="M6.00009 5.90231C5.39759 5.90231 4.83209 5.66791 4.40679 5.24121L0.553286 1.37691C0.260786 1.08391 0.261786 0.609307 0.554786 0.316407C0.848686 0.024407 1.32379 0.0244068 1.61579 0.318407L5.46879 4.18271C5.75249 4.46591 6.24809 4.46591 6.53079 4.18271L10.3843 0.318407C10.6763 0.0245068 11.1514 0.024507 11.4453 0.316407C11.7383 0.609407 11.7392 1.08401 11.4468 1.37691L7.59329 5.24121C7.16799 5.66801 6.60219 5.90231 6.00009 5.90231Z" fill="#404040" />
                            </svg></span>
                        </div>
                    </div>
                    <div className={styles.blockcontainer}>
                        <div className={styles.accordinheader}>
                            <h3>General Settings</h3>
                            <span><svg xmlns="http://www.w3.org/2000/svg" width="12" height="6" viewBox="0 0 12 6" fill="none">
                                <path d="M6.00009 5.90231C5.39759 5.90231 4.83209 5.66791 4.40679 5.24121L0.553286 1.37691C0.260786 1.08391 0.261786 0.609307 0.554786 0.316407C0.848686 0.024407 1.32379 0.0244068 1.61579 0.318407L5.46879 4.18271C5.75249 4.46591 6.24809 4.46591 6.53079 4.18271L10.3843 0.318407C10.6763 0.0245068 11.1514 0.024507 11.4453 0.316407C11.7383 0.609407 11.7392 1.08401 11.4468 1.37691L7.59329 5.24121C7.16799 5.66801 6.60219 5.90231 6.00009 5.90231Z" fill="#404040" />
                            </svg></span>
                        </div>
                    </div>
                </div>
            </div>
            <div className={styles.createbtn}>
                <button>Cancel</button>
                <button>Create</button>
            </div>
        </>
    )
}
export default Rolepermission
