import React, { useEffect, useState } from "react";
import './scss/worktualtab.scss';

interface Tab {
  label: string;
  content: React.ReactNode;
  removeTopSpace?:boolean
}

// interface TabsProps {
//   tabs: Tab[];
//   activeTab: number;                        // controlled index
//   onTabChange: (index: number) => void;     // controlled change handler
// }

interface TabsProps {
  tabs: Tab[];
  /** Controlled mode */
  activeTab?: number;
  onTabChange?: (index: number) => void;
  /** Uncontrolled mode */
  activeTabIndex?: number;
}

const WorktualTab: React.FC<TabsProps> = ({ tabs, activeTab, onTabChange, activeTabIndex }) => {
  const [internalActiveTab, setInternalActiveTab] = useState(activeTabIndex ?? 0);

  // use controlled if activeTab is provided, otherwise fallback to internal
  const currentTab = activeTab !== undefined ? activeTab : internalActiveTab;

  const handleTabSwitch = (index: number) => {
    if (activeTab !== undefined && onTabChange) {
      // controlled mode
      if (document.startViewTransition) {
        document.startViewTransition(() => onTabChange(index));
      } else {
        onTabChange(index);
      }
    } else {
      // uncontrolled mode
      setInternalActiveTab(index);
    }
  };

  return (
    <div className="wttabview">
      <div className={`wttabheaderouter ${tabs[currentTab]?.removeTopSpace ? 'wttabheaderouternospace' : ''}`}>
        <div className="wttabheader">
          {tabs.map((tab, index) => (
            <span
              key={index}
              onClick={() => handleTabSwitch(index)}
              className={`wttab ${currentTab === index ? 'wtactivetab' : ''}`}
            >
              {tab.label}
            </span>
          ))}
        </div>
      </div>
      <div className="wttabcontent">{tabs[currentTab].content}</div>
    </div>
  );
};



export default WorktualTab;
