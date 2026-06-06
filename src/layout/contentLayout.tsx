import React, { Ref } from 'react';
import styles from './scss/contentlayout.module.scss';

import { useEffect, useRef } from 'react';

interface layoutProps {
    children: React.ReactNode;
    noPadding?: boolean;
    noPaddingBlock?:boolean;
}

const ContentLayout = ({ children, noPadding = false, noPaddingBlock=false }: layoutProps) => {

    return (
        <div className={` contentlayout ${styles.contentlayout} ${noPaddingBlock ? styles.nopaddingblock : ''} ${noPadding ? styles.nopadding : ''}`}>
            <div className={`${styles.contentmain}`}>
                {children}
            </div>
        </div>
    )
}

export default ContentLayout
