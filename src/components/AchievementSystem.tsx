import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';

interface Achievement {
  id: string;
  title: string;
  description: string;
  icon: string;
  unlocked: boolean;
  timestamp?: number;
  rarity: 'common' | 'rare' | 'epic' | 'legendary';
}

const ACHIEVEMENTS: Achievement[] = [
  {
    id: 'first_visit',
    title: 'Holy Shit!',
    description: 'You discovered the legendary repository',
    icon: '💩',
    unlocked: false,
    rarity: 'common'
  },
  {
    id: 'fireworks_master',
    title: 'Fireworks Master',
    description: 'Triggered 50 firework explosions',
    icon: '🎆',
    unlocked: false,
    rarity: 'rare'
  },
  {
    id: 'dino_player',
    title: 'Shit Dino Champion',
    description: 'Found and played the hidden dino game',
    icon: '🦕',
    unlocked: false,
    rarity: 'epic'
  },
  {
    id: 'console_hacker',
    title: 'Console Hacker',
    description: 'Used console commands like a true dev',
    icon: '👨‍💻',
    unlocked: false,
    rarity: 'rare'
  },
  {
    id: 'konami_master',
    title: 'Konami Code Master',
    description: 'Entered the legendary Konami code',
    icon: '🎮',
    unlocked: false,
    rarity: 'epic'
  },
  {
    id: 'share_legend',
    title: 'Shit Spreader',
    description: 'Shared the legendary repository',
    icon: '📢',
    unlocked: false,
    rarity: 'common'
  },
  {
    id: 'time_traveler',
    title: 'Time Traveler',
    description: 'Spent over 5 minutes exploring',
    icon: '⏰',
    unlocked: false,
    rarity: 'rare'
  },
  {
    id: 'secret_typer',
    title: 'Secret Message Master',
    description: 'Found the hidden typing secret',
    icon: '🔮',
    unlocked: false,
    rarity: 'epic'
  },
  {
    id: 'explosion_maniac',
    title: 'Explosion Maniac',
    description: 'Triggered 100+ explosions',
    icon: '💥',
    unlocked: false,
    rarity: 'legendary'
  },
  {
    id: 'completionist',
    title: 'The Completionist',
    description: 'Unlocked all other achievements',
    icon: '👑',
    unlocked: false,
    rarity: 'legendary'
  }
];

