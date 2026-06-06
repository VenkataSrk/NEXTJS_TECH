import React, { useEffect, useState } from 'react';
import styles from '../../../../assets/styles/team.module.scss';
import { getTeamList } from '../../../../apiservice/myplanapi';
import nodataimg from "../../../../../../src/styles/images/icons/nodata.svg"
const Teams = () => {

    const [teamList, setTeamList] = useState<any>([]);
    console.log("teamList:",teamList);
    useEffect(() => {
        const fetchTeamList = async () => {
            try {
                const teamListData = await getTeamList();
                setTeamList(teamListData);
            } catch (error) {
                console.error("Error fetching team list:", error);
            }
        };

        fetchTeamList();
    }, []);

    return (
        <>
            <div className={styles.createmaincontainer}>
                <div className={styles.headerole}>
                    <h3>Teams</h3>
                    <p>Manage users by organizing them into teams for easier access control.</p>
                </div>
                <div className={styles.createroleblock}>
                    {teamList?.length > 0 ? teamList?.map((team: any, id: any) => {
                        return (
                            <div id={id} className={styles.rolename}>
                                <div className={styles.administratormodule}>
                                    <label>{team?.department}</label>
                                    <p>Full access to all modules, user management, and settings.</p>
                                </div>
                                {/* <span>Typography</span> */}
                            </div>
                        )
                    }) :
                       <div className={"nodataclass"}><img src={nodataimg}/>No Data found</div>
                    }
                </div>
            </div>
        </>
    )
}
export default Teams
