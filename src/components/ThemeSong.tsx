import React, { useState, useRef, useEffect } from 'react';
import { motion } from 'framer-motion';
import { Volume2, VolumeX, Music, Play, Pause } from 'lucide-react';

export const ThemeSong: React.FC = () => {
  const [isPlaying, setIsPlaying] = useState(false);
  const [isMuted, setIsMuted] = useState(false);
  const [currentTime, setCurrentTime] = useState(0);
  const [duration, setDuration] = useState(0);
  const audioRef = useRef<HTMLAudioElement>(null);

  // Shit anthem lyrics
  const lyrics = [
    "🎵 Welcome to the shit show, where legends are born",
    "🎵 One billion repos, and shit leads the dawn",
    "🎵 From GitHub's first line to this cosmic mess",
    "🎵 AasishPokhrel created greatness, I confess",
    "🎵 Holy shit, holy shit, it's the billionth one",
    "🎵 Holy shit, holy shit, our work here is done",
    "🎵 In the halls of version control, where the pull requests flow",
    "🎵 There's a repo called shit, that steals the whole show",
    "🎵 Fork it, star it, clone it with pride",
    "🎵 The shit repository, our technological guide",
  ];

  const [currentLyricIndex, setCurrentLyricIndex] = useState(0);

  useEffect(() => {
    const audio = audioRef.current;
    if (!audio) return;

    const updateTime = () => setCurrentTime(audio.currentTime);
    const updateDuration = () => setDuration(audio.duration);

    audio.addEventListener('timeupdate', updateTime);
    audio.addEventListener('loadedmetadata', updateDuration);

    return () => {
      audio.removeEventListener('timeupdate', updateTime);
      audio.removeEventListener('loadedmetadata', updateDuration);
    };
  }, []);

  useEffect(() => {
    if (isPlaying) {
      const interval = setInterval(() => {
        setCurrentLyricIndex((prev) => (prev + 1) % lyrics.length);
      }, 3000);
      return () => clearInterval(interval);
    }
  }, [isPlaying, lyrics.length]);

  const togglePlay = () => {
    const audio = audioRef.current;
    if (!audio) return;

    if (isPlaying) {
      audio.pause();
    } else {
      // Since we don't have an actual audio file, we'll simulate playing
      setCurrentTime(0);
      setDuration(180); // 3 minutes
    }
    setIsPlaying(!isPlaying);
  };

  const toggleMute = () => {
    const audio = audioRef.current;
    if (audio) {
      audio.muted = !audio.muted;
    }
    setIsMuted(!isMuted);
  };

  const formatTime = (time: number) => {
    const minutes = Math.floor(time / 60);
    const seconds = Math.floor(time % 60);
    return `${minutes}:${seconds.toString().padStart(2, '0')}`;
  };

  return (
    <div className="fixed bottom-4 left-4 z-50">
      <motion.div
        initial={{ scale: 0, rotate: -180 }}
        animate={{ scale: 1, rotate: 0 }}
        className="bg-gradient-to-br from-orange-900/95 to-red-900/95 backdrop-blur-sm rounded-lg border border-orange-500/30 p-4 shadow-2xl max-w-sm"
      >
        <div className="flex items-center gap-2 mb-3">
          <Music className="w-4 h-4 text-orange-300" />
          <span className="text-xs text-orange-200 font-semibold">
            🎵 The Shit Anthem • Official Theme Song
          </span>
        </div>

        {/* Lyrics Display */}
        {isPlaying && (
          <motion.div
            key={currentLyricIndex}
            initial={{ opacity: 0, y: 10 }}
            animate={{ opacity: 1, y: 0 }}
            exit={{ opacity: 0, y: -10 }}
            className="mb-3 p-2 bg-orange-800/30 rounded-md border border-orange-500/20"
          >
            <p className="text-orange-100 text-sm text-center font-medium">
              {lyrics[currentLyricIndex]}
            </p>
          </motion.div>
        )}

        {/* Player Controls */}
        <div className="space-y-3">
          <div className="flex items-center gap-3">
            <button
              onClick={togglePlay}
              className="bg-orange-600 hover:bg-orange-700 text-white p-2 rounded-full transition-colors"
            >
              {isPlaying ? <Pause className="w-4 h-4" /> : <Play className="w-4 h-4" />}
            </button>
            
            <button
              onClick={toggleMute}
              className="bg-orange-700/50 hover:bg-orange-600/50 text-orange-200 p-2 rounded-full transition-colors"
            >
              {isMuted ? <VolumeX className="w-4 h-4" /> : <Volume2 className="w-4 h-4" />}
            </button>

            <div className="flex-1 text-xs text-orange-300">
              {formatTime(currentTime)} / {formatTime(duration)}
            </div>
          </div>

          {/* Progress Bar */}
          <div className="w-full bg-orange-800/30 rounded-full h-2">
            <motion.div
              className="bg-gradient-to-r from-orange-500 to-red-500 h-2 rounded-full"
              style={{
                width: duration > 0 ? `${(currentTime / duration) * 100}%` : '0%'
              }}
              animate={isPlaying ? { width: '100%' } : {}}
              transition={{ duration: duration, ease: 'linear' }}
            />
          </div>

          <div className="text-xs text-orange-300 text-center">
            🎼 Composed by the GitHub Community • Performed by 2.8K Stars
          </div>
        </div>
      </motion.div>

      {/* Hidden audio element for future implementation */}
      <audio
        ref={audioRef}
        loop
        // src="/path/to/shit-anthem.mp3" // Add actual audio file here
      />
    </div>
  );
};
