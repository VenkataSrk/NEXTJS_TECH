import React, { useEffect, useState } from "react";
import styles from "./scss/emailsetup.module.scss";
import DataTable from 'react-data-table-component';
import { getEmailConfigDomain } from "../../base/service/setup";
import Nodata from "../component/nodata";
import Skeleton from "../MyPlan/skeleton";
const Emailsetup = () => {
    const [activeTab, setActiveTab] = useState("emailaddress");
    const [records, setRecords] = useState([]);
    const [loader, setLoader] = useState(false);

    useEffect(() => {
        setLoader(true)
        getEmailConfigDomain()
            .then((res) => {
                setLoader(false)
                setRecords(res)
            })
    }, [])


    const columns = [
        {
            name: 'Email',
            selector: (row: any) => row.emailAddress,
            cell: (row) => (
                <span className={styles.clickable}>
                {row.emailAddress}
                </span>
            ),
        },
        {
            name: 'Alias name',
            selector: (row: any) => row.aliasName,
        },
        {
            name: 'Configuration',
            selector: (row: any) => row?.smtp_json?.provider,
            cell: (row) => (
                <span >
                {row?.smtp_json?.provider || "Domain"}
                </span>
            ),
        },
        {
            name: 'Bot Status',
            selector: (row: any) => row?.isBot,
            cell: (row) => {
                const isEnabled = Number(row?.isBot) === 1;

                return (
                    <span
                        className={`${styles.tag} ${isEnabled ? styles.green : styles.red
                            }`}
                    >
                        {isEnabled ? "Enabled" : "Disabled"}
                    </span>
                );
            },
        },
        {
            name: 'Handled By',
            selector: (row: any) => row?.handled_by,
           cell: (row) => (
                row?.handled_by ? (
                    <span >
                    {row?.handled_by}
                    </span>
                ) : (
                    '-'
                )
            ),
        },
        {
            name: 'Email Activity',
            selector: (row: any) => row?.primaryEmail,
            cell: (row) => {
                const isActive = Number(row?.primaryEmail) === 1;

                return (
                    <span
                        className={`${styles.tag} ${isActive ? styles.green : styles.red
                            }`}
                    >
                        {isActive ? "Active" : "Inactive"}
                    </span>
                );
            },
        },
    ];
    return (
        <>
            <div className={styles.emailsetuplist}>

                <div className="tabscontainer">
                    <div className={`tabsinner ${activeTab === "emailaddress" ? 'active' : ""}`} onClick={() => setActiveTab("emailaddress")}>Email Address List</div>
                    {/* <div className={`tabsinner ${activeTab === "suspendedemail" ? 'active' : ""}`} onClick={() => setActiveTab("suspendedemail")}>Suspended Email</div> */}
                </div>
                <div>
                    {activeTab === "emailaddress" &&
                        <div className="worktualtable">
                            {loader
                                ? <Skeleton count={4} /> :
                                <DataTable
                                    columns={columns}
                                    data={records}
                                    fixedHeader
                                    responsive
                                    noDataComponent={<Nodata content="Not Configured Yet" isChildren={true} />}
                                />}
                        </div>
                    }
                </div>
            </div>
        </>
    )
}
export default Emailsetup;