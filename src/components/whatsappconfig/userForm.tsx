// import styles from "./scss/config.module.scss";

// interface AccountData {
//   businessName: string;
//   createdDate: string;
//   domainId: number;
//   id: number;
//   isactive: number;
//   messageLimit: number;
//   mobileNumber: string;
//   status: string;
// }

// interface UserFormProps {
//   account: AccountData;
//   onGoBack: () => void;
// }

// const UserForm: React.FC<UserFormProps> = ({ account, onGoBack }) => {
//   return (
//       <div className={styles.campaignformmain}>
//         <form className="formsection">
//           <div className="formrow">
//             <div className="rowleft">
//               <label>WhatsApp Number</label>
//             </div>
//             <div className="rowright">
//               <input type="text" value={account.mobileNumber} readOnly />
//             </div>
//           </div>
//           <div className="formrow">
//             <div className="rowleft">
//               <label>WhatsApp Business Name</label>
//             </div>
//             <div className="rowright">
//               <input type="text" value={account.businessName} readOnly />
//             </div>
//           </div>
//           <div className="formrow">
//             <div className="rowleft">
//               <label>Message Limit</label>
//             </div>
//             <div className="rowright">
//               <input type="text" value={account.messageLimit} readOnly />
//             </div>
//           </div>
//           <div className="formrow">
//             <div className="rowleft">
//               <label>Phone Number Status</label>
//             </div>
//             <div className="rowright">
//               <input type="text" value={account.status} readOnly />
//             </div>
//           </div>
//           <div className="formbtnsec">
//             <button className="cancelbtn" onClick={onGoBack}>
//               Go Back
//             </button>
//           </div>
//         </form>
//       </div>
//   );
// };

// export default UserForm;
