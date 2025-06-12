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

// Audio context for sound effects and music
let audioContext = null;
let soundEnabled = true;
let musicEnabled = true;
let musicPlaying = false;
let musicNodes = [];
const soundToggleBtn = document.getElementById('soundToggle');
const musicToggleBtn = document.getElementById('musicToggle');

// Initialize audio context
function initAudio() {
    try {
        audioContext = new (window.AudioContext || window.webkitAudioContext)();
        console.log('🔊 Audio initialized successfully!');
    } catch (error) {
        console.log('Audio not supported:', error);
    }
}

// Toggle sound on/off
function toggleSound() {
    soundEnabled = !soundEnabled;
    const soundToggle = document.getElementById('soundToggle');
    if (soundEnabled) {
        soundToggle.textContent = '🔊 Sound ON';
        soundToggle.style.background = 'rgba(76, 175, 80, 0.3)';
    } else {
        soundToggle.textContent = '🔇 Sound OFF';
        soundToggle.style.background = 'rgba(244, 67, 54, 0.3)';
    }

    // Save preference
    localStorage.setItem('hitTheShitSoundEnabled', soundEnabled);
}

// Play collection sound effect - now with random funny variations!
function playCollectionSound() {
    if (!audioContext || !soundEnabled) return;

    try {
        // Random funny sound variations
        const soundTypes = ['plop', 'squish', 'fart', 'splash'];
        const randomSound = soundTypes[Math.floor(Math.random() * soundTypes.length)];

        switch (randomSound) {
            case 'plop':
                playPlopSound();
                break;
            case 'squish':
                playSquishSound();
                break;
            case 'fart':
                playFartSound();
                break;
            case 'splash':
                playSplashSound();
                break;
        }

    } catch (error) {
        console.log('Error playing sound:', error);
    }
}

function playPlopSound() {
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();

    oscillator.connect(gainNode);
    gainNode.connect(audioContext.destination);

    // Deep plop sound
    oscillator.frequency.setValueAtTime(150, audioContext.currentTime);
    oscillator.frequency.exponentialRampToValueAtTime(80, audioContext.currentTime + 0.2);

    gainNode.gain.setValueAtTime(0, audioContext.currentTime);
    gainNode.gain.linearRampToValueAtTime(0.4, audioContext.currentTime + 0.01);
    gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.2);

    oscillator.start(audioContext.currentTime);
    oscillator.stop(audioContext.currentTime + 0.2);
}

function playSquishSound() {
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();
    const filter = audioContext.createBiquadFilter();

    oscillator.connect(filter);
    filter.connect(gainNode);
    gainNode.connect(audioContext.destination);

    oscillator.type = 'sawtooth';
    filter.type = 'lowpass';
    filter.frequency.setValueAtTime(300, audioContext.currentTime);

    // Squishy sound
    oscillator.frequency.setValueAtTime(200, audioContext.currentTime);
    oscillator.frequency.linearRampToValueAtTime(100, audioContext.currentTime + 0.15);

    gainNode.gain.setValueAtTime(0, audioContext.currentTime);
    gainNode.gain.linearRampToValueAtTime(0.3, audioContext.currentTime + 0.01);
    gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.15);

    oscillator.start(audioContext.currentTime);
    oscillator.stop(audioContext.currentTime + 0.15);
}

function playFartSound() {
    // Create a hilarious fart sound!
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();

    oscillator.connect(gainNode);
    gainNode.connect(audioContext.destination);

    oscillator.type = 'sawtooth';

    // Classic fart frequency pattern
    oscillator.frequency.setValueAtTime(80, audioContext.currentTime);
    oscillator.frequency.linearRampToValueAtTime(120, audioContext.currentTime + 0.05);
    oscillator.frequency.linearRampToValueAtTime(60, audioContext.currentTime + 0.1);
    oscillator.frequency.linearRampToValueAtTime(100, audioContext.currentTime + 0.15);
    oscillator.frequency.linearRampToValueAtTime(40, audioContext.currentTime + 0.25);

    gainNode.gain.setValueAtTime(0, audioContext.currentTime);
    gainNode.gain.linearRampToValueAtTime(0.35, audioContext.currentTime + 0.01);
    gainNode.gain.linearRampToValueAtTime(0.2, audioContext.currentTime + 0.1);
    gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.25);

    oscillator.start(audioContext.currentTime);
    oscillator.stop(audioContext.currentTime + 0.25);
}

