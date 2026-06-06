import React, { ReactNode, useEffect, useState } from 'react';
import styles from './scss/layout.module.scss';
import TopBar from './topBar';
import LeftNav from './leftNav';
import { Outlet } from 'react-router-dom';
import RightNav from './rightNav';


type layoutprops = {
  children?: ReactNode
}

const Layout = ({ children }: layoutprops) => {
  //  useEffect(() => {
  //     const loader = document.getElementById('contact_center_mainloader');
  //     if (loader) {
  //       loader.style.display = 'none';
  //     }
  //   }, []);

    useEffect(() => {
    const timer = setTimeout(() => {
      const loaders = document.getElementById('contact_center_mainloader');
      if (loaders) {
        loaders.style.display = 'none';
      }
    }, 1000); // 1 second
  
    // cleanup → remove timer
    return () => clearTimeout(timer);
  }, []);
  return (
    <div className={styles.layoutsec}>
      <header className={styles.ltstopbar}>
        <TopBar />
      </header>
      <section className={styles.ltsmainsec}>
        <div className={styles.ltsleftnavsec}>
          <LeftNav />
        </div>
        <div className={`${styles.ltscentersec} ltscenter`}>
         {children}
        </div>
        <div className={styles.ltsrightnavsec}><RightNav /></div>
      </section>
    </div>
  )
}

export default Layout
