
import { assets } from "../../../../styles/assets/assets";
import styles from "../../../assets/styles/accountinfo.module.scss";
// import editicon from "../../../assets/images/editicon.svg";
// import Planslogo from '../../../../MyPlan/assets/images/myplan/planslogo.svg'
import CustomSwitch from "../../../components/CustomSwitch";
export default function Useradminrolesteams() {

    return (
       <div className={styles.infocontainer}>
            <div className={styles.yourinfo}>
                <div className={styles.titleblock}>
                <div className={styles.breadcrumb}>
                        <p>User <svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                            <path d="M0.847363 11.6653C0.654963 11.6653 0.462564 11.5921 0.316164 11.4446C0.0236636 11.1516 0.0246635 10.677 0.318163 10.3841L4.18246 6.53165C4.32456 6.39005 4.40266 6.20055 4.40266 5.99945C4.40266 5.79925 4.32456 5.61075 4.18246 5.46915L0.317764 1.61565C0.0247639 1.32265 0.0238637 0.848048 0.316264 0.555148C0.608264 0.261248 1.08336 0.261248 1.37726 0.553148L5.24156 4.40665C5.66736 4.83145 5.90216 5.39685 5.90266 5.99945C5.90316 6.60205 5.66826 7.16745 5.24156 7.59415L1.37686 11.4466C1.23086 11.5921 1.03876 11.6653 0.847363 11.6653Z" fill="black" />
                        </svg></p>
                        <h3>Charlotte Linlin</h3>
                </div>
                </div>
                <div className={`${styles.yourinfotop}`}>
                    <div className={styles.profile}>
                        <div className={`${styles.profileinner} ${styles.profileinnerpart}`}>
                        <div className={styles.profileleft}>
                            {/* <img src={Planslogo} alt="" /> */}
                        </div>
                        <div className={styles.profileright}>
                            <div className={styles.agentmanage}>Agent</div>
                        </div>
                        </div>
                        <div className={styles.activeprofile}>Active</div>
                    </div>
                    <div className={`${styles.fieldareamain} ${styles.fieldareamainrst}`}>
                        <div className={styles.fieldmain}>
                            <div className={styles.fieldleft}>
                                <h3>Full Name</h3>
                                <p>Sep 10, 2025</p>
                            </div>
                            <div className={styles.fieldleft}>
                                <h3>Sophie Reed</h3>
                                <p>Sep 10, 2025</p>
                            </div>
                        </div>
                        <div className={styles.fieldmain}>
                            <div className={styles.fieldleft}>
                                <h3>Full Name</h3>
                                <p>Sep 10, 2025</p>
                            </div>
                            <div className={styles.fieldleft}>
                                <h3>Sophie Reed</h3>
                                <p>Sep 10, 2025</p>
                            </div>
                        </div>
                    </div>
                </div>
                   <div className={styles.yourinfotop}>
                    <div className={styles.yourtitle}>Team Information</div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Team Category</div>
                        <div className={styles.value}>fdsfsdfsdfds</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Reporting To</div>
                        <div className={styles.value}>gfgdf</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Last Reviewed</div>
                        <div className={styles.value}>dfsfdsfsf</div>
                        <div className={styles.icon}></div>
                    </div>
                    <div className={styles.fieldareamain}>
                        <div className={styles.label}>Created date</div>
                        <div className={styles.value}>fdfdsfsdfd</div>
                        <div className={styles.icon}></div>
                    </div>
                </div>
                <div className={`${styles.iconaccess}`}>
                    <div className={`${styles.accesspermisson}  ${styles.iconaccessheader}`}>
                        <div className={styles.accesspert}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="23" height="15" viewBox="0 0 23 15" fill="none">
                                <path d="M10.4258 14.9248H20.8516C21.5046 14.9248 21.9639 14.8307 22.2295 14.6426C22.4951 14.46 22.6279 14.186 22.6279 13.8208C22.6279 13.2896 22.4647 12.7279 22.1382 12.1357C21.8172 11.5436 21.3524 10.9902 20.7437 10.4756C20.1405 9.9554 19.4072 9.53483 18.5439 9.21387C17.6862 8.88737 16.7178 8.72412 15.6387 8.72412C14.5651 8.72412 13.5967 8.88737 12.7334 9.21387C11.8757 9.53483 11.1424 9.9554 10.5337 10.4756C9.92497 10.9902 9.46012 11.5436 9.13916 12.1357C8.8182 12.7279 8.65771 13.2896 8.65771 13.8208C8.65771 14.186 8.78776 14.46 9.04785 14.6426C9.31348 14.8307 9.77279 14.9248 10.4258 14.9248ZM15.647 7.23828C16.2391 7.23828 16.7869 7.0778 17.2905 6.75684C17.7941 6.43034 18.1981 5.9904 18.5024 5.43701C18.8068 4.88363 18.959 4.26107 18.959 3.56934C18.959 2.88867 18.8068 2.27995 18.5024 1.74316C18.1981 1.20085 17.7941 0.77474 17.2905 0.464844C16.7869 0.154948 16.2391 0 15.647 0C15.0549 0 14.507 0.157715 14.0034 0.473145C13.4998 0.788574 13.0931 1.21745 12.7832 1.75977C12.4788 2.30208 12.3267 2.91081 12.3267 3.58594C12.3267 4.27214 12.4788 4.89193 12.7832 5.44531C13.0876 5.99316 13.4915 6.43034 13.9951 6.75684C14.4987 7.0778 15.0493 7.23828 15.647 7.23828ZM1.46094 14.9248H7.76123C7.54541 14.6094 7.44303 14.2303 7.4541 13.7876C7.46517 13.3449 7.56755 12.88 7.76123 12.3931C7.95492 11.9061 8.22054 11.4357 8.55811 10.9819C8.9012 10.5226 9.29688 10.1187 9.74512 9.77002C9.28027 9.46012 8.74626 9.20557 8.14307 9.00635C7.54541 8.80159 6.85921 8.69922 6.08447 8.69922C5.14925 8.69922 4.30534 8.85417 3.55273 9.16406C2.80566 9.46842 2.1665 9.8724 1.63525 10.376C1.10954 10.874 0.705566 11.4191 0.42334 12.0112C0.141113 12.6034 0 13.1872 0 13.7627C0 14.1335 0.110677 14.4185 0.332031 14.6177C0.558919 14.8224 0.935221 14.9248 1.46094 14.9248ZM6.08447 7.4292C6.60465 7.4292 7.08333 7.28809 7.52051 7.00586C7.95768 6.72363 8.30908 6.3418 8.57471 5.86035C8.84033 5.37337 8.97314 4.83105 8.97314 4.2334C8.97314 3.63574 8.84033 3.10449 8.57471 2.63965C8.30908 2.16927 7.95492 1.80127 7.51221 1.53564C7.07503 1.26449 6.59912 1.12891 6.08447 1.12891C5.57536 1.12891 5.09945 1.26725 4.65674 1.54395C4.21956 1.8151 3.8654 2.18587 3.59424 2.65625C3.32308 3.12663 3.19027 3.65788 3.1958 4.25C3.1958 4.84212 3.32861 5.37891 3.59424 5.86035C3.85986 6.3418 4.21126 6.72363 4.64844 7.00586C5.09115 7.28809 5.56982 7.4292 6.08447 7.4292Z" fill="black" />
                            </svg>
                            <div className={styles.featuresa}>
                                <h3>Members with this Role</h3>
                                <p>This role contributes to ongoing operations across the organization.</p>
                            </div>
                        </div>
                        <div className={styles.featurenet}>
                            <p>View All Permission<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                                <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#6B46C1" />
                            </svg></p>
                        </div>
                    </div>
                    <div className={styles.accesspermisson}>
                        <div className={styles.accesspert}>
                            <span className={styles.profileic}>J</span>
                            <div className={styles.featuresa}>
                                <h3>Access & Permissions</h3>
                                <p>Manage feature access and privileges</p>
                            </div>
                        </div>
                        <div className={styles.featurenet}>
                            <div className={styles.assignedactive}>
                                <p>Assigned</p>
                                <p>Sep 12, 2025</p>
                            </div>
                            <span>Active</span>
                        </div>
                    </div>
                    </div>
                    <div className={`${styles.iconaccess}`}>
                    <div className={`${styles.accesspermisson}  ${styles.iconaccessheader}`}>
                        <div className={styles.accesspert}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="15" height="18" viewBox="0 0 15 18" fill="none">
                                <path d="M6.30228 11.8052C6.01128 11.8052 5.73198 11.688 5.52787 11.4771L4.12167 10.0289C3.89117 9.79163 3.89707 9.41173 4.13437 9.18033C4.37067 8.94983 4.75058 8.95573 4.98298 9.19303L6.29157 10.5407L9.46447 6.40983C9.66657 6.14523 10.0446 6.09833 10.3053 6.29943C10.568 6.50063 10.6178 6.87753 10.4157 7.14023L7.15688 11.3834C6.96738 11.6314 6.67838 11.7857 6.36488 11.8033C6.34438 11.8053 6.32278 11.8052 6.30228 11.8052Z" fill="black" />
                                <path d="M7.24558 17.9809C7.17038 17.9809 7.09518 17.9692 7.02288 17.9467C-0.290625 15.6772 -0.00152474 9.01902 0.00237526 8.95262L0.00137533 2.93993C0.00137533 2.38423 0.386175 1.89692 0.935975 1.75442L7.04147 0.027825C7.17427 -0.009275 7.31687 -0.009275 7.44968 0.027825L13.5718 1.75922C14.106 1.89592 14.4907 2.38323 14.4907 2.93993V8.99462C14.4936 9.01902 14.7827 15.6772 7.46818 17.9467C7.39588 17.9692 7.32078 17.9809 7.24558 17.9809ZM1.50148 3.15382V8.99462C1.48978 9.25732 1.30128 14.4702 7.24558 16.4428C13.1909 14.4701 13.0024 9.25822 12.9917 9.03462L12.9907 3.15373L7.24558 1.52873L1.50148 3.15382ZM13.1704 3.20463C13.1743 3.20563 13.1772 3.20663 13.1811 3.20753L13.1704 3.20463ZM1.33538 3.20073L1.32758 3.20272C1.33048 3.20173 1.33348 3.20173 1.33538 3.20073Z" fill="black" />
                            </svg>
                            <div className={styles.featuresa}>
                                <h3>Permissions by Module</h3>
                                <p>Review or modify what this role can access</p>
                            </div>
                        </div>
                        <div className={styles.featurenet}>
                            <p>View All Permission<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                                <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#6B46C1" />
                            </svg></p>
                        </div>
                    </div>
                    <div className={`${styles.accesspermisson} ${styles.accesspermissonnr}`}>
                        <div className={styles.accesspert}>
                           <svg xmlns="http://www.w3.org/2000/svg" width="20" height="13" viewBox="0 0 20 13" fill="none">
  <path d="M10 1.5001C13.4703 1.5001 16.6418 3.2586 18.4916 6.2066C16.6418 9.1546 13.4704 10.9131 10 10.9131C6.52965 10.9131 3.35805 9.1543 1.50845 6.2068C3.35805 3.2588 6.52955 1.5001 10 1.5001ZM10 0C5.86775 0 2.24915 2.178 0.21525 5.4456C-0.07175 5.9065 -0.07175 6.5066 0.21525 6.9678C2.24915 10.2351 5.86775 12.4131 10 12.4131C14.1323 12.4131 17.7507 10.2351 19.7847 6.9678C20.0718 6.5066 20.0718 5.9065 19.7847 5.4456C17.7509 2.178 14.1321 0 10 0Z" fill="black"/>
  <path d="M9.99995 9.0026C11.5441 9.0026 12.7959 7.75083 12.7959 6.2067C12.7959 4.66257 11.5441 3.4108 9.99995 3.4108C8.45582 3.4108 7.20405 4.66257 7.20405 6.2067C7.20405 7.75083 8.45582 9.0026 9.99995 9.0026Z" fill="black"/>
</svg>
                            <div className={styles.featuresa}>
                                <h3>Access & Permissions</h3>
                                <p>Manage feature access and privileges</p>
                            </div>
                        </div>
                        <div className={styles.featurenet}>
                            <CustomSwitch
                                checked={true}
                                label="Enable Notifications"
                                onChange={(val: any) => console.log('Switch toggled:', val)}
                            />
                        </div>
                    </div>
                    <div className={`${styles.accesspermisson} ${styles.accesspermissonnr}`}>
                        <div className={styles.accesspert}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="20" height="17" viewBox="0 0 20 17" fill="none">
  <path d="M9.99584 6.67908C11.8401 6.67908 13.3352 5.18399 13.3352 3.33954C13.3352 1.49509 11.8401 0 9.99584 0C8.1516 0 6.65613 1.49519 6.65613 3.33954C6.65613 5.18389 8.1513 6.67908 9.99584 6.67908ZM9.99584 1.29947C11.1206 1.29947 12.0358 2.2146 12.0358 3.33954C12.0358 4.46449 11.1207 5.37961 9.99584 5.37961C8.871 5.37961 7.95569 4.46449 7.95569 3.33954C7.95569 2.2146 8.8708 1.29947 9.99584 1.29947Z" fill="black"/>
  <path d="M11.647 7.64379H8.34442C6.68841 7.64379 5.34567 8.98635 5.34567 10.6426V16.3197C5.34567 16.5957 5.56948 16.8195 5.84546 16.8195H14.146C14.422 16.8195 14.6458 16.5957 14.6458 16.3197V10.6426C14.6458 8.98635 13.3031 7.64379 11.647 7.64379ZM13.3463 15.5201H6.64523V10.6426C6.64523 9.70555 7.40741 8.94326 8.34452 8.94326H11.6471C12.5843 8.94326 13.3464 9.70555 13.3464 10.6426L13.3463 15.5201Z" fill="black"/>
  <path d="M16.1347 8.30263C17.5628 8.30263 18.7208 7.1448 18.7208 5.71658C18.7208 4.28836 17.5629 3.13043 16.1347 3.13043C14.7065 3.13043 13.5486 4.28826 13.5486 5.71658C13.5486 7.1449 14.7063 8.30263 16.1347 8.30263ZM16.1347 4.4299C16.8441 4.4299 17.4212 5.00716 17.4212 5.71658C17.4212 6.42599 16.8441 7.00315 16.1347 7.00315C15.4253 7.00315 14.8481 6.42599 14.8481 5.71658C14.8481 5.00716 15.4253 4.4299 16.1347 4.4299Z" fill="black"/>
  <path d="M17.0415 9.1096H15.9277C15.7758 9.1096 15.6302 9.13219 15.4844 9.15428C15.6604 9.5998 15.7646 10.0815 15.7646 10.5888V16.8195H19.4872C19.7632 16.8195 19.987 16.5957 19.987 16.3197V12.055C19.987 10.4283 18.6683 9.1096 17.0415 9.1096ZM18.6875 15.5201H17.0642V10.5889C17.0642 10.529 17.063 10.469 17.061 10.4093C17.9595 10.4197 18.6875 11.1541 18.6875 12.0551V15.5201Z" fill="black"/>
  <path d="M3.85219 8.30263C5.2806 8.30263 6.43832 7.1448 6.43832 5.71658C6.43832 4.28836 5.2806 3.13043 3.85219 3.13043C2.42379 3.13043 1.26637 4.28826 1.26637 5.71658C1.26637 7.1449 2.42409 8.30263 3.85219 8.30263ZM3.85219 4.4299C4.5616 4.4299 5.13876 5.00716 5.13876 5.71658C5.13876 6.42599 4.5616 7.00315 3.85219 7.00315C3.14279 7.00315 2.56583 6.42599 2.56583 5.71658C2.56583 5.00716 3.14269 4.4299 3.85219 4.4299Z" fill="black"/>
  <path d="M2.94527 9.1096C1.31855 9.1096 0 10.4284 0 12.0551V16.3198C0 16.5958 0.223507 16.8196 0.499792 16.8196H4.22234V10.5889C4.22234 10.0815 4.3265 9.5999 4.50222 9.15438C4.35678 9.13219 4.21085 9.1097 4.05901 9.1097L2.94527 9.1096ZM2.92278 10.5889V15.5201H1.29946V12.0551C1.29946 11.1539 2.02746 10.4196 2.92598 10.4093C2.92398 10.4691 2.92278 10.529 2.92278 10.5889Z" fill="black"/>
</svg>
                            <div className={styles.featuresa}>
                                <h3>Access & Permissions</h3>
                                <p>Manage feature access and privileges</p>
                            </div>
                        </div>
                        <div className={styles.featurenet}>
                            <CustomSwitch
                                checked={true}
                                label="Enable Notifications"
                                onChange={(val: any) => console.log('Switch toggled:', val)}
                            />
                        </div>
                    </div>
                    <div className={`${styles.accesspermisson} ${styles.accesspermissonnr}`}>
                        <div className={styles.accesspert}>
                            <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
  <path d="M15.9863 3.3689L15.0759 4.6588C15.9453 5.881 16.4619 7.3703 16.4619 8.981C16.4619 13.106 13.1059 16.4619 8.9809 16.4619C4.8559 16.4619 1.4999 13.106 1.4999 8.981C1.4999 4.856 4.8559 1.5 8.9809 1.5C10.1247 1.5 11.2053 1.7656 12.1757 2.2266L13.3708 1.1502C12.0722 0.4205 10.5766 0 8.9809 0C4.021 0.0001 0 4.0209 0 8.981C0 13.9411 4.021 17.9619 8.981 17.9619C13.941 17.9619 17.962 13.941 17.962 8.981C17.962 6.8561 17.22 4.9068 15.9863 3.3689Z" fill="black"/>
  <path d="M15.5984 0.4902L8.7302 6.6756C8.7168 6.6885 8.7094 6.7023 8.6968 6.7153C7.5657 6.8568 6.687 7.8107 6.6868 8.9801C6.6863 10.2472 7.7132 11.2747 8.9803 11.2751C10.2469 11.2751 11.2749 10.2484 11.2754 8.9816C11.2754 8.7865 11.2437 8.6002 11.1978 8.4193C12.1402 7.0843 16.334 1.1435 16.334 1.1435C16.6775 0.6863 16.0156 0.0944 15.5984 0.4902Z" fill="black"/>
  <path d="M8.98067 4.25087C9.35131 4.25087 9.65177 3.95041 9.65177 3.57977C9.65177 3.20914 9.35131 2.90867 8.98067 2.90867C8.61003 2.90867 8.30957 3.20914 8.30957 3.57977C8.30957 3.95041 8.61003 4.25087 8.98067 4.25087Z" fill="black"/>
  <path d="M5.15655 5.83457C5.52725 5.83457 5.82775 5.53406 5.82775 5.16337C5.82775 4.79268 5.52725 4.49217 5.15655 4.49217C4.78586 4.49217 4.48535 4.79268 4.48535 5.16337C4.48535 5.53406 4.78586 5.83457 5.15655 5.83457Z" fill="black"/>
  <path d="M3.57442 8.31126C3.20382 8.31126 2.90332 8.61176 2.90332 8.98236C2.90332 9.35296 3.20382 9.65346 3.57442 9.65346C3.94502 9.65346 4.24582 9.35296 4.24582 8.98236C4.24582 8.61176 3.94502 8.31126 3.57442 8.31126Z" fill="black"/>
  <path d="M14.3762 9.65358C14.7468 9.65358 15.0473 9.35312 15.0473 8.98248C15.0473 8.61185 14.7468 8.31138 14.3762 8.31138C14.0055 8.31138 13.7051 8.61185 13.7051 8.98248C13.7051 9.35312 14.0055 9.65358 14.3762 9.65358Z" fill="black"/>
</svg>
                            <div className={styles.featuresa}>
                                <h3>Access & Permissions</h3>
                                <p>Manage feature access and privileges</p>
                            </div>
                        </div>
                        <div className={styles.featurenet}>
                            <CustomSwitch
                                checked={true}
                                label="Enable Notifications"
                                onChange={(val: any) => console.log('Switch toggled:', val)}
                            />
                        </div>
                    </div>
                </div>
                  <div className={styles.iconaccess}>
                            <div className={`${styles.accesspermisson}`}>
                                <div className={styles.accesspert}>
                                    <svg xmlns="http://www.w3.org/2000/svg" width="18" height="18" viewBox="0 0 18 18" fill="none">
  <path d="M2.83887 0.5H12.6514C12.6117 0.717601 12.5879 0.943907 12.5879 1.17871C12.5879 3.49779 14.4742 5.38354 16.7939 5.38379C17.038 5.38379 17.2738 5.35891 17.5 5.31641V15.1377C17.5 16.4262 16.4536 17.4726 15.1611 17.4727H2.83887C1.54638 17.4725 0.5 16.4261 0.5 15.1377V2.83496C0.5 1.54634 1.54636 0.500126 2.83887 0.5ZM13.9727 6.01562C13.4979 5.65438 12.8317 5.72164 12.4346 6.14648L12.3594 6.23633L9.86914 9.50977L7.05469 7.41309H7.05371C6.56474 7.05003 5.86518 7.12672 5.47363 7.61035L2.0498 11.8223C1.64694 12.3161 1.72388 13.0417 2.2168 13.4414C2.43056 13.6156 2.68819 13.6992 2.94238 13.6992C3.27502 13.6992 3.60837 13.5537 3.83594 13.2725L6.56445 9.91699L9.4082 12.0371L9.40918 12.0381C9.91592 12.4131 10.6307 12.3127 11.0127 11.8086L11.0137 11.8096L14.1934 7.62793C14.5788 7.12152 14.4778 6.39968 13.9727 6.01562Z" stroke="black"/>
</svg>
                                    <div className={styles.featuresa}>
                                        <h3>Activity Log</h3>
                                        <p>Recent team configuration changes</p>
                                    </div>
                                </div>
                                <div className={styles.featurenet}>
                                    <p>View Full Audit Log<svg xmlns="http://www.w3.org/2000/svg" width="6" height="12" viewBox="0 0 6 12" fill="none">
                                        <path d="M0.749951 11.3309C0.557551 11.3309 0.365152 11.2577 0.218752 11.1102C-0.0737485 10.8172 -0.0727486 10.3426 0.220751 10.0497L4.08505 6.19718C4.22715 6.05558 4.30525 5.86608 4.30525 5.66498C4.30525 5.46478 4.22715 5.27628 4.08505 5.13468L0.220352 1.28118C-0.0726482 0.988176 -0.0735484 0.513576 0.218852 0.220676C0.510852 -0.0732242 0.985952 -0.0732245 1.27985 0.218675L5.14415 4.07218C5.56995 4.49698 5.80475 5.06238 5.80525 5.66498C5.80575 6.26758 5.57085 6.83298 5.14415 7.25968L1.27945 11.1122C1.13345 11.2577 0.941351 11.3309 0.749951 11.3309Z" fill="#6B46C1" />
                                    </svg></p>
                                </div>
                            </div>
                            <div className={styles.accesslogges}>
                                <div className={styles.chromeinput}>
                                    <span></span>
                                    <div className={styles.windowste}>
                                        <h3>Role assignments updated</h3>
                                        <p>Modified by Pei Lin </p>
                                    </div>
                                </div>
                                <p className={styles.alarchr}>09:12 AM</p>
                            </div>
                            <div className={styles.accesslogges}>
                                <div className={styles.chromeinput}>
                                    <span></span>
                                    <div className={styles.windowste}>
                                        <h3>Recent team configuration changes</h3>
                                        <p>Added by Admin</p>
                                    </div>
                                </div>
                                <p className={styles.alarchr}>09:12 AM</p>
                            </div>
                            <div className={styles.accesslogges}>
                                <div className={styles.chromeinput}>
                                    <span></span>
                                    <div className={styles.windowste}>
                                        <h3>Member Rhonda Patel added</h3>
                                        <p>Member Rhonda Patel added</p>
                                    </div>
                                </div>
                                <p className={styles.alarchr}>09:12 AM</p>
                            </div>
                            <div className={styles.accesslogges}>
                                <div className={styles.chromeinput}>
                                    <span></span>
                                    <div className={styles.windowste}>
                                        <h3>Logged in</h3>
                                        <p>Session started from Chrome / Windows</p>
                                    </div>
                                </div>
                                <p className={styles.alarchr}>09:12 AM</p>
                            </div>
                            <div className={styles.accesslogges}>
                                <div className={styles.chromeinput}>
                                    <span></span>
                                    <div className={styles.windowste}>
                                        <h3>Logged in</h3>
                                        <p>Session started from Chrome / Windows</p>
                                    </div>
                                </div>
                                <p className={styles.alarchr}>09:12 AM</p>
                            </div>
                        </div>
            </div>
        </div>
    );
}
