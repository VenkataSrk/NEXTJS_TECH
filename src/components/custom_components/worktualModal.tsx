import React from 'react';
import './scss/worktualmodal.scss';

interface CustomModalProps {
    show?: boolean;
    title?: any;
    customTitle?:React.ReactNode;
    children: React.ReactNode;
    onClose?: () => void;
    mdsize?: string;
    footer?: React.ReactNode;
}

const WorktualModal: React.FC<CustomModalProps> = ({ show = false, title,customTitle, children, onClose, mdsize = 'modalsm', footer }) => {
    if (!show) {
        return null;
    }
    else {
        return (
            <div className='modalview'>
                <div className={`modalcontainer ${mdsize}`}>
                    <div className='modalheader'>
                        {title && <>
                            <h2>{title}</h2>
                            <svg className='modelclose' onClick={onClose} xmlns="http://www.w3.org/2000/svg" width="12" height="12" viewBox="0 0 10 10" fill="none">
                                <path d="M5.80079 4.99937L9.70412 1.09571C9.92545 0.874973 9.92545 0.51564 9.70412 0.294906C9.48279 0.0735729 9.12339 0.0735729 8.90332 0.294906L4.99999 4.19851L1.09665 0.294906C0.876586 0.0735729 0.515919 0.0735729 0.295852 0.294906C0.074519 0.51564 0.074519 0.874973 0.295852 1.09571L4.19919 4.99937L0.295852 8.90304C0.074519 9.12377 0.074519 9.48311 0.295852 9.70384C0.406519 9.81451 0.551052 9.86984 0.696252 9.86984C0.841452 9.86984 0.986586 9.81451 1.09665 9.70384L4.99999 5.80024L8.90332 9.70384C9.01332 9.81451 9.15852 9.86984 9.30372 9.86984C9.44892 9.86984 9.59345 9.81451 9.70412 9.70384C9.92545 9.48311 9.92545 9.12377 9.70412 8.90304L5.80079 4.99937Z" fill="#989898" />
                            </svg>
                        </>
                        }
                        {
                            customTitle && customTitle
                        }

                    </div>
                    <div className='modalbody'>
                        {children}
                    </div>
                    {
                        footer && <div className='modalfooter'>{footer}</div>
                    }

                </div>
            </div>
        )
    }
}

export default WorktualModal;
