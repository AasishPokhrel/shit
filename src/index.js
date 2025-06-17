const { app, BrowserWindow, Menu, shell } = require('electron');
const path = require('path');
const { exit } = require('process');

const iconPath = path.join(__dirname, 'icons', 
	process.platform === 'win32' ? 'icon-windows.ico' :
	process.platform === 'darwin' ? 'icon-macos.icns' :
	'icon-linux.png'
);

const iconAboutPath = path.join(__dirname, 'icons', 
	process.platform === 'win32' ? 'about-windows.ico' :
	process.platform === 'darwin' ? 'about-macos.icns' :
	'about-linux.png'
);

function createWindow() {
	const win = new BrowserWindow({
		width: 800,
		height: 600,
        title: 'SHIT VIEWER',
		icon: iconPath,
		webPreferences: {
			nodeIntegration: true,
			contextIsolation: false,
            sandbox: false,
            devTools: true,
		}
	});

	win.webContents.zoomFactor = 0
	win.webContents.zoomLevel = 0
	win.loadFile(path.join(__dirname, 'viewer/readmeviewer.html'));

	const fileSubmenu = [
		{ label: 'June 11 2025 - 7:50', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june11-2025-7-50.md'), 'June 11 2025 - 7:50') },
		{ label: 'June 13 2025 - 19:43', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june13-2025-19-43.md',), 'June 13 2025 - 19:43') },
		{ label: 'June 13 2025 - 20:35', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june13-2025-20-35.md',), 'June 13 2025 - 20:35') },
		{ label: 'June 13 2025 - 21:10', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june13-2025-21-10.md',), 'June 13 2025 - 21:10') },
		{ label: 'June 14 2025 - 10:33', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june14-2025-10-33.md',), 'June 14 2025 - 10:33') },
		{ label: 'June 14 2025 - 5:44', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june14-2025-5-44.md'),  'June 14 2025 - 5:44') },
		{ label: 'June 15 2025 - 15:27', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-15-27.md'), 'June 15 2025 - 15:27') },
		{ label: 'June 15 2025 - 15:28', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-15-28.md'), 'June 15 2025 - 15:28') },
		{ label: 'June 15 2025 - 15:30', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-15-30.md'), 'June 15 2025 - 15:30') },
		{ label: 'June 15 2025 - 15:41', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-15-41.md'), 'June 15 2025 - 15:41') },
		{ label: 'June 15 2025 - 18:02', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-18-02.md'), 'June 15 2025 - 18:02') },
		{ label: 'June 15 2025 - 18:05', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-18-05.md'), 'June 15 2025 - 18:05') },
		{ label: 'June 15 2025 - 18:05 (02)', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-18-05-02.md'), 'June 15 2025 - 18:05 (02)') },
		{ label: 'June 15 2025 - 18:05 (03)', click: () => win.webContents.send('load-md', path.join(__dirname, 'readmes/june15-2025-18-05-03.md'), 'June 15 2025 - 18:05 (03)') }
	];

	const menu = Menu.buildFromTemplate([
		{ label: 'file 📄', submenu: [
			{
				label: "quit ❌",
				click: (menuItem, browserWindow) => {
					exit()
				}
			},
		]},
		{ label: 'historay ⌚', submenu: fileSubmenu },
		{ label: 'view 🔍', submenu: [
			{ 
				label: 'make it poo colored 💩', 
				click: (menuItem, browserWindow) => {
					if (browserWindow) win.webContents.send("poo")
				}
			},
			{ type: "separator" },
			{ 
				label: 'zoom in 🔎', 
				click: (menuItem, browserWindow) => {
					if (browserWindow) win.webContents.zoomFactor += 0.1
				}
			},
			{
				label: 'zoom out 🔍', 
				click: (menuItem, browserWindow) => {
					if (browserWindow) win.webContents.zoomFactor -= 0.1
				}
			}
		]},
		{ label: 'tool 🔧', submenu: [
			{ 
				label: 'dev tool 🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧🔧', 
				click: (menuItem, browserWindow) => {
					if (browserWindow) win.webContents.openDevTools();
				}
			},
			{
				label: "reload 🔃",
				click: (menuItem, browserWindow) => {
					if (browserWindow) win.webContents.reload();
				}
			}
		]},
		{ label: 'help ❓', submenu: [
			{ 
				label: 'about this app ℹ️', 
				click: (menuItem, browserWindow) => {
					if (browserWindow) createAboutWindow();
				}
			},
			{ type: 'separator' },
			{ 
				label: 'shit repo 💩', 
				click: (menuItem, browserWindow) => {
					if (browserWindow) shell.openExternal("https://github.com/AasishPokhrel/shit")
				}
			}
		]},
	]);

	Menu.setApplicationMenu(menu);
}

function createAboutWindow() {
	const win = new BrowserWindow({
		width: 640,
		height: 280,
        title: 'about this app',
		icon: iconAboutPath,
		autoHideMenuBar: true,
		webPreferences: {
			nodeIntegration: true,
			contextIsolation: false,
            sandbox: false,
            devTools: true,
		}
	});

	win.loadFile(path.join(__dirname, 'about.html'));
}

app.whenReady().then(createWindow);
