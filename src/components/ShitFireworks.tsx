import React, { useEffect, useState, useCallback } from "react";
import { motion, AnimatePresence } from "framer-motion";

interface Firework {
  id: string;
  x: number;
  y: number;
  particles: Particle[];
}

interface Particle {
  id: string;
  x: number;
  y: number;
  vx: number;
  vy: number;
  emoji: string;
  size: number;
  life: number;
  maxLife: number;
}

export const ShitFireworks: React.FC = () => {
  const [fireworks, setFireworks] = useState<Firework[]>([]);

  const shitEmojis = [
    '💩', '🔥', '⚡', '🚀', '💥', '✨', '🌟', '💫', '🎉', '🎊',
    '🦄', '🌈', '💖', '💝', '🎈', '🎁', '🍰', '🥳', '😂', '🤩'
  ];

  const createFirework = useCallback((x: number, y: number) => {
    const particleCount = Math.random() * 15 + 10;
    const particles: Particle[] = [];

    for (let i = 0; i < particleCount; i++) {
      const angle = (Math.PI * 2 * i) / particleCount + (Math.random() - 0.5) * 0.5;
      const velocity = Math.random() * 8 + 4;
      const life = Math.random() * 60 + 40;

      particles.push({
        id: `particle-${Date.now()}-${i}`,
        x: 0,
        y: 0,
        vx: Math.cos(angle) * velocity,
        vy: Math.sin(angle) * velocity,
        emoji: shitEmojis[Math.floor(Math.random() * shitEmojis.length)],
        size: Math.random() * 20 + 15,
        life,
        maxLife: life,
      });
    }

    const firework: Firework = {
      id: `firework-${Date.now()}`,
      x,
      y,
      particles,
    };

    setFireworks(prev => [...prev, firework]);

    // Remove firework after animation
    setTimeout(() => {
      setFireworks(prev => prev.filter(f => f.id !== firework.id));
    }, 2000);
  }, []);

  useEffect(() => {
    const handleClick = (e: MouseEvent) => {
      // Only trigger on actual clickable elements or with Ctrl+Click
      const target = e.target as HTMLElement;
      const isClickable = target.tagName === 'BUTTON' || 
                         target.tagName === 'A' || 
                         target.onclick || 
                         target.classList.contains('cursor-pointer') ||
                         target.closest('button') ||
                         target.closest('a') ||
                         target.closest('[onclick]') ||
                         target.closest('.cursor-pointer') ||
                         e.ctrlKey ||
                         e.metaKey;

      if (isClickable) {
        createFirework(e.clientX, e.clientY);
      }
    };

    // Add special firework triggers
    const handleSpecialClick = (e: MouseEvent) => {
      const target = e.target as HTMLElement;
      
      // Trigger on emoji clicks
      if (target.textContent && /[\u{1F300}-\u{1F9FF}]|[\u{2600}-\u{26FF}]|[\u{2700}-\u{27BF}]/u.test(target.textContent)) {
        createFirework(e.clientX, e.clientY);
      }
    };

    document.addEventListener('click', handleClick);
    document.addEventListener('click', handleSpecialClick);

    return () => {
      document.removeEventListener('click', handleClick);
      document.removeEventListener('click', handleSpecialClick);
    };
  }, [createFirework]);

  // Global firework trigger function
  useEffect(() => {
    // @ts-ignore - Adding to window for console access
    window.shitFirework = (x?: number, y?: number) => {
      const clientX = x ?? window.innerWidth / 2;
      const clientY = y ?? window.innerHeight / 2;
      createFirework(clientX, clientY);
    };

    // @ts-ignore - Adding party mode
    window.shitParty = () => {
      for (let i = 0; i < 10; i++) {
        setTimeout(() => {
          createFirework(
            Math.random() * window.innerWidth,
            Math.random() * window.innerHeight
          );
        }, i * 200);
      }
    };

    return () => {
      // @ts-ignore
      delete window.shitFirework;
      // @ts-ignore
      delete window.shitParty;
    };
  }, [createFirework]);

  return (
    <div className="fixed inset-0 pointer-events-none z-40">
      <AnimatePresence>
        {fireworks.map(firework => (
          <div
            key={firework.id}
            className="absolute"
            style={{
              left: firework.x,
              top: firework.y,
            }}
          >
            {firework.particles.map(particle => (
              <motion.div
                key={particle.id}
                className="absolute select-none"
                initial={{
                  x: 0,
                  y: 0,
                  scale: 1,
                  opacity: 1,
                }}
                animate={{
                  x: particle.vx * 50,
                  y: particle.vy * 50,
                  scale: [1, 1.5, 0],
                  opacity: [1, 0.8, 0],
                  rotate: [0, 360],
                }}
                transition={{
                  duration: 2,
                  ease: "easeOut",
                }}
                style={{
                  fontSize: `${particle.size}px`,
                }}
              >
                {particle.emoji}
              </motion.div>
            ))}
          </div>
        ))}
      </AnimatePresence>
    </div>
  );
};

// Hook for manual firework triggering
export const useShitFireworks = () => {
  const triggerFirework = useCallback((x?: number, y?: number) => {
    const event = new CustomEvent('triggerShitFirework', {
      detail: { x, y }
    });
    window.dispatchEvent(event);
  }, []);

  return { triggerFirework };
};