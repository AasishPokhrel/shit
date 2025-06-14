import React, { useState, useEffect, useCallback, useRef } from "react";
import { motion } from "framer-motion";

interface GameState {
  isPlaying: boolean;
  isGameOver: boolean;
  score: number;
  highScore: number;
  playerY: number;
  playerVelocity: number;
  obstacles: Obstacle[];
  gameSpeed: number;
}

interface Obstacle {
  id: number;
  x: number;
  type: 'toilet' | 'plunger' | 'poop';
  emoji: string;
}

export const ShitDinoGame: React.FC = () => {
  const [gameState, setGameState] = useState<GameState>({
    isPlaying: false,
    isGameOver: false,
    score: 0,
    highScore: parseInt(localStorage.getItem('shitDinoHighScore') || '0'),
    playerY: 0,
    playerVelocity: 0,
    obstacles: [],
    gameSpeed: 4,
  });

  const gameLoopRef = useRef<number>();
  const [isVisible, setIsVisible] = useState(false);

  const GRAVITY = 0.8;
  const JUMP_FORCE = -15;
  const GROUND_Y = 200;
  const PLAYER_X = 100;

  const obstacleTypes = [
    { type: 'toilet' as const, emoji: '🚽', width: 40, height: 60 },
    { type: 'plunger' as const, emoji: '🪠', width: 30, height: 50 },
    { type: 'poop' as const, emoji: '💩', width: 35, height: 35 },
  ];

  const jump = useCallback(() => {
    if (!gameState.isPlaying || gameState.isGameOver) return;
    if (gameState.playerY >= 0) {
      setGameState(prev => ({
        ...prev,
        playerVelocity: JUMP_FORCE,
      }));
    }
  }, [gameState.isPlaying, gameState.isGameOver, gameState.playerY]);

  const startGame = () => {
    // Unlock achievement for playing the game
    if ((window as any).unlockShitAchievement) {
      (window as any).unlockShitAchievement('dino_player');
    }
    
    setGameState({
      isPlaying: true,
      isGameOver: false,
      score: 0,
      highScore: gameState.highScore,
      playerY: 0,
      playerVelocity: 0,
      obstacles: [],
      gameSpeed: 4,
    });
  };

  const generateObstacle = useCallback((): Obstacle => {
    const obstacleType = obstacleTypes[Math.floor(Math.random() * obstacleTypes.length)];
    return {
      id: Date.now(),
      x: 800,
      type: obstacleType.type,
      emoji: obstacleType.emoji,
    };
  }, []);

  const gameLoop = useCallback(() => {
    setGameState(prev => {
      if (!prev.isPlaying || prev.isGameOver) return prev;

      let newPlayerY = prev.playerY + prev.playerVelocity;
      let newPlayerVelocity = prev.playerVelocity + GRAVITY;

      // Ground collision
      if (newPlayerY >= 0) {
        newPlayerY = 0;
        newPlayerVelocity = 0;
      }

      // Move obstacles
      const newObstacles = prev.obstacles
        .map(obstacle => ({ ...obstacle, x: obstacle.x - prev.gameSpeed }))
        .filter(obstacle => obstacle.x > -100);

      // Generate new obstacles
      if (newObstacles.length === 0 || newObstacles[newObstacles.length - 1].x < 600) {
        if (Math.random() < 0.02) {
          newObstacles.push(generateObstacle());
        }
      }

      // Check collisions
      const playerLeft = PLAYER_X;
      const playerRight = PLAYER_X + 40;
      const playerTop = GROUND_Y - 40 + newPlayerY;
      const playerBottom = GROUND_Y - 5 + newPlayerY;

      let isGameOver = false;
      for (const obstacle of newObstacles) {
        const obsLeft = obstacle.x;
        const obsRight = obstacle.x + 40;
        const obsTop = GROUND_Y - 60;
        const obsBottom = GROUND_Y;

        if (
          playerRight > obsLeft &&
          playerLeft < obsRight &&
          playerBottom > obsTop &&
          playerTop < obsBottom
        ) {
          isGameOver = true;
          break;
        }
      }

      if (isGameOver) {
        const newHighScore = Math.max(prev.score, prev.highScore);
        localStorage.setItem('shitDinoHighScore', newHighScore.toString());
        return {
          ...prev,
          isGameOver: true,
          highScore: newHighScore,
        };
      }

      return {
        ...prev,
        playerY: newPlayerY,
        playerVelocity: newPlayerVelocity,
        obstacles: newObstacles,
        score: prev.score + 1,
        gameSpeed: Math.min(prev.gameSpeed + 0.001, 8),
      };
    });

    gameLoopRef.current = requestAnimationFrame(gameLoop);
  }, [generateObstacle]);

  useEffect(() => {
    if (gameState.isPlaying && !gameState.isGameOver) {
      gameLoopRef.current = requestAnimationFrame(gameLoop);
    }

    return () => {
      if (gameLoopRef.current) {
        cancelAnimationFrame(gameLoopRef.current);
      }
    };
  }, [gameState.isPlaying, gameState.isGameOver, gameLoop]);

  useEffect(() => {
    const handleKeyPress = (e: KeyboardEvent) => {
      if (e.code === 'Space' || e.code === 'ArrowUp') {
        e.preventDefault();
        if (gameState.isGameOver) {
          startGame();
        } else {
          jump();
        }
      }
    };

    window.addEventListener('keydown', handleKeyPress);
    return () => window.removeEventListener('keydown', handleKeyPress);
  }, [jump, gameState.isGameOver]);

  // Toggle game visibility with Konami code
  useEffect(() => {
    const konamiCode = ['ArrowUp', 'ArrowUp', 'ArrowDown', 'ArrowDown', 'ArrowLeft', 'ArrowRight', 'ArrowLeft', 'ArrowRight', 'KeyB', 'KeyA'];
    let konamiIndex = 0;

    const handleKonami = (e: KeyboardEvent) => {
      if (e.code === konamiCode[konamiIndex]) {
        konamiIndex++;
        if (konamiIndex === konamiCode.length) {
          setIsVisible(true);
          konamiIndex = 0;
          console.log('💩 Secret Shit Dino Game Activated! 💩');
        }
      } else {
        konamiIndex = 0;
      }
    };

    window.addEventListener('keydown', handleKonami);
    return () => window.removeEventListener('keydown', handleKonami);
  }, []);

  if (!isVisible) return null;

  return (
    <motion.div
      initial={{ opacity: 0, y: 50 }}
      animate={{ opacity: 1, y: 0 }}
      className="fixed bottom-4 right-4 z-50 bg-black border-2 border-terminal-green rounded-lg p-4 shadow-2xl"
    >
      <div className="mb-2 text-center">
        <div className="text-terminal-green font-terminal text-sm">💩 SHIT DINO GAME 💩</div>
        <div className="text-xs text-gray-400">Score: {gameState.score} | High: {gameState.highScore}</div>
      </div>

      <div 
        className="relative bg-gray-900 border border-gray-700 rounded cursor-pointer"
        style={{ width: '400px', height: '200px' }}
        onClick={gameState.isGameOver ? startGame : jump}
      >
        {/* Ground */}
        <div className="absolute bottom-0 w-full h-1 bg-terminal-green"></div>

        {/* Player */}
        <div
          className="absolute text-3xl transition-all duration-75"
          style={{
            left: `${PLAYER_X}px`,
            bottom: `${5 - gameState.playerY}px`,
            transform: gameState.playerVelocity < 0 ? 'rotate(-10deg)' : 'rotate(0deg)',
          }}
        >
          💩
        </div>

        {/* Obstacles */}
        {gameState.obstacles.map(obstacle => (
          <div
            key={obstacle.id}
            className="absolute text-3xl"
            style={{
              left: `${obstacle.x}px`,
              bottom: '5px',
            }}
          >
            {obstacle.emoji}
          </div>
        ))}

        {/* Game Over Screen */}
        {gameState.isGameOver && (
          <div className="absolute inset-0 bg-black/80 flex flex-col items-center justify-center text-terminal-green font-terminal">
            <div className="text-xl mb-2">💩 GAME OVER! 💩</div>
            <div className="text-sm mb-4">Score: {gameState.score}</div>
            <div className="text-xs text-gray-400">Press SPACE or click to restart</div>
          </div>
        )}

        {/* Start Screen */}
        {!gameState.isPlaying && !gameState.isGameOver && (
          <div className="absolute inset-0 bg-black/80 flex flex-col items-center justify-center text-terminal-green font-terminal">
            <div className="text-lg mb-2">💩 SHIT DINO 💩</div>
            <div className="text-xs text-gray-400 text-center">
              Press SPACE or click to start<br/>
              Jump over toilet obstacles!
            </div>
          </div>
        )}
      </div>

      <div className="text-xs text-gray-500 text-center mt-2">
        Use SPACE or click to jump
      </div>
    </motion.div>
  );
};