import React, { useEffect, useState } from "react";
import { motion, useInView } from "framer-motion";
import { useRef } from "react";

interface StatProps {
  number: string;
  label: string;
  emoji: string;
  description: string;
  delay?: number;
}

const AnimatedStat: React.FC<StatProps> = ({ number, label, emoji, description, delay = 0 }) => {
  const [displayNumber, setDisplayNumber] = useState("0");
  const [hasAnimated, setHasAnimated] = useState(false);
  const ref = useRef(null);
  const isInView = useInView(ref, { once: true });

  useEffect(() => {
    if (isInView && !hasAnimated) {
      setTimeout(() => {
        // If it's a number, animate the counting
        if (/^\d+/.test(number)) {
          const targetNum = parseInt(number.replace(/,/g, ""));
          const duration = 2000; // 2 seconds
          const startTime = Date.now();

          const animateCount = () => {
            const elapsed = Date.now() - startTime;
            const progress = Math.min(elapsed / duration, 1);
            const easeOutQuart = 1 - Math.pow(1 - progress, 4);
            const currentNum = Math.floor(easeOutQuart * targetNum);
            
            setDisplayNumber(currentNum.toLocaleString());

            if (progress < 1) {
              requestAnimationFrame(animateCount);
            } else {
              setDisplayNumber(number);
              setHasAnimated(true);
            }
          };

          animateCount();
        } else {
          // For non-numeric values, just show them after delay
          setDisplayNumber(number);
          setHasAnimated(true);
        }
      }, delay);
    }
  }, [isInView, number, delay, hasAnimated]);

  return (
    <motion.div
      ref={ref}
      className="bg-black/80 border-2 border-terminal-green rounded-lg p-4 sm:p-5 md:p-6 hover:border-shit-gold transition-all duration-300 hover:scale-105 hover:shadow-lg hover:shadow-terminal-green/20 transform hover:-rotate-1"
      initial={{ opacity: 0, y: 30 }}
      whileInView={{ opacity: 1, y: 0 }}
      transition={{ duration: 0.8, delay: delay / 1000 }}
      viewport={{ once: true }}
    >
      <div className="text-center space-y-4">
        <div className="text-4xl">{emoji}</div>
        <div className="text-2xl sm:text-3xl md:text-4xl font-terminal font-bold text-shit-gold">
          {displayNumber}
        </div>
        <div className="text-base sm:text-lg font-terminal text-terminal-green font-semibold">
          {label}
        </div>
        <div className="text-xs sm:text-sm text-gray-400 leading-relaxed">
          {description}
        </div>
      </div>
    </motion.div>
  );
};

