import React, { useEffect, useState } from "react";
import { motion, AnimatePresence } from "framer-motion";

interface SurpriseElement {
  id: string;
  type: 'cursor' | 'shake' | 'confetti' | 'glitch' | 'zoom' | 'rainbow';
  x?: number;
  y?: number;
  duration: number;
}

export const SurpriseElements: React.FC = () => {
  const [activeElements, setActiveElements] = useState<SurpriseElement[]>([]);
  const [cursorEmoji, setCursorEmoji] = useState<string>('');
  const [showSecretMessage, setShowSecretMessage] = useState(false);

  const shitEmojis = ['💩', '🔥', '⚡', '🚀', '💥', '✨', '🌟', '💫', '🎉', '🦄'];

  // Random surprise triggers
  useEffect(() => {
    const triggerRandomSurprise = () => {
      const surprises = [
        'cursor',
        'shake',
        'confetti',
        'glitch',
        'zoom',
        'rainbow'
      ];

      const randomSurprise = surprises[Math.floor(Math.random() * surprises.length)];
      
      const newElement: SurpriseElement = {
        id: Date.now().toString(),
        type: randomSurprise as any,
        duration: Math.random() * 3000 + 2000,
      };

      setActiveElements(prev => [...prev, newElement]);

      setTimeout(() => {
        setActiveElements(prev => prev.filter(el => el.id !== newElement.id));
      }, newElement.duration);
    };

    // Random surprise every 30-60 seconds
    const interval = setInterval(() => {
      if (Math.random() < 0.3) {
        triggerRandomSurprise();
      }
    }, 30000);

    return () => clearInterval(interval);
  }, []);

  // Cursor emoji trail
  useEffect(() => {
    let isActive = false;
    
    const handleMouseMove = (e: MouseEvent) => {
      if (!isActive) return;
      
      const trail = document.createElement('div');
      trail.textContent = cursorEmoji;
      trail.style.position = 'fixed';
      trail.style.left = e.clientX + 'px';
      trail.style.top = e.clientY + 'px';
      trail.style.pointerEvents = 'none';
      trail.style.fontSize = '20px';
      trail.style.zIndex = '1000';
      trail.style.animation = 'fade-out 1s ease-out forwards';
      
      const style = document.createElement('style');
      style.textContent = `
        @keyframes fade-out {
          0% { opacity: 1; transform: scale(1); }
          100% { opacity: 0; transform: scale(0.5) rotate(180deg); }
        }
      `;
      if (!document.head.querySelector('[data-fade-out]')) {
        style.setAttribute('data-fade-out', 'true');
        document.head.appendChild(style);
      }
      
      document.body.appendChild(trail);
      setTimeout(() => trail.remove(), 1000);
    };

    activeElements.forEach(element => {
      if (element.type === 'cursor') {
        isActive = true;
        setCursorEmoji(shitEmojis[Math.floor(Math.random() * shitEmojis.length)]);
        document.addEventListener('mousemove', handleMouseMove);
      }
    });

    return () => {
      document.removeEventListener('mousemove', handleMouseMove);
      if (!activeElements.some(el => el.type === 'cursor')) {
        setCursorEmoji('');
      }
    };
  }, [activeElements, cursorEmoji]);

  // Screen shake effect
  useEffect(() => {
    const shouldShake = activeElements.some(el => el.type === 'shake');
    
    if (shouldShake) {
      document.body.style.animation = 'shake 0.5s ease-in-out infinite';
      const shakeStyle = document.createElement('style');
      shakeStyle.textContent = `
        @keyframes shake {
          0%, 100% { transform: translateX(0); }
          25% { transform: translateX(-5px) rotate(1deg); }
          75% { transform: translateX(5px) rotate(-1deg); }
        }
      `;
      if (!document.head.querySelector('[data-shake]')) {
        shakeStyle.setAttribute('data-shake', 'true');
        document.head.appendChild(shakeStyle);
      }
    } else {
      document.body.style.animation = '';
    }
  }, [activeElements]);

  // Glitch effect
  useEffect(() => {
    const shouldGlitch = activeElements.some(el => el.type === 'glitch');
    
    if (shouldGlitch) {
      document.body.style.filter = 'hue-rotate(90deg) contrast(150%)';
      document.body.style.animation = 'glitch-shake 0.3s ease-in-out infinite';
      
      const glitchStyle = document.createElement('style');
      glitchStyle.textContent = `
        @keyframes glitch-shake {
          0%, 100% { transform: translate(0); filter: hue-rotate(0deg); }
          20% { transform: translate(-2px, 2px); filter: hue-rotate(90deg); }
          40% { transform: translate(-2px, -2px); filter: hue-rotate(180deg); }
          60% { transform: translate(2px, 2px); filter: hue-rotate(270deg); }
          80% { transform: translate(2px, -2px); filter: hue-rotate(360deg); }
        }
      `;
      if (!document.head.querySelector('[data-glitch]')) {
        glitchStyle.setAttribute('data-glitch', 'true');
        document.head.appendChild(glitchStyle);
      }
    } else {
      document.body.style.filter = '';
    }
  }, [activeElements]);

  // Zoom effect
  useEffect(() => {
    const shouldZoom = activeElements.some(el => el.type === 'zoom');
    
    if (shouldZoom) {
      document.body.style.transform = 'scale(1.1)';
      document.body.style.transition = 'transform 0.5s ease-in-out';
    } else {
      document.body.style.transform = '';
    }
  }, [activeElements]);

  // Rainbow effect
  useEffect(() => {
    const shouldRainbow = activeElements.some(el => el.type === 'rainbow');
    
    if (shouldRainbow) {
      document.body.style.animation = 'rainbow-cycle 2s linear infinite';
      
      const rainbowStyle = document.createElement('style');
      rainbowStyle.textContent = `
        @keyframes rainbow-cycle {
          0% { filter: hue-rotate(0deg); }
          100% { filter: hue-rotate(360deg); }
        }
      `;
      if (!document.head.querySelector('[data-rainbow]')) {
        rainbowStyle.setAttribute('data-rainbow', 'true');
        document.head.appendChild(rainbowStyle);
      }
    }
  }, [activeElements]);

  // Secret message on special key combination
  useEffect(() => {
    let keySequence: string[] = [];
    const secretCode = ['s', 'h', 'i', 't'];
    
    const handleKeyPress = (e: KeyboardEvent) => {
      keySequence.push(e.key.toLowerCase());
      
      if (keySequence.length > secretCode.length) {
        keySequence = keySequence.slice(-secretCode.length);
      }
      
      if (keySequence.join('') === secretCode.join('')) {
        setShowSecretMessage(true);
        setTimeout(() => setShowSecretMessage(false), 3000);
        keySequence = [];
        
        // Trigger celebration
        if (window.shitFirework) {
          window.shitFirework();
        }
      }
    };

    window.addEventListener('keydown', handleKeyPress);
    return () => window.removeEventListener('keydown', handleKeyPress);
  }, []);

  // Double-click surprise
  useEffect(() => {
    let clickCount = 0;
    let clickTimer: number;

    const handleDoubleClick = (e: MouseEvent) => {
      clickCount++;
      
      if (clickCount === 1) {
        clickTimer = setTimeout(() => {
          clickCount = 0;
        }, 500);
      } else if (clickCount === 2) {
        clearTimeout(clickTimer);
        clickCount = 0;
        
        // Create explosion of emojis at click location
        const explosionCount = 8;
        for (let i = 0; i < explosionCount; i++) {
          const emoji = document.createElement('div');
          emoji.textContent = shitEmojis[Math.floor(Math.random() * shitEmojis.length)];
          emoji.style.position = 'fixed';
          emoji.style.left = e.clientX + 'px';
          emoji.style.top = e.clientY + 'px';
          emoji.style.pointerEvents = 'none';
          emoji.style.fontSize = '30px';
          emoji.style.zIndex = '1000';
          emoji.style.animation = `explode-${i} 1s ease-out forwards`;
          
          const angle = (360 / explosionCount) * i;
          const distance = 100;
          const x = Math.cos(angle * Math.PI / 180) * distance;
          const y = Math.sin(angle * Math.PI / 180) * distance;
          
          const style = document.createElement('style');
          style.textContent = `
            @keyframes explode-${i} {
              0% { transform: translate(0, 0) scale(1) rotate(0deg); opacity: 1; }
              100% { transform: translate(${x}px, ${y}px) scale(0) rotate(720deg); opacity: 0; }
            }
          `;
          document.head.appendChild(style);
          document.body.appendChild(emoji);
          
          setTimeout(() => {
            emoji.remove();
            style.remove();
          }, 1000);
        }
      }
    };

    document.addEventListener('click', handleDoubleClick);
    return () => document.removeEventListener('click', handleDoubleClick);
  }, []);

  return (
    <>
      {/* Secret Message */}
      <AnimatePresence>
        {showSecretMessage && (
          <motion.div
            initial={{ opacity: 0, scale: 0.5, y: 50 }}
            animate={{ opacity: 1, scale: 1, y: 0 }}
            exit={{ opacity: 0, scale: 0.5, y: -50 }}
            className="fixed top-1/2 left-1/2 transform -translate-x-1/2 -translate-y-1/2 z-50 bg-black border-4 border-terminal-green rounded-lg p-8 text-center"
          >
            <div className="text-4xl mb-4">🎉 SECRET UNLOCKED! 🎉</div>
            <div className="text-xl text-terminal-green font-terminal">
              You found the secret code!
            </div>
            <div className="text-lg text-shit-gold mt-2">
              💩 ULTIMATE SHIT MASTER 💩
            </div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Confetti Effect */}
      <AnimatePresence>
        {activeElements.filter(el => el.type === 'confetti').map(element => (
          <div key={element.id} className="fixed inset-0 pointer-events-none z-30">
            {Array.from({ length: 50 }, (_, i) => (
              <motion.div
                key={i}
                className="absolute text-2xl"
                initial={{
                  x: Math.random() * window.innerWidth,
                  y: -50,
                  rotate: 0,
                  scale: 1,
                }}
                animate={{
                  y: window.innerHeight + 50,
                  rotate: 360,
                  scale: [1, 1.5, 0.5],
                }}
                transition={{
                  duration: Math.random() * 2 + 3,
                  delay: Math.random() * 0.5,
                  ease: "easeOut",
                }}
              >
                {shitEmojis[Math.floor(Math.random() * shitEmojis.length)]}
              </motion.div>
            ))}
          </div>
        ))}
      </AnimatePresence>

      {/* Cursor Custom Style */}
      {cursorEmoji && (
        <style>
          {`
            * {
              cursor: none !important;
            }
            body::after {
              content: "${cursorEmoji}";
              position: fixed;
              pointer-events: none;
              z-index: 9999;
              font-size: 20px;
              animation: cursor-bounce 0.5s ease-in-out infinite alternate;
            }
            @keyframes cursor-bounce {
              0% { transform: scale(1); }
              100% { transform: scale(1.2); }
            }
          `}
        </style>
      )}
    </>
  );
};