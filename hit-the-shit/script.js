// Game variables
const canvas = document.getElementById('gameCanvas');
const ctx = canvas.getContext('2d');
const startBtn = document.getElementById('startBtn');
const restartBtn = document.getElementById('restartBtn');
const gameInfo = document.getElementById('gameInfo');
const gameOver = document.getElementById('gameOver');
const scoreElement = document.getElementById('score');
const highScoreElement = document.getElementById('highScore');
const finalScoreElement = document.getElementById('finalScore');

// Game state
let gameState = 'menu'; // 'menu', 'playing', 'gameOver'
let score = 0;
let highScore = localStorage.getItem('hitTheShitHighScore') || 0;
let gameSpeed = 4;
let frameCount = 0;

// Visitor counter - Global tracking with CountAPI
let totalVisitors = 0;
let hasVisitedThisSession = sessionStorage.getItem('hitTheShitVisited') === 'true';

// CountAPI configuration
const COUNTAPI_URL = 'https://api.countapi.xyz';
const COUNTER_NAMESPACE = 'hit-the-shit-game';
const COUNTER_KEY = 'total-visitors';

// Load global visitor count from CountAPI
async function loadVisitorCount() {
    try {
        // First, try to get current count
        const getResponse = await fetch(`${COUNTAPI_URL}/get/${COUNTER_NAMESPACE}/${COUNTER_KEY}`);

        if (getResponse.ok) {
            const getData = await getResponse.json();
            totalVisitors = getData.value || 0;
            console.log(`Current global visitors: ${totalVisitors}`);
        } else {
            // Counter doesn't exist yet, create it
            console.log('Creating new global counter...');
            const createResponse = await fetch(`${COUNTAPI_URL}/create?namespace=${COUNTER_NAMESPACE}&key=${COUNTER_KEY}&value=0`);
            if (createResponse.ok) {
                const createData = await createResponse.json();
                totalVisitors = createData.value || 0;
                console.log(`Created new counter with value: ${totalVisitors}`);
            }
        }

        // Increment if new session
        if (!hasVisitedThisSession) {
            await incrementVisitorCount();
            sessionStorage.setItem('hitTheShitVisited', 'true');
        }

    } catch (error) {
        console.log('CountAPI unavailable, using fallback:', error);
        // Fallback to localStorage
        totalVisitors = parseInt(localStorage.getItem('hitTheShitFallbackCount')) || 42;
        if (!hasVisitedThisSession) {
            totalVisitors++;
            localStorage.setItem('hitTheShitFallbackCount', totalVisitors);
            sessionStorage.setItem('hitTheShitVisited', 'true');
        }
    }

    updateVisitorDisplay();
}

// Increment global visitor count
async function incrementVisitorCount() {
    try {
        const response = await fetch(`${COUNTAPI_URL}/hit/${COUNTER_NAMESPACE}/${COUNTER_KEY}`);
        if (response.ok) {
            const data = await response.json();
            totalVisitors = data.value;
            console.log(`🎉 New visitor! Global count: ${totalVisitors}`);
            updateVisitorDisplay();

            // Add celebration effect for new visitors
            celebrateNewVisitor();
        }
    } catch (error) {
        console.log('Could not increment global counter:', error);
        // Fallback increment
        totalVisitors++;
        localStorage.setItem('hitTheShitFallbackCount', totalVisitors);
    }
}

// Celebration effect for new visitors
function celebrateNewVisitor() {
    const visitorElement = document.getElementById('visitorCount');
    visitorElement.style.color = '#00FF00';
    visitorElement.style.transform = 'scale(1.3)';

    setTimeout(() => {
        visitorElement.style.color = '#FFD700';
        visitorElement.style.transform = 'scale(1)';
    }, 1000);
}

// Player object
const player = {
    x: 100,
    y: 260, // Position on the green ground (300 - 40 = 260)
    width: 40,
    height: 40,
    velocityY: 0,
    jumpPower: -15,
    gravity: 0.8,
    grounded: true,
    groundY: 260, // Ground position
    color: '#ff6b6b'
};

// Game objects arrays
let shitObjects = [];
let trees = [];
let nextShitSpawn = 0;
let nextTreeSpawn = 0;

