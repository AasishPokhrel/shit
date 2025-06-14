import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';

export const MobileOptimization: React.FC = () => {
  const [isMobile, setIsMobile] = useState(false);
  const [orientation, setOrientation] = useState<'portrait' | 'landscape'>('portrait');
  const [showMobileHint, setShowMobileHint] = useState(false);

  useEffect(() => {
    const checkMobile = () => {
      const isMobileDevice = /Android|iPhone|iPad|iPod|BlackBerry|IEMobile|Opera Mini/i.test(navigator.userAgent) ||
                            window.innerWidth <= 768;
      setIsMobile(isMobileDevice);
      
      // Show mobile-specific hints
      if (isMobileDevice && !localStorage.getItem('mobile-hint-shown')) {
        setShowMobileHint(true);
        localStorage.setItem('mobile-hint-shown', 'true');
      }
    };

    const handleOrientationChange = () => {
      setOrientation(window.innerHeight > window.innerWidth ? 'portrait' : 'landscape');
    };

    checkMobile();
    handleOrientationChange();

    window.addEventListener('resize', checkMobile);
    window.addEventListener('resize', handleOrientationChange);
    window.addEventListener('orientationchange', handleOrientationChange);

    return () => {
      window.removeEventListener('resize', checkMobile);
      window.removeEventListener('resize', handleOrientationChange);
      window.removeEventListener('orientationchange', handleOrientationChange);
    };
  }, []);

  // Mobile-specific touch interactions
  useEffect(() => {
    if (!isMobile) return;

    let touchStartY = 0;
    let touchEndY = 0;

    const handleTouchStart = (e: TouchEvent) => {
      touchStartY = e.changedTouches[0].screenY;
    };

    const handleTouchEnd = (e: TouchEvent) => {
      touchEndY = e.changedTouches[0].screenY;
      const swipeDistance = touchStartY - touchEndY;

      // Trigger fireworks on upward swipe
      if (swipeDistance > 50) {
        const touch = e.changedTouches[0];
        if ((window as any).shitFirework) {
          (window as any).shitFirework(touch.clientX, touch.clientY);
        }
      }
    };

    // Double tap for special effects
    let lastTap = 0;
    const handleTouchEndForDoubleTap = (e: TouchEvent) => {
      const currentTime = new Date().getTime();
      const tapLength = currentTime - lastTap;
      
      if (tapLength < 500 && tapLength > 0) {
        // Double tap detected
        const touch = e.changedTouches[0];
        
        // Create emoji explosion
        for (let i = 0; i < 6; i++) {
          setTimeout(() => {
            const emoji = document.createElement('div');
            emoji.textContent = '💩';
            emoji.style.position = 'fixed';
            emoji.style.left = `${touch.clientX + (Math.random() - 0.5) * 100}px`;
            emoji.style.top = `${touch.clientY + (Math.random() - 0.5) * 100}px`;
            emoji.style.fontSize = '2rem';
            emoji.style.pointerEvents = 'none';
            emoji.style.zIndex = '9999';
            emoji.style.transition = 'all 1s ease-out';
            
            document.body.appendChild(emoji);
            
            setTimeout(() => {
              emoji.style.transform = 'scale(0) rotate(720deg)';
              emoji.style.opacity = '0';
              setTimeout(() => emoji.remove(), 1000);
            }, 50);
          }, i * 100);
        }
      }
      lastTap = currentTime;
    };

    document.addEventListener('touchstart', handleTouchStart, { passive: true });
    document.addEventListener('touchend', handleTouchEnd, { passive: true });
    document.addEventListener('touchend', handleTouchEndForDoubleTap, { passive: true });

    return () => {
      document.removeEventListener('touchstart', handleTouchStart);
      document.removeEventListener('touchend', handleTouchEnd);
      document.removeEventListener('touchend', handleTouchEndForDoubleTap);
    };
  }, [isMobile]);

  if (!isMobile) return null;

  return (
    <>
      {/* Mobile Hint Popup */}
      <AnimatePresence>
        {showMobileHint && (
          <motion.div
            className="fixed inset-0 z-50 bg-black/80 backdrop-blur-sm flex items-center justify-center p-4"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
          >
            <motion.div
              className="bg-black border border-terminal-green rounded-lg p-6 max-w-sm text-center"
              initial={{ scale: 0.8, opacity: 0 }}
              animate={{ scale: 1, opacity: 1 }}
              exit={{ scale: 0.8, opacity: 0 }}
            >
              <div className="text-3xl mb-4">📱💩</div>
              <h3 className="text-xl font-terminal text-shit-gold mb-4">
                Mobile Shit Experience!
              </h3>
              <div className="text-sm text-gray-300 space-y-2 mb-6">
                <p>🔥 Swipe up for fireworks</p>
                <p>💥 Double-tap for explosions</p>
                <p>🎮 Tap corners for secrets</p>
                <p>🌈 Pinch to zoom chaos</p>
              </div>
              <button
                onClick={() => setShowMobileHint(false)}
                className="bg-terminal-green text-black px-6 py-2 rounded-lg font-bold hover:bg-shit-gold transition-colors"
              >
                Let's Go! 🚀
              </button>
            </motion.div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Mobile Quick Actions */}
      <div className="fixed bottom-6 left-1/2 transform -translate-x-1/2 z-40 flex gap-2">
        <motion.button
          onTouchEnd={() => {
            if ((window as any).shit?.party) {
              (window as any).shit.party();
            }
          }}
          className="bg-shit-gold text-black p-3 rounded-full shadow-lg"
          whileTap={{ scale: 0.9 }}
        >
          <span className="text-lg">🎆</span>
        </motion.button>
        
        <motion.button
          onTouchEnd={() => {
            if ((window as any).shit?.surprise) {
              (window as any).shit.surprise();
            }
          }}
          className="bg-terminal-green text-black p-3 rounded-full shadow-lg"
          whileTap={{ scale: 0.9 }}
        >
          <span className="text-lg">🎲</span>
        </motion.button>
        
        <motion.button
          onTouchEnd={() => {
            if ((window as any).shit?.rainbow) {
              (window as any).shit.rainbow();
            }
          }}
          className="bg-purple-600 text-white p-3 rounded-full shadow-lg"
          whileTap={{ scale: 0.9 }}
        >
          <span className="text-lg">🌈</span>
        </motion.button>
      </div>

      {/* Orientation Warning */}
      {orientation === 'landscape' && (
        <motion.div
          className="fixed top-4 left-1/2 transform -translate-x-1/2 z-40 bg-yellow-600 text-black px-4 py-2 rounded-lg text-sm font-bold"
          initial={{ y: -50, opacity: 0 }}
          animate={{ y: 0, opacity: 1 }}
        >
          📱 Rotate for better shit experience!
        </motion.div>
      )}
    </>
  );
};