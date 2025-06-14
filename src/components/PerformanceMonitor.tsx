import React, { useEffect, useState } from 'react';
import { motion, AnimatePresence } from 'framer-motion';

interface PerformanceStats {
  fps: number;
  loadTime: number;
  memoryUsage: number;
  animationCount: number;
  renderTime: number;
}

export const PerformanceMonitor: React.FC = () => {
  const [stats, setStats] = useState<PerformanceStats>({
    fps: 60,
    loadTime: 0,
    memoryUsage: 0,
    animationCount: 0,
    renderTime: 0
  });
  const [showStats, setShowStats] = useState(false);
  const [isLowPerformance, setIsLowPerformance] = useState(false);

  useEffect(() => {
    // Measure initial load time
    const loadStartTime = performance.timing?.navigationStart || performance.timeOrigin;
    const loadEndTime = performance.timing?.loadEventEnd || Date.now();
    const loadTime = loadEndTime - loadStartTime;
    
    setStats(prev => ({ ...prev, loadTime }));

    // FPS monitoring
    let frameCount = 0;
    let lastTime = performance.now();
    let fpsInterval: number;

    const measureFPS = () => {
      const currentTime = performance.now();
      frameCount++;

      if (currentTime >= lastTime + 1000) {
        const fps = Math.round((frameCount * 1000) / (currentTime - lastTime));
        setStats(prev => ({ ...prev, fps }));
        
        // Check for low performance
        if (fps < 30) {
          setIsLowPerformance(true);
        } else if (fps > 45) {
          setIsLowPerformance(false);
        }
        
        frameCount = 0;
        lastTime = currentTime;
      }

      fpsInterval = requestAnimationFrame(measureFPS);
    };

    measureFPS();

    // Memory usage monitoring (if available)
    const checkMemory = () => {
      if ('memory' in performance) {
        const memory = (performance as any).memory;
        const memoryUsage = Math.round(memory.usedJSHeapSize / 1024 / 1024);
        setStats(prev => ({ ...prev, memoryUsage }));
      }
    };

    const memoryInterval = setInterval(checkMemory, 2000);

    // Animation count monitoring
    const countAnimations = () => {
      const animatedElements = document.querySelectorAll('[style*="animation"], [style*="transition"], .animate');
      setStats(prev => ({ ...prev, animationCount: animatedElements.length }));
    };

    const animationInterval = setInterval(countAnimations, 1000);

    // Performance optimizations based on device capabilities
    const optimizeForDevice = () => {
      const isLowEndDevice = navigator.hardwareConcurrency <= 2 || 
                           /Android.*Chrome\/[1-6][0-9]/.test(navigator.userAgent) ||
                           window.innerWidth <= 480;

      if (isLowEndDevice) {
        // Reduce animations for low-end devices
        document.documentElement.style.setProperty('--animation-duration', '0.1s');
        document.documentElement.style.setProperty('--particle-count', '5');
        
        // Disable some expensive effects
        const style = document.createElement('style');
        style.textContent = `
          .reduce-motion * {
            animation-duration: 0.1s !important;
            transition-duration: 0.1s !important;
          }
          .reduce-motion .matrix-background {
            opacity: 0.3 !important;
          }
          .reduce-motion .floating-shit {
            opacity: 0.5 !important;
          }
        `;
        document.head.appendChild(style);
        document.body.classList.add('reduce-motion');
      }
    };

    optimizeForDevice();

    // Show performance stats in console for debugging
    const logPerformance = () => {
      console.group('🚀 Shit Website Performance Stats');
      console.log('FPS:', stats.fps);
      console.log('Load Time:', stats.loadTime + 'ms');
      console.log('Memory Usage:', stats.memoryUsage + 'MB');
      console.log('Active Animations:', stats.animationCount);
      console.log('Low Performance Mode:', isLowPerformance);
      console.groupEnd();
    };

    // Log performance every 10 seconds
    const logInterval = setInterval(logPerformance, 10000);

    // Expose performance controls globally
    (window as any).shitPerformance = {
      getStats: () => stats,
      toggleLowPerformance: () => setIsLowPerformance(!isLowPerformance),
      showStats: () => setShowStats(true),
      hideStats: () => setShowStats(false),
      optimize: optimizeForDevice
    };

    return () => {
      cancelAnimationFrame(fpsInterval);
      clearInterval(memoryInterval);
      clearInterval(animationInterval);
      clearInterval(logInterval);
      delete (window as any).shitPerformance;
    };
  }, []);

  // Auto-optimization when performance drops
  useEffect(() => {
    if (isLowPerformance) {
      // Reduce particle effects
      const particles = document.querySelectorAll('.floating-shit > div');
      particles.forEach((particle, index) => {
        if (index % 2 === 0) {
          (particle as HTMLElement).style.display = 'none';
        }
      });

      // Reduce matrix background intensity
      const matrix = document.querySelector('.matrix-background');
      if (matrix) {
        (matrix as HTMLElement).style.opacity = '0.3';
      }

      // Show performance warning
      console.warn('🐌 Low performance detected. Optimizing experience...');
    }
  }, [isLowPerformance]);

  // Developer keyboard shortcut to toggle stats
  useEffect(() => {
    const handleKeyPress = (e: KeyboardEvent) => {
      if (e.ctrlKey && e.shiftKey && e.key === 'P') {
        setShowStats(!showStats);
      }
    };

    window.addEventListener('keydown', handleKeyPress);
    return () => window.removeEventListener('keydown', handleKeyPress);
  }, [showStats]);

  return (
    <>
      {/* Performance Warning */}
      <AnimatePresence>
        {isLowPerformance && (
          <motion.div
            className="fixed top-4 right-4 z-50 bg-yellow-600 text-black p-3 rounded-lg text-sm max-w-xs"
            initial={{ opacity: 0, x: 100 }}
            animate={{ opacity: 1, x: 0 }}
            exit={{ opacity: 0, x: 100 }}
          >
            <div className="flex items-center gap-2">
              <span>🐌</span>
              <div>
                <div className="font-bold">Performance Mode</div>
                <div className="text-xs">Reduced effects for better experience</div>
              </div>
            </div>
          </motion.div>
        )}
      </AnimatePresence>

      {/* Performance Stats Panel */}
      <AnimatePresence>
        {showStats && (
          <motion.div
            className="fixed bottom-4 left-4 z-50 bg-black/90 border border-terminal-green rounded-lg p-4 text-xs font-mono"
            initial={{ opacity: 0, y: 100 }}
            animate={{ opacity: 1, y: 0 }}
            exit={{ opacity: 0, y: 100 }}
          >
            <div className="flex justify-between items-center mb-3">
              <h3 className="text-terminal-green font-bold">🚀 Performance</h3>
              <button
                onClick={() => setShowStats(false)}
                className="text-gray-400 hover:text-white"
              >
                ×
              </button>
            </div>

            <div className="space-y-1">
              <div className="flex justify-between">
                <span className="text-gray-400">FPS:</span>
                <span className={stats.fps >= 45 ? 'text-green-400' : stats.fps >= 30 ? 'text-yellow-400' : 'text-red-400'}>
                  {stats.fps}
                </span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-400">Load Time:</span>
                <span className="text-blue-400">{Math.round(stats.loadTime)}ms</span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-400">Memory:</span>
                <span className="text-purple-400">{stats.memoryUsage}MB</span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-400">Animations:</span>
                <span className="text-cyan-400">{stats.animationCount}</span>
              </div>
              <div className="flex justify-between">
                <span className="text-gray-400">Mode:</span>
                <span className={isLowPerformance ? 'text-yellow-400' : 'text-green-400'}>
                  {isLowPerformance ? 'Optimized' : 'Normal'}
                </span>
              </div>
            </div>

            <div className="mt-3 pt-2 border-t border-gray-700 text-center">
              <div className="text-gray-500 text-xs">
                Ctrl+Shift+P to toggle
              </div>
            </div>
          </motion.div>
        )}
      </AnimatePresence>
    </>
  );
};