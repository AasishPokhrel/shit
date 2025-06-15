import React, { useState, useEffect } from 'react';
import { motion, AnimatePresence } from 'framer-motion';
import { GitPullRequest, Check, X, MessageSquare, GitMerge, Clock } from 'lucide-react';

const SAMPLE_PRS = [
  {
    id: 426,
    title: 'feat: shit in Python',
    author: 'HenryXiaoYang',
    status: 'open' as const,
    description: 'Adding Python implementation of shit',
    language: 'Python',
    additions: 42,
    deletions: 0,
    files: 1
  },
  {
    id: 409,
    title: 'Add a theme song',
    author: 'rick2047',
    status: 'open' as const,
    description: 'The awesome repo deserves an awesome theme song',
    language: 'Audio',
    additions: 4,
    deletions: 1,
    files: 1
  },
  {
    id: 396,
    title: 'feat(README): ✨ Add initial content to README.vi.md',
    author: 'nguyenphutrong',
    status: 'open' as const,
    description: 'Vietnamese translation support',
    language: 'Markdown',
    additions: 156,
    deletions: 0,
    files: 1
  },
  {
    id: 332,
    title: 'Add Galician, Euskera and Chinese support',
    author: 'willysuna',
    status: 'merged' as const,
    description: 'Multi-language support expansion',
    language: 'Markdown',
    additions: 89,
    deletions: 12,
    files: 3
  },
  {
    id: 299,
    title: 'Fix critical shit bug',
    author: 'bugfixer',
    status: 'closed' as const,
    description: 'This PR was not worthy of the legendary repo',
    language: 'TypeScript',
    additions: 12,
    deletions: 45,
    files: 2
  },
  {
    id: 325,
    title: 'docs: SHITME.md',
    author: 'alexchexes',
    status: 'open' as const,
    description: 'Documentation improvements',
    language: 'Markdown',
    additions: 265,
    deletions: 0,
    files: 1
  },
  {
    id: 313,
    title: '🆕🎉 Add Full Support for Valve Source 2013 SDK! 🎮💻',
    author: 'Edouard127',
    status: 'open' as const,
    description: 'Gaming integration for the legendary repo',
    language: 'C++',
    additions: 1247,
    deletions: 23,
    files: 15
  },
  {
    id: 312,
    title: 'Add Brainrot language support',
    author: 'JeroenoBoy',
    status: 'open' as const,
    description: 'For the truly enlightened developers',
    language: 'Brainrot',
    additions: 69,
    deletions: 0,
    files: 1
  }
];

