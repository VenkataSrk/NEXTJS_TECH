import React from 'react'
import { leftnavassets } from '../styles/assets/assets'
import styles from './scss/rightnav.module.scss';

const RightNav = () => {
  return (
    <div className={styles.rightnavsec}>
      <ul className={styles.rgtnlistssec}>
        {/* <li className={styles.rgtnlist}>
          <img src={leftnavassets.quickaction_1} alt="quickaction_1" />
        </li>
        <li className={styles.rgtnlist}>
          <img src={leftnavassets.quickaction_2} alt="quickaction_2" />
        </li> */}
      </ul>
    </div>
  )
}

export default RightNav
