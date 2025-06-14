import { 
  MatrixBackground, 
  FloatingShit, 
  HeroSection, 
  StorySection, 
  ShitStatsSection, 
  ShitQuotesCarousel,
  ShitDinoGame,
  MultilingualShitBackground,
  ShitFireworks,
  ConsoleFeatures,
  SurpriseElements,
  MobileOptimization,
  PerformanceMonitor,
  HelpOverlay
} from "./components";
import { LoadingScreen } from "./components/LoadingScreen";
import { VisitorAnalytics } from "./components/VisitorAnalytics";
import { SocialSharing } from "./components/SocialSharing";
import { AchievementSystem } from "./components/AchievementSystem";
import { useState } from "react";

function App() {
  const [isLoaded, setIsLoaded] = useState(false);

  return (
    <>
      {/* Loading Screen */}
      {!isLoaded && <LoadingScreen onComplete={() => setIsLoaded(true)} />}
      
      {/* Main App */}
      {isLoaded && (
        <div className="min-h-screen bg-black text-terminal-white relative overflow-x-hidden">
          {/* Background Effects */}
          <MatrixBackground />
          <MultilingualShitBackground />
          <FloatingShit count={15} />
          
          {/* Interactive Elements */}
          <ShitFireworks />
          <SurpriseElements />
          <ConsoleFeatures />
          
          {/* New Enhanced Features */}
          <VisitorAnalytics />
          <SocialSharing />
          <AchievementSystem />
          <MobileOptimization />
          <PerformanceMonitor />
          <HelpOverlay />
          
          {/* Hidden Games */}
          <ShitDinoGame />
          
          {/* Main Content */}
          <main className="relative z-10">
        <HeroSection />
        <StorySection />
        <ShitStatsSection />
        <ShitQuotesCarousel />
        
        {/* Footer */}
        <footer className="bg-black border-t-2 border-terminal-green py-8 sm:py-10 md:py-12 px-4">
          <div className="max-w-4xl mx-auto text-center">
            <div className="text-3xl mb-4">💩</div>
            <h3 className="text-xl sm:text-2xl font-terminal text-shit-gold mb-4">
              Repository #1,000,000,000
            </h3>
            <p className="text-base sm:text-lg text-gray-300 mb-6">
              The most legendary coincidence in coding history
            </p>
            
            {/* Repository Links */}
            <div className="flex flex-wrap justify-center gap-3 sm:gap-4 md:gap-6 text-xs sm:text-sm text-gray-400 mb-6">
              <a 
                href="https://github.com/AasishPokhrel/shit" 
                target="_blank" 
                rel="noopener noreferrer"
                className="hover:text-terminal-green transition-colors flex items-center gap-2"
              >
                <span>🔗</span> Original Legendary Repo
              </a>
              <span>•</span>
              <a 
                href="https://github.com/mayphus/shit" 
                target="_blank" 
                rel="noopener noreferrer"
                className="hover:text-shit-gold transition-colors flex items-center gap-2"
              >
                <span>🚀</span> Enhanced Fork
              </a>
              <span>•</span>
              <a 
                href="https://github.com" 
                target="_blank" 
                rel="noopener noreferrer"
                className="hover:text-terminal-green transition-colors flex items-center gap-2"
              >
                <span>💼</span> GitHub
              </a>
            </div>

            {/* Fun Stats */}
            <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-3 sm:gap-4 text-xs text-gray-500 mb-6">
              <div className="bg-gray-900 p-3 rounded border border-gray-700 hover:border-terminal-green transition-all duration-300 hover:scale-105 hover:rotate-1 hover:glow-terminal">
                <div className="text-terminal-green font-bold wobble-hover">Secret Features</div>
                <div className="chaos-hover">🎮 Hidden Games</div>
                <div className="chaos-hover">🎆 Click Fireworks</div>
                <div className="chaos-hover">💻 Console Commands</div>
              </div>
              <div className="bg-gray-900 p-3 rounded border border-gray-700 hover:border-shit-gold transition-all duration-300 hover:scale-105 hover:-rotate-1 hover:glow-shit">
                <div className="text-shit-gold font-bold wobble-hover">Interactive Elements</div>
                <div className="chaos-hover">🌍 Multilingual Background</div>
                <div className="chaos-hover">✨ Surprise Animations</div>
                <div className="chaos-hover">🎯 Easter Eggs</div>
              </div>
              <div className="bg-gray-900 p-3 rounded border border-gray-700 hover:border-terminal-green transition-all duration-300 hover:scale-105 hover:skew-x-2 hover:glow-terminal">
                <div className="text-terminal-green font-bold wobble-hover">Try This</div>
                <div className="chaos-hover">Type "shit" to unlock secret</div>
                <div className="chaos-hover">Konami code for game</div>
                <div className="chaos-hover">F12 console commands</div>
              </div>
            </div>

            <div className="flex flex-wrap justify-center gap-2 sm:gap-4 text-xs sm:text-sm">
              <span className="text-gray-400">Made with 💩 and ❤️</span>
              <span className="text-gray-600">•</span>
              <span className="text-gray-400">Enhanced by mayphus</span>
              <span className="text-gray-600">•</span>
              <span className="text-gray-400">Original by AasishPokhrel</span>
            </div>
            
            <div className="mt-4 sm:mt-6 text-xs text-gray-500">
              Created on June 11, 2025 - The day GitHub made history
            </div>
          </div>
        </footer>
      </main>
    </div>
      )}
    </>
  );
}

export default App;