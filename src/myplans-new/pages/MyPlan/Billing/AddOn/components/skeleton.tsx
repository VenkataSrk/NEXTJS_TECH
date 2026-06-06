const Skeleton = ({ count = 1 }: { count?: number }) => {

    const SkeletonBox = ({ width = "100%", height = "16px", borderRadius = "6px" }) => (
        <div className={"skeletonBox"} style={{ width, height, borderRadius }} />
    );

    if (!count) return <></>

    return <>
        {Array.from({ length: count }).map((_, index) => (
            <div key={index} className="skeletonTextGroup">
                <SkeletonBox width="70%" height="20px" />
                <SkeletonBox width="80%" height="14px" />
                <SkeletonBox width="60%" height="14px" />
            </div>
        ))}
    </>;

};
export default Skeleton;