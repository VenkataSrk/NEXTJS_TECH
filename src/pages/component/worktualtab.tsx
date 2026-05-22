import React, { useState } from "react";
import styles from "./scss/worktualtab.module.scss";

interface TabItem {
  label: string;
  content: React.ReactNode;
}

interface CustomTabsProps {
  tabs: TabItem[];
  defaultIndex?: number;
}

const CustomTabs: React.FC<CustomTabsProps> = ({
  tabs,
  defaultIndex = 0,
}) => {
  const [activeIndex, setActiveIndex] = useState(defaultIndex);

  return (
    <div className={styles.tabsWrapper}>
      <div className={styles.tabsHeader}>
        {tabs.map((tab, index) => (
          <button
            key={index}
            className={`${styles.tabBtn} ${
              activeIndex === index ? styles.active : ""
            }`}
            onClick={() => setActiveIndex(index)}
          >
            {tab.label}
          </button>
        ))}
      </div>

      <div className={styles.tabsContent}>
        {tabs[activeIndex]?.content}
      </div>
    </div>
  );
};

export default CustomTabs;
