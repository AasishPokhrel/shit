import React, { useEffect, useState } from 'react';
import { motion, AnimatePresence } from 'framer-motion';

interface LoadingScreenProps {
  onComplete: () => void;
}

export const LoadingScreen: React.FC<LoadingScreenProps> = ({ onComplete }) => {
  const [loadingText, setLoadingText] = useState('');
  const [progress, setProgress] = useState(0);
  const loadingMessages = [
    'Initializing shit...',
    'Loading legendary repository...',
    'Counting to 1 billion...',
    'Activating fireworks...',
    'Preparing matrix rain...',
    'Shit loading complete!'
  ];

  useEffect(() => {
    let messageIndex = 0;
    let currentProgress = 0;

    const interval = setInterval(() => {
      currentProgress += Math.random() * 20;
      if (currentProgress > 100) {
        currentProgress = 100;
        setTimeout(() => {
          onComplete();
        }, 500);
        clearInterval(interval);
      }
      
      setProgress(currentProgress);
      
      if (messageIndex < loadingMessages.length - 1) {
        setLoadingText(loadingMessages[Math.min(messageIndex, loadingMessages.length - 1)]);
        messageIndex = Math.floor((currentProgress / 100) * loadingMessages.length);
      }
    }, 300);

    return () => clearInterval(interval);
  }, [onComplete]);

  return (
    <AnimatePresence>
      <motion.div
        className="fixed inset-0 z-50 bg-black flex items-center justify-center"
        initial={{ opacity: 1 }}
        exit={{ opacity: 0 }}
        transition={{ duration: 0.5 }}
      >
        <div className="text-center max-w-md mx-auto p-8">
          {/* Animated Shit Logo */}
          <motion.div
            className="text-8xl mb-8"
            animate={{
              rotate: [0, 360],
              scale: [1, 1.2, 1]
            }}
            transition={{
              duration: 2,
              repeat: Infinity,
              ease: "easeInOut"
            }}
          >
            💩
          </motion.div>

          {/* Loading Text */}
          <motion.h2
            className="text-2xl font-terminal text-shit-gold mb-6"
            initial={{ opacity: 0, y: 20 }}
            animate={{ opacity: 1, y: 0 }}
            key={loadingText}
          >
            {loadingText}
          </motion.h2>

          {/* Progress Bar */}
          <div className="w-full bg-gray-800 rounded-full h-3 mb-4 overflow-hidden">
            <motion.div
              className="h-full bg-gradient-to-r from-terminal-green to-shit-gold"
              initial={{ width: "0%" }}
              animate={{ width: `${progress}%` }}
              transition={{ duration: 0.3, ease: "easeOut" }}
            />
          </div>

          {/* Progress Percentage */}
          <motion.div
            className="text-terminal-green font-mono text-lg"
            animate={{ opacity: [0.5, 1, 0.5] }}
            transition={{ duration: 1, repeat: Infinity }}
          >
            {Math.floor(progress)}%
          </motion.div>

          {/* Repository Counter Animation */}
          <motion.div
            className="mt-6 text-sm text-gray-400"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            transition={{ delay: 1 }}
          >
            Repository #{(progress * 10000000).toLocaleString()}
          </motion.div>
        </div>
      </motion.div>
    </AnimatePresence>
  );
};