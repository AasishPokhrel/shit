import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';

export const HelpOverlay: React.FC = () => {
  const [showHelp, setShowHelp] = useState(false);
  const [currentStep, setCurrentStep] = useState(0);

  const helpSteps = [
    {
      title: "Welcome to GitHub's Billionth Repository! 💩",
      content: "This is the most legendary repository in coding history. Let's explore what you can do!",
      position: "center"
    },
    {
      title: "Earn Achievements 🏆",
      content: "Click interactions, use console commands, and discover secrets to unlock 10 achievements!",
      position: "top-right",
      highlight: ".achievement-button"
    },
    {
      title: "Trigger Fireworks 🎆",
      content: "Click on buttons, emojis, or use Ctrl+Click anywhere to create explosive celebrations!",
      position: "center"
    },
    {
      title: "Check Your Stats 📊",
      content: "See your visit count, achievements, and engagement metrics in real-time!",
      position: "bottom-right",
      highlight: ".visitor-analytics"
    },
    {
      title: "Share the Legend 📢",
      content: "Spread the word about this historic milestone across social media!",
      position: "left",
      highlight: ".social-sharing"
    },
    {
      title: "Hidden Secrets 🎮",
      content: "Try typing 'shit', the Konami code (↑↑↓↓←→←→BA), or open console (F12) for more!",
      position: "center"
    },
    {
      title: "Mobile Features 📱",
      content: "On mobile? Swipe up for fireworks, double-tap for explosions, and use the quick action buttons!",
      position: "bottom"
    }
  ];

  useEffect(() => {
    // Show help for first-time visitors
    const hasSeenHelp = localStorage.getItem('shit-help-seen');
    if (!hasSeenHelp) {
      setTimeout(() => setShowHelp(true), 3000); // Show after loading
    }

    // Keyboard shortcut to show help
    const handleKeyPress = (e: KeyboardEvent) => {
      if (e.key === 'F1' || (e.key === '?' && e.shiftKey)) {
        e.preventDefault();
        setShowHelp(true);
        setCurrentStep(0);
      }
    };

    window.addEventListener('keydown', handleKeyPress);
    return () => window.removeEventListener('keydown', handleKeyPress);
  }, []);

  const nextStep = () => {
    if (currentStep < helpSteps.length - 1) {
      setCurrentStep(currentStep + 1);
    } else {
      closeHelp();
    }
  };

  const prevStep = () => {
    if (currentStep > 0) {
      setCurrentStep(currentStep - 1);
    }
  };

  const closeHelp = () => {
    setShowHelp(false);
    setCurrentStep(0);
    localStorage.setItem('shit-help-seen', 'true');
    
    // Unlock help achievement
    if ((window as any).unlockShitAchievement) {
      (window as any).unlockShitAchievement('help_reader');
    }
  };

  const skipTour = () => {
    setCurrentStep(helpSteps.length - 1);
  };

  // Expose help function globally
  useEffect(() => {
    (window as any).shitHelp = () => {
      setShowHelp(true);
      setCurrentStep(0);
    };
  }, []);

  if (!showHelp) {
    return (
      // Help button for returning users
      <motion.button
        onClick={() => setShowHelp(true)}
        className="fixed bottom-4 right-20 z-40 bg-blue-600 text-white p-3 rounded-full shadow-lg hover:bg-blue-700 transition-colors"
        whileHover={{ scale: 1.1 }}
        whileTap={{ scale: 0.95 }}
        title="Help & Tutorial (F1 or Shift+?)"
      >
        <span className="text-lg">❓</span>
      </motion.button>
    );
  }

  const currentStepData = helpSteps[currentStep];

  return (
    <AnimatePresence>
      <motion.div
        className="fixed inset-0 z-[100] bg-black/70 backdrop-blur-sm"
        initial={{ opacity: 0 }}
        animate={{ opacity: 1 }}
        exit={{ opacity: 0 }}
      >
        {/* Help Modal */}
        <motion.div
          className={`fixed ${getPositionClass(currentStepData.position)} max-w-md w-full mx-4`}
          initial={{ scale: 0.8, opacity: 0 }}
          animate={{ scale: 1, opacity: 1 }}
          exit={{ scale: 0.8, opacity: 0 }}
        >
          <div className="bg-black border-2 border-shit-gold rounded-lg p-6 shadow-xl">
            {/* Header */}
            <div className="flex justify-between items-start mb-4">
              <h3 className="text-xl font-terminal text-shit-gold">
                {currentStepData.title}
              </h3>
              <button
                onClick={closeHelp}
                className="text-gray-400 hover:text-white text-xl"
              >
                ×
              </button>
            </div>

            {/* Content */}
            <p className="text-gray-300 mb-6 leading-relaxed">
              {currentStepData.content}
            </p>

            {/* Progress */}
            <div className="mb-6">
              <div className="flex justify-between text-sm text-gray-400 mb-2">
                <span>Step {currentStep + 1} of {helpSteps.length}</span>
                <span>{Math.round(((currentStep + 1) / helpSteps.length) * 100)}%</span>
              </div>
              <div className="w-full bg-gray-800 rounded-full h-2">
                <motion.div
                  className="h-full bg-gradient-to-r from-terminal-green to-shit-gold rounded-full"
                  initial={{ width: "0%" }}
                  animate={{ width: `${((currentStep + 1) / helpSteps.length) * 100}%` }}
                  transition={{ duration: 0.3 }}
                />
              </div>
            </div>

            {/* Controls */}
            <div className="flex justify-between items-center">
              <div className="flex gap-2">
                {currentStep > 0 && (
                  <button
                    onClick={prevStep}
                    className="px-4 py-2 bg-gray-700 text-white rounded hover:bg-gray-600 transition-colors"
                  >
                    Previous
                  </button>
                )}
                {currentStep < helpSteps.length - 2 && (
                  <button
                    onClick={skipTour}
                    className="px-4 py-2 text-gray-400 hover:text-white transition-colors"
                  >
                    Skip Tour
                  </button>
                )}
              </div>

              <button
                onClick={nextStep}
                className="px-6 py-2 bg-shit-gold text-black rounded font-bold hover:bg-yellow-400 transition-colors"
              >
                {currentStep === helpSteps.length - 1 ? 'Get Started!' : 'Next'}
              </button>
            </div>
          </div>
        </motion.div>

        {/* Highlight overlay for specific elements */}
        {currentStepData.highlight && (
          <motion.div
            className="absolute inset-0 pointer-events-none"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
          >
            {/* This would highlight specific elements - simplified for this implementation */}
            <div className="absolute top-4 right-4 w-16 h-16 border-2 border-shit-gold rounded-full animate-pulse" />
          </motion.div>
        )}

        {/* Keyboard shortcuts hint */}
        <div className="fixed bottom-4 left-1/2 transform -translate-x-1/2 text-center text-sm text-gray-400">
          <p>Press F1 or Shift+? anytime for help • ESC to close</p>
        </div>
      </motion.div>
    </AnimatePresence>
  );
};

function getPositionClass(position: string): string {
  switch (position) {
    case 'top-right':
      return 'top-4 right-4';
    case 'bottom-right':
      return 'bottom-4 right-4';
    case 'left':
      return 'top-1/2 left-4 transform -translate-y-1/2';
    case 'bottom':
      return 'bottom-4 left-1/2 transform -translate-x-1/2';
    case 'center':
    default:
      return 'top-1/2 left-1/2 transform -translate-x-1/2 -translate-y-1/2';
  }
}