// SUPER AWESOME JAVASCRIPT CODE!!!
// MADE BY A GENIUS CODER!!!

// Global variables are awesome!!!
var counter = 0;
var popupInterval;
var audioElements = [];
var colors = ['red', 'blue', 'green', 'yellow', 'purple', 'orange', 'pink'];

// Wait for page to load
window.onload = function() {
    // Create annoying popup
    setTimeout(function() {
        alert('WELCOME TO MY AMAZING WEBSITE!!!!');
        setTimeout(function() {
            alert('PLEASE SUBSCRIBE TO MY NEWSLETTER!!!');
            setTimeout(function() {
                alert('DON\'T FORGET TO LIKE AND SHARE!!!');
                startRandomPopups();
            }, 2000);
        }, 1000);
    }, 500);

    // Add event listener to all links
    var links = document.getElementsByTagName('a');
    for(var i = 0; i < links.length; i++) {
        links[i].addEventListener('click', function(e) {
            e.preventDefault();
            alert('THANKS FOR CLICKING!!! BUT THIS LINK GOES NOWHERE LOL!!!');
            makeEverythingDance();
        });
    }

    // Create cursor follower
    var cursorFollower = document.createElement('div');
    cursorFollower.innerHTML = '💩';
    cursorFollower.style.cssText = 'position: fixed; font-size: 30px; pointer-events: none; z-index: 9999;';
    document.body.appendChild(cursorFollower);

    document.addEventListener('mousemove', function(e) {
        cursorFollower.style.left = (e.clientX + 10) + 'px';
        cursorFollower.style.top = (e.clientY + 10) + 'px';
    });

    // Add random audio elements
    createAudioElement('https://www.myinstants.com/media/sounds/airhorn_1.mp3');
    createAudioElement('https://www.myinstants.com/media/sounds/epic-sax-guy.mp3');

    // Play random sounds
    setInterval(function() {
        var randomIndex = Math.floor(Math.random() * audioElements.length);
        audioElements[randomIndex].play();
    }, 10000);

    // Make the page title change
    setInterval(changeTitleText, 1000);

    // Create a visitor counter
    createVisitorCounter();

    // Make everything rainbow colored
    setInterval(makeEverythingRainbow, 500);
};

function startRandomPopups() {
    popupInterval = setInterval(function() {
        var randomMessage = [
            'HEY THERE!!!',
            'DON\'T LEAVE YET!!!',
            'HAVE YOU SUBSCRIBED???',
            'CLICK HERE FOR FREE STUFF!!!',
            'YOU ARE VISITOR #' + Math.floor(Math.random() * 1000000)
        ];
        var randomIndex = Math.floor(Math.random() * randomMessage.length);
        
        var popup = document.createElement('div');
        popup.textContent = randomMessage[randomIndex];
        popup.style.cssText = 'position: fixed; background-color: ' + getRandomColor() + '; color: ' + getRandomColor() + '; padding: 20px; border: 5px solid ' + getRandomColor() + '; border-radius: 10px; font-size: 24px; font-weight: bold; z-index: 9999; cursor: pointer;';
        popup.style.left = Math.random() * (window.innerWidth - 200) + 'px';
        popup.style.top = Math.random() * (window.innerHeight - 100) + 'px';
        
        popup.onclick = function() {
            document.body.removeChild(this);
            alert('THANKS FOR CLICKING!!! YOU WIN NOTHING!!!');
        };
        
        document.body.appendChild(popup);
        
        setTimeout(function() {
            if(document.body.contains(popup)) {
                document.body.removeChild(popup);
            }
        }, 3000);
    }, 5000);
}

function createAudioElement(src) {
    var audio = document.createElement('audio');
    audio.src = src;
    audio.preload = 'auto';
    document.body.appendChild(audio);
    audioElements.push(audio);
}

function getRandomColor() {
    return colors[Math.floor(Math.random() * colors.length)];
}

function changeTitleText() {
    var titles = [
        '💩 BEST SITE EVER 💩',
        '💩 YOU SHOULD SUBSCRIBE 💩',
        '💩 OMG AMAZING CONTENT 💩',
        '💩 DON\'T CLOSE THIS TAB 💩',
        '💩 CLICK HERE NOW 💩'
    ];
    document.title = titles[Math.floor(Math.random() * titles.length)];
}

function createVisitorCounter() {
    var counter = document.createElement('div');
    counter.style.cssText = 'position: fixed; bottom: 10px; left: 10px; background-color: red; color: yellow; padding: 10px; border: 3px solid black; font-size: 18px; font-weight: bold;';
    
    setInterval(function() {
        counter.textContent = 'Visitors: ' + (Math.floor(Math.random() * 100) + 9000);
    }, 2000);
    
    document.body.appendChild(counter);
}

function makeEverythingDance() {
    var elements = document.getElementsByTagName('*');
    for(var i = 0; i < elements.length; i++) {
        elements[i].style.animation = 'shake 0.5s infinite';
    }
    
    setTimeout(function() {
        for(var i = 0; i < elements.length; i++) {
            elements[i].style.animation = '';
        }
    }, 5000);
}

function makeEverythingRainbow() {
    var elements = document.getElementsByTagName('*');
    for(var i = 0; i < elements.length; i++) {
        if(Math.random() > 0.9) {
            elements[i].style.backgroundColor = getRandomColor();
            elements[i].style.color = getRandomColor();
        }
    }
}

// Console log spam
setInterval(function() {
    console.log('💩💩💩 HELLO FROM THE CONSOLE 💩💩💩');
    console.log('💩💩💩 THIS SITE IS AWESOME 💩💩💩');
    console.log('💩💩💩 MADE WITH LOVE BY ME 💩💩💩');
}, 1000);

// Create a memory leak for extra shittiness
setInterval(function() {
    var leakyArray = [];
    for(var i = 0; i < 1000; i++) {
        leakyArray.push('leak ' + i);
    }
    window['leak_' + counter++] = leakyArray;
}, 5000);
