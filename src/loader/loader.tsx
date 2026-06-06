import Lottie from 'lottie-react';
import worktualLoader from "./worktual-loader.json"

interface LoadingProps {
  size?: "small" | "medium" | "large";
}

const Loaders: React.FC<LoadingProps> = ({ size = "medium" }) => {
  return (
    <div className={`spinner ${size}`}>
      <Lottie
        style={{ width: 75, margin: "auto", marginTop: -120 }}
        animationData={worktualLoader}
      />
    </div>
  );
};

export default Loaders;