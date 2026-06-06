// import { useRef, useState, useEffect } from "react";

// export default function useScrollPercentage() {
//     const scrollRef = useRef(null);
//     const [scrollPercentage, setScrollPercentage] = useState(NaN);

//     const reportScroll = e => {
//         setScrollPercentage(getScrollPercentage(e.target));
//     };

//     useEffect(
//         () => {
//             const node = scrollRef.current;
//             if (node !== null) {
//                 node.addEventListener("scroll", reportScroll, { passive: true });
//                 if (Number.isNaN(scrollPercentage)) {
//                     setScrollPercentage(getScrollPercentage(node));
//                 }
//             }
//             return () => {
//                 if (node !== null) {
//                     node.removeEventListener("scroll", reportScroll);
//                 }
//             };
//         },
//         [scrollPercentage]
//     );

//     return [scrollRef, Number.isNaN(scrollPercentage) ? 0 : scrollPercentage];
// }

// function getScrollPercentage(element) {
//     if (element === null) {
//         return NaN;
//     }
//     const height = element.scrollHeight - element.clientHeight;
//     return Math.round((element.scrollTop / height) * 100);
// }


import { useRef, useState, useEffect } from "react";

export default function useScrollPercentage() {
    const scrollRef = useRef(null);
    const [scrollPercentage, setScrollPercentage] = useState(0);

    const reportScroll = (e) => {
        setScrollPercentage(getScrollPercentage(e.target));
    };

    useEffect(() => {
        const node = scrollRef.current;
        if (node) {
            node.addEventListener("scroll", reportScroll, { passive: true });

            // Initial percentage
            setScrollPercentage(getScrollPercentage(node));
        }

        return () => {
            if (node) {
                node.removeEventListener("scroll", reportScroll);
            }
        };
    }, []); // 👈 run only once

    return [scrollRef, scrollPercentage];
}

function getScrollPercentage(element) {
    if (!element) return 0;
    const height = element.scrollHeight - element.clientHeight;
    return height > 0
        ? Math.round((element.scrollTop / height) * 100)
        : 0;
}
