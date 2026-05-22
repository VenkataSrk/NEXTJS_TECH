type SkeletonBoxProps = {
    width?: string;
    height?: string;
    borderRadius?: string;
    count?: number
};

const SkeletonBox = ({ width = "100%", height = "16px", borderRadius = "6px" , count = 1}: SkeletonBoxProps) => {
    
    return (
        <>
            {[...Array(count)].map((_, index) => (
                <div key={index} className="skeletonBox" style={{ width, height, borderRadius }}></div>
            ))}
        </>
    )
};

export default SkeletonBox;
