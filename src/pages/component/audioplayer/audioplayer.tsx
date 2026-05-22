import React, { useEffect, useRef, useState } from 'react';
import WaveSurfer from 'wavesurfer.js';
import Styles from './audioplayer.module.scss';

interface WorktualVisualizationProps {
  src: string;
}
let currentlyPlayingWaveSurfer: WaveSurfer | null = null;

export default function WorktualVisualization({ src}: WorktualVisualizationProps): React.JSX.Element {
  const waveformRef = useRef<HTMLDivElement | null>(null);
  const wavesurferRef = useRef<WaveSurfer | null>(null);
  const [isPlaying, setIsPlaying] = useState(false);
  const [duration, setDuration] = useState('00:00');
  const [currentTime, setCurrentTime] = useState('00:00');

  // Format seconds to MM:SS
  const formatTime = (seconds: number) => {
    const mins = Math.floor(seconds / 60).toString().padStart(2, '0');
    const secs = Math.floor(seconds % 60).toString().padStart(2, '0');
    return `${mins}:${secs}`;
  };

  // Init wavesurfer
  useEffect(() => {
    if (waveformRef.current && !wavesurferRef.current) {
      const wavesurfer = WaveSurfer.create({
        container: waveformRef.current,
        waveColor: '#D1D5DB',
        progressColor: '#6366F1',
        cursorColor: 'transparent',
        height: 32,
        barWidth: 4,
        barGap: 2,
        barRadius: 6,
        normalize: true,
        interact: true,
      });

      wavesurfer.load(src);
      wavesurferRef.current = wavesurfer;

      wavesurfer.on('ready', () => {
        setDuration(formatTime(wavesurfer.getDuration()));
      });

      wavesurfer.on('audioprocess', () => {
        if (wavesurfer.isPlaying()) {
          setCurrentTime(formatTime(wavesurfer.getCurrentTime()));
        }
      });

      // wavesurfer.on('finish', () => {
      //   setIsPlaying(false);
      // });
      wavesurfer.on('finish', () => {
        setIsPlaying(false);
      
        if (currentlyPlayingWaveSurfer === wavesurfer) {
          currentlyPlayingWaveSurfer = null;
        }
      });
      
    }

    return () => {
      wavesurferRef.current?.destroy();
      wavesurferRef.current = null;
    };
  }, [src]);

  // const togglePlayback = () => {
  //   if (wavesurferRef.current) {
  //     wavesurferRef.current.playPause();
  //     setIsPlaying((prev) => !prev);
  //   }
  // };

  const togglePlayback = () => {
    if (!wavesurferRef.current) return;
  
    // If another audio is playing, stop it first
    if (
      currentlyPlayingWaveSurfer &&
      currentlyPlayingWaveSurfer !== wavesurferRef.current
    ) {
      currentlyPlayingWaveSurfer.pause();
    }
  
    // Play or pause this one
    wavesurferRef.current.playPause();
  
    // If now playing, mark as current
    if (wavesurferRef.current.isPlaying()) {
      currentlyPlayingWaveSurfer = wavesurferRef.current;
      setIsPlaying(true);
    } else {
      setIsPlaying(false);
      if (currentlyPlayingWaveSurfer === wavesurferRef.current) {
        currentlyPlayingWaveSurfer = null;
      }
    }
  };
  

  return (
    <div className={Styles.wavecontainer} style={{padding: '16px 0 0'}}>
      <div className={`${Styles.wavecontent} `}>
           {isPlaying ? (
            // Pause Icon
            <svg xmlns="http://www.w3.org/2000/svg" width="10" height="16" viewBox="0 0 10 16" fill="none" onClick={togglePlayback} >
              <path d="M1.49938 0C0.67112 0 0 0.67112 0 1.49938V14.4842C0 15.3119 0.67112 15.9835 1.49938 15.9835C2.32763 15.9835 2.99875 15.3119 2.99875 14.4842V1.49938C2.99875 0.67112 2.32763 0 1.49938 0Z" fill="#404040"/>
              <path d="M8.50626 0C7.6785 0 7.00688 0.67112 7.00688 1.49938V14.4842C7.00688 15.3119 7.6785 15.9835 8.50626 15.9835C9.33401 15.9835 10.0056 15.3119 10.0056 14.4842V1.49938C10.0056 0.67112 9.33401 0 8.50626 0Z" fill="#404040"/>
            </svg>
            ) : (
            // Play Icon
            <svg xmlns="http://www.w3.org/2000/svg" width="15" height="16" viewBox="0 0 15 16" fill="none"   onClick={togglePlayback}>
              <path d="M13.2845 5.90754L8.44648 3.1187L3.6085 0.329863C3.21866 0.109954 2.80883 0 2.409 0C1.14952 0 0 1.00958 0 2.409V13.5743C0 14.9838 1.14952 15.9833 2.409 15.9833C2.80883 15.9833 3.21866 15.8834 3.6085 15.6535L8.44648 12.8646L13.2845 10.0758C14.8938 9.14619 14.8938 6.82716 13.2845 5.90754Z" fill="#404040"/>
            </svg>
            )}
  
        <div ref={waveformRef} className={`${Styles.waveref} wrappercontent`}/>
        <span className={Styles.timecode}>{currentTime} / {duration}</span>
      </div>
    </div>
  );
}