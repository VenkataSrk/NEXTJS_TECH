import React, { memo, useMemo, useState } from "react";
import styles from "./scss/agenttransfer.module.scss";


function Agenttransfer() {
  const [currentTab, setCurrentTab] = useState("all");
  const [search, setSearch] = useState("");

  const sData = search.toLowerCase().trim();

  // ✅ Static Agents
  const agents = [
    { id: 1, UserName: "John Doe", statusName: "Ready" },
    { id: 2, UserName: "Ajay", statusName: "Busy" },
    { id: 3, UserName: "Ravi", statusName: "Not Ready" }
  ];

  // ✅ Static Queues
  const queues = [
    { qid: 1, name: "Support Queue", activeAgents: 2 },
    { qid: 2, name: "Sales Queue", activeAgents: 0 }
  ];

  // ✅ Static Call Flow
  const callFlows = [
    { cfid: 1, templateName: "Welcome Flow" },
    { cfid: 2, templateName: "IVR Main Flow" }
  ];

  const tabs = [
    { label: "All", value: "all" },
    { label: "Agents", value: "agents" },
    { label: "Queue", value: "queue" },
    { label: "Call Flow", value: "callflow" }
  ];

  const filterAgentList = useMemo(
    () =>
      agents.filter(
        (item) =>
          item.UserName.toLowerCase().includes(sData) ||
          item.statusName.toLowerCase().includes(sData)
      ),
    [sData]
  );

  const filterQueueList = useMemo(
    () => queues.filter((item) => item.name.toLowerCase().includes(sData)),
    [sData]
  );

  const filterCallFlowList = useMemo(
    () =>
      callFlows.filter((item) =>
        item.templateName.toLowerCase().includes(sData)
      ),
    [sData]
  );

  return (
    <div className={styles.agenttransfercontainer}>
      {/* 🔍 Search */}
      <div className={styles.searchBar}>
        <input
          type="text"
          placeholder="Search"
          className={styles.input}
          onChange={(e) => setSearch(e.target.value)}
        />
      </div>

      {/* 📌 Tabs */}
      <div className={styles.tabs}>
        {tabs.map((item) => (
          <div
            key={item.value}
            className={`${styles.button} ${
              currentTab === item.value ? styles.active : ""
            }`}
            onClick={() => setCurrentTab(item.value)}
          >
            {item.label}
          </div>
        ))}
      </div>

      {/* 📋 Content */}
      <div className={styles.list}>
        {(currentTab === "all" || currentTab === "agents") && (
          <List filterList={filterAgentList} type="agents" />
        )}

        {(currentTab === "all" || currentTab === "queue") && (
          <List filterList={filterQueueList} type="queue" />
        )}

        {(currentTab === "all" || currentTab === "callflow") && (
          <List filterList={filterCallFlowList} type="callflow" />
        )}
      </div>
    </div>
  );
}

const List = memo(({ filterList, type }: any) => {
  const defaultStatus: any = {
    ready: styles.ready,
    "not ready": styles.notReady,
    busy: styles.busy
  };

  return (
    <>
      {filterList.map((item: any, index: number) => (
        <div className={styles.row} key={index}>
          <div className={styles.left}>
            <div className={styles.icon}>
              {type === "agents" ? (
               
"img"
              ) : (
    "img"

              )}
            </div>

            <div className={styles.details}>
              <p className={styles.name}>
                {type === "agents" && item.UserName}
                {type === "queue" && item.name}
                {type === "callflow" && item.templateName}
              </p>

              {type !== "callflow" && (
                <p className={styles.status}>
                  {type === "agents" && (
                    <div className={styles.stswithdot}>
                      <div
                        className={`${
                          defaultStatus[item.statusName?.toLowerCase()]
                        } ${styles.whiteDot}`}
                      />
                      {item.statusName}
                    </div>
                  )}

                  {type === "queue" && (
                    <div className={styles.stswithdot}>
                      <div
                        style={{
                          backgroundColor:
                            item.activeAgents > 0 ? "green" : "red"
                        }}
                        className={styles.whiteDot}
                      />
                      Active Agent(s) ({item.activeAgents})
                    </div>
                  )}
                </p>
              )}
            </div>
          </div>
        </div>
      ))}
    </>
  );
});

export default Agenttransfer;