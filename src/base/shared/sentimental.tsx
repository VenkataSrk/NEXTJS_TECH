export function SentimentScore({ value = 0 }: { value?: number }) {

    const safeValue = Number.isFinite(value) ? Number(value) : 0;
    const normalizedValue = Math.max(0, Math.min(100, safeValue));
    const scoreColor =
        normalizedValue >= 70 ? "#00b87c" : normalizedValue >= 40 ? "#f5b400" : "#e5484d";

    const size = 60;
    const center = size / 2;
    const radius = 24;
    const strokeWidth = 6;

    const circumference = 2 * Math.PI * radius;
    const dashOffset = circumference * (1 - normalizedValue / 100);

    return (
        <svg width={size} height={size} viewBox={`0 0 ${size} ${size}`}>
            {/* Background */}
            <circle
                cx={center}
                cy={center}
                r={radius}
                stroke="#d9d9d9"
                strokeWidth={strokeWidth}
                fill="none"
            />

            {/* Progress */}
            <circle
                cx={center}
                cy={center}
                r={radius}
                stroke={scoreColor}
                strokeWidth={strokeWidth}
                fill="none"
                strokeDasharray={circumference}
                strokeDashoffset={dashOffset}
                strokeLinecap="round"
                transform={`rotate(-90 ${center} ${center})`}
            />

            {/* Value */}
            <text
                x="50%"
                y="50%"
                textAnchor="middle"
                dy=".35em"
                fontSize="18"
                fontWeight="600"
                fill="#222"
            >
                {value}
            </text>
        </svg>
    );
}
