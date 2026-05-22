import React, { useState, useEffect } from "react";

interface TimeAgoProps {
    timestamp: string;
}

const TimeAgo: React.FC<TimeAgoProps> = ({ timestamp }) => {
    const [timeAgo, setTimeAgo] = useState("");

    // Convert timestamp to proper Date object
    const getDate = () => {
        // Replace space with 'T' to make it ISO compatible
        return new Date(timestamp.replace(" ", "T"));
    };

    const calculateTimeAgo = () => {
        const now = new Date();
        const past = getDate();
        const diffMs = now.getTime() - past.getTime();

        const diffSeconds = Math.floor(diffMs / 1000);
        const diffMinutes = Math.floor(diffSeconds / 60);
        const diffHours = Math.floor(diffMinutes / 60);
        const diffDays = Math.floor(diffHours / 24);

        if (diffSeconds < 60) return "Generated just now";
        if (diffMinutes < 60)
            return `Generated ${diffMinutes} minute${diffMinutes > 1 ? "s" : ""} ago`;
        if (diffHours < 24)
            return `Generated ${diffHours} hour${diffHours > 1 ? "s" : ""} ago`;
        return `Generated ${diffDays} day${diffDays > 1 ? "s" : ""} ago`;
    };

    useEffect(() => {
        // Initial calculation
        setTimeAgo(calculateTimeAgo());

        // Update every second for lively feel
        const interval = setInterval(() => {
            setTimeAgo(calculateTimeAgo());
        }, 1000);

        return () => clearInterval(interval);
    }, [timestamp]);

    return <span>{timeAgo}</span>;
};

export default TimeAgo;