function playSplashSound() {
    const oscillator = audioContext.createOscillator();
    const gainNode = audioContext.createGain();
    const filter = audioContext.createBiquadFilter();

    oscillator.connect(filter);
    filter.connect(gainNode);
    gainNode.connect(audioContext.destination);

    oscillator.type = 'square';
    filter.type = 'highpass';
    filter.frequency.setValueAtTime(200, audioContext.currentTime);

    // Splash sound
    oscillator.frequency.setValueAtTime(400, audioContext.currentTime);
    oscillator.frequency.exponentialRampToValueAtTime(100, audioContext.currentTime + 0.3);

    gainNode.gain.setValueAtTime(0, audioContext.currentTime);
    gainNode.gain.linearRampToValueAtTime(0.25, audioContext.currentTime + 0.01);
    gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.3);

    oscillator.start(audioContext.currentTime);
    oscillator.stop(audioContext.currentTime + 0.3);
}

// Play game over sound
function playGameOverSound() {
    if (!audioContext || !soundEnabled) return;

    try {
        // Create a descending "fail" sound
        const oscillator = audioContext.createOscillator();
        const gainNode = audioContext.createGain();

        oscillator.connect(gainNode);
        gainNode.connect(audioContext.destination);

        // Descending tone for game over
        oscillator.frequency.setValueAtTime(400, audioContext.currentTime);
        oscillator.frequency.exponentialRampToValueAtTime(100, audioContext.currentTime + 0.5);

        // Volume envelope
        gainNode.gain.setValueAtTime(0, audioContext.currentTime);
        gainNode.gain.linearRampToValueAtTime(0.2, audioContext.currentTime + 0.01);
        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.5);

        oscillator.start(audioContext.currentTime);
        oscillator.stop(audioContext.currentTime + 0.5);

    } catch (error) {
        console.log('Error playing game over sound:', error);
    }
}

// Toggle music on/off
function toggleMusic() {
    musicEnabled = !musicEnabled;
    const musicToggle = document.getElementById('musicToggle');
    if (musicEnabled) {
        musicToggle.textContent = '🎵 Music ON';
        musicToggle.style.background = 'rgba(76, 175, 80, 0.3)';
        if (gameState === 'playing') {
            createShitSoundtrack();
        }
    } else {
        musicToggle.textContent = '🎶 Music OFF';
        musicToggle.style.background = 'rgba(244, 67, 54, 0.3)';
        stopMusic();
    }

    // Save preference
    localStorage.setItem('hitTheShitMusicEnabled', musicEnabled);
}

// Create shit-inspired funky soundtrack
function createShitSoundtrack() {
    if (!audioContext || !musicEnabled) return;

    try {
        // Stop any existing music
        stopMusic();

        // Create a funky, comedic shit-inspired beat
        const masterGain = audioContext.createGain();
        masterGain.connect(audioContext.destination);
        masterGain.gain.setValueAtTime(0.08, audioContext.currentTime); // Keep music volume low

        // Bass line - deep and funky like... well, you know 💩
        const bassOsc = audioContext.createOscillator();
        const bassGain = audioContext.createGain();
        bassOsc.connect(bassGain);
        bassGain.connect(masterGain);

        bassOsc.type = 'sawtooth';
        bassOsc.frequency.setValueAtTime(55, audioContext.currentTime); // Low A
        bassGain.gain.setValueAtTime(0.4, audioContext.currentTime);

        // Melody - silly and playful 🎵
        const melodyOsc = audioContext.createOscillator();
        const melodyGain = audioContext.createGain();
        melodyOsc.connect(melodyGain);
        melodyGain.connect(masterGain);

        melodyOsc.type = 'triangle';
        melodyGain.gain.setValueAtTime(0.2, audioContext.currentTime);

        // Create a funky bass pattern
        const bassPattern = [55, 55, 73, 55, 65, 55, 73, 82]; // A, A, D, A, C, A, D, E
        const melodyPattern = [220, 247, 262, 294, 262, 247, 220, 196]; // A4 to G4 scale
        let bassIndex = 0;
        let melodyIndex = 0;

        const playBassNote = () => {
            if (!musicPlaying) return;
            bassOsc.frequency.setValueAtTime(bassPattern[bassIndex], audioContext.currentTime);
            bassIndex = (bassIndex + 1) % bassPattern.length;
            setTimeout(playBassNote, 600); // Funky timing
        };

        const playMelodyNote = () => {
            if (!musicPlaying) return;
            melodyOsc.frequency.setValueAtTime(melodyPattern[melodyIndex], audioContext.currentTime);
            melodyIndex = (melodyIndex + 1) % melodyPattern.length;
            setTimeout(playMelodyNote, 1200); // Slower melody
        };

        // Start the oscillators
        bassOsc.start(audioContext.currentTime);
        melodyOsc.start(audioContext.currentTime);

        // Store references for cleanup
        musicNodes = [bassOsc, melodyOsc, masterGain];
        musicPlaying = true;

        // Start the musical patterns
        playBassNote();
        playMelodyNote();

        console.log('🎵 Shit-inspired soundtrack started! 💩');

    } catch (error) {
        console.log('Error creating soundtrack:', error);
    }
}