// Difficulty levels
const difficultyLevels = [
    { name: "EASY", minScore: 0, speed: 4, spawnRate: 120, color: '#4CAF50' },
    { name: "MEDIUM", minScore: 50, speed: 5.5, spawnRate: 90, color: '#FF9800' },
    { name: "HARD", minScore: 150, speed: 7, spawnRate: 70, color: '#F44336' },
    { name: "IMPOSSIBLE", minScore: 300, speed: 9, spawnRate: 50, color: '#9C27B0' },
    { name: "INSANE", minScore: 500, speed: 12, spawnRate: 40, color: '#000000' }
];

let currentDifficulty = difficultyLevels[0];

// Initialize displays
highScoreElement.textContent = highScore;
loadVisitorCount(); // Load global visitor count

function updateVisitorDisplay() {
    const visitorElement = document.getElementById('visitorCount');
    visitorElement.textContent = totalVisitors.toLocaleString(); // Add comma separators for large numbers

    // Add a little animation when the count updates
    visitorElement.style.transform = 'scale(1.1)';
    setTimeout(() => {
        visitorElement.style.transform = 'scale(1)';
    }, 200);

    // Update page title with visitor count for extra engagement
    document.title = `💩 Hit the Shit - ${totalVisitors.toLocaleString()} players! 💩`;
}

// Game functions
function resetGame() {
    score = 0;
    gameSpeed = 4;
    frameCount = 0;
    shitObjects = [];
    trees = [];
    nextShitSpawn = 0;
    nextTreeSpawn = 100;
    currentDifficulty = difficultyLevels[0];
    player.y = player.groundY;
    player.velocityY = 0;
    player.grounded = true;
    scoreElement.textContent = score;
}

function updateDifficulty() {
    for (let i = difficultyLevels.length - 1; i >= 0; i--) {
        if (score >= difficultyLevels[i].minScore) {
            currentDifficulty = difficultyLevels[i];
            gameSpeed = currentDifficulty.speed;
            break;
        }
    }
}

function spawnShit() {
    if (frameCount > nextShitSpawn) {
        shitObjects.push({
            x: canvas.width,
            y: 230, // Fixed height - player must jump to hit
            width: 30,
            height: 30,
            hit: false
        });
        nextShitSpawn = frameCount + Math.random() * currentDifficulty.spawnRate + 40;
    }
}

function spawnTree() {
    if (frameCount > nextTreeSpawn) {
        trees.push({
            x: canvas.width,
            y: 220, // Tree base on ground
            width: 20,
            height: 80,
            trunkHeight: 50,
            crownHeight: 30
        });
        nextTreeSpawn = frameCount + Math.random() * 200 + 150; // Trees spawn less frequently
    }
}

function updatePlayer() {
    // Apply gravity
    if (!player.grounded) {
        player.velocityY += player.gravity;
    }

    // Update position
    player.y += player.velocityY;

    // Ground collision
    if (player.y >= player.groundY) {
        player.y = player.groundY;
        player.velocityY = 0;
        player.grounded = true;
    }

    // Ceiling collision
    if (player.y <= 50) {
        player.y = 50;
        player.velocityY = 0;
    }
}

function updateShit() {
    for (let i = shitObjects.length - 1; i >= 0; i--) {
        const shit = shitObjects[i];
        shit.x -= gameSpeed;

        // Remove off-screen shit
        if (shit.x + shit.width < 0) {
            if (!shit.hit) {
                // Player missed the shit - game over!
                gameState = 'gameOver';
                finalScoreElement.textContent = score;
                gameOver.style.display = 'block';

                // Update high score
                if (score > highScore) {
                    highScore = score;
                    localStorage.setItem('hitTheShitHighScore', highScore);
                    highScoreElement.textContent = highScore;
                }
            }
            shitObjects.splice(i, 1);
        }
    }
}

function updateTrees() {
    for (let i = trees.length - 1; i >= 0; i--) {
        const tree = trees[i];
        tree.x -= gameSpeed;

        // Remove off-screen trees
        if (tree.x + tree.width < 0) {
            trees.splice(i, 1);
        }
    }
}

function checkCollisions() {
    for (let shit of shitObjects) {
        if (!shit.hit &&
            player.x < shit.x + shit.width &&
            player.x + player.width > shit.x &&
            player.y < shit.y + shit.height &&
            player.y + player.height > shit.y) {

            // Hit the shit!
            shit.hit = true;
            score += 10;
            scoreElement.textContent = score;

            // Update difficulty based on score
            updateDifficulty();
        }
    }
}

