import styles from './scss/tooltip.module.scss';
 
/**
 * Tooltip component
 *
 * Props:
 * @param {string}  text        - content inside the tooltip
 * @param {string}  position    - 'top' | 'bottom' | 'left' | 'right'  (default: 'top')
 * @param {node}    children    - the element that triggers the tooltip on hover
 * @param {string}  className   - optional extra class on the wrapper
 */
export default function Tooltip({
  text,
  position = 'top',
  children,
  className = '',
}) {
  return (
    <div className={`${styles.wrapper} ${className} `}>
      {children}
      <div className={`${styles.tooltip} ${styles[position]}`}>
        {text}
        <span className={styles.arrow} />
      </div>
    </div>
  );
}
 