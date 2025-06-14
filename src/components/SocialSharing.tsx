import React, { useState } from 'react';
import { motion, AnimatePresence } from 'framer-motion';

interface ShareButtonProps {
  platform: string;
  url: string;
  text: string;
  icon: string;
  color: string;
}

const ShareButton: React.FC<ShareButtonProps> = ({ url, icon, color }) => {
  const [isShared, setIsShared] = useState(false);

  const handleShare = () => {
    window.open(url, '_blank', 'width=600,height=400');
    setIsShared(true);
    
    // Update analytics
    if ((window as any).updateShitStats) {
      (window as any).updateShitStats('shitstorms', 1);
    }

    setTimeout(() => setIsShared(false), 2000);
  };

  return (
    <motion.button
      onClick={handleShare}
      className={`relative p-3 rounded-full ${color} text-white hover:scale-110 transition-all duration-300`}
      whileHover={{ scale: 1.1 }}
      whileTap={{ scale: 0.95 }}
    >
      <span className="text-xl">{icon}</span>
      
      <AnimatePresence>
        {isShared && (
          <motion.div
            className="absolute -top-8 left-1/2 transform -translate-x-1/2 bg-green-500 text-white px-2 py-1 rounded text-xs whitespace-nowrap"
            initial={{ opacity: 0, y: 10 }}
            animate={{ opacity: 1, y: 0 }}
            exit={{ opacity: 0, y: -10 }}
          >
            Shared! 💩
          </motion.div>
        )}
      </AnimatePresence>
    </motion.button>
  );
};

export const SocialSharing: React.FC = () => {
  const [isVisible, setIsVisible] = useState(false);
  const [isExpanded, setIsExpanded] = useState(false);

  const baseUrl = window.location.href;
  const shareText = "🔥 Holy Shit! I found GitHub's BILLIONTH repository! 💩 This is the most legendary repo in coding history! #GitHubBillion #LegendaryShit";

  const shareButtons = [
    {
      platform: 'Twitter',
      url: `https://twitter.com/intent/tweet?text=${encodeURIComponent(shareText)}&url=${encodeURIComponent(baseUrl)}`,
      text: shareText,
      icon: '🐦',
      color: 'bg-blue-400 hover:bg-blue-500'
    },
    {
      platform: 'LinkedIn',
      url: `https://www.linkedin.com/sharing/share-offsite/?url=${encodeURIComponent(baseUrl)}`,
      text: shareText,
      icon: '💼',
      color: 'bg-blue-600 hover:bg-blue-700'
    },
    {
      platform: 'Reddit',
      url: `https://reddit.com/submit?url=${encodeURIComponent(baseUrl)}&title=${encodeURIComponent(shareText)}`,
      text: shareText,
      icon: '🤖',
      color: 'bg-orange-500 hover:bg-orange-600'
    },
    {
      platform: 'Facebook',
      url: `https://www.facebook.com/sharer/sharer.php?u=${encodeURIComponent(baseUrl)}`,
      text: shareText,
      icon: '📘',
      color: 'bg-blue-500 hover:bg-blue-600'
    },
    {
      platform: 'Copy Link',
      url: '#',
      text: shareText,
      icon: '🔗',
      color: 'bg-gray-600 hover:bg-gray-700'
    }
  ];

  const handleCopyLink = async () => {
    try {
      await navigator.clipboard.writeText(baseUrl);
      // Show success message
    } catch (err) {
      // Fallback for older browsers
      const textArea = document.createElement('textarea');
      textArea.value = baseUrl;
      document.body.appendChild(textArea);
      textArea.select();
      document.execCommand('copy');
      document.body.removeChild(textArea);
    }
  };

  // Show sharing options after user has been on page for 10 seconds
  React.useEffect(() => {
    const timer = setTimeout(() => setIsVisible(true), 10000);
    return () => clearTimeout(timer);
  }, []);

  if (!isVisible) return null;

  return (
    <motion.div
      className="fixed left-4 top-1/2 transform -translate-y-1/2 z-40"
      initial={{ opacity: 0, x: -100 }}
      animate={{ opacity: 1, x: 0 }}
      transition={{ delay: 0.5 }}
    >
      <motion.div className="flex flex-col gap-3">
        {/* Main Share Button */}
        <motion.button
          onClick={() => setIsExpanded(!isExpanded)}
          className="bg-shit-gold text-black p-3 rounded-full hover:scale-110 transition-all duration-300 shadow-lg"
          whileHover={{ scale: 1.1, rotate: 15 }}
          whileTap={{ scale: 0.95 }}
        >
          <span className="text-xl">📢</span>
        </motion.button>

        {/* Expanded Share Options */}
        <AnimatePresence>
          {isExpanded && (
            <motion.div
              className="flex flex-col gap-2"
              initial={{ opacity: 0, scale: 0.8 }}
              animate={{ opacity: 1, scale: 1 }}
              exit={{ opacity: 0, scale: 0.8 }}
              transition={{ duration: 0.3 }}
            >
              {shareButtons.map((button, index) => (
                <motion.div
                  key={button.platform}
                  initial={{ opacity: 0, x: -50 }}
                  animate={{ opacity: 1, x: 0 }}
                  transition={{ delay: index * 0.1 }}
                >
                  {button.platform === 'Copy Link' ? (
                    <motion.button
                      onClick={handleCopyLink}
                      className={`relative p-3 rounded-full ${button.color} text-white hover:scale-110 transition-all duration-300`}
                      whileHover={{ scale: 1.1 }}
                      whileTap={{ scale: 0.95 }}
                    >
                      <span className="text-xl">{button.icon}</span>
                    </motion.button>
                  ) : (
                    <ShareButton {...button} />
                  )}
                </motion.div>
              ))}
            </motion.div>
          )}
        </AnimatePresence>

        {/* Share Hint */}
        {!isExpanded && (
          <motion.div
            className="absolute left-16 top-0 bg-black/80 text-shit-gold px-3 py-2 rounded-lg text-sm whitespace-nowrap"
            initial={{ opacity: 0, x: -20 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ delay: 1 }}
          >
            Share the legendary shit! 💩
            <div className="absolute left-0 top-1/2 transform -translate-y-1/2 -translate-x-1 w-2 h-2 bg-black/80 rotate-45"></div>
          </motion.div>
        )}
      </motion.div>
    </motion.div>
  );
};