# Shit 💩

A realistic 3D visualization of shit using React, Three.js, and TypeScript. This project demonstrates advanced 3D graphics techniques including custom geometry generation, shader materials, and interactive controls.

![3D Visualization](https://img.shields.io/badge/3D-Three.js-black)
![TypeScript](https://img.shields.io/badge/TypeScript-5.8-blue)
![React](https://img.shields.io/badge/React-19.1-61DAFB)
![Test Coverage](https://img.shields.io/badge/coverage-100%25-brightgreen)

## 🚀 Features

- **Realistic 3D Model**: Anatomically accurate coiled pile shape with 2.5 spiral rotations
- **Custom Geometry**: Procedurally generated tube geometry with varying radius
- **Advanced Materials**: MeshStandardMaterial with bump mapping and vertex colors
- **Natural Variations**: Organic irregularities, bulges, and color variations
- **Interactive Controls**: Orbit controls for rotating and zooming
- **Optimized Performance**: Efficient geometry generation and rendering
- **Full Test Coverage**: 100% test coverage with Vitest

## 🛠️ Technical Stack

- **React 19.1** - UI library
- **Three.js 0.177** - 3D graphics library
- **React Three Fiber** - React renderer for Three.js
- **TypeScript 5.8** - Type safety
- **Vite 6.3** - Build tool and dev server
- **Vitest** - Testing framework
- **ESLint & Prettier** - Code quality and formatting
- **Husky** - Git hooks for code quality

## 📦 Installation

1. Clone the repository:

```bash
git clone https://github.com/champi-dev/shit.git
cd shit
```

2. Install dependencies:

```bash
npm install
```

3. Start the development server:

```bash
npm run dev
```

The application will be available at `http://localhost:3000`

## 🧪 Testing

Run tests:

```bash
npm test
```

Run tests with coverage:

```bash
npm run test:coverage
```

The project maintains 100% test coverage across all components and utilities.

## 🚀 Deployment

### Vercel (Recommended)

1. Install Vercel CLI:

```bash
npm i -g vercel
```

2. Deploy:

```bash
vercel
```

3. Follow the prompts to link your project

### Manual Deployment

1. Build the project:

```bash
npm run build
```

2. The production-ready files will be in the `dist` directory

3. Deploy the `dist` directory to any static hosting service

### Environment Variables

No environment variables are required for this project.

## 📁 Project Structure

```
shit/
├── src/
│   ├── components/
│   │   ├── DogWasteVisualization.tsx  # Main 3D component
│   │   └── DogWasteVisualization.test.tsx
│   ├── test/
│   │   └── setup.ts                   # Test configuration
│   ├── App.tsx                        # Root component
│   ├── App.test.tsx
│   ├── main.tsx                       # Entry point
│   └── index.css                      # Global styles
├── public/                            # Static assets
├── .husky/                            # Git hooks
├── vitest.config.ts                   # Test configuration
├── vite.config.ts                     # Vite configuration
├── tsconfig.json                      # TypeScript configuration
└── package.json
```

## 🎨 Technical Details

### Geometry Generation

The visualization uses a custom tube geometry with:

- **Spiral Path**: 2.5 rotations with tapering radius
- **Variable Radius**: 0.45 units at base to 0.15 units at tip
- **Natural Irregularities**: Sine wave modulations for organic appearance
- **Smooth Connections**: Proper start and end handling

### Material System

- **Primary Color**: #5D4037 (dark brown)
- **Color Variations**: #3E2723, #4E342E, #6D4C41
- **Surface Properties**:
  - Roughness: 0.7
  - Metalness: 0.1
  - Bump mapping for texture

### Performance Optimizations

- Geometry cached with `useMemo`
- Efficient vertex generation
- Optimized shadow mapping
- Proper disposal of Three.js resources

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Code Style

- ESLint and Prettier are configured
- Pre-commit hooks ensure code quality
- All code must pass linting and have tests

## 📄 License

This project is licensed under the ISC License.

## 🙏 Acknowledgments

- Three.js community for excellent documentation
- React Three Fiber for the amazing React integration
- Vite team for the blazing fast build tool
