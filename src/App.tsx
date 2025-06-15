import { 
  HeroSection, 
  StorySection, 
  ShitStatsSection, 
  ShitQuotesCarousel,
  GitHubHeader,
  GitHubInsights
} from "./components";
import { LoadingScreen } from "./components/LoadingScreen";
import { useState } from "react";

function App() {
  const [isLoaded, setIsLoaded] = useState(false);

  return (
    <>
      {/* Loading Screen */}
      {!isLoaded && <LoadingScreen onComplete={() => setIsLoaded(true)} />}
      
      {/* Main App - GitHub-like Layout */}
      {isLoaded && (
        <div className="min-h-screen bg-white text-gray-900 relative">
          {/* GitHub Header */}
          <GitHubHeader />
          
          {/* Main Content Container */}
          <main className="max-w-7xl mx-auto px-4 sm:px-6 lg:px-8 py-8">
            {/* Repository Banner Section */}
            <div className="mb-8">
              <HeroSection />
            </div>
            
            {/* GitHub Insights */}
            <div className="mb-8">
              <GitHubInsights />
            </div>
            
            {/* Repository Story */}
            <div className="mb-8">
              <StorySection />
            </div>
            
            {/* Statistics Section */}
            <div className="mb-8">
              <ShitStatsSection />
            </div>
            
            {/* Quotes Section */}
            <div className="mb-8">
              <ShitQuotesCarousel />
            </div>
          </main>
          
          {/* Footer */}
          <footer className="bg-gray-50 border-t border-gray-200 py-12 px-4">
            <div className="max-w-7xl mx-auto">
              <div className="text-center mb-8">
                <div className="text-4xl mb-4">💩</div>
                <h3 className="text-2xl font-semibold text-gray-900 mb-2">
                  Repository #1,000,000,000
                </h3>
                <p className="text-lg text-gray-600 mb-6">
                  The most legendary coincidence in coding history
                </p>
              </div>
              
              {/* Repository Links */}
              <div className="flex flex-wrap justify-center gap-6 text-sm text-gray-600 mb-8">
                <a 
                  href="https://github.com/AasishPokhrel/shit" 
                  target="_blank" 
                  rel="noopener noreferrer"
                  className="hover:text-blue-600 transition-colors flex items-center gap-2 font-medium"
                >
                  <span>🔗</span> Original Repository
                </a>
                <span>•</span>
                <a 
                  href="https://github.com/mayphus/shit" 
                  target="_blank" 
                  rel="noopener noreferrer"
                  className="hover:text-blue-600 transition-colors flex items-center gap-2 font-medium"
                >
                  <span>🚀</span> Enhanced Fork
                </a>
                <span>•</span>
                <a 
                  href="https://github.com" 
                  target="_blank" 
                  rel="noopener noreferrer"
                  className="hover:text-blue-600 transition-colors flex items-center gap-2 font-medium"
                >
                  <span>💼</span> GitHub
                </a>
              </div>

              {/* Repository Information Grid */}
              <div className="grid grid-cols-1 md:grid-cols-3 gap-6 mb-8">
                <div className="bg-white p-6 rounded-lg border border-gray-200 hover:border-gray-300 transition-colors">
                  <div className="text-blue-600 font-semibold mb-2">Repository Details</div>
                  <div className="text-sm text-gray-600 space-y-1">
                    <div>� Historic Milestone</div>
                    <div>📅 Created June 11, 2025</div>
                    <div>� Community Driven</div>
                  </div>
                </div>
                <div className="bg-white p-6 rounded-lg border border-gray-200 hover:border-gray-300 transition-colors">
                  <div className="text-green-600 font-semibold mb-2">Project Features</div>
                  <div className="text-sm text-gray-600 space-y-1">
                    <div>⚡ React + TypeScript</div>
                    <div>🎨 Tailwind CSS</div>
                    <div>📱 Mobile Responsive</div>
                  </div>
                </div>
                <div className="bg-white p-6 rounded-lg border border-gray-200 hover:border-gray-300 transition-colors">
                  <div className="text-purple-600 font-semibold mb-2">Community</div>
                  <div className="text-sm text-gray-600 space-y-1">
                    <div>🌍 Global Recognition</div>
                    <div>💡 Open Source</div>
                    <div>🤝 Collaborative</div>
                  </div>
                </div>
              </div>

              <div className="flex flex-wrap justify-center gap-4 text-sm text-gray-500">
                <span>Made with 💩 and ❤️</span>
                <span>•</span>
                <span>Enhanced by mayphus</span>
                <span>•</span>
                <span>Original by AasishPokhrel</span>
              </div>
              
              <div className="mt-4 text-center text-sm text-gray-400">
                Created on June 11, 2025 - The day GitHub made history
              </div>
            </div>
          </footer>
        </div>
      )}
    </>
  );
}

export default App;