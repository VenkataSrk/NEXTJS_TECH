// components/StackedBarChart.tsx

import React from 'react';
import Chart from 'react-apexcharts';

interface BarChartProps {
    categories: string[];
    series: ApexAxisChartSeries;
    colors?: string[];
    title?: string;
    yTitle?: string;
    height?: number;
}

const ApexLineChart: React.FC<BarChartProps> = ({
    categories,
    series,
    colors,
    title = '',
    height = 300,
}) => {
    const options: ApexCharts.ApexOptions = {
        chart: {
            type: 'line',
            toolbar: {
                show: false, // Use case: Show tools like download, zoom, pan
            },
            zoom: {
                enabled: false, // Use case: Allow users to zoom into dense data
            },
            animations: {
                enabled: true,
                //   easing: 'easeinout',
                speed: 800, // Use case: Smooth entry for charts (great for dashboards)
            },
        },

        //   title: {
        //     text: title,
        //     align: 'left', // Use case: Label your chart clearly
        //   },

        xaxis: {
            categories: categories, // Use case: X axis labels (dates, categories, etc.)
            // title: {
            //   text: 'Day of the Week', // Use case: Describe the x-axis clearly
            // },
            labels: {
                formatter: (val) => `${val}:00`, // show hours nicely on x-axis
            },
        },

        yaxis: {
            // title: {
            //   text: 'Visits', //  Use case: Describe what values mean
            // },
        },

        stroke: {
            curve: 'smooth',  //  Use case: Smooth lines look cleaner for trends eg: straight
            width: 1,        //  Thicker or thinner lines for visual emphasis
        },

        dataLabels: {
            enabled: false, //  Use case: Hide values on each point (use only when helpful)
        },

        markers: {
            size: 3,         // Use case: Emphasize points
            hover: {
                sizeOffset: 4, // Use case: Increase size on hover for interactivity
            },
        },

        tooltip: {
            enabled: true,
            // theme: 'dark',   //  Use case: Better visibility on dark backgrounds
            // x: {
            //   format: 'dd MMM', // Use case: Format X tooltip if using date strings
            // },
            x: {
                formatter: (val) => `${val}:00`, // hour in tooltip
            },
            y: {
                formatter: (val) => `${val.toLocaleString()} sales`, // format y-values
            },
        },

        grid: {
            show: true,
            borderColor: '#BFBFBF',     //  Use case: Visual guidance
            strokeDashArray: 4,         //  Use case: Dashed grid lines
        },

        //   legend: {
        //     position: 'top',            //  Use case: Better readability for multiple series
        //     horizontalAlign: 'right',
        //   },

        colors: colors, //  Use case: Custom brand colors
    };


    return <Chart options={options} series={series} type="line" height={height} />;
};

export default ApexLineChart;

