import { useEffect } from "react";

export const ConsoleFeatures: React.FC = () => {
  useEffect(() => {
    // Console ASCII Art
    const shitArt = `
╔══════════════════════════════════════════════╗
║               💩 SHIT CONSOLE 💩              ║
╚══════════════════════════════════════════════╝

Welcome to the secret shit console! 🎉

Available commands:
• shit.party() - Trigger massive fireworks party
• shit.konami() - Reveal hidden game
• shit.matrix() - Toggle matrix mode
• shit.rainbow() - Rainbow shit mode
• shit.stats() - Show secret stats
• shit.clean() - Clean up the mess
• shit.surprise() - Random surprise
• shit.languages() - Show all shit languages
• shit.achievement() - Unlock secret achievement

Type any command to get started!
Remember: With great shit comes great responsibility 💩
    `;

    console.log(shitArt);

    // Hidden achievement system
    const unlockAchievement = (name: string, description: string) => {
      if ((window as any).unlockShitAchievement) {
        (window as any).unlockShitAchievement('console_hacker');
      }
      console.log(`🏆 Achievement Unlocked: ${name} - ${description}`);
      
      // Trigger celebration
      if ((window as any).shitFirework) {
        for (let i = 0; i < 5; i++) {
          setTimeout(() => {
            (window as any).shitFirework?.(
              Math.random() * window.innerWidth,
              Math.random() * window.innerHeight
            );
          }, i * 100);
        }
      }
    };

    // Create global shit object with commands
    const shit = {
      party: () => {
        console.log('💩 SHIT PARTY ACTIVATED! 💩');
        if (window.shitParty) {
          window.shitParty();
        }
        unlockAchievement('Party Animal', 'Started the ultimate shit party!');
      },

      konami: () => {
        console.log('🎮 Konami code activated! Use ↑↑↓↓←→←→BA to unlock the secret game!');
        const event = new KeyboardEvent('keydown', { code: 'ArrowUp' });
        for (let i = 0; i < 10; i++) {
          setTimeout(() => window.dispatchEvent(event), i * 100);
        }
        unlockAchievement('Code Master', 'Discovered the Konami code!');
      },

      matrix: () => {
        console.log('🕶️ Welcome to the Matrix... of shit!');
        document.body.style.filter = document.body.style.filter === 'hue-rotate(120deg)' ? '' : 'hue-rotate(120deg)';
        unlockAchievement('Neo', 'Entered the shit matrix!');
      },

      rainbow: () => {
        console.log('🌈 Rainbow shit mode activated!');
        const style = document.createElement('style');
        style.textContent = `
          .rainbow-shit {
            animation: rainbow-shift 2s linear infinite;
          }
          @keyframes rainbow-shift {
            0% { filter: hue-rotate(0deg); }
            100% { filter: hue-rotate(360deg); }
          }
        `;
        document.head.appendChild(style);
        document.body.classList.toggle('rainbow-shit');
        unlockAchievement('Rainbow Master', 'Unlocked rainbow shit mode!');
      },

      stats: () => {
        const savedAchievements = JSON.parse(localStorage.getItem('shit-achievements') || '[]');
        const achievementCount = savedAchievements.filter((a: any) => a.unlocked).length;
        
        const stats = {
          'Clicks': Math.floor(Math.random() * 1000 + 500),
          'Shit Quality': 'Premium 💩',
          'Achievements': achievementCount,
          'Secret Level': '💯',
          'Toilet Paper Used': Math.floor(Math.random() * 100 + 50) + ' rolls',
          'Flushed': Math.floor(Math.random() * 50 + 25) + ' times',
          'Legendary Status': 'ULTIMATE SHIT MASTER',
        };
        console.table(stats);
        unlockAchievement('Data Analyst', 'Checked the secret stats!');
      },

      clean: () => {
        console.log('🧽 Cleaning up the shit...');
        document.body.style.filter = '';
        document.body.classList.remove('rainbow-shit');
        const rainbowStyle = document.querySelector('style');
        if (rainbowStyle) rainbowStyle.remove();
        console.log('✨ All clean! (but the memories remain)');
        unlockAchievement('Janitor', 'Cleaned up the mess!');
      },

      surprise: () => {
        const surprises = [
          () => {
            console.log('🎲 Surprise: Making all text upside down!');
            document.body.style.transform = document.body.style.transform === 'rotate(180deg)' ? '' : 'rotate(180deg)';
          },
          () => {
            console.log('🎵 Surprise: Console music! 🎶');
            const notes = ['🎵', '🎶', '🎼', '🎤', '🎧'];
            notes.forEach((note, i) => {
              setTimeout(() => console.log(note), i * 200);
            });
          },
          () => {
            console.log('👻 Surprise: Spooky ghost mode!');
            document.body.style.opacity = document.body.style.opacity === '0.5' ? '1' : '0.5';
          },
          () => {
            console.log('🌪️ Surprise: Spinning shit!');
            document.body.style.animation = document.body.style.animation ? '' : 'spin 2s linear infinite';
            const spinStyle = document.createElement('style');
            spinStyle.textContent = '@keyframes spin { from { transform: rotate(0deg); } to { transform: rotate(360deg); } }';
            document.head.appendChild(spinStyle);
          }
        ];
        const randomSurprise = surprises[Math.floor(Math.random() * surprises.length)];
        randomSurprise();
        unlockAchievement('Surprise Me', 'Triggered a random surprise!');
      },

      languages: () => {
        const languages = {
          '🇺🇸 English': 'shit, crap, poop',
          '🇪🇸 Spanish': 'mierda, caca',
          '🇫🇷 French': 'merde, caca',
          '🇩🇪 German': 'scheiße, kacke',
          '🇮🇹 Italian': 'merda, cacca',
          '🇵🇹 Portuguese': 'merda, cocô',
          '🇷🇺 Russian': 'говно, дерьмо',
          '🇯🇵 Japanese': 'うんち, クソ',
          '🇨🇳 Chinese': '屎, 粪',
          '🇰🇷 Korean': '똥, 대변',
          '🇮🇳 Hindi': 'गू, टट्टी',
          '🇸🇦 Arabic': 'خرا, براز'
        };
        console.log('🌍 Shit in Different Languages:');
        console.table(languages);
        unlockAchievement('Polyglot', 'Learned shit in many languages!');
      },

      achievement: () => {
        const savedAchievements = JSON.parse(localStorage.getItem('shit-achievements') || '[]');
        const unlockedAchievements = savedAchievements.filter((a: any) => a.unlocked);
        
        console.log('🏆 Your Achievements:');
        if (unlockedAchievements.length === 0) {
          console.log('No achievements yet! Try some commands to unlock them.');
        } else {
          unlockedAchievements.forEach((achievement: any, index: number) => {
            console.log(`${index + 1}. ${achievement.title} - ${achievement.description}`);
          });
        }
        console.log(`Total: ${unlockedAchievements.length}/10 achievements`);
        
        if (unlockedAchievements.length >= 8) {
          unlockAchievement('Achievement Hunter', 'Unlocked most achievements!');
        }
      },

      // Secret easter eggs
      help: () => {
        console.log('💩 Shit Help Center:');
        console.log('Available commands: party, konami, matrix, rainbow, stats, clean, surprise, languages, achievement');
        console.log('Type shit.[command]() to execute');
        unlockAchievement('Help Seeker', 'Asked for help!');
      },

      version: () => {
        console.log('💩 Shit Console v2.0.1337 - "The Legendary Edition"');
        unlockAchievement('Version Checker', 'Checked the version!');
      },

      credits: () => {
        console.log(`
💩 SHIT CREDITS 💩
Original Repo: AasishPokhrel/shit
Enhanced by: mayphus
Built with: React, TypeScript, Framer Motion, and lots of 💩
Special thanks to: GitHub for reaching 1 billion repos!
        `);
        unlockAchievement('Credits Watcher', 'Watched the credits!');
      }
    };

    // Make it globally available
    (window as any).shit = shit;

    // Add some fun console styling
    console.log('%c💩 SHIT CONSOLE LOADED 💩', 'color: #8B4513; font-size: 20px; font-weight: bold; text-shadow: 2px 2px 4px rgba(0,0,0,0.5);');
    console.log('%cType "shit.help()" for available commands!', 'color: #FFD700; font-size: 14px;');

    // Random console messages
    const randomMessages = [
      'Did you know? This is repository #1,000,000,000 on GitHub! 🎉',
      'Fun fact: The word "shit" appears 42 times in this codebase 💩',
      'Easter egg: Try the Konami code! ↑↑↓↓←→←→BA',
      'Pro tip: Ctrl+Click anywhere for fireworks! 🎆',
      'Secret: There are multiple hidden games on this page 🎮',
    ];

    setTimeout(() => {
      const randomMessage = randomMessages[Math.floor(Math.random() * randomMessages.length)];
      console.log(`💡 ${randomMessage}`);
    }, 5000);

    // Achievement check on load
    const savedAchievements = JSON.parse(localStorage.getItem('shit-achievements') || '[]');
    const hasConsoleAchievement = savedAchievements.some((a: any) => a.id === 'console_hacker');
    
    if (!hasConsoleAchievement) {
      unlockAchievement('Console Explorer', 'Opened the developer console!');
    }

    // Cleanup
    return () => {
      delete (window as any).shit;
    };
  }, []);

  return null;
};

export default ConsoleFeatures;