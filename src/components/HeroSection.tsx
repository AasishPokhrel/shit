import React, { useEffect, useState } from "react";
import { motion, useAnimation } from "framer-motion";
import { Github, Star, GitFork } from "lucide-react";
import { getCurrentRepoCount } from "../services/githubData";

export const HeroSection: React.FC = () => {
  const [count, setCount] = useState(0);
  const [isExploding, setIsExploding] = useState(false);
  const [realCount, setRealCount] = useState(1000000000);
  const controls = useAnimation();

  useEffect(() => {
    // Get real current count
    getCurrentRepoCount().then(setRealCount);

    // Animate counter to 1 billion
    const targetCount = 1000000000;
    const duration = 3000; // 3 seconds
    const startTime = Date.now();

    const animateCounter = () => {
      const elapsed = Date.now() - startTime;
      const progress = Math.min(elapsed / duration, 1);
      
      // Easing function for smooth animation
      const easeOutQuart = 1 - Math.pow(1 - progress, 4);
      const currentCount = Math.floor(easeOutQuart * targetCount);
      
      setCount(currentCount);

      if (progress < 1) {
        requestAnimationFrame(animateCounter);
      } else {
        // Trigger explosion effect when counter reaches 1 billion
        setIsExploding(true);
        controls.start({
          scale: [1, 1.2, 1],
          transition: { duration: 0.6, ease: "easeOut" }
        });
      }
    };

    const timeout = setTimeout(animateCounter, 500);
    return () => clearTimeout(timeout);
  }, [controls]);

  const formatNumber = (num: number) => {
    return num.toLocaleString();
  };

  const createExplosionEmojis = () => {
    const emojis = ["💩", "🎉", "🔥", "⚡", "🚀", "💥", "✨", "🌟"];
    return Array.from({ length: 20 }, (_, i) => (
      <motion.div
        key={i}
        className="absolute text-4xl"
        initial={{ 
          x: 0, 
          y: 0, 
          scale: 0,
          rotate: 0,
        }}
        animate={isExploding ? {
          x: (Math.random() - 0.5) * 400,
          y: (Math.random() - 0.5) * 400,
          scale: [0, 1.5, 0],
          rotate: Math.random() * 720,
          opacity: [0, 1, 0],
        } : {}}
        transition={{
          duration: 2,
          delay: i * 0.05,
          ease: "easeOut"
        }}
      >
        {emojis[Math.floor(Math.random() * emojis.length)]}
      </motion.div>
    ));
  };

  return (
    <section id="home" className="relative min-h-screen flex items-center justify-center bg-gradient-to-b from-black via-gray-900 to-black overflow-hidden pt-20 sm:pt-24 md:pt-28">
      {/* GitHub Integration Branding */}
      <div className="absolute top-24 left-1/2 transform -translate-x-1/2 z-20">
        <motion.div 
          className="flex items-center space-x-3 bg-black/80 border border-terminal-green rounded-lg px-4 py-2"
          initial={{ opacity: 0, y: -20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ delay: 0.1 }}
        >
          <Github className="w-6 h-6 text-white" />
          <span className="text-white font-medium">GitHub</span>
          <span className="text-gray-400">×</span>
          <span className="text-shit-gold font-terminal">Repository #1,000,000,000</span>
          <span className="text-2xl">💩</span>
        </motion.div>
      </div>

      {/* Hero Content */}
      <div className="text-center z-10 max-w-6xl mx-auto px-4">
        {/* Main Title */}
        <motion.h1 
          className="text-4xl sm:text-5xl md:text-7xl lg:text-8xl xl:text-9xl font-terminal font-bold mb-4 sm:mb-6 md:mb-8 text-terminal-green transform hover:scale-105 transition-transform duration-300 mt-2 sm:mt-3 md:mt-4"
          initial={{ opacity: 0, y: -50, rotateX: 90 }}
          animate={{ opacity: 1, y: 0, rotateX: 0 }}
          transition={{ duration: 1, delay: 0.2, type: "spring", stiffness: 100 }}
          whileHover={{ 
            textShadow: "0 0 20px #00FF00, 0 0 40px #00FF00, 0 0 80px #00FF00",
            filter: "saturate(150%) brightness(120%)"
          }}
        >
          💩 HOLY SHIT! 💩
        </motion.h1>

        {/* Subtitle with GitHub branding */}
        <motion.h2 
          className="text-xl sm:text-2xl md:text-3xl lg:text-4xl xl:text-5xl mb-6 sm:mb-8 text-terminal-amber px-2 sm:px-4"
          initial={{ opacity: 0, y: 30, scale: 0.8 }}
          animate={{ opacity: 1, y: 0, scale: 1 }}
          transition={{ duration: 1, delay: 0.5, type: "spring" }}
        >
          <span className="flex items-center justify-center flex-wrap gap-3">
            <Github className="w-8 h-8 sm:w-10 sm:h-10 md:w-12 md:h-12 text-white" />
            GitHub Hit{" "}
            <motion.span 
              className="text-terminal-green font-bold"
              animate={{
                scale: [1, 1.1, 1],
                textShadow: [
                  "0 0 5px #00FF00",
                  "0 0 20px #00FF00, 0 0 35px #00FF00",
                  "0 0 5px #00FF00"
                ]
              }}
              transition={{ duration: 2, repeat: Infinity }}
            >
              1,000,000,000
            </motion.span>{" "}
            Repositories!
          </span>
        </motion.h2>

        {/* Counter Display */}
        <motion.div 
          className="mb-8 sm:mb-10 md:mb-12 px-2 sm:px-4"
          initial={{ opacity: 0, scale: 0.8, rotateY: 180 }}
          animate={{ opacity: 1, scale: 1, rotateY: 0 }}
          transition={{ duration: 1, delay: 0.8, type: "spring", stiffness: 80 }}
        >
          <div className="text-base sm:text-lg md:text-xl lg:text-2xl mb-2 sm:mb-4 text-gray-300">
            GitHub Repository Count:
          </div>
          <motion.div 
            className="text-3xl sm:text-4xl md:text-6xl lg:text-7xl xl:text-8xl 2xl:text-9xl font-terminal font-bold text-shit-gold border-2 sm:border-4 border-terminal-green bg-black/50 p-3 sm:p-4 md:p-6 lg:p-8 rounded-lg shadow-2xl shadow-terminal-green/20 transform hover:rotate-1 transition-all duration-300"
            animate={controls}
            whileHover={{ 
              scale: 1.05,
              borderColor: "#FFD700",
              boxShadow: "0 0 30px rgba(255, 215, 0, 0.5), 0 0 60px rgba(255, 215, 0, 0.3)"
            }}
          >
            {formatNumber(count)}
          </motion.div>
          <div className="text-sm text-gray-400 mt-2">
            Live count: {formatNumber(realCount)} repositories
          </div>
        </motion.div>

        {/* GitHub Action Buttons */}
        <motion.div
          className="mb-8 flex flex-wrap justify-center gap-4"
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 1, delay: 1.2 }}
        >
          <motion.a
            href="https://github.com/AasishPokhrel/shit"
            target="_blank"
            rel="noopener noreferrer"
            className="flex items-center space-x-2 bg-terminal-green text-black px-6 py-3 rounded-lg font-medium hover:bg-shit-gold transition-colors"
            whileHover={{ scale: 1.05 }}
            whileTap={{ scale: 0.95 }}
          >
            <Star className="w-5 h-5" />
            <span>Star the Legendary Repo</span>
          </motion.a>
          
          <motion.a
            href="https://github.com/AasishPokhrel/shit/fork"
            target="_blank"
            rel="noopener noreferrer"
            className="flex items-center space-x-2 bg-white/10 text-white border border-white/20 px-6 py-3 rounded-lg font-medium hover:bg-white/20 transition-colors"
            whileHover={{ scale: 1.05 }}
            whileTap={{ scale: 0.95 }}
          >
            <GitFork className="w-5 h-5" />
            <span>Fork It</span>
          </motion.a>
        </motion.div>

        {/* Community Celebration Banner */}
        <motion.div
          className="mb-8 sm:mb-10 px-4"
          initial={{ opacity: 0, y: 30 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 1, delay: 1.5 }}
        >
          <div className="bg-gradient-to-r from-purple-900/50 to-pink-900/50 border border-purple-500/30 rounded-lg p-4 sm:p-6 backdrop-blur-sm">
            <div className="text-center">
              <div className="text-lg sm:text-xl font-bold text-purple-300 mb-2">
                🌍 Global Community Celebration! 🌍
              </div>
              <div className="text-sm sm:text-base text-purple-200 mb-3">
                <span className="inline-flex items-center gap-2 mx-2">
                  <span className="animate-pulse">🇨🇳</span> 打卡
                </span>
                <span className="inline-flex items-center gap-2 mx-2">
                  <span className="animate-pulse">🇺🇸</span> Holy shit!
                </span>
                <span className="inline-flex items-center gap-2 mx-2">
                  <span className="animate-pulse">🇺🇦</span> Легенда!
                </span>
                <span className="inline-flex items-center gap-2 mx-2">
                  <span className="animate-pulse">🇪🇸</span> ¡Increíble!
                </span>
              </div>
              <div className="flex flex-wrap justify-center gap-2 text-xs text-purple-300">
                <span className="bg-purple-800/30 px-2 py-1 rounded">2.8K+ Stars</span>
                <span className="bg-purple-800/30 px-2 py-1 rounded">208 Forks</span>
                <span className="bg-purple-800/30 px-2 py-1 rounded">303 Issues</span>
                <span className="bg-purple-800/30 px-2 py-1 rounded">101 PRs</span>
                <span className="bg-purple-800/30 px-2 py-1 rounded">史前留名</span>
              </div>
            </div>
          </div>
        </motion.div>

        {/* The Legendary Reveal */}
        <motion.div
          className="mb-12"
          initial={{ opacity: 0 }}
          animate={{ opacity: 1 }}
          transition={{ duration: 1, delay: 2 }}
        >
          <div className="text-xl md:text-3xl mb-6 text-gray-300">
            And Repository #{" "}
            <span className="text-shit-gold font-bold">1,000,000,000</span>{" "}
            is named:
          </div>
          
          <motion.div 
            className="relative inline-block px-2 sm:px-4"
            initial={{ scale: 0.8, opacity: 0, rotateZ: -10 }}
            animate={{ scale: 1, opacity: 1, rotateZ: 0 }}
            transition={{ duration: 0.8, delay: 2.5, type: "spring", stiffness: 120 }}
            whileHover={{ 
              scale: 1.05, 
              rotateZ: [0, 2, -2, 0],
              transition: { duration: 0.5 }
            }}
          >
            <motion.div 
              className="text-4xl sm:text-6xl md:text-7xl lg:text-8xl xl:text-9xl 2xl:text-[12rem] font-terminal font-bold text-shit-brown bg-gradient-to-r from-shit-gold via-terminal-amber to-shit-600 bg-clip-text text-transparent p-3 sm:p-4 md:p-6 lg:p-8 rounded-lg border-2 sm:border-4 border-shit-gold shadow-2xl shadow-shit-gold/30 transform hover:skew-x-2 transition-all duration-300"
              animate={{
                backgroundPosition: ["0% 50%", "100% 50%", "0% 50%"],
              }}
              transition={{ duration: 3, repeat: Infinity, ease: "linear" }}
              style={{ backgroundSize: "200% 200%" }}
            >
              "SHIT"
            </motion.div>
            
            {/* Explosion Effect */}
            <div className="absolute inset-0 flex items-center justify-center">
              {createExplosionEmojis()}
            </div>
          </motion.div>
        </motion.div>

        {/* Epic Statement */}
        <motion.p 
          className="text-lg sm:text-xl md:text-2xl lg:text-3xl text-terminal-amber mb-6 sm:mb-8 max-w-4xl mx-auto px-4 sm:px-6"
          initial={{ opacity: 0, y: 30, filter: "blur(10px)" }}
          animate={{ opacity: 1, y: 0, filter: "blur(0px)" }}
          transition={{ duration: 1, delay: 3.5 }}
        >
          The most{" "}
          <motion.span 
            className="text-terminal-green font-bold"
            animate={{
              scale: [1, 1.1, 1],
              opacity: [0.8, 1, 0.8],
              textShadow: [
                "0 0 10px #00FF00",
                "0 0 25px #00FF00, 0 0 50px #00FF00",
                "0 0 10px #00FF00"
              ]
            }}
            transition={{ duration: 2, repeat: Infinity, ease: "easeInOut" }}
          >
            legendary coincidence
          </motion.span>{" "}
          in coding history! 🎉
        </motion.p>

        {/* Creator Credit */}
        <motion.div 
          className="text-base sm:text-lg md:text-xl text-gray-400 px-4 sm:px-6"
          initial={{ opacity: 0, y: 20 }}
          animate={{ opacity: 1, y: 0 }}
          transition={{ duration: 1, delay: 4 }}
        >
          Created by{" "}
          <motion.span 
            className="text-shit-gold font-bold"
            whileHover={{ 
              scale: 1.1,
              textShadow: "0 0 15px #FFD700",
              transition: { duration: 0.3 }
            }}
          >
            AasishPokhrel
          </motion.span>{" "}
          on{" "}
          <motion.span 
            className="text-terminal-green"
            whileHover={{ 
              scale: 1.1,
              textShadow: "0 0 15px #00FF00",
              transition: { duration: 0.3 }
            }}
          >
            June 11, 2025
          </motion.span>
        </motion.div>

        {/* Terminal Command */}
        <motion.div 
          className="mt-8 sm:mt-10 md:mt-12 text-left bg-black border-2 border-terminal-green rounded-lg p-3 sm:p-4 md:p-6 max-w-xs sm:max-w-lg md:max-w-2xl lg:max-w-3xl mx-auto font-terminal text-terminal-green overflow-x-auto"
          initial={{ opacity: 0, y: 30, scale: 0.9 }}
          animate={{ opacity: 1, y: 0, scale: 1 }}
          transition={{ duration: 1, delay: 4.5, type: "spring" }}
          whileHover={{ 
            borderColor: "#FFD700",
            boxShadow: "0 0 20px rgba(0, 255, 0, 0.3)",
            transition: { duration: 0.3 }
          }}
        >
          <div className="mb-2 text-terminal-amber text-xs sm:text-sm md:text-base break-all">$ curl -s https://api.github.com/repos/AasishPokhrel/shit</div>
          <div className="text-xs sm:text-sm break-all">
            {`{
  "id": 1000000000,
  "name": "shit",
  "full_name": "AasishPokhrel/shit",
  "description": "The billionth repository on GitHub 💩",
  "created_at": "2025-06-11T15:30:00Z",
  "legendary_status": true
}`}
          </div>
        </motion.div>
      </div>

      {/* Scroll Indicator */}
      <motion.div 
        className="absolute bottom-4 sm:bottom-6 md:bottom-8 left-1/2 transform -translate-x-1/2 text-terminal-green"
        initial={{ opacity: 0 }}
        animate={{ 
          opacity: 1,
          y: [0, -10, 0],
          scale: [1, 1.1, 1]
        }}
        transition={{ 
          duration: 1, 
          delay: 5,
          y: { duration: 2, repeat: Infinity, ease: "easeInOut" },
          scale: { duration: 2, repeat: Infinity, ease: "easeInOut" }
        }}
        whileHover={{
          scale: 1.2,
          rotate: [0, 5, -5, 0],
          transition: { duration: 0.5 }
        }}
      >
        <div className="text-center cursor-pointer">
          <motion.div 
            className="text-base sm:text-lg mb-2"
            animate={{
              color: ["#00FF00", "#FFD700", "#00FF00"]
            }}
            transition={{ duration: 3, repeat: Infinity }}
          >
            Keep scrolling for more shit!
          </motion.div>
          <motion.div 
            className="text-2xl sm:text-3xl"
            animate={{
              rotate: [0, 10, -10, 0]
            }}
            transition={{ duration: 1, repeat: Infinity, ease: "easeInOut" }}
          >
            ⬇️
          </motion.div>
        </div>
      </motion.div>
    </section>
  );
};