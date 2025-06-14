import React, { useEffect, useState } from "react";

interface ShitEmoji {
  id: number;
  x: number;
  y: number;
  size: number;
  speed: number;
  emoji: string;
  rotation: number;
  rotationSpeed: number;
}

interface FloatingShitProps {
  count?: number;
  className?: string;
}

export const FloatingShit: React.FC<FloatingShitProps> = ({ 
  count = 15, 
  className = "" 
}) => {
  const [emojis, setEmojis] = useState<ShitEmoji[]>([]);

  const shitEmojis = ["💩", "🔥", "⚡", "🚀", "💻", "📦", "🎉", "✨", "🌟", "💥"];

  useEffect(() => {
    const newEmojis: ShitEmoji[] = [];
    
    for (let i = 0; i < count; i++) {
      newEmojis.push({
        id: i,
        x: Math.random() * window.innerWidth,
        y: Math.random() * window.innerHeight,
        size: Math.random() * 30 + 15, // 15-45px
        speed: Math.random() * 2 + 0.5, // 0.5-2.5 speed
        emoji: shitEmojis[Math.floor(Math.random() * shitEmojis.length)],
        rotation: Math.random() * 360,
        rotationSpeed: (Math.random() - 0.5) * 4, // -2 to 2 degrees per frame
      });
    }
    
    setEmojis(newEmojis);
  }, [count]);

  useEffect(() => {
    const animateEmojis = () => {
      setEmojis(prev => prev.map(emoji => ({
        ...emoji,
        y: emoji.y <= -50 ? window.innerHeight + 50 : emoji.y - emoji.speed,
        x: emoji.x + Math.sin(emoji.y * 0.01) * 0.5, // slight horizontal drift
        rotation: emoji.rotation + emoji.rotationSpeed,
      })));
    };

    const interval = setInterval(animateEmojis, 50);
    return () => clearInterval(interval);
  }, []);

  const handleEmojiClick = (id: number) => {
    setEmojis(prev => prev.map(emoji => 
      emoji.id === id 
        ? {
            ...emoji,
            size: emoji.size * 1.5,
            rotationSpeed: emoji.rotationSpeed * 3,
          }
        : emoji
    ));

    // Reset after animation
    setTimeout(() => {
      setEmojis(prev => prev.map(emoji => 
        emoji.id === id 
          ? {
              ...emoji,
              size: Math.random() * 30 + 15,
              rotationSpeed: (Math.random() - 0.5) * 4,
            }
          : emoji
      ));
    }, 500);
  };

  return (
    <div className={`fixed inset-0 pointer-events-none -z-10 ${className}`}>
      {emojis.map(emoji => (
        <div
          key={emoji.id}
          className="absolute cursor-pointer pointer-events-auto transition-all duration-300 hover:scale-110"
          style={{
            left: `${emoji.x}px`,
            top: `${emoji.y}px`,
            transform: `rotate(${emoji.rotation}deg)`,
            fontSize: `${emoji.size}px`,
            userSelect: "none",
          }}
          onClick={() => handleEmojiClick(emoji.id)}
        >
          {emoji.emoji}
        </div>
      ))}
    </div>
  );
};