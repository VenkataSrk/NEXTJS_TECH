import styles from "./scss/deals.module.scss";

function Activitytimeline({ activities }: any) {

  const formatTimelineTime = (value?: string) => {
    if (!value) return "-";
  
    const parsedDate = new Date(value);
    if (isNaN(parsedDate.getTime())) return value;
  
    return parsedDate.toLocaleString("en-IN", {
      month: "short",
      day: "numeric",
      year: "numeric",
      hour: "2-digit",
      minute: "2-digit",
      hour12: true,
    });
  };

  const timelineActivities = Array.isArray(activities) ? activities : [];

  return (
    <div className={styles.timeline}>
      {timelineActivities.map((item: any, index: number) => (
        <div key={index} className={styles.item}>
          <div className={styles.left}>
            <span className={styles.dot} />
            {index !== timelineActivities.length - 1 && (
              <span className={styles.line} />
            )}
          </div>

          <div className={styles.content}>
            <p className="innertitle">{item?.title}</p>
            <p className="content mt12">{item?.desc}</p>
          </div>

          <div className={styles.time}>
            <span className="content">
              {formatTimelineTime(item?.time)}
            </span>
          </div>
        </div>
      ))}
    </div>
  );
}

export default Activitytimeline;