function drawPlayer() {
    ctx.fillStyle = player.color;
    ctx.fillRect(player.x, player.y, player.width, player.height);

    // Draw simple face
    ctx.fillStyle = 'white';
    ctx.fillRect(player.x + 8, player.y + 8, 6, 6); // Left eye
    ctx.fillRect(player.x + 26, player.y + 8, 6, 6); // Right eye

    ctx.fillStyle = 'black';
    ctx.fillRect(player.x + 10, player.y + 10, 2, 2); // Left pupil
    ctx.fillRect(player.x + 28, player.y + 10, 2, 2); // Right pupil

    // Draw mouth
    ctx.fillRect(player.x + 15, player.y + 25, 10, 3);
}

function drawShit() {
    for (let shit of shitObjects) {
        if (shit.hit) {
            ctx.fillStyle = '#4CAF50'; // Green when hit
        } else {
            ctx.fillStyle = '#8B4513'; // Brown shit color
        }

        ctx.fillRect(shit.x, shit.y, shit.width, shit.height);

        // Draw shit emoji-like shape
        ctx.fillStyle = shit.hit ? '#2E7D32' : '#654321';
        ctx.fillRect(shit.x + 5, shit.y + 5, 20, 20);
        ctx.fillRect(shit.x + 10, shit.y - 5, 10, 10);
    }
}

function drawTrees() {
    for (let tree of trees) {
        // Draw trunk
        ctx.fillStyle = '#8B4513';
        ctx.fillRect(tree.x + 5, tree.y + tree.crownHeight, 10, tree.trunkHeight);

        // Draw crown (leaves)
        ctx.fillStyle = '#228B22';
        ctx.fillRect(tree.x, tree.y, tree.width, tree.crownHeight);

        // Add some texture to the crown
        ctx.fillStyle = '#32CD32';
        ctx.fillRect(tree.x + 2, tree.y + 2, tree.width - 4, tree.crownHeight - 4);
    }
}

function drawBackground() {
    // Clear canvas
    ctx.fillStyle = '#87CEEB';
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Draw ground
    ctx.fillStyle = '#90EE90';
    ctx.fillRect(0, 300, canvas.width, 100);

    // Draw clouds
    ctx.fillStyle = 'white';
    for (let i = 0; i < 3; i++) {
        const x = (frameCount * 0.5 + i * 200) % (canvas.width + 100) - 50;
        ctx.fillRect(x, 50 + i * 30, 60, 20);
        ctx.fillRect(x + 20, 40 + i * 30, 60, 20);
        ctx.fillRect(x + 40, 50 + i * 30, 60, 20);
    }
}

function drawUI() {
    // Draw difficulty indicator
    ctx.fillStyle = currentDifficulty.color;
    ctx.font = 'bold 20px Courier New';
    ctx.fillText(`DIFFICULTY: ${currentDifficulty.name}`, 10, 30);

    // Draw speed indicator
    ctx.fillStyle = 'white';
    ctx.font = '16px Courier New';
    ctx.fillText(`Speed: ${gameSpeed.toFixed(1)}x`, 10, 55);
}

function gameLoop() {
    if (gameState === 'playing') {
        frameCount++;

        // Update game objects
        updatePlayer();
        spawnShit();
        spawnTree();
        updateShit();
        updateTrees();
        checkCollisions();

        // Draw everything
        drawBackground();
        drawTrees();
        drawPlayer();
        drawShit();
        drawUI();
    }

    requestAnimationFrame(gameLoop);
}

// Event listeners
function jump() {
    if (gameState === 'playing' && player.grounded) {
        player.velocityY = player.jumpPower;
        player.grounded = false;
    }
}

// Keyboard controls
document.addEventListener('keydown', (e) => {
    if (e.code === 'Space') {
        e.preventDefault();
        jump();
    }
});

// Mouse/touch controls
canvas.addEventListener('click', jump);
canvas.addEventListener('touchstart', (e) => {
    e.preventDefault();
    jump();
});

// Button event listeners
startBtn.addEventListener('click', () => {
    gameState = 'playing';
    gameInfo.style.display = 'none';
    resetGame();
});

restartBtn.addEventListener('click', () => {
    gameState = 'playing';
    gameOver.style.display = 'none';
    resetGame();
});

// Start the game loop
gameLoop();