export const AchievementSystem: React.FC = () => {
  const [achievements, setAchievements] = useState<Achievement[]>(ACHIEVEMENTS);
  const [newUnlock, setNewUnlock] = useState<Achievement | null>(null);
  const [showPanel, setShowPanel] = useState(false);

  // Load achievements from localStorage
  useEffect(() => {
    const saved = localStorage.getItem('shit-achievements');
    if (saved) {
      const savedAchievements = JSON.parse(saved);
      setAchievements(prevAchievements => 
        prevAchievements.map(achievement => {
          const savedAchievement = savedAchievements.find((s: Achievement) => s.id === achievement.id);
          return savedAchievement ? { ...achievement, ...savedAchievement } : achievement;
        })
      );
    } else {
      // First visit achievement
      unlockAchievement('first_visit');
    }

    // Time traveler achievement
    const timeCheck = setTimeout(() => {
      unlockAchievement('time_traveler');
    }, 5 * 60 * 1000); // 5 minutes

    return () => clearTimeout(timeCheck);
  }, []);

  const unlockAchievement = (achievementId: string) => {
    setAchievements(prev => {
      const newAchievements = prev.map(achievement => {
        if (achievement.id === achievementId && !achievement.unlocked) {
          const unlockedAchievement = {
            ...achievement,
            unlocked: true,
            timestamp: Date.now()
          };
          
          // Show notification
          setNewUnlock(unlockedAchievement);
          setTimeout(() => setNewUnlock(null), 4000);

          return unlockedAchievement;
        }
        return achievement;
      });

      // Save to localStorage
      localStorage.setItem('shit-achievements', JSON.stringify(newAchievements));
      
      // Check for completionist achievement
      const unlockedCount = newAchievements.filter(a => a.unlocked && a.id !== 'completionist').length;
      const totalCount = newAchievements.filter(a => a.id !== 'completionist').length;
      
      if (unlockedCount === totalCount) {
        setTimeout(() => unlockAchievement('completionist'), 500);
      }

      return newAchievements;
    });
  };

  // Expose unlock function globally
  useEffect(() => {
    (window as any).unlockShitAchievement = unlockAchievement;
  }, []);

  const getRarityColor = (rarity: Achievement['rarity']) => {
    switch (rarity) {
      case 'common': return 'border-gray-500 text-gray-300';
      case 'rare': return 'border-blue-500 text-blue-300';
      case 'epic': return 'border-purple-500 text-purple-300';
      case 'legendary': return 'border-yellow-500 text-yellow-300';
    }
  };

  const unlockedCount = achievements.filter(a => a.unlocked).length;
  const progress = (unlockedCount / achievements.length) * 100;

  return (
    <>
      {/* Achievement Button */}
      <motion.button
        onClick={() => setShowPanel(true)}
        className="fixed top-4 right-4 z-40 bg-black/90 border border-shit-gold p-3 rounded-lg hover:scale-105 transition-all"
        whileHover={{ scale: 1.05 }}
        whileTap={{ scale: 0.95 }}
      >
        <div className="text-shit-gold text-xl">🏆</div>
        <div className="text-xs text-shit-gold">{unlockedCount}/{achievements.length}</div>
      </motion.button>

      {/* Achievement Panel */}
      <AnimatePresence>
        {showPanel && (
          <motion.div
            className="fixed inset-0 z-50 bg-black/80 backdrop-blur-sm flex items-center justify-center p-4"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            onClick={() => setShowPanel(false)}
          >
            <motion.div
              className="bg-black border border-terminal-green rounded-lg p-6 max-w-2xl w-full max-h-[80vh] overflow-y-auto"
              initial={{ scale: 0.8, opacity: 0 }}
              animate={{ scale: 1, opacity: 1 }}
              exit={{ scale: 0.8, opacity: 0 }}
              onClick={(e) => e.stopPropagation()}
            >
              <div className="flex justify-between items-center mb-6">
                <h2 className="text-2xl font-terminal text-shit-gold">🏆 Achievements</h2>
                <button
                  onClick={() => setShowPanel(false)}
                  className="text-gray-400 hover:text-white text-2xl"
                >
                  ×
                </button>
              </div>

              {/* Progress Bar */}
              <div className="mb-6">
                <div className="flex justify-between text-sm text-gray-400 mb-2">
                  <span>Progress</span>
                  <span>{unlockedCount}/{achievements.length} ({Math.round(progress)}%)</span>
                </div>
                <div className="w-full bg-gray-800 rounded-full h-3">
                  <motion.div
                    className="h-full bg-gradient-to-r from-terminal-green to-shit-gold rounded-full"
                    initial={{ width: 0 }}
                    animate={{ width: `${progress}%` }}
                    transition={{ duration: 1, ease: "easeOut" }}
                  />
                </div>
              </div>

              {/* Achievement Grid */}
              <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
                {achievements.map((achievement, index) => (
                  <motion.div
                    key={achievement.id}
                    className={`p-4 rounded-lg border-2 ${
                      achievement.unlocked
                        ? `${getRarityColor(achievement.rarity)} bg-gray-900/50`
                        : 'border-gray-700 text-gray-500 bg-gray-900/20'
                    }`}
                    initial={{ opacity: 0, y: 20 }}
                    animate={{ opacity: 1, y: 0 }}
                    transition={{ delay: index * 0.1 }}
                  >
                    <div className="flex items-center gap-3">
                      <div className={`text-3xl ${achievement.unlocked ? '' : 'grayscale'}`}>
                        {achievement.icon}
                      </div>
                      <div className="flex-1">
                        <h3 className={`font-bold ${achievement.unlocked ? 'text-white' : 'text-gray-600'}`}>
                          {achievement.title}
                        </h3>
                        <p className="text-sm opacity-80">{achievement.description}</p>
                        {achievement.unlocked && achievement.timestamp && (
                          <p className="text-xs text-gray-400 mt-1">
                            Unlocked: {new Date(achievement.timestamp).toLocaleDateString()}
                          </p>
                        )}
                      </div>
                    </div>
                  </motion.div>
                ))}
              </div>
            </motion.div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* New Achievement Notification */}
      <AnimatePresence>
        {newUnlock && (
          <motion.div
            className="fixed top-20 right-4 z-50 bg-black border-2 border-shit-gold rounded-lg p-4 max-w-sm"
            initial={{ opacity: 0, x: 100, scale: 0.8 }}
            animate={{ opacity: 1, x: 0, scale: 1 }}
            exit={{ opacity: 0, x: 100, scale: 0.8 }}
          >
            <div className="text-shit-gold font-bold text-sm mb-2">🎉 ACHIEVEMENT UNLOCKED!</div>
            <div className="flex items-center gap-3">
              <div className="text-2xl">{newUnlock.icon}</div>
              <div>
                <div className="text-white font-bold">{newUnlock.title}</div>
                <div className="text-gray-300 text-sm">{newUnlock.description}</div>
              </div>
            </div>
          </motion.div>
        )}
      </AnimatePresence>
    </>
  );
};