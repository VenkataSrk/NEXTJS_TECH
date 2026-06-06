import Lottie from 'lottie-react';
import worktualLoader from "./worktual-loader.json"

const Loaders = () => {
  return (
    <>
      <div className={"loaders"}>
        <Lottie
          style={{ width: 100, margin: "0 auto" }}
          animationData={worktualLoader}
        />
      </div>
    </>
  );
};
export default Loaders;