import React, { useEffect, useState } from "react";
import styles from "../../assets/styles/team.module.scss";
import Skeleton from "../../skeleton";
import { useNavigate } from "react-router-dom";
import { getQueueList } from "../../apiservice/myplanapi";

const Teams = () => {
  const navigate = useNavigate();
  const [groupList, setGroupList] = useState<any[]>([]);
  const [loader, setLoader] = useState(false);
  const [teamInfo, setTeamInfo] = useState({});

  useEffect(() => {
    const fetchTeamList = async () => {
      try {
        setLoader(true);
        const teamListData = await getQueueList();
        setGroupList(Array.isArray(teamListData) ? teamListData : []);
      } catch (error) {
        console.error("Error fetching team list:", error);
      } finally {
        setLoader(false);
      }
    };
    fetchTeamList();
  }, []);

  const getParticularTeam = (data: any) => {
    try {
      const body = { qId: Number(data?.qid) };
      setTeamInfo(body);
      navigate(`/useradminrolesteams?queueId=${data?.qid}&teamName=${data?.name}`);
    } catch (error) {
      console.log("getUserDetail Error", error);
    }
  };

  return (
    <div className={styles.createmaincontainer}>
      {/* <div className={styles.headerole}>
        <h3>Teams</h3>
        <p>
          Organize users for task assignment and streamline the business
          operations.
        </p>
      </div> */}

      <div className={styles.createroleblock}>
        {loader && <Skeleton count={6} />}

        {groupList?.map((team: any, id: number) => (
          <div key={team?.qid ?? id} className={styles.rolename}>
            <div
              className={styles.administratormodule}
              onClick={() => getParticularTeam(team)}
            >
              <div className={styles.accesstitle}>{team?.name}</div>
              <div className={styles.labeltext}>
                {team?.description ?? "_"}
              </div>
            </div>
          </div>
        ))}

        {!loader && !groupList?.length && (
          <div className={styles.nodatacontainer}>
            <div className={styles.nodatabox}>
              {/* <img src={assets.emptyData} alt="No data" /> */}
              <div className={styles.connected}>
                <div className={styles.callstitle}>
                  No teams available
                </div>
                <div className={styles.callmessage}>
                  There are no teams to display at the moment.
                </div>
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default Teams;
