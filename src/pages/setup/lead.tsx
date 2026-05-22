
import { useRef, useState, useEffect } from "react";
import styles from "./scss/set.module.scss";

export default function LeadSetup() {
    const dtls = [
        {
            img:
<svg width="55" height="45" viewBox="0 0 55 45" fill="none" xmlns="http://www.w3.org/2000/svg">
  <path d="M8 0.370117H47C51.2136 0.370117 54.6299 3.78637 54.6299 8V36.543C54.6298 40.7565 51.2136 44.1719 47 44.1719H8C3.78645 44.1719 0.370239 40.7565 0.370117 36.543V8C0.370119 3.78637 3.78637 0.370117 8 0.370117Z" fill="white"/>
  <path d="M8 0.370117H47C51.2136 0.370117 54.6299 3.78637 54.6299 8V36.543C54.6298 40.7565 51.2136 44.1719 47 44.1719H8C3.78645 44.1719 0.370239 40.7565 0.370117 36.543V8C0.370119 3.78637 3.78637 0.370117 8 0.370117Z" stroke="#D2D3D4" stroke-width="0.741106"/>
  <image href="data:image/png;base64,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" x="7.411" y="7.411" width="40.178" height="29.721"/>
</svg>

            , title: "Hubspot", desc:"Only the administrator needs to establish a connection; others can access it afterward.", sts: "active"
        },
//          {
//             img:
//     <svg width="55" height="45" viewBox="0 0 55 45" fill="none" xmlns="http://www.w3.org/2000/svg">
//   <path d="M8 0.370117H47C51.2136 0.370117 54.6299 3.78637 54.6299 8V36.543C54.6298 40.7565 51.2136 44.1719 47 44.1719H8C3.78645 44.1719 0.370239 40.7565 0.370117 36.543V8C0.370119 3.78637 3.78637 0.370117 8 0.370117Z" fill="white"/>
//   <path d="M8 0.370117H47C51.2136 0.370117 54.6299 3.78637 54.6299 8V36.543C54.6298 40.7565 51.2136 44.1719 47 44.1719H8C3.78645 44.1719 0.370239 40.7565 0.370117 36.543V8C0.370119 3.78637 3.78637 0.370117 8 0.370117Z" stroke="#D2D3D4" stroke-width="0.741106"/>
//   <image href="data:image/png;base64,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" x="7.411" y="7.411" width="40.178" height="29.721"/>
// </svg>
//             , title: "wewe", desc:"Only the administrator needs to establish a connection; others can access it afterward.", sts: "inactive"
//         },
//          {
//             img:
//     <svg width="55" height="45" viewBox="0 0 55 45" fill="none" xmlns="http://www.w3.org/2000/svg">
//   <path d="M8 0.370117H47C51.2136 0.370117 54.6299 3.78637 54.6299 8V36.543C54.6298 40.7565 51.2136 44.1719 47 44.1719H8C3.78645 44.1719 0.370239 40.7565 0.370117 36.543V8C0.370119 3.78637 3.78637 0.370117 8 0.370117Z" fill="white"/>
//   <path d="M8 0.370117H47C51.2136 0.370117 54.6299 3.78637 54.6299 8V36.543C54.6298 40.7565 51.2136 44.1719 47 44.1719H8C3.78645 44.1719 0.370239 40.7565 0.370117 36.543V8C0.370119 3.78637 3.78637 0.370117 8 0.370117Z" stroke="#D2D3D4" stroke-width="0.741106"/>
//   <image href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAGQAAABkCAYAAABw4pVUAAAACXBIWXMAABYlAAAWJQFJUiTwAAAAAXNSR0IArs4c6QAAAARnQU1BAACxjwv8YQUAAAo5SURBVHgB7Zx/bBPnGcef853txCEOJHFaakJLElCD+BEJSDShJnQtCDa6MdimogINaqcNbWgak7pu2pBA0/6YBFuFNlUaG0kgo6paaLuMpIWVJmhbU1It0I5QcByI4xCwnRA7seMf5+v7nHNg0sTc+c6OA+9Hgovfe/285/f7vs/7vO97dwAUCoVCoVAoFAqFQqFQKBQKhUKhUCgUCoVCoVAoFIpSGEgHbzqywQtWYEPLQGDygWWsd84JEABBCJBLuQo8Z4OXi23wEJNaQVAIX3ANcGwNqfhsWd9hYJAI1Ay1JZ+AUmLC3y3n5eJBmGGkTpC/2jYoEmIicoU57CgDLlQFjK6MlJU/SQ4nCFEbjBnaYVexEzIc7QV505EPo+EXgGHKQAuEaCvsLD3xlfQ6eyUpY8MUIkxhS7BBjr4Rvp+5PUdbQVCMQGS3okqSRfQTqC1tvFOGWsF5vgVeKmuGDEQ7QVImhgQRxWRo1rAMJ5i4Q6S3BCCD0E6QI/bdmrmpqWAwIktyTJqcjBOFAy1Afw4pFgPRVgzECv7QZnJsvCcVozUP6YVZoXzSyLLFsJw3BMBMBEyxeOp7CF58IPJK6lxVGhCih8UKZ4ILgWWXAgo1FWL0R6I2nDclE5rfB/WCxHrHCzCTSdYVqpkzTWlSLXU9r0CiFvUwgMKEuUYtVhl0oAaMrB52MRB01xyJ/nAyrBJ1goRCVIx4WHY9HOneDCpQJ0hEl/rIaqbB6GrUiKJOEIHROgx9MEBR/tZTA0mgThDK1DCwYXyMVQQVJFUwJIzGNTeFqBOEETJqHSjjwKUk3B5QgDpBdNyM2wBKO2xYUSisTpAoaLrdur9iTnnXJquqsFFiRb7BjLbcW+fX/mvdo2thusBegstLMlEnSC3ZgcM9cY1YkMuan8zTzwMNeH/do5stWaz59S987Q320UswnfjDS+VmVb/ay0A7+X8NZBAvluVaC4w688/ODzX98dKwHaYdRvYEWp0gR3rWwBRi7FmcV/Kr5XnVBQaduXeUd73n8Hftbvd0YmW9usRcZclmzZjPFeC92895Tnd4xnwTbXyj2FR4YMWcasw7GhaC9baRzr2dQ12S/Z8uNlfNz2Et8fYPVRZUbC3JqcA8P1w0q2LNI8Z5m87eahPTS2Pp8WVKZXR4gs6VBUaridMZH3/LcRxd3rFqy7PSdV4cDPd9/f0bZ+LT8ZpabwbsO8652yExC0EmyQtS140hXeVkp/CiD6ias/G9Xn/nCUege2W+wUIqTvxhC2dx5ivDYffbvf6uPPLjsfKOPlWwtvwd54mJNt6qtmz5/+1w31+uDLc9Ny+79DfL89Z2j0S8Z5x+t2R/b2egDe0vnq234PeGQ9HgaEQImjghiJX8P0/Y1fBUYdX2kllVx+wj7ddHee+PFuVWtawt2rL8Xedxi5E1opt8PIeznO4PdF0YCruwbHR5aA9dHh6j0dg1YTra/92F4bYaIjbarmzbCFiRJ7gUtBDhIx/+ihj0DEXiatqgSSQLwinvwqU+xMlC51u9u+QJPIFMUIrUBLDMO1wm4wbe5J/ZEHZzdYzZHKYdnB8Zblm2FGsutEqi7J4fSNQ7gV7xRCZiWsghmgOlNLgqIFoRJM7Q2YuZPbNsjayk9Kqxj1NahmS4T47hjMadD8cd1hcmYhCPvCR2AoFIwwCqx8kIji1FiGe5B/YOdbzHYhk1t0mmqDjTmjlfpIqHpJl24KT4kD2IIGTuWkUA1H/SNthsnGl59eDIFTBTAbFSHIypyXaPaeOwnBQBjp+qfiIlyDE7kBEn6wjW5jS50xDXBLiGmFn8WeQAaTn9UxI3bWXyK9fBqkGly1ir3xK3GNjg/dHqYiU1KDNiwPkwLKtJGJJvSAh9qS4qHfQcRKKSI8NR6xEpNiDM/gCALxjH28S35mZbwZKXw9BUh2Z4STtxSeSXkfKBNK7H2LgWsRNm1SAdj2kd8xw0isIvicEN220FgXt8eoW9TKF9LosCYzI2AhZqNTg/SiSGDPwdX6TMT2CIFrMXzCiwr2HB6BnSEyfIBJ1jkpg+Bpl8xRxca8Ftj94bzCdfkEkjjrKSFi8NHb/l3DvAy53J5c2slzz2YMoBIVCoVAoFAqFQqFQKBQKhUKhUCgUCoVCoVAoFAqFkjxfApr2vq4Dz5R3AAAAAElFTkSuQmCC" x="7.411" y="7.411" width="40.178" height="29.721"/>
// </svg>
//             , title: "wewe", desc:"Only the administrator needs to establish a connection; others can access it afterward.", sts: "Yet to connect"
//         },
    ]
    return (
        <>
        <div className={styles.wrappercards}>
  {
                dtls.map((el, i) => (
                    <div key={i} className={styles.integrationacard}>
                        <div className={styles.top}>
 <div className={styles.lefts}>
                            {el.img}
                            <div className={styles.carddetails}>
                               
                            <div className={styles.title}>{el.title}</div>
                          
                            </div>
                        </div>
                        <div>
                            <span className={`${styles.badge} ${el.sts === "active" ? styles.green : ""}`}>{el.sts === "active" ? "Active" :"Inactive"}</span>
                        </div>
                        </div>
                         <div className={styles.desc}>{el.desc}</div>
                    </div>
                ))
            }
        </div>
          

        </>
    );
}