export const PullRequestSimulator: React.FC = () => {
  const [currentPR, setCurrentPR] = useState(0);
  const [isVisible, setIsVisible] = useState(true);
  const [autoApprove, setAutoApprove] = useState(false);

  useEffect(() => {
    const interval = setInterval(() => {
      setIsVisible(false);
      setTimeout(() => {
        setCurrentPR((prev) => (prev + 1) % SAMPLE_PRS.length);
        setIsVisible(true);
      }, 300);
    }, 6000);

    return () => clearInterval(interval);
  }, []);

  const pr = SAMPLE_PRS[currentPR];

  const getStatusIcon = (status: string) => {
    switch (status) {
      case 'merged':
        return <GitMerge className="w-4 h-4 text-purple-400" />;
      case 'closed':
        return <X className="w-4 h-4 text-red-400" />;
      default:
        return <GitPullRequest className="w-4 h-4 text-green-400" />;
    }
  };

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'merged':
        return 'from-purple-900/95 to-indigo-900/95 border-purple-500/30';
      case 'closed':
        return 'from-red-900/95 to-pink-900/95 border-red-500/30';
      default:
        return 'from-green-900/95 to-emerald-900/95 border-green-500/30';
    }
  };

  const simulateApproval = () => {
    setAutoApprove(true);
    setTimeout(() => setAutoApprove(false), 3000);
  };

  return (
    <div className="fixed bottom-4 left-1/2 transform -translate-x-1/2 z-50 max-w-md w-full mx-4">
      <motion.div
        className={`bg-gradient-to-br ${getStatusColor(pr.status)} backdrop-blur-sm rounded-lg border p-4 shadow-2xl`}
        initial={{ y: 100, opacity: 0 }}
        animate={{ y: 0, opacity: 1 }}
        transition={{ duration: 0.5 }}
      >
        <div className="flex items-center justify-between mb-3">
          <div className="flex items-center gap-2">
            {getStatusIcon(pr.status)}
            <span className="text-xs text-white font-semibold">
              Pull Request #{pr.id}
            </span>
          </div>
          <span className={`text-xs px-2 py-1 rounded-full ${
            pr.status === 'merged' ? 'bg-purple-700/50 text-purple-200' :
            pr.status === 'closed' ? 'bg-red-700/50 text-red-200' :
            'bg-green-700/50 text-green-200'
          }`}>
            {pr.status.toUpperCase()}
          </span>
        </div>

        <AnimatePresence mode="wait">
          {isVisible && (
            <motion.div
              key={pr.id}
              initial={{ opacity: 0, x: 20 }}
              animate={{ opacity: 1, x: 0 }}
              exit={{ opacity: 0, x: -20 }}
              transition={{ duration: 0.3 }}
              className="space-y-3"
            >
              <div>
                <h3 className="text-white text-sm font-semibold mb-1">
                  {pr.title}
                </h3>
                <p className="text-gray-300 text-xs">
                  by {pr.author} • {pr.description}
                </p>
              </div>

              <div className="flex items-center gap-4 text-xs">
                <div className="flex items-center gap-1">
                  <div className="w-3 h-3 bg-green-500 rounded"></div>
                  <span className="text-green-300">+{pr.additions}</span>
                </div>
                <div className="flex items-center gap-1">
                  <div className="w-3 h-3 bg-red-500 rounded"></div>
                  <span className="text-red-300">-{pr.deletions}</span>
                </div>
                <div className="text-gray-400">
                  {pr.files} file{pr.files !== 1 ? 's' : ''}
                </div>
                <div className="bg-gray-700 px-2 py-0.5 rounded text-gray-300">
                  {pr.language}
                </div>
              </div>

              {pr.status === 'open' && (
                <div className="flex gap-2">
                  <button
                    onClick={simulateApproval}
                    className="flex-1 bg-green-600 hover:bg-green-700 text-white text-xs px-3 py-1.5 rounded-md transition-colors flex items-center justify-center gap-1"
                  >
                    <Check className="w-3 h-3" />
                    Approve & Merge
                  </button>
                  <button className="bg-gray-600 hover:bg-gray-700 text-white text-xs px-3 py-1.5 rounded-md transition-colors flex items-center justify-center gap-1">
                    <MessageSquare className="w-3 h-3" />
                    Review
                  </button>
                </div>
              )}

              {autoApprove && (
                <motion.div
                  initial={{ opacity: 0, scale: 0.8 }}
                  animate={{ opacity: 1, scale: 1 }}
                  className="bg-green-800/30 border border-green-500/30 rounded p-2 text-center"
                >
                  <div className="text-xs text-green-200 flex items-center justify-center gap-1">
                    <Check className="w-3 h-3" />
                    Auto-approved! This shit looks good 💩
                  </div>
                </motion.div>
              )}
            </motion.div>
          )}
        </AnimatePresence>

        <div className="mt-3 flex justify-between items-center text-xs text-gray-400">
          <span>{currentPR + 1} of {SAMPLE_PRS.length}</span>
          <div className="flex items-center gap-1">
            <Clock className="w-3 h-3" />
            <span>Live PR feed</span>
          </div>
        </div>

        <div className="mt-2 flex justify-center gap-1">
          {SAMPLE_PRS.map((_, index) => (
            <div
              key={index}
              className={`w-1.5 h-1.5 rounded-full transition-colors ${
                index === currentPR ? 'bg-white' : 'bg-gray-600'
              }`}
            />
          ))}
        </div>
      </motion.div>
    </div>
  );
};
