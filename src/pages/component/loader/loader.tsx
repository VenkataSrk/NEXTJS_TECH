
import styles from "./loader.module.scss";
import Loadergif from './loader.gif'

const Loader = () => {
  return (
    <div className={styles.mainLoadercontainer}>
      {/* <Lottie
        className="svgLoadinglistcenter"
        animationData={loader6}
        style={{ width: 100 }}
      /> */}
      <img src={Loadergif} alt="" style={{height: '80px'}}/>
    </div>
  ); 
};

export default Loader;
