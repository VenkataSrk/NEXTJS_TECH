// import { useState } from "react";
// import styles from "./scss/config.module.scss";
// import CustomSearch from "../../customcomponents/customSearch";
// import BootstrapTable from 'react-bootstrap-table-next';
// import paginationFactory from "react-bootstrap-table2-paginator";
// import AddAccountBtn from "./addAccountBtn";
// import NoTableData from "../../customcomponents/noTableData";
// import {expandRowTable,tableOptions} from '../../customcomponents/tableOptions';
// import { whatsappService } from "../../base/services/apiservices";

// interface AccountData {
//   businessName: string;
//   createdDate: string;
//   domainId: number;
//   id: number;
//   isactive: any;
//   messageLimit: number;
//   mobileNumber: string;
//   status: string;
// }

// interface UserTableProps {
//   accDatas: AccountData[];
//   onLaunchSignup: () => void;
//   onSelectAccount: (account: AccountData) => void;
// }

// const UserTable: React.FC<UserTableProps> = ({ accDatas, onLaunchSignup, onSelectAccount }) => {
//   const [searchUser, setSearchUser] = useState<string>('');
//   const [userCopy, setUserCopy] = useState<any>(accDatas);
//   console.log("userCopy:::", userCopy);

//   const searchUserByKeyword = (searchValue: string) => {
//     setSearchUser(searchValue);
//     if (searchValue.trim() !== '') {
//       const searchLower = searchValue.toLowerCase();
//       const result = accDatas.filter((user) =>
//         user.businessName.toLowerCase().includes(searchLower) ||
//         user.mobileNumber.includes(searchValue)
//       );
//       setUserCopy(result);
//     } else {
//       setUserCopy(accDatas);
//     }
//   };

//   const options = {...tableOptions,sizePerPageList:[...tableOptions.sizePerPageList,{
//     text:"All",
//     value:userCopy.length,
//   }]}
//   const handleStatusChange = async (mobileNumber: string) => {
//     try {
//       const data = { mobileNumber };
//       const response = await whatsappService.post(data, `/change_account_status`);
//       if (response?.data?.statusCode === 200) {
//         setUserCopy((prevUsers: AccountData[]) =>
//           prevUsers.map((user) =>
//             user.mobileNumber === mobileNumber
//               ? { ...user, isactive: user.isactive === 1 ? 0 : 1 }
//               : user
//           )
//         );
//       } else {
//         console.error("Failed to change account status", response?.data);
//       }
//     } catch (error) {
//       console.error("Error changing status:", error);
//     }
//   };

//   const data = [{}]
//   const columns = [
//     {
//       dataField: "businessName",
//       text: "Business Name",
//       sort: true,
//       formatter: (cell: string, row: AccountData) => <div className="clickabletxt" onClick={() => onSelectAccount(row)}>{cell}</div>,
//     },
//     {
//       dataField: "mobileNumber",
//       text: "Number",
//       sort: true,
//       formatter: (cell: string) => <div>{cell}</div>,
//     },
//     {
//       dataField: "status",
//       text: "Status",
//       sort: true,
//       formatter: (cell: string) => <div className={`${styles.stsbar} ${cell === 'Connected' ? styles.green : styles.yellow}`}>{cell}</div>,
//     },
//     {
//       dataField: "isactive",
//       text: "Action",
//       formatter: (cell: string, row: AccountData) => {
//         const isChecked = row.isactive;
//         const isDisabled = row.status === "Not Connected";
//         return (
//           <label className="switch">
//             <input
//               type="checkbox"
//               name="checkbox"
//               checked={isChecked}
//               disabled={isDisabled}
//               onChange={() => handleStatusChange(row?.mobileNumber)}
//             />
//             <span className="slider round"></span>
//           </label>
//         );
//       },
//     },
//   ]

//   const expandRow: any = {...expandRowTable,
//     renderer: (item: any) => (
//       <div className="expandrowtable">
//         <p>
//           <strong>Number</strong>{item?.mobileNumber}
//         </p>
//         <p>
//           <strong>Status</strong> {<div className={`${styles.stsbar} ${item.status === 'Connected' ? styles.green : styles.yellow}`}>{item.status}</div>}
//         </p>  
//         <p>
//           <strong>Action</strong> {<label className="switch">
//           <input
//             type="checkbox"
//             name="checkbox"
//             onChange={() => { }}
//           />
//           <span className="slider round"></span>
//         </label>}
//         </p>  
//       </div>
//     ),
//   };

//   return (
//     <div className={styles.configbody}>
//       <div className="topsection">
//         <div className="toprowsection">
//           <div className="lefts">
//             <div className="searchelem">
//               <CustomSearch onChange={(e: any) => { searchUserByKeyword(e.target.value) }}
//                 value={searchUser} />
//             </div>
//           </div>
//           <div className="rights">
//             <AddAccountBtn btnClass="primarybtn" onClickSignup={onLaunchSignup} btnLabel="Add Account" />
//           </div>
//         </div>
//       </div>
//         <div className='desktoptable'>
//           <BootstrapTable
//             keyField="id"
//             data={userCopy}
//             columns={columns}
//             pagination={paginationFactory(options)}
//             noDataIndication={<NoTableData />}
//           />

//         </div>
//         <div className='mobiletable'>
//           <BootstrapTable
//             keyField="id"
//             data={userCopy}
//             columns={columns}
//             expandRow={expandRow}
//             noDataIndication={<NoTableData />}
//           />
//         </div>

//     </div>

//   );
// };

// export default UserTable;
