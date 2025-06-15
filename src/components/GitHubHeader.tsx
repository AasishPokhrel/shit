import React, { useState } from "react";
import { motion } from "framer-motion";
import { Github, Star, GitFork, Users, BookOpen, Home } from "lucide-react";

export const GitHubHeader: React.FC = () => {
  const [isHovered, setIsHovered] = useState(false);

  return (
    <header className="fixed top-0 left-0 right-0 z-50 bg-black/95 backdrop-blur-sm border-b border-terminal-green">
      <div className="max-w-7xl mx-auto px-4 py-3">
        <div className="flex items-center justify-between">
          {/* Logo Section */}
          <div className="flex items-center space-x-4">
            <motion.div
              className="flex items-center space-x-2"
              whileHover={{ scale: 1.05 }}
              onHoverStart={() => setIsHovered(true)}
              onHoverEnd={() => setIsHovered(false)}
            >
              {/* Shittocat - GitHub Octocat with shit theme */}
              <div className="relative">
                <Github className="w-8 h-8 text-white" />
                <motion.div
                  className="absolute -top-1 -right-1 text-xs"
                  animate={isHovered ? { rotate: [0, 10, -10, 0] } : {}}
                  transition={{ duration: 0.5 }}
                >
                  💩
                </motion.div>
              </div>
              <div className="hidden sm:block">
                <span className="text-white font-bold text-lg">GitHub</span>
                <span className="text-shit-gold ml-2 font-terminal">× Shit</span>
              </div>
            </motion.div>
          </div>

          {/* Navigation */}
          <nav className="hidden md:flex items-center space-x-6">
            <a
              href="#home"
              className="flex items-center space-x-1 text-gray-300 hover:text-white transition-colors"
            >
              <Home className="w-4 h-4" />
              <span>Home</span>
            </a>
            <a
              href="#story"
              className="flex items-center space-x-1 text-gray-300 hover:text-white transition-colors"
            >
              <BookOpen className="w-4 h-4" />
              <span>Story</span>
            </a>
            <a
              href="#stats"
              className="flex items-center space-x-1 text-gray-300 hover:text-white transition-colors"
            >
              <Users className="w-4 h-4" />
              <span>Stats</span>
            </a>
            <a
              href="https://github.com/AasishPokhrel/shit"
              target="_blank"
              rel="noopener noreferrer"
              className="flex items-center space-x-1 text-gray-300 hover:text-terminal-green transition-colors"
            >
              <Star className="w-4 h-4" />
              <span>Original Repo</span>
            </a>
          </nav>

          {/* CTA Section */}
          <div className="flex items-center space-x-3">
            <motion.a
              href="https://github.com/AasishPokhrel/shit"
              target="_blank"
              rel="noopener noreferrer"
              className="hidden sm:flex items-center space-x-2 bg-terminal-green text-black px-4 py-2 rounded-lg font-medium hover:bg-shit-gold transition-colors"
              whileHover={{ scale: 1.05 }}
              whileTap={{ scale: 0.95 }}
            >
              <GitFork className="w-4 h-4" />
              <span>Fork It</span>
            </motion.a>
            
            {/* Repository Counter */}
            <div className="text-right">
              <div className="text-xs text-gray-400">Repositories</div>
              <div className="text-sm font-terminal text-shit-gold">1,000,000,000+</div>
            </div>
          </div>
        </div>

        {/* Mobile Menu */}
        <div className="md:hidden mt-3 pt-3 border-t border-gray-800">
          <div className="flex justify-around text-sm">
            <a href="#home" className="text-gray-300 hover:text-white">Home</a>
            <a href="#story" className="text-gray-300 hover:text-white">Story</a>
            <a href="#stats" className="text-gray-300 hover:text-white">Stats</a>
            <a href="https://github.com/AasishPokhrel/shit" target="_blank" rel="noopener noreferrer" className="text-terminal-green">
              Original Repo
            </a>
          </div>
        </div>
      </div>
    </header>
  );
};
