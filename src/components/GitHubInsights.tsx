import React, { useEffect, useState } from "react";
import { motion } from "framer-motion";
import { Github, TrendingUp, Users, Building, Code, Globe } from "lucide-react";
import { GITHUB_REAL_DATA } from "../services/githubData";

export const GitHubInsights: React.FC = () => {
  const [currentTime, setCurrentTime] = useState(new Date());
  const [repoGrowth, setRepoGrowth] = useState(0);

  useEffect(() => {
    const interval = setInterval(() => {
      setCurrentTime(new Date());
      // Simulate real-time repo growth (approximately 12M repos per day = 139 per second)
      setRepoGrowth(prev => prev + Math.floor(Math.random() * 200 + 100));
    }, 1000);

    return () => clearInterval(interval);
  }, []);

  const stats = [
    {
      icon: <Github className="w-6 h-6" />,
      label: "Total Repositories",
      value: (1000000000 + repoGrowth).toLocaleString(),
      trend: "+12M/day",
      color: "text-terminal-green",
    },
    {
      icon: <Users className="w-6 h-6" />,
      label: "Active Developers",
      value: GITHUB_REAL_DATA.currentStats.totalUsers.toLocaleString(),
      trend: "+2M/month",
      color: "text-blue-400",
    },
    {
      icon: <Building className="w-6 h-6" />,
      label: "Organizations",
      value: GITHUB_REAL_DATA.currentStats.totalOrganizations.toLocaleString(),
      trend: "+500K/month",
      color: "text-purple-400",
    },
    {
      icon: <Code className="w-6 h-6" />,
      label: "Lines of Code",
      value: "31T+",
      trend: "Growing fast",
      color: "text-shit-gold",
    },
    {
      icon: <Globe className="w-6 h-6" />,
      label: "Countries",
      value: GITHUB_REAL_DATA.currentStats.countriesRepresented.toString(),
      trend: "Global reach",
      color: "text-green-400",
    },
  ];

  return (
    <section className="bg-gray-900/50 border-y border-terminal-green py-8 relative overflow-hidden">
      <div className="max-w-7xl mx-auto px-4">
        {/* Section Header */}
        <motion.div
          className="text-center mb-8"
          initial={{ opacity: 0, y: -20 }}
          whileInView={{ opacity: 1, y: 0 }}
          viewport={{ once: true }}
        >
          <h3 className="text-2xl md:text-3xl font-terminal text-terminal-green mb-2">
            🔥 Live GitHub Universe Stats 🔥
          </h3>
          <p className="text-gray-400 text-sm md:text-base">
            Real-time glimpse into the world's largest developer platform
          </p>
          <div className="text-xs text-gray-500 mt-2">
            Last updated: {currentTime.toLocaleTimeString()}
          </div>
        </motion.div>

        {/* Stats Grid */}
        <div className="grid grid-cols-1 sm:grid-cols-2 lg:grid-cols-5 gap-4 md:gap-6">
          {stats.map((stat, index) => (
            <motion.div
              key={stat.label}
              className="bg-black/60 border border-gray-700 rounded-lg p-4 hover:border-terminal-green/50 transition-all duration-300"
              initial={{ opacity: 0, y: 20 }}
              whileInView={{ opacity: 1, y: 0 }}
              transition={{ delay: index * 0.1 }}
              viewport={{ once: true }}
              whileHover={{ scale: 1.02, y: -2 }}
            >
              <div className="flex items-center space-x-3 mb-3">
                <div className={`${stat.color}`}>
                  {stat.icon}
                </div>
                <div className="text-xs text-gray-400 uppercase tracking-wide">
                  {stat.label}
                </div>
              </div>
              
              <div className="space-y-1">
                <div className={`text-xl md:text-2xl font-terminal font-bold ${stat.color}`}>
                  {stat.value}
                </div>
                <div className="flex items-center space-x-1 text-xs text-gray-500">
                  <TrendingUp className="w-3 h-3" />
                  <span>{stat.trend}</span>
                </div>
              </div>
            </motion.div>
          ))}
        </div>

        {/* Language Distribution */}
        <motion.div
          className="mt-8 bg-black/40 border border-gray-700 rounded-lg p-6"
          initial={{ opacity: 0 }}
          whileInView={{ opacity: 1 }}
          transition={{ delay: 0.5 }}
          viewport={{ once: true }}
        >
          <h4 className="text-lg font-terminal text-terminal-green mb-4 text-center">
            Top Programming Languages on GitHub
          </h4>
          
          <div className="grid grid-cols-2 md:grid-cols-5 gap-4">
            {GITHUB_REAL_DATA.topLanguages.slice(0, 10).map((lang, index) => (
              <motion.div
                key={lang.name}
                className="text-center"
                initial={{ opacity: 0, scale: 0.8 }}
                whileInView={{ opacity: 1, scale: 1 }}
                transition={{ delay: index * 0.05 }}
                viewport={{ once: true }}
              >
                <div 
                  className="w-4 h-4 rounded-full mx-auto mb-2"
                  style={{ backgroundColor: lang.color }}
                />
                <div className="text-sm text-white font-medium">{lang.name}</div>
                <div className="text-xs text-gray-400">{lang.percentage}%</div>
              </motion.div>
            ))}
          </div>
        </motion.div>

        {/* Fun Fact Ticker */}
        <motion.div
          className="mt-6 text-center"
          initial={{ opacity: 0 }}
          whileInView={{ opacity: 1 }}
          transition={{ delay: 0.7 }}
          viewport={{ once: true }}
        >
          <div className="text-sm text-gray-400 mb-2">💡 Did you know?</div>
          <motion.div
            className="text-terminal-amber text-sm md:text-base max-w-3xl mx-auto"
            key={Math.floor(currentTime.getSeconds() / 10)}
            initial={{ opacity: 0, x: 20 }}
            animate={{ opacity: 1, x: 0 }}
            transition={{ duration: 0.5 }}
          >
            {GITHUB_REAL_DATA.funFacts[Math.floor(currentTime.getSeconds() / 10) % GITHUB_REAL_DATA.funFacts.length]}
          </motion.div>
        </motion.div>
      </div>
    </section>
  );
};
