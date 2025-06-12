// Reviews system with file locking simulation
let reviews = [];
let isSubmitting = false;

// DOM elements
const reviewForm = document.getElementById('reviewForm');
const reviewsList = document.getElementById('reviewsList');
const messageDiv = document.getElementById('message');
const refreshBtn = document.getElementById('refreshBtn');

// Initialize reviews page
document.addEventListener('DOMContentLoaded', function () {
    loadReviews();
    setupForm();
    setupRefreshButton();
});

// Load reviews from both JSON file and localStorage, merge them
async function loadReviews() {
    let fileReviews = [];
    let localReviews = [];

    try {
        // Try to load from JSON file first
        const response = await fetch('reviews.json?t=' + Date.now());
        if (response.ok) {
            const data = await response.json();
            fileReviews = data.reviews || [];
            console.log('Loaded', fileReviews.length, 'reviews from file');
        }
    } catch (error) {
        console.log('Could not load from file:', error);
    }

    try {
        // Load from localStorage (new reviews)
        const stored = localStorage.getItem('hitTheShitReviews');
        if (stored) {
            const data = JSON.parse(stored);
            localReviews = data.reviews || [];
            console.log('Loaded', localReviews.length, 'reviews from localStorage');
        }
    } catch (error) {
        console.log('Could not load from localStorage:', error);
    }

    // Merge reviews: localStorage (new) + file (existing), remove duplicates
    const allReviews = [...localReviews, ...fileReviews];
    const uniqueReviews = [];
    const seenIds = new Set();

    for (const review of allReviews) {
        if (!seenIds.has(review.id)) {
            seenIds.add(review.id);
            uniqueReviews.push(review);
        }
    }

    reviews = uniqueReviews;

    // If no reviews found, use demo reviews
    if (reviews.length === 0) {
        reviews = getDemoReviews();
        console.log('Using demo reviews');
    }

    console.log('Total reviews loaded:', reviews.length);
    displayReviews();
}

// Demo reviews to show initially
function getDemoReviews() {
    return [
        {
            id: 1,
            name: "ShitMaster2024",
            review: "HOLY SHIT! This game is absolutely AMAZING! I can't stop hitting the shit! 💩🔥",
            date: "2024-12-19T10:30:00Z",
            timestamp: Date.now() - 3600000
        },
        {
            id: 2,
            name: "PoopCollectorPro",
            review: "Best shit game ever made! The sounds are hilarious and the music is funky as hell! 🎵💩",
            date: "2024-12-19T09:15:00Z",
            timestamp: Date.now() - 7200000
        },
        {
            id: 3,
            name: "StinkyLegend",
            review: "I've been playing for hours! This shit is addictive! More games like this please! 🚀",
            date: "2024-12-19T08:45:00Z",
            timestamp: Date.now() - 9000000
        }
    ];
}

// Display reviews in the UI
function displayReviews() {
    if (reviews.length === 0) {
        reviewsList.innerHTML = '<div class="no-reviews">No reviews yet! Be the first to share your shit experience! 💩</div>';
        return;
    }

    // Sort reviews by timestamp (newest first) - ensure proper sorting
    const sortedReviews = reviews.sort((a, b) => {
        const timestampA = a.timestamp || new Date(a.date).getTime();
        const timestampB = b.timestamp || new Date(b.date).getTime();
        return timestampB - timestampA; // Newest first
    });

    console.log('Displaying', sortedReviews.length, 'reviews, newest first');

    reviewsList.innerHTML = sortedReviews.map((review, index) => `
        <div class="review-item" ${index === 0 ? 'style="border-left: 4px solid #4CAF50; background: rgba(76, 175, 80, 0.1);"' : ''}>
            <div class="review-header">
                <span class="reviewer-name">${escapeHtml(review.name)}</span>
                <span class="review-date">${formatDate(review.date)} ${index === 0 ? '🆕' : ''}</span>
            </div>
            <div class="review-text">${escapeHtml(review.review)}</div>
        </div>
    `).join('');
}

// Setup form submission
function setupForm() {
    reviewForm.addEventListener('submit', async function (e) {
        e.preventDefault();

        if (isSubmitting) {
            showMessage('Please wait, submitting your epic review...', 'loading');
            return;
        }

        const formData = new FormData(reviewForm);
        const name = formData.get('name').trim();
        const reviewText = formData.get('review').trim();

        // Validation
        if (!name || !reviewText) {
            showMessage('Please fill in all fields, you shit! 💩', 'error');
            return;
        }

        if (name.length > 50) {
            showMessage('Name too long! Keep it under 50 characters!', 'error');
            return;
        }

        if (reviewText.length > 500) {
            showMessage('Review too long! Keep it under 500 characters!', 'error');
            return;
        }

        await submitReview(name, reviewText);
    });
}

