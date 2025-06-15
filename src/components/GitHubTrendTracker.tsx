import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { TrendingUp, Github, MessageSquare, Users, Activity } from 'lucide-react';

interface TrendData {
  metric: string;
  value: number;
  change: number;
  icon: React.ReactNode;
  color: string;
}

export const GitHubTrendTracker: React.FC = () => {
  const [trends, setTrends] = useState<TrendData[]>([
    {
      metric: 'Stars/Hour',
      value: 47,
      change: 12.5,
      icon: <TrendingUp className="w-4 h-4" />,
      color: 'text-yellow-400'
    },
    {
      metric: 'New Issues',
      value: 23,
      change: 156.7,
      icon: <MessageSquare className="w-4 h-4" />,
      color: 'text-orange-400'
    },
    {
      metric: 'Active Visitors',
      value: 892,
      change: 89.2,
      icon: <Users className="w-4 h-4" />,
      color: 'text-blue-400'
    },
    {
      metric: 'Global Rank',
      value: 1,
      change: 0,
      icon: <Activity className="w-4 h-4" />,
      color: 'text-purple-400'
    }
  ]);

  const [currentIndex, setCurrentIndex] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => {
      setTrends(prev => prev.map(trend => ({
        ...trend,
        value: trend.metric === 'Global Rank' ? 1 : trend.value + Math.floor(Math.random() * 5),
        change: trend.metric === 'Global Rank' ? 0 : trend.change + (Math.random() - 0.5) * 10
      })));
    }, 5000);

    return () => clearInterval(interval);
  }, []);

  useEffect(() => {
    const cycleInterval = setInterval(() => {
      setCurrentIndex((prev) => (prev + 1) % trends.length);
    }, 3000);

    return () => clearInterval(cycleInterval);
  }, [trends.length]);

  const currentTrend = trends[currentIndex];

  return (
    <div className="fixed top-1/2 left-4 transform -translate-y-1/2 z-40">
      <motion.div
        initial={{ x: -100, opacity: 0 }}
        animate={{ x: 0, opacity: 1 }}
        className="bg-gradient-to-br from-indigo-900/95 to-purple-900/95 backdrop-blur-sm rounded-lg border border-indigo-500/30 p-4 shadow-2xl"
      >
        <div className="flex items-center gap-2 mb-3">
          <Github className="w-4 h-4 text-indigo-400" />
          <span className="text-xs text-indigo-200 font-semibold">
            Trending Now • Live Metrics
          </span>
        </div>

        <motion.div
          key={currentIndex}
          initial={{ opacity: 0, scale: 0.8 }}
          animate={{ opacity: 1, scale: 1 }}
          exit={{ opacity: 0, scale: 0.8 }}
          transition={{ duration: 0.5 }}
          className="space-y-3"
        >
          <div className="flex items-center gap-3">
            <div className={`${currentTrend.color} p-2 bg-white/10 rounded-full`}>
              {currentTrend.icon}
            </div>
            
            <div className="flex-1">
              <div className="text-white text-lg font-bold">
                {currentTrend.metric === 'Global Rank' ? '#' : ''}{currentTrend.value.toLocaleString()}
              </div>
              <div className="text-xs text-indigo-300">
                {currentTrend.metric}
              </div>
            </div>
          </div>

          {currentTrend.change !== 0 && (
            <div className="flex items-center gap-1">
              <TrendingUp className={`w-3 h-3 ${currentTrend.change > 0 ? 'text-green-400' : 'text-red-400'}`} />
              <span className={`text-xs font-medium ${currentTrend.change > 0 ? 'text-green-400' : 'text-red-400'}`}>
                {currentTrend.change > 0 ? '+' : ''}{currentTrend.change.toFixed(1)}%
              </span>
              <span className="text-xs text-indigo-300">vs. last hour</span>
            </div>
          )}

          <div className="bg-indigo-800/30 rounded-md p-2">
            <div className="text-xs text-indigo-200 text-center">
              💩 Most legendary repository on GitHub
            </div>
          </div>
        </motion.div>

        {/* Indicator dots */}
        <div className="flex justify-center gap-1 mt-3">
          {trends.map((_, index) => (
            <motion.div
              key={index}
              className={`w-2 h-2 rounded-full transition-colors ${
                index === currentIndex ? 'bg-indigo-400' : 'bg-indigo-700'
              }`}
              animate={index === currentIndex ? { scale: [1, 1.2, 1] } : { scale: 1 }}
              transition={{ repeat: index === currentIndex ? Infinity : 0, duration: 2 }}
            />
          ))}
        </div>

        <div className="mt-2 text-xs text-indigo-400 text-center">
          Real-time GitHub Analytics
        </div>
      </motion.div>
    </div>
  );
};