function stopMusic() {
    musicPlaying = false;
    musicNodes.forEach(node => {
        try {
            if (node.stop) node.stop();
            if (node.disconnect) node.disconnect();
        } catch (e) {
            // Node might already be stopped
        }
    });
    musicNodes = [];
}

// Initialize displays
highScoreElement.textContent = highScore;
loadVisitorCount(); // Load global visitor count

// Load sound and music preferences
soundEnabled = localStorage.getItem('hitTheShitSoundEnabled') !== 'false';
musicEnabled = localStorage.getItem('hitTheShitMusicEnabled') !== 'false';

if (soundToggleBtn) {
    if (soundEnabled) {
        soundToggleBtn.textContent = '🔊 Sound ON';
        soundToggleBtn.style.background = 'rgba(76, 175, 80, 0.3)';
    } else {
        soundToggleBtn.textContent = '🔇 Sound OFF';
        soundToggleBtn.style.background = 'rgba(244, 67, 54, 0.3)';
    }

    // Add click listener
    soundToggleBtn.addEventListener('click', toggleSound);
}

if (musicToggleBtn) {
    if (musicEnabled) {
        musicToggleBtn.textContent = '🎵 Music ON';
        musicToggleBtn.style.background = 'rgba(76, 175, 80, 0.3)';
    } else {
        musicToggleBtn.textContent = '🎶 Music OFF';
        musicToggleBtn.style.background = 'rgba(244, 67, 54, 0.3)';
    }

    // Add click listener
    musicToggleBtn.addEventListener('click', toggleMusic);
}

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

                // Add hilarious game over message
                addFunnyGameOverMessage();

                gameOver.style.display = 'block';

                // Stop music and play game over sound
                stopMusic();
                playGameOverSound();

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

            // Play collection sound effect
            playCollectionSound();

            // Show hilarious floating text
            showFloatingText(shit.x, shit.y);

            // Add screen shake for extra impact
            addScreenShake();

            // Update difficulty based on score
            updateDifficulty();

            // Celebration for milestone scores
            if (score % 100 === 0 && score > 0) {
                celebrateMilestone(score);
            }
        }
    }
}

// Array of hilarious messages
const funnyMessages = [
    "PLOP! 💩", "SQUISHY! 🤢", "STINKY! 🦨", "NASTY! 🤮",
    "GROSS! 🤧", "YUCKY! 😷", "SMELLY! 👃", "EWWW! 😵",
    "POOP! 💩", "FART! 💨", "STINK! 🦨", "YIKES! 😱",
    "OH SHIT! 😂", "HOLY CRAP! 🙏", "DAMN! 😈", "WTF! 🤯"
];

let floatingTexts = [];

function showFloatingText(x, y) {
    const message = funnyMessages[Math.floor(Math.random() * funnyMessages.length)];
    floatingTexts.push({
        text: message,
        x: x,
        y: y,
        life: 60, // frames
        opacity: 1
    });
}

function updateFloatingTexts() {
    for (let i = floatingTexts.length - 1; i >= 0; i--) {
        const text = floatingTexts[i];
        text.y -= 2; // Float upward
        text.life--;
        text.opacity = text.life / 60;

        if (text.life <= 0) {
            floatingTexts.splice(i, 1);
        }
    }
}

function drawFloatingTexts() {
    ctx.save();
    ctx.font = 'bold 16px Courier New';
    ctx.textAlign = 'center';

    for (let text of floatingTexts) {
        ctx.globalAlpha = text.opacity;
        ctx.fillStyle = '#FFD700';
        ctx.strokeStyle = '#000';
        ctx.lineWidth = 2;

        // Draw text with outline
        ctx.strokeText(text.text, text.x, text.y);
        ctx.fillText(text.text, text.x, text.y);
    }

    ctx.restore();
}

// Screen shake effect
let shakeIntensity = 0;
let shakeDecay = 0.9;