// Submit new review with file locking simulation
async function submitReview(name, reviewText) {
    isSubmitting = true;
    showMessage('Submitting your epic shit review... 🚀', 'loading');

    try {
        // Simulate file locking by adding a random delay
        await new Promise(resolve => setTimeout(resolve, 1000 + Math.random() * 2000));

        const newReview = {
            id: Date.now() + Math.random(),
            name: name,
            review: reviewText,
            date: new Date().toISOString(),
            timestamp: Date.now()
        };

        // Add to local array
        reviews.unshift(newReview);

        // Simulate saving to file (in a real implementation, this would be a server endpoint)
        await saveReviewsToFile();

        // Update display
        displayReviews();

        // Clear form
        reviewForm.reset();

        showMessage('🎉 EPIC REVIEW SUBMITTED! Thanks for sharing your shit experience! 💩', 'success');

    } catch (error) {
        showMessage('Shit! Something went wrong. Try again! 💩', 'error');
        console.error('Error submitting review:', error);
    } finally {
        isSubmitting = false;
    }
}

// Save reviews to JSON file (requires backend server)
async function saveReviewsToFile() {
    try {
        const reviewsData = {
            reviews: reviews,
            lastUpdated: new Date().toISOString(),
            version: Date.now()
        };

        // Try to save to server endpoint (if available)
        try {
            const response = await fetch('/api/save-reviews', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify(reviewsData)
            });

            if (response.ok) {
                console.log('✅ Reviews saved to server successfully!');
                return;
            } else {
                throw new Error('Server save failed');
            }
        } catch (serverError) {
            console.log('⚠️ Server not available, using fallback methods');

            // Fallback 1: Try to download updated JSON file for manual replacement
            downloadUpdatedReviewsFile(reviewsData);

            // Fallback 2: Save to localStorage for temporary persistence
            localStorage.setItem('hitTheShitReviews', JSON.stringify(reviewsData));

            // Show instructions to user
            showFileUpdateInstructions();
        }

    } catch (error) {
        throw new Error('Failed to save reviews: ' + error.message);
    }
}

// Download updated reviews.json file for manual replacement
function downloadUpdatedReviewsFile(reviewsData) {
    try {
        const jsonString = JSON.stringify(reviewsData, null, 2);
        const blob = new Blob([jsonString], { type: 'application/json' });
        const url = URL.createObjectURL(blob);

        const a = document.createElement('a');
        a.href = url;
        a.download = 'reviews.json';
        document.body.appendChild(a);
        a.click();
        document.body.removeChild(a);
        URL.revokeObjectURL(url);

        console.log('📥 Updated reviews.json file downloaded');
    } catch (error) {
        console.log('Failed to download file:', error);
    }
}

// Show instructions for manual file update
function showFileUpdateInstructions() {
    const instructions = `
        <div class="file-update-instructions" style="background: rgba(255, 193, 7, 0.2); border: 2px solid #FFC107; padding: 15px; border-radius: 8px; margin: 10px 0;">
            <h4>📁 Manual File Update Required</h4>
            <p><strong>Your review was saved locally, but to make it visible to all users:</strong></p>
            <ol>
                <li>A new <code>reviews.json</code> file has been downloaded</li>
                <li>Replace the existing <code>reviews.json</code> file in your project</li>
                <li>Restart your server to see the new review</li>
            </ol>
            <p><em>For automatic saving, set up a backend server with the /api/save-reviews endpoint.</em></p>
        </div>
    `;

    messageDiv.innerHTML = instructions;

    // Auto-hide after 15 seconds
    setTimeout(() => {
        if (messageDiv.innerHTML.includes('Manual File Update Required')) {
            messageDiv.innerHTML = '';
        }
    }, 15000);
}

// Utility functions
function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

function formatDate(dateString) {
    const date = new Date(dateString);
    return date.toLocaleDateString() + ' ' + date.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' });
}

function showMessage(text, type) {
    messageDiv.innerHTML = `<div class="${type}">${text}</div>`;

    // Auto-hide success/error messages after 5 seconds
    if (type === 'success' || type === 'error') {
        setTimeout(() => {
            messageDiv.innerHTML = '';
        }, 5000);
    }
}

// Load reviews from localStorage if available (fallback)
function loadFromLocalStorage() {
    try {
        const stored = localStorage.getItem('hitTheShitReviews');
        if (stored) {
            const data = JSON.parse(stored);
            return data.reviews || [];
        }
    } catch (error) {
        console.log('Error loading from localStorage:', error);
    }
    return [];
}

// Auto-refresh reviews every 30 seconds to show new reviews from other users
setInterval(async () => {
    console.log('Auto-refreshing reviews...');
    await loadReviews();
}, 30000);
