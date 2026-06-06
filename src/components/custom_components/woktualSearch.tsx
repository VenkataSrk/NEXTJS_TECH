import React, { useState } from 'react';
import styles from './scss/worktualsearch.module.scss';
// import { assets } from '../styles/images/imgassests';

interface CustomSelectProps {
  onChange:any;
  placeholder?:string,
  id?:string,
  name?:string,
  value:string|number,
}

const WorktualSearch:React.FC<CustomSelectProps> = ({placeholder='Search',onChange,id='',name='',value}) => {

  return (
    <div className={styles.searchinp}>
      <input type="text" placeholder={placeholder} value={value}  id={id} name={name} onChange={onChange}/>
      {/* {value && <img className={styles.clear} src={assets.deleteblack_icon} alt="clear" />} */}
    </div>
  )
}

export default WorktualSearch;