function addScreenShake() {
    shakeIntensity = 8;
}

function updateScreenShake() {
    if (shakeIntensity > 0.1) {
        shakeIntensity *= shakeDecay;
    } else {
        shakeIntensity = 0;
    }
}

// Hilarious game over messages
const gameOverMessages = [
    "💩 YOU MISSED THE SHIT! 💩",
    "🤦‍♂️ SHIT HAPPENS... BUT YOU MISSED IT! 🤦‍♂️",
    "💨 THAT SHIT FLEW RIGHT BY YOU! 💨",
    "🎯 AIM FOR THE SHIT NEXT TIME! 🎯",
    "😂 EVEN MY GRANDMA HITS MORE SHIT! 😂",
    "🦨 YOU STINK AT THIS GAME! 🦨",
    "💩 SHIT! TRY AGAIN! 💩",
    "🤮 THAT WAS SHITTY PERFORMANCE! 🤮",
    "🚽 FLUSH YOUR SKILLS AND TRY AGAIN! 🚽",
    "💩 NO SHIT, YOU SUCK! 💩"
];

function addFunnyGameOverMessage() {
    const gameOverTitle = document.querySelector('#gameOver h2');
    const randomMessage = gameOverMessages[Math.floor(Math.random() * gameOverMessages.length)];
    gameOverTitle.textContent = randomMessage;
}

// Celebration for milestone scores
function celebrateMilestone(score) {
    // Add multiple floating texts for celebration
    const celebrationMessages = [
        `🎉 ${score} SHITS! 🎉`,
        "💩 SHIT MASTER! 💩",
        "🔥 ON FIRE! 🔥",
        "⭐ LEGENDARY! ⭐",
        "🚀 EPIC SHIT! 🚀"
    ];

    // Show multiple celebration texts
    for (let i = 0; i < 3; i++) {
        setTimeout(() => {
            const message = celebrationMessages[Math.floor(Math.random() * celebrationMessages.length)];
            floatingTexts.push({
                text: message,
                x: canvas.width / 2 + (Math.random() - 0.5) * 200,
                y: canvas.height / 2 + (Math.random() - 0.5) * 100,
                life: 120, // Longer life for celebration
                opacity: 1
            });
        }, i * 200);
    }

    // Extra screen shake for celebration
    shakeIntensity = 15;

    // Play celebration sound
    if (audioContext && soundEnabled) {
        playCelebrationSound();
    }
}

function playCelebrationSound() {
    try {
        // Create a triumphant celebration sound
        const oscillator1 = audioContext.createOscillator();
        const oscillator2 = audioContext.createOscillator();
        const gainNode = audioContext.createGain();

        oscillator1.connect(gainNode);
        oscillator2.connect(gainNode);
        gainNode.connect(audioContext.destination);

        // Triumphant chord
        oscillator1.frequency.setValueAtTime(523, audioContext.currentTime); // C5
        oscillator2.frequency.setValueAtTime(659, audioContext.currentTime); // E5

        gainNode.gain.setValueAtTime(0, audioContext.currentTime);
        gainNode.gain.linearRampToValueAtTime(0.3, audioContext.currentTime + 0.1);
        gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 1);

        oscillator1.start(audioContext.currentTime);
        oscillator2.start(audioContext.currentTime);
        oscillator1.stop(audioContext.currentTime + 1);
        oscillator2.stop(audioContext.currentTime + 1);

    } catch (error) {
        console.log('Error playing celebration sound:', error);
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
        updateFloatingTexts();
        updateScreenShake();

        // Apply screen shake
        ctx.save();
        if (shakeIntensity > 0) {
            const shakeX = (Math.random() - 0.5) * shakeIntensity;
            const shakeY = (Math.random() - 0.5) * shakeIntensity;
            ctx.translate(shakeX, shakeY);
        }

        // Draw everything
        drawBackground();
        drawTrees();
        drawPlayer();
        drawShit();
        drawFloatingTexts();
        drawUI();

        ctx.restore();
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
    // Initialize audio on first user interaction
    if (!audioContext) {
        initAudio();
    }

    gameState = 'playing';
    gameInfo.style.display = 'none';
    resetGame();

    // Start music if enabled
    if (musicEnabled) {
        createShitSoundtrack();
    }
});

restartBtn.addEventListener('click', () => {
    gameState = 'playing';
    gameOver.style.display = 'none';
    resetGame();

    // Start music if enabled
    if (musicEnabled) {
        createShitSoundtrack();
    }
});

// Start the game loop
gameLoop();
