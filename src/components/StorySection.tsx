import React, { useState } from "react";
import { motion } from "framer-motion";
import { TypewriterText } from "./TypewriterText";

export const StorySection: React.FC = () => {
  const [currentStoryStep, setCurrentStoryStep] = useState(0);
  const [showNextStep, setShowNextStep] = useState(false);

  const storySteps = [
    {
      title: "Chapter 1: The Genesis",
      content: "In the beginning, there was GitHub. Founded in 2008 by Tom Preston-Werner, Chris Wanstrath, and PJ Hyett, it started as a simple idea: make Git more social and collaborative.",
      terminalCommand: "git init github",
      terminalOutput: "Initialized empty Git repository in /world/coding/",
    },
    {
      title: "Chapter 2: The Exponential Growth",
      content: "What started with a few repositories grew exponentially. Developers worldwide embraced the platform, creating everything from 'Hello World' programs to enterprise software that powers the internet.",
      terminalCommand: "git log --oneline --graph",
      terminalOutput: "* 2008: 46 repositories\n* 2011: 1 million repositories\n* 2013: 10 million repositories\n* 2018: 100 million repositories",
    },
    {
      title: "Chapter 3: The Countdown Begins",
      content: "By 2025, GitHub had become the epicenter of the coding universe. Millions of developers pushed billions of commits. The countdown to the billionth repository was on everyone's minds.",
      terminalCommand: "watch -n 1 'curl -s https://api.github.com/search/repositories | jq .total_count'",
      terminalOutput: "999,999,995\n999,999,996\n999,999,997\n999,999,998\n999,999,999...",
    },
    {
      title: "Chapter 4: The Moment of Truth",
      content: "June 11, 2025, 15:30 UTC. The tech world held its breath. Who would create the billionth repository? What would it be called? The answer was more beautiful than anyone could have imagined.",
      terminalCommand: "git push origin main",
      terminalOutput: "Counting objects: 3, done.\nWriting objects: 100% (3/3), 420 bytes | 0 bytes/s, done.\nTotal 3 (delta 0), reused 0 (delta 0)\nTo https://github.com/AasishPokhrel/shit.git\n[main] Repository #1,000,000,000 created",
    },
    {
      title: "Chapter 5: The Legend is Born",
      content: "And so it was that AasishPokhrel, unknowingly, created history. Repository 1,000,000,000 was named 'shit' - a perfect encapsulation of the beautiful chaos, the creative mess, and the honest reality of coding life.",
      terminalCommand: "echo 'Sometimes the best things in life come from the most unexpected places'",
      terminalOutput: "Sometimes the best things in life come from the most unexpected places\n💩 Repository #1,000,000,000: LEGENDARY STATUS ACHIEVED 💩",
    },
  ];

  const handleStoryComplete = () => {
    if (currentStoryStep < storySteps.length - 1) {
      setTimeout(() => {
        setShowNextStep(true);
      }, 1000);
    }
  };

  const nextStep = () => {
    setCurrentStoryStep(prev => prev + 1);
    setShowNextStep(false);
  };

  return (
    <section className="min-h-screen bg-gradient-to-b from-black via-gray-900 to-black py-12 sm:py-16 md:py-20 px-4">
      <div className="max-w-6xl mx-auto">
        {/* Section Title */}
        <motion.div
          className="text-center mb-16"
          initial={{ opacity: 0, y: -30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 1 }}
          viewport={{ once: true }}
        >
          <h2 className="text-3xl sm:text-4xl md:text-5xl lg:text-6xl xl:text-7xl font-terminal font-bold text-terminal-green mb-4 sm:mb-6 px-2">
            💩 THE EPIC TALE 💩
          </h2>
          <p className="text-lg sm:text-xl md:text-2xl text-terminal-amber px-2">
            How GitHub's Billionth Repository Became Legendary
          </p>
        </motion.div>

        {/* Story Container */}
        <div className="max-w-4xl mx-auto">
          <motion.div
            className="bg-black border-2 border-terminal-green rounded-lg overflow-hidden shadow-2xl shadow-terminal-green/20"
            initial={{ opacity: 0, scale: 0.95 }}
            whileInView={{ opacity: 1, scale: 1 }}
            transition={{ duration: 0.8 }}
            viewport={{ once: true }}
          >
            {/* Terminal Header */}
            <div className="bg-gray-800 px-4 py-3 flex items-center gap-2">
              <div className="w-3 h-3 rounded-full bg-red-500"></div>
              <div className="w-3 h-3 rounded-full bg-yellow-500"></div>
              <div className="w-3 h-3 rounded-full bg-green-500"></div>
              <div className="ml-4 text-gray-400 font-terminal text-sm">
                shit-story.sh - The Chronicle of Repository #1,000,000,000
              </div>
            </div>

            {/* Story Content */}
            <div className="p-8 space-y-8">
              {storySteps.slice(0, currentStoryStep + 1).map((step, index) => (
                <motion.div
                  key={index}
                  className="space-y-6"
                  initial={{ opacity: 0, x: -20 }}
                  animate={{ opacity: 1, x: 0 }}
                  transition={{ duration: 0.8, delay: index * 0.2 }}
                >
                  {/* Chapter Title */}
                  <h3 className="text-lg sm:text-xl md:text-2xl lg:text-3xl font-terminal font-bold text-shit-gold border-l-2 sm:border-l-4 border-shit-gold pl-2 sm:pl-4">
                    {step.title}
                  </h3>

                  {/* Story Text */}
                  <div className="text-base sm:text-lg md:text-xl text-gray-300 leading-relaxed">
                    {index === currentStoryStep ? (
                      <TypewriterText
                        text={step.content}
                        speed={30}
                        onComplete={handleStoryComplete}
                        className="block"
                      />
                    ) : (
                      step.content
                    )}
                  </div>

                  {/* Terminal Simulation */}
                  <div className="bg-gray-900 border border-gray-700 rounded-lg p-3 sm:p-4 font-terminal overflow-x-auto">
                    <div className="text-terminal-green text-xs sm:text-sm mb-2 break-all">
                      <span className="text-shit-gold">user@github</span>
                      <span className="text-gray-500">:</span>
                      <span className="text-blue-400">~/history</span>
                      <span className="text-gray-500">$ </span>
                      {index === currentStoryStep ? (
                        <TypewriterText
                          text={step.terminalCommand}
                          speed={50}
                          delay={step.content.length * 30 + 1000}
                        />
                      ) : (
                        step.terminalCommand
                      )}
                    </div>
                    <div className="text-gray-300 text-xs sm:text-sm whitespace-pre-line break-words">
                      {index === currentStoryStep ? (
                        <TypewriterText
                          text={step.terminalOutput}
                          speed={40}
                          delay={step.content.length * 30 + step.terminalCommand.length * 50 + 2000}
                        />
                      ) : (
                        step.terminalOutput
                      )}
                    </div>
                  </div>

                  {/* Separator */}
                  {index < storySteps.length - 1 && (
                    <div className="flex items-center justify-center my-8">
                      <div className="flex-1 h-px bg-gray-600"></div>
                      <div className="mx-4 text-2xl">💩</div>
                      <div className="flex-1 h-px bg-gray-600"></div>
                    </div>
                  )}
                </motion.div>
              ))}

              {/* Continue Button */}
              {showNextStep && currentStoryStep < storySteps.length - 1 && (
                <motion.div
                  className="flex justify-center mt-8"
                  initial={{ opacity: 0, y: 20 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ duration: 0.5 }}
                >
                  <button
                    onClick={nextStep}
                    className="bg-terminal-green text-black font-terminal font-bold px-8 py-3 rounded-lg hover:bg-terminal-amber transition-colors duration-300 shadow-lg shadow-terminal-green/30"
                  >
                    Continue the Epic Tale →
                  </button>
                </motion.div>
              )}

              {/* The End */}
              {currentStoryStep === storySteps.length - 1 && (
                <motion.div
                  className="text-center mt-12 space-y-6"
                  initial={{ opacity: 0, y: 30 }}
                  animate={{ opacity: 1, y: 0 }}
                  transition={{ duration: 1, delay: 2 }}
                >
                  <div className="text-2xl sm:text-3xl md:text-4xl font-terminal text-shit-gold">
                    ✨ THE END ✨
                  </div>
                  <div className="text-lg sm:text-xl text-gray-300 px-2">
                    And thus, the legend of Repository #1,000,000,000 was born.
                  </div>
                  <div className="text-base sm:text-lg text-terminal-green px-2">
                    Sometimes the most beautiful things come from the most unexpected places.
                  </div>
                  <div className="text-6xl animate-shit-bounce">💩</div>
                </motion.div>
              )}
            </div>
          </motion.div>
        </div>

        {/* Fun Fact */}
        <motion.div
          className="mt-16 text-center bg-gray-900/50 border border-shit-gold rounded-lg p-8 max-w-3xl mx-auto"
          initial={{ opacity: 0, y: 30 }}
          whileInView={{ opacity: 1, y: 0 }}
          transition={{ duration: 1, delay: 0.5 }}
          viewport={{ once: true }}
        >
          <h3 className="text-xl sm:text-2xl font-terminal text-shit-gold mb-4">Fun Fact 💡</h3>
          <p className="text-base sm:text-lg text-gray-300">
            The word "shit" appears in over{" "}
            <span className="text-terminal-green font-bold">1.2 million</span> repository names on GitHub.
            But only ONE gets to be{" "}
            <span className="text-shit-gold font-bold">Repository #1,000,000,000</span>! 🏆
          </p>
        </motion.div>
      </div>
    </section>
  );
};