export const ShitStatsSection: React.FC = () => {
  const stats: StatProps[] = [
    {
      number: "1,000,000,000",
      label: "Total Repositories",
      emoji: "📦",
      description: "The epic milestone that started this whole legendary story",
      delay: 200,
    },
    {
      number: "100,000,000",
      label: "Active Developers",
      emoji: "👨‍💻",
      description: "Coding warriors contributing to the global repository count",
      delay: 400,
    },
    {
      number: "420,690",
      label: "Repos Named 'Shit'",
      emoji: "💩",
      description: "Because sometimes that's the most honest name for your code",
      delay: 600,
    },
    {
      number: "13,337",
      label: "Daily Shit Commits",
      emoji: "🔥",
      description: "Average commits per day to repositories with 'shit' in the name",
      delay: 800,
    },
    {
      number: "∞",
      label: "Developers Who Give a Shit",
      emoji: "❤️",
      description: "The number of developers who truly care about their craft",
      delay: 1000,
    },
    {
      number: "1",
      label: "Legendary Billionth Repo",
      emoji: "🏆",
      description: "AasishPokhrel/shit - The repo that made history",
      delay: 1200,
    },
  ];

  const funFacts = [
    "The first repository on GitHub was called 'grit' by Tom Preston-Werner",
    "Repository #999,999,999 was created just 42 minutes before #1,000,000,000",
    "The word 'shit' has been used in over 2.8 million commit messages",
    "'Hello World' appears in 15% of all repository names",
    "The longest repository name on GitHub has 214 characters",
    "42% of repositories have never received a single star",
  ];

  const [currentFactIndex, setCurrentFactIndex] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => {
      setCurrentFactIndex((prev) => (prev + 1) % funFacts.length);
    }, 4000);

    return () => clearInterval(interval);
  }, [funFacts.length]);

  return (
    <section className="min-h-screen bg-gradient-to-b from-gray-900 via-black to-gray-900 py-12 sm:py-16 md:py-20 px-4">
      <div className="max-w-7xl mx-auto">
        {/* Section Title */}
        <motion.div
          className="text-center mb-16"
          initial={{ opacity: 0, y: -30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 1 }}
          viewport={{ once: true }}
        >
          <h2 className="text-3xl sm:text-4xl md:text-5xl lg:text-6xl xl:text-7xl font-terminal font-bold text-terminal-green mb-4 sm:mb-6 px-2">
            📊 SHIT STATISTICS 📊
          </h2>
          <p className="text-lg sm:text-xl md:text-2xl text-terminal-amber px-2">
            The Numbers Behind the Legend
          </p>
        </motion.div>

        {/* Stats Grid */}
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-3 gap-4 sm:gap-6 md:gap-8 mb-12 sm:mb-14 md:mb-16">
          {stats.map((stat, index) => (
            <AnimatedStat key={index} {...stat} />
          ))}
        </div>

        {/* Fun Facts Carousel */}
        <motion.div
          className="bg-gray-900/80 border-2 border-shit-gold rounded-lg p-8 max-w-4xl mx-auto"
          initial={{ opacity: 0, scale: 0.95 }}
          whileInView={{ opacity: 1, scale: 1 }}
          transition={{ duration: 0.8 }}
          viewport={{ once: true }}
        >
          <div className="text-center">
            <h3 className="text-2xl md:text-3xl font-terminal text-shit-gold mb-6">
              💡 Random Shit Facts 💡
            </h3>
            <div className="relative h-24 flex items-center justify-center">
              <motion.p
                key={currentFactIndex}
                className="text-lg md:text-xl text-gray-300 leading-relaxed max-w-3xl"
                initial={{ opacity: 0, y: 20 }}
                animate={{ opacity: 1, y: 0 }}
                exit={{ opacity: 0, y: -20 }}
                transition={{ duration: 0.5 }}
              >
                {funFacts[currentFactIndex]}
              </motion.p>
            </div>
            <div className="flex justify-center space-x-2 mt-6">
              {funFacts.map((_, index) => (
                <div
                  key={index}
                  className={`w-2 h-2 rounded-full transition-colors duration-300 ${
                    index === currentFactIndex ? "bg-shit-gold" : "bg-gray-600"
                  }`}
                />
              ))}
            </div>
          </div>
        </motion.div>

        {/* GitHub Growth Timeline */}
        <motion.div
          className="mt-16 bg-black/80 border-2 border-terminal-green rounded-lg p-8"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 1 }}
          viewport={{ once: true }}
        >
          <h3 className="text-3xl font-terminal text-terminal-green text-center mb-8">
            🚀 GitHub's Journey to 1 Billion 🚀
          </h3>
          
          <div className="space-y-6">
            {[
              { year: "2008", repos: "46", event: "GitHub founded" },
              { year: "2010", repos: "1,000", event: "First thousand repositories" },
              { year: "2011", repos: "1,000,000", event: "One million milestone" },
              { year: "2013", repos: "10,000,000", event: "Ten million reached" },
              { year: "2018", repos: "100,000,000", event: "Hundred million achieved" },
              { year: "2025", repos: "1,000,000,000", event: "THE LEGENDARY BILLION! 💩" },
            ].map((milestone, index) => (
              <motion.div
                key={index}
                className="flex items-center space-x-4 p-4 bg-gray-900 rounded-lg border border-gray-700 hover:border-shit-gold transition-colors duration-300"
                initial={{ opacity: 0, x: -50 }}
                whileInView={{ opacity: 1, x: 0 }}
                transition={{ duration: 0.6, delay: index * 0.1 }}
                viewport={{ once: true }}
              >
                <div className="text-2xl font-terminal text-shit-gold font-bold min-w-[80px]">
                  {milestone.year}
                </div>
                <div className="flex-1">
                  <div className="text-lg font-terminal text-terminal-green">
                    {milestone.repos.toLocaleString()} repositories
                  </div>
                  <div className="text-gray-400">
                    {milestone.event}
                  </div>
                </div>
                {index === 5 && (
                  <div className="text-3xl animate-shit-bounce">🎉</div>
                )}
              </motion.div>
            ))}
          </div>
        </motion.div>

        {/* Call to Action */}
        <motion.div
          className="mt-16 text-center"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 1 }}
          viewport={{ once: true }}
        >
          <p className="text-xl md:text-2xl text-gray-300 mb-6">
            Want to add your own shit to the collection?
          </p>
          <motion.a
            href="https://github.com/new"
            target="_blank"
            rel="noopener noreferrer"
            className="inline-block bg-terminal-green text-black font-terminal font-bold px-8 py-4 rounded-lg hover:bg-shit-gold transition-colors duration-300 shadow-lg shadow-terminal-green/30"
            whileHover={{ scale: 1.05 }}
            whileTap={{ scale: 0.95 }}
          >
            Create Your Own Legendary Repository 🚀
          </motion.a>
        </motion.div>
      </div>
    </section>
  );
};