import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';

const COMMUNITY_MESSAGES = [
  { id: '431', message: '慕名前来打卡', language: 'Chinese', flag: '🇨🇳' },
  { id: '430', message: "Shit's gone!", language: 'English', flag: '🇺🇸' },
  { id: '427', message: 'awesome', language: 'English', flag: '🇺🇸' },
  { id: '424', message: 'Awesome Shit', language: 'English', flag: '🇺🇸' },
  { id: '423', message: '接！', language: 'Chinese', flag: '🇨🇳' },
  { id: '422', message: "Also available on Shithub - don't Google that 💩", language: 'English', flag: '🇺🇸' },
  { id: '421', message: 'Anyway, great SHIT!', language: 'English', flag: '🇺🇸' },
  { id: '420', message: '打卡', language: 'Chinese', flag: '🇨🇳' },
  { id: '419', message: '名留青史', language: 'Chinese', flag: '🇨🇳' },
  { id: '418', message: '火钳刘明', language: 'Chinese', flag: '🇨🇳' },
  { id: '417', message: '大家都来都来留下一根脚毛', language: 'Chinese', flag: '🇨🇳' },
  { id: '416', message: 'wow，a nice shit!', language: 'English', flag: '🇺🇸' },
  { id: '414', message: '史前留名', language: 'Chinese', flag: '🇨🇳' },
  { id: '413', message: 'This is really the shit project in Github!!!', language: 'English', flag: '🇺🇸' },
  { id: '412', message: '合影', language: 'Chinese', flag: '🇨🇳' },
  { id: '411', message: '合影', language: 'Chinese', flag: '🇨🇳' },
  { id: '410', message: 'Чи можна написати переклад на українську мову?', language: 'Ukrainian', flag: '🇺🇦' },
  { id: '408', message: '排泄点合影', language: 'Chinese', flag: '🇨🇳' },
  { id: '407', message: '合影', language: 'Chinese', flag: '🇨🇳' },
  { id: '404', message: '我是史学家，这就是历史性的一个项目', language: 'Chinese', flag: '🇨🇳' },
];

export const CommunityGuestbook: React.FC = () => {
  const [currentMessage, setCurrentMessage] = useState(0);
  const [isVisible, setIsVisible] = useState(true);

  useEffect(() => {
    const interval = setInterval(() => {
      setIsVisible(false);
      setTimeout(() => {
        setCurrentMessage((prev) => (prev + 1) % COMMUNITY_MESSAGES.length);
        setIsVisible(true);
      }, 300);
    }, 4000);

    return () => clearInterval(interval);
  }, []);

  const message = COMMUNITY_MESSAGES[currentMessage];

  return (
    <div className="fixed bottom-4 right-4 z-50 max-w-sm">
      <div className="bg-gradient-to-br from-purple-900/95 to-pink-900/95 backdrop-blur-sm rounded-lg border border-purple-500/30 p-4 shadow-2xl">
        <div className="flex items-center gap-2 mb-2">
          <div className="w-2 h-2 bg-green-400 rounded-full animate-pulse"></div>
          <span className="text-xs text-purple-200 font-semibold">
            Community Guestbook • Live from GitHub
          </span>
        </div>
        
        <AnimatePresence mode="wait">
          {isVisible && (
            <motion.div
              key={message.id}
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              exit={{ opacity: 0, y: -20 }}
              transition={{ duration: 0.3 }}
              className="space-y-2"
            >
              <div className="flex items-center gap-2">
                <span className="text-lg">{message.flag}</span>
                <span className="text-xs text-purple-300">Issue #{message.id}</span>
                <span className="text-xs bg-purple-700/50 px-2 py-0.5 rounded-full text-purple-200">
                  {message.language}
                </span>
              </div>
              
              <div className="text-white text-sm font-medium leading-relaxed">
                "{message.message}"
              </div>
              
              <div className="flex justify-between items-center text-xs text-purple-300">
                <span>{currentMessage + 1} of {COMMUNITY_MESSAGES.length}</span>
                <span>💩 {Math.floor(Math.random() * 50) + 10} reactions</span>
              </div>
            </motion.div>
          )}
        </AnimatePresence>
        
        <div className="mt-3 flex gap-2">
          <button className="flex-1 bg-purple-600 hover:bg-purple-700 text-white text-xs px-3 py-1.5 rounded-md transition-colors">
            Add Your Message
          </button>
          <button className="bg-purple-700/50 hover:bg-purple-600/50 text-purple-200 text-xs px-2 py-1.5 rounded-md transition-colors">
            💩
          </button>
        </div>
      </div>
    </div>
  );
};
