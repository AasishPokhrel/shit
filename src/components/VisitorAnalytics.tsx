import React, { useEffect, useState } from 'react';
import { motion } from 'framer-motion';

interface VisitorStats {
  totalVisits: number;
  uniqueVisitors: number;
  todayVisits: number;
  averageTime: number;
  topCountry: string;
  shitstorms: number; // Total fireworks triggered
  achievements: number;
  easterEggs: number;
}

export const VisitorAnalytics: React.FC = () => {
  const [stats, setStats] = useState<VisitorStats>({
    totalVisits: 0,
    uniqueVisitors: 0,
    todayVisits: 0,
    averageTime: 0,
    topCountry: 'Unknown',
    shitstorms: 0,
    achievements: 0,
    easterEggs: 0
  });
  const [isVisible, setIsVisible] = useState(false);

  useEffect(() => {
    // Load stats from localStorage
    const loadStats = () => {
      const savedStats = localStorage.getItem('shit-analytics');
      if (savedStats) {
        setStats(JSON.parse(savedStats));
      }
      
      // Increment visit count
      incrementVisit();
    };

    const incrementVisit = () => {
      const today = new Date().toDateString();
      const lastVisit = localStorage.getItem('shit-last-visit');
      const isNewVisitor = !lastVisit;
      const isNewDay = lastVisit !== today;

      setStats(prev => {
        const newStats = {
          ...prev,
          totalVisits: prev.totalVisits + 1,
          uniqueVisitors: isNewVisitor ? prev.uniqueVisitors + 1 : prev.uniqueVisitors,
          todayVisits: isNewDay ? 1 : prev.todayVisits + 1,
        };
        
        localStorage.setItem('shit-analytics', JSON.stringify(newStats));
        localStorage.setItem('shit-last-visit', today);
        return newStats;
      });
    };

    // Get user's country
    fetch('https://ipapi.co/json/')
      .then(res => res.json())
      .then(data => {
        setStats(prev => ({ ...prev, topCountry: data.country_name || 'Unknown' }));
      })
      .catch(() => {
        setStats(prev => ({ ...prev, topCountry: 'Unknown' }));
      });

    loadStats();

    // Show analytics after 5 seconds
    const timer = setTimeout(() => setIsVisible(true), 5000);
    return () => clearTimeout(timer);
  }, []);

  // Function to update specific stats (called by other components)
  const updateStat = (statName: keyof VisitorStats, increment = 1) => {
    setStats(prev => {
      const currentValue = typeof prev[statName] === 'number' ? prev[statName] as number : 0;
      const newStats = { ...prev, [statName]: currentValue + increment };
      localStorage.setItem('shit-analytics', JSON.stringify(newStats));
      return newStats;
    });
  };

  // Expose function globally for other components
  useEffect(() => {
    (window as any).updateShitStats = updateStat;
  }, []);

  if (!isVisible) return null;

  return (
    <motion.div
      className="fixed bottom-4 right-4 z-40 bg-black/90 backdrop-blur-sm border border-terminal-green rounded-lg p-4 text-xs max-w-xs"
      initial={{ opacity: 0, x: 100 }}
      animate={{ opacity: 1, x: 0 }}
      exit={{ opacity: 0, x: 100 }}
    >
      <div className="flex justify-between items-center mb-3">
        <h3 className="text-terminal-green font-bold">💩 Live Stats</h3>
        <button
          onClick={() => setIsVisible(false)}
          className="text-gray-400 hover:text-white"
        >
          ×
        </button>
      </div>

      <div className="space-y-2">
        <div className="flex justify-between">
          <span className="text-gray-400">Total Visits:</span>
          <span className="text-shit-gold font-mono">{stats.totalVisits.toLocaleString()}</span>
        </div>
        <div className="flex justify-between">
          <span className="text-gray-400">Unique Visitors:</span>
          <span className="text-terminal-green font-mono">{stats.uniqueVisitors.toLocaleString()}</span>
        </div>
        <div className="flex justify-between">
          <span className="text-gray-400">Today:</span>
          <span className="text-blue-400 font-mono">{stats.todayVisits.toLocaleString()}</span>
        </div>
        <div className="flex justify-between">
          <span className="text-gray-400">Shitstorms:</span>
          <span className="text-red-400 font-mono">{stats.shitstorms.toLocaleString()}</span>
        </div>
        <div className="flex justify-between">
          <span className="text-gray-400">From:</span>
          <span className="text-purple-400 font-mono">{stats.topCountry}</span>
        </div>
      </div>

      <div className="mt-3 pt-2 border-t border-gray-700 text-center">
        <div className="text-gray-500 text-xs">
          You are visitor #{stats.totalVisits}
        </div>
      </div>
    </motion.div>
  );
};