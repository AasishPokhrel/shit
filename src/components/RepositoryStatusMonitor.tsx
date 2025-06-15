import React, { useState, useEffect } from 'react';
import { motion } from 'framer-motion';
import { AlertTriangle, CheckCircle, Clock, Users, Star, GitFork } from 'lucide-react';

interface RepoStatus {
  isAccessible: boolean;
  responseTime: number;
  stars: number;
  forks: number;
  issues: number;
  lastChecked: Date;
}

export const RepositoryStatusMonitor: React.FC = () => {
  const [status, setStatus] = useState<RepoStatus>({
    isAccessible: true,
    responseTime: 142,
    stars: 2800,
    forks: 208,
    issues: 303,
    lastChecked: new Date(),
  });

  const [isExpanded, setIsExpanded] = useState(false);

  useEffect(() => {
    // Simulate status checks every 30 seconds
    const interval = setInterval(() => {
      setStatus(prev => ({
        ...prev,
        responseTime: Math.floor(Math.random() * 200) + 50,
        stars: prev.stars + Math.floor(Math.random() * 5),
        issues: prev.issues + Math.floor(Math.random() * 3) - 1,
        lastChecked: new Date(),
        isAccessible: Math.random() > 0.1, // 90% uptime simulation
      }));
    }, 30000);

    return () => clearInterval(interval);
  }, []);

  const getStatusColor = () => {
    if (!status.isAccessible) return 'from-red-900/95 to-pink-900/95 border-red-500/30';
    if (status.responseTime > 500) return 'from-yellow-900/95 to-orange-900/95 border-yellow-500/30';
    return 'from-green-900/95 to-emerald-900/95 border-green-500/30';
  };

  const getStatusIcon = () => {
    if (!status.isAccessible) return <AlertTriangle className="w-4 h-4 text-red-400" />;
    if (status.responseTime > 500) return <Clock className="w-4 h-4 text-yellow-400" />;
    return <CheckCircle className="w-4 h-4 text-green-400" />;
  };

  const getStatusText = () => {
    if (!status.isAccessible) return 'Repository Inaccessible';
    if (status.responseTime > 500) return 'Slow Response';
    return 'All Systems Operational';
  };

  return (
    <div className="fixed top-4 right-4 z-50">
      <motion.div
        className={`bg-gradient-to-br ${getStatusColor()} backdrop-blur-sm rounded-lg border p-4 shadow-2xl cursor-pointer`}
        onClick={() => setIsExpanded(!isExpanded)}
        animate={{ width: isExpanded ? 320 : 280 }}
        transition={{ duration: 0.3 }}
      >
        <div className="flex items-center gap-2 mb-2">
          {getStatusIcon()}
          <span className="text-xs text-white font-semibold">
            GitHub/AasishPokhrel/shit • {getStatusText()}
          </span>
        </div>

        <div className="space-y-2">
          <div className="flex justify-between text-xs">
            <span className="text-gray-300">Response Time:</span>
            <span className="text-white font-mono">{status.responseTime}ms</span>
          </div>

          {isExpanded && (
            <motion.div
              initial={{ opacity: 0, height: 0 }}
              animate={{ opacity: 1, height: 'auto' }}
              exit={{ opacity: 0, height: 0 }}
              className="space-y-2"
            >
              <div className="flex justify-between text-xs">
                <div className="flex items-center gap-1">
                  <Star className="w-3 h-3" />
                  <span className="text-gray-300">Stars:</span>
                </div>
                <span className="text-white font-mono">{status.stars.toLocaleString()}</span>
              </div>

              <div className="flex justify-between text-xs">
                <div className="flex items-center gap-1">
                  <GitFork className="w-3 h-3" />
                  <span className="text-gray-300">Forks:</span>
                </div>
                <span className="text-white font-mono">{status.forks}</span>
              </div>

              <div className="flex justify-between text-xs">
                <div className="flex items-center gap-1">
                  <AlertTriangle className="w-3 h-3" />
                  <span className="text-gray-300">Open Issues:</span>
                </div>
                <span className="text-white font-mono">{status.issues}</span>
              </div>

              <div className="flex justify-between text-xs">
                <div className="flex items-center gap-1">
                  <Users className="w-3 h-3" />
                  <span className="text-gray-300">Contributors:</span>
                </div>
                <span className="text-white font-mono">3</span>
              </div>

              <div className="border-t border-gray-600 pt-2 mt-2">
                <div className="text-xs text-gray-400">
                  Last checked: {status.lastChecked.toLocaleTimeString()}
                </div>
              </div>

              {!status.isAccessible && (
                <motion.div
                  initial={{ opacity: 0 }}
                  animate={{ opacity: 1 }}
                  className="bg-red-900/30 border border-red-500/30 rounded p-2 mt-2"
                >
                  <div className="text-xs text-red-200">
                    ⚠️ Repository returning 404 Not Found
                  </div>
                  <div className="text-xs text-red-300 mt-1">
                    Possible causes: Deleted, private, or renamed
                  </div>
                </motion.div>
              )}
            </motion.div>
          )}
        </div>

        <div className="mt-2 text-xs text-gray-400 text-center">
          {isExpanded ? 'Click to minimize' : 'Click for details'}
        </div>
      </motion.div>
    </div>
  );
};
