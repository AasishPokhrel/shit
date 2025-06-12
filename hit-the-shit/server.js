const http = require('http');
const fs = require('fs');
const path = require('path');
const url = require('url');

const PORT = 8000;
const REVIEWS_FILE = path.join(__dirname, 'reviews.json');

// MIME types for different file extensions
const mimeTypes = {
    '.html': 'text/html',
    '.js': 'text/javascript',
    '.css': 'text/css',
    '.json': 'application/json',
    '.png': 'image/png',
    '.jpg': 'image/jpeg',
    '.gif': 'image/gif',
    '.ico': 'image/x-icon'
};

// File locking mechanism
let isWriting = false;
const writeQueue = [];

// Process write queue
async function processWriteQueue() {
    if (isWriting || writeQueue.length === 0) return;
    
    isWriting = true;
    const { data, callback } = writeQueue.shift();
    
    try {
        await fs.promises.writeFile(REVIEWS_FILE, JSON.stringify(data, null, 2));
        callback(null);
        console.log('✅ Reviews saved to file successfully');
    } catch (error) {
        callback(error);
        console.error('❌ Error saving reviews:', error);
    } finally {
        isWriting = false;
        // Process next item in queue
        setTimeout(processWriteQueue, 100);
    }
}

// Save reviews with file locking
function saveReviews(data, callback) {
    writeQueue.push({ data, callback });
    processWriteQueue();
}

// Load reviews from file
function loadReviews() {
    try {
        if (fs.existsSync(REVIEWS_FILE)) {
            const data = fs.readFileSync(REVIEWS_FILE, 'utf8');
            return JSON.parse(data);
        }
    } catch (error) {
        console.error('Error loading reviews:', error);
    }
    
    // Return default structure if file doesn't exist or error
    return {
        reviews: [],
        lastUpdated: new Date().toISOString(),
        version: Date.now()
    };
}

// Create HTTP server
const server = http.createServer((req, res) => {
    const parsedUrl = url.parse(req.url, true);
    const pathname = parsedUrl.pathname;
    
    // Enable CORS
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
    
    // Handle OPTIONS request (CORS preflight)
    if (req.method === 'OPTIONS') {
        res.writeHead(200);
        res.end();
        return;
    }
    
    // API endpoint to save reviews
    if (pathname === '/api/save-reviews' && req.method === 'POST') {
        let body = '';
        
        req.on('data', chunk => {
            body += chunk.toString();
        });
        
        req.on('end', () => {
            try {
                const newData = JSON.parse(body);
                
                saveReviews(newData, (error) => {
                    if (error) {
                        res.writeHead(500, { 'Content-Type': 'application/json' });
                        res.end(JSON.stringify({ error: 'Failed to save reviews' }));
                    } else {
                        res.writeHead(200, { 'Content-Type': 'application/json' });
                        res.end(JSON.stringify({ success: true, message: 'Reviews saved successfully' }));
                    }
                });
                
            } catch (error) {
                res.writeHead(400, { 'Content-Type': 'application/json' });
                res.end(JSON.stringify({ error: 'Invalid JSON data' }));
            }
        });
        
        return;
    }
    
    // Serve static files
    let filePath = path.join(__dirname, pathname === '/' ? 'index.html' : pathname);
    
    // Security check - prevent directory traversal
    if (!filePath.startsWith(__dirname)) {
        res.writeHead(403);
        res.end('Forbidden');
        return;
    }
    
    // Get file extension
    const ext = path.extname(filePath).toLowerCase();
    const contentType = mimeTypes[ext] || 'application/octet-stream';
    
    // Check if file exists
    fs.access(filePath, fs.constants.F_OK, (err) => {
        if (err) {
            res.writeHead(404, { 'Content-Type': 'text/html' });
            res.end('<h1>404 - File Not Found</h1>');
            return;
        }
        
        // Read and serve the file
        fs.readFile(filePath, (err, data) => {
            if (err) {
                res.writeHead(500, { 'Content-Type': 'text/html' });
                res.end('<h1>500 - Internal Server Error</h1>');
                return;
            }
            
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(data);
        });
    });
});

// Start server
server.listen(PORT, () => {
    console.log(`🚀 Hit the Shit server running at http://localhost:${PORT}`);
    console.log(`📁 Serving files from: ${__dirname}`);
    console.log(`💾 Reviews file: ${REVIEWS_FILE}`);
    console.log(`🔒 File locking enabled for concurrent writes`);
});

// Handle server shutdown gracefully
process.on('SIGINT', () => {
    console.log('\n🛑 Shutting down server...');
    server.close(() => {
        console.log('✅ Server closed');
        process.exit(0);
    });
});
