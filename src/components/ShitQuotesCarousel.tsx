import React, { useState, useEffect } from "react";
import { motion, AnimatePresence } from "framer-motion";

interface Quote {
  text: string;
  author: string;
  context: string;
}

export const ShitQuotesCarousel: React.FC = () => {
  const [currentQuoteIndex, setCurrentQuoteIndex] = useState(0);
  const [isPlaying, setIsPlaying] = useState(true);

  const shitQuotes: Quote[] = [
    {
      text: "Talk is cheap. Show me the shit.",
      author: "Linus Torvalds",
      context: "(adapted from 'Show me the code')",
    },
    {
      text: "There are only two hard things in Computer Science: cache invalidation, naming shit, and off-by-one errors.",
      author: "Phil Karlton",
      context: "(adapted classic CS joke)",
    },
    {
      text: "Any fool can write shit that a computer can understand. Good programmers write shit that humans can understand.",
      author: "Martin Fowler",
      context: "(on readable code)",
    },
    {
      text: "Premature optimization is the root of all shit.",
      author: "Donald Knuth",
      context: "(on coding philosophy)",
    },
    {
      text: "First, solve the problem. Then, write some shit.",
      author: "John Johnson",
      context: "(programming wisdom)",
    },
    {
      text: "The best way to get a project done faster is to start some shit.",
      author: "Anonymous Developer",
      context: "(on productivity)",
    },
    {
      text: "There's nothing more permanent than temporary shit.",
      author: "Every Senior Developer",
      context: "(on technical debt)",
    },
    {
      text: "It's not a bug, it's an undocumented shit.",
      author: "Anonymous",
      context: "(classic developer excuse)",
    },
    {
      text: "If debugging is the process of removing software bugs, then programming must be the process of putting shit in.",
      author: "Edsger Dijkstra",
      context: "(on the nature of coding)",
    },
    {
      text: "Code is like humor. When you have to explain it, your shit is bad.",
      author: "Cory House",
      context: "(on code clarity)",
    },
    {
      text: "Walking on water and developing software from a specification are easy if both are frozen shit.",
      author: "Edward V. Berard",
      context: "(on project requirements)",
    },
    {
      text: "There are two ways to write error-free programs; only the third shit works.",
      author: "Alan J. Perlis",
      context: "(on software reliability)",
    },
  ];

  useEffect(() => {
    if (!isPlaying) return;

    const interval = setInterval(() => {
      setCurrentQuoteIndex((prev) => (prev + 1) % shitQuotes.length);
    }, 5000);

    return () => clearInterval(interval);
  }, [isPlaying, shitQuotes.length]);

  const nextQuote = () => {
    setCurrentQuoteIndex((prev) => (prev + 1) % shitQuotes.length);
  };

  const prevQuote = () => {
    setCurrentQuoteIndex((prev) => (prev - 1 + shitQuotes.length) % shitQuotes.length);
  };

  const togglePlayPause = () => {
    setIsPlaying(!isPlaying);
  };

  return (
    <section className="min-h-screen bg-gradient-to-b from-black via-gray-900 to-black py-20 px-4 flex items-center">
      <div className="max-w-6xl mx-auto w-full">
        {/* Section Title */}
        <motion.div
          className="text-center mb-16"
          initial={{ opacity: 0, y: -30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 1 }}
          viewport={{ once: true }}
        >
          <h2 className="text-5xl md:text-7xl font-terminal font-bold text-terminal-green mb-6">
            💬 SHIT QUOTES 💬
          </h2>
          <p className="text-xl md:text-2xl text-terminal-amber">
            Programming Wisdom (Shit Edition)
          </p>
        </motion.div>

        {/* Quote Carousel */}
        <div className="relative max-w-4xl mx-auto">
          <motion.div
            className="bg-black/90 border-2 border-terminal-green rounded-lg p-8 md:p-12 min-h-[400px] flex flex-col justify-center shadow-2xl shadow-terminal-green/20"
            initial={{ opacity: 0, scale: 0.95 }}
            whileInView={{ opacity: 1, scale: 1 }}
            transition={{ duration: 0.8 }}
            viewport={{ once: true }}
          >
            <AnimatePresence mode="wait">
              <motion.div
                key={currentQuoteIndex}
                className="text-center"
                initial={{ opacity: 0, y: 30 }}
                animate={{ opacity: 1, y: 0 }}
                exit={{ opacity: 0, y: -30 }}
                transition={{ duration: 0.5 }}
              >
                {/* Quote Text */}
                <blockquote className="text-2xl md:text-4xl lg:text-5xl font-terminal text-shit-gold mb-8 leading-relaxed">
                  "{shitQuotes[currentQuoteIndex].text}"
                </blockquote>

                {/* Author and Context */}
                <div className="space-y-2">
                  <cite className="text-xl md:text-2xl text-terminal-green font-semibold not-italic">
                    — {shitQuotes[currentQuoteIndex].author}
                  </cite>
                  <div className="text-lg text-gray-400">
                    {shitQuotes[currentQuoteIndex].context}
                  </div>
                </div>
              </motion.div>
            </AnimatePresence>
          </motion.div>

          {/* Navigation Controls */}
          <div className="flex items-center justify-center gap-6 mt-8">
            {/* Previous Button */}
            <motion.button
              onClick={prevQuote}
              className="bg-gray-800 hover:bg-terminal-green hover:text-black border-2 border-terminal-green text-terminal-green font-bold p-3 rounded-lg transition-all duration-300"
              whileHover={{ scale: 1.1 }}
              whileTap={{ scale: 0.95 }}
            >
              <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
                <path fillRule="evenodd" d="M12.707 5.293a1 1 0 010 1.414L9.414 10l3.293 3.293a1 1 0 01-1.414 1.414l-4-4a1 1 0 010-1.414l4-4a1 1 0 011.414 0z" clipRule="evenodd" />
              </svg>
            </motion.button>

            {/* Play/Pause Button */}
            <motion.button
              onClick={togglePlayPause}
              className="bg-shit-gold hover:bg-shit-600 text-black font-bold p-3 rounded-lg transition-all duration-300"
              whileHover={{ scale: 1.1 }}
              whileTap={{ scale: 0.95 }}
            >
              {isPlaying ? (
                <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
                  <path fillRule="evenodd" d="M18 10a8 8 0 11-16 0 8 8 0 0116 0zM7 8a1 1 0 012 0v4a1 1 0 11-2 0V8zm5-1a1 1 0 00-1 1v4a1 1 0 002 0V8a1 1 0 00-1-1z" clipRule="evenodd" />
                </svg>
              ) : (
                <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
                  <path fillRule="evenodd" d="M10 18a8 8 0 100-16 8 8 0 000 16zM9.555 7.168A1 1 0 008 8v4a1 1 0 001.555.832l3-2a1 1 0 000-1.664l-3-2z" clipRule="evenodd" />
                </svg>
              )}
            </motion.button>

            {/* Next Button */}
            <motion.button
              onClick={nextQuote}
              className="bg-gray-800 hover:bg-terminal-green hover:text-black border-2 border-terminal-green text-terminal-green font-bold p-3 rounded-lg transition-all duration-300"
              whileHover={{ scale: 1.1 }}
              whileTap={{ scale: 0.95 }}
            >
              <svg className="w-6 h-6" fill="currentColor" viewBox="0 0 20 20">
                <path fillRule="evenodd" d="M7.293 14.707a1 1 0 010-1.414L10.586 10 7.293 6.707a1 1 0 011.414-1.414l4 4a1 1 0 010 1.414l-4 4a1 1 0 01-1.414 0z" clipRule="evenodd" />
              </svg>
            </motion.button>
          </div>

          {/* Progress Indicators */}
          <div className="flex justify-center space-x-2 mt-6">
            {shitQuotes.map((_, index) => (
              <button
                key={index}
                onClick={() => setCurrentQuoteIndex(index)}
                className={`w-3 h-3 rounded-full transition-all duration-300 ${
                  index === currentQuoteIndex 
                    ? "bg-shit-gold scale-125" 
                    : "bg-gray-600 hover:bg-gray-500"
                }`}
              />
            ))}
          </div>

          {/* Quote Counter */}
          <div className="text-center mt-4 text-gray-400 font-terminal">
            {currentQuoteIndex + 1} / {shitQuotes.length}
          </div>
        </div>

        {/* Fun Programming Facts */}
        <motion.div
          className="mt-16 bg-gray-900/80 border border-shit-gold rounded-lg p-6 max-w-3xl mx-auto"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 1 }}
          viewport={{ once: true }}
        >
          <h3 className="text-2xl font-terminal text-shit-gold text-center mb-4">
            💡 Did You Know? 💡
          </h3>
          <div className="text-center text-gray-300">
            <p className="mb-2">
              The average programmer writes approximately{" "}
              <span className="text-terminal-green font-bold">50 lines of shit</span> per day
            </p>
            <p className="mb-2">
              But deletes{" "}
              <span className="text-terminal-amber font-bold">100 lines</span> in the process!
            </p>
            <p className="text-sm text-gray-400 mt-4">
              "Measuring programming progress by lines of shit is like measuring aircraft building progress by weight." - Bill Gates
            </p>
          </div>
        </motion.div>
      </div>
    </section>
  );
};