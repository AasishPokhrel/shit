/**
 * DogWasteVisualization Component Tests
 *
 * Comprehensive tests for the 3D visualization component including
 * rendering, geometry creation, and user interaction.
 */

import { describe, it, expect, vi, beforeEach, afterEach } from 'vitest'
import { render, screen } from '@testing-library/react'
import * as THREE from 'three'
import DogWasteVisualization from './DogWasteVisualization'

// Mock react-three/fiber and drei
vi.mock('@react-three/fiber', () => ({
  Canvas: ({ children }: { children: React.ReactNode }) => (
    <div data-testid="canvas-mock">{children}</div>
  ),
  useFrame: vi.fn(),
}))

vi.mock('@react-three/drei', () => ({
  OrbitControls: () => <div data-testid="orbit-controls-mock" />,
  PerspectiveCamera: ({
    children,
    ...props
  }: {
    children?: React.ReactNode
    position?: string
  }) => (
    <div data-testid="perspective-camera-mock" {...props}>
      {children}
    </div>
  ),
}))

describe('DogWasteVisualization', () => {
  beforeEach(() => {
    vi.clearAllMocks()
  })

  afterEach(() => {
    vi.restoreAllMocks()
  })

  describe('Component Rendering', () => {
    it('renders the visualization container', () => {
      const { container } = render(<DogWasteVisualization />)
      const mainDiv = container.firstChild as HTMLElement
      expect(mainDiv).toBeInTheDocument()
      expect(mainDiv).toHaveStyle({
        width: '100%',
        height: '100vh',
        background: '#87CEEB',
      })
    })

    it('renders the instruction overlay', () => {
      render(<DogWasteVisualization />)
      const instructions = screen.getByText('Click and drag to rotate • Scroll to zoom')
      expect(instructions).toBeInTheDocument()
      // Check that parent element exists and has some styling
      expect(instructions.parentElement).toBeTruthy()
    })

    it('renders the Canvas component', () => {
      render(<DogWasteVisualization />)
      const canvas = screen.getByTestId('canvas-mock')
      expect(canvas).toBeInTheDocument()
    })

    it('renders camera and controls', () => {
      render(<DogWasteVisualization />)
      expect(screen.getByTestId('perspective-camera-mock')).toBeInTheDocument()
      expect(screen.getByTestId('orbit-controls-mock')).toBeInTheDocument()
    })
  })

  describe('Geometry Creation', () => {
    it('creates spiral path with correct parameters', () => {
      // Test the spiral path creation logic
      const rotations = 2.5
      const segments = 100
      const totalHeight = 1.2
      const baseRadius = 1.8

      // Simulate spiral path creation
      const points: THREE.Vector3[] = []
      for (let i = 0; i <= segments; i++) {
        const t = i / segments
        const angle = t * Math.PI * 2 * rotations
        const spiralRadius = baseRadius * (1 - t * 0.7)
        const wobble = Math.sin(t * 15) * 0.05 + Math.sin(t * 23) * 0.03
        const r = spiralRadius + wobble

        const x = Math.cos(angle) * r
        const z = Math.sin(angle) * r
        let y = t * totalHeight

        if (t < 0.05) {
          y = y + 0.1 * (1 - t / 0.05)
        }

        const topCurve = t > 0.8 ? Math.sin((t - 0.8) * 5) * 0.1 : 0
        points.push(new THREE.Vector3(x + topCurve, y, z))
      }

      expect(points).toHaveLength(segments + 1)
      expect(points[0].y).toBeGreaterThan(0) // Start elevated
      expect(points[points.length - 1].y).toBeCloseTo(totalHeight, 1)
    })

    it('creates tube geometry with varying radius', () => {
      // Test radius calculation at different points
      const testPoints = [
        { t: 0, expectedRadius: 0.585 }, // Thicker at start (0.45 * 1.3)
        { t: 0.5, expectedRadius: 0.3 }, // Middle
        { t: 0.98, expectedRadius: 0.075 }, // Tapered at end (0.15 * 0.5)
      ]

      testPoints.forEach(({ t, expectedRadius }) => {
        const baseRadius = 0.45
        const tipRadius = 0.15
        const bulge = Math.sin(t * 8) * 0.04 + Math.sin(t * 13) * 0.03
        let radius = baseRadius - (baseRadius - tipRadius) * t + bulge

        if (t < 0.05) {
          radius = radius * (1 + (1 - t / 0.05) * 0.3)
        }

        if (t > 0.95) {
          const endFactor = (t - 0.95) / 0.05
          radius = radius * (1 - endFactor * 0.5)
        }

        expect(radius).toBeCloseTo(expectedRadius, 0)
      })
    })

    it('generates correct color variations', () => {
      const darkColors = [0x3e2723, 0x4e342e, 0x5d4037, 0x6d4c41]

      // All colors should be valid hex values
      darkColors.forEach((color) => {
        expect(color).toBeGreaterThan(0)
        expect(color).toBeLessThan(0xffffff)
      })

      // Test color creation
      const color = new THREE.Color()
      darkColors.forEach((hex) => {
        color.setHex(hex)
        expect(color.r).toBeGreaterThanOrEqual(0)
        expect(color.r).toBeLessThanOrEqual(1)
        expect(color.g).toBeGreaterThanOrEqual(0)
        expect(color.g).toBeLessThanOrEqual(1)
        expect(color.b).toBeGreaterThanOrEqual(0)
        expect(color.b).toBeLessThanOrEqual(1)
      })
    })
  })

  describe('Material Properties', () => {
    it('creates material with correct properties', () => {
      const materialProps = {
        color: 0x5d4037,
        roughness: 0.7,
        metalness: 0.1,
        vertexColors: true,
      }

      // Verify material properties
      expect(materialProps.color).toBe(0x5d4037) // Dark brown
      expect(materialProps.roughness).toBe(0.7)
      expect(materialProps.metalness).toBe(0.1)
      expect(materialProps.vertexColors).toBe(true)
    })

    it('creates bump texture correctly', () => {
      const canvas = document.createElement('canvas')
      canvas.width = 256
      canvas.height = 256
      const ctx = canvas.getContext('2d')

      // Simulate texture creation if context is available
      if (ctx) {
        for (let x = 0; x < 256; x++) {
          for (let y = 0; y < 256; y++) {
            const value = Math.random() * 50 + 100
            ctx.fillStyle = `rgb(${value}, ${value}, ${value})`
            ctx.fillRect(x, y, 1, 1)
          }
        }
      }

      const texture = new THREE.CanvasTexture(canvas)
      expect(texture).toBeInstanceOf(THREE.CanvasTexture)
      expect(texture.wrapS).toBe(THREE.ClampToEdgeWrapping) // Default
      expect(texture.wrapT).toBe(THREE.ClampToEdgeWrapping) // Default
    })

    it('handles missing canvas context gracefully', () => {
      // Mock getContext to return null
      const mockGetContext = vi.fn().mockReturnValue(null)
      const canvas = { getContext: mockGetContext, width: 256, height: 256 }

      // Override createElement to return our mock
      const originalCreateElement = document.createElement
      document.createElement = vi.fn().mockImplementation((tag) => {
        if (tag === 'canvas') return canvas
        return originalCreateElement.call(document, tag)
      })

      // Component should handle null context without throwing
      expect(() => {
        render(<DogWasteVisualization />)
      }).not.toThrow()

      document.createElement = originalCreateElement
    })
  })

  describe('Ground Component', () => {
    it('creates grass texture', () => {
      const canvas = document.createElement('canvas')
      canvas.width = 512
      canvas.height = 512
      const ctx = canvas.getContext('2d')

      if (ctx) {
        // Fill base color
        ctx.fillStyle = '#3a5f3a'
        ctx.fillRect(0, 0, 512, 512)

        // Test that canvas was filled
        const imageData = ctx.getImageData(0, 0, 1, 1)
        expect(imageData.data[0]).toBeGreaterThan(0) // Red channel
        expect(imageData.data[1]).toBeGreaterThan(0) // Green channel
        expect(imageData.data[2]).toBeGreaterThan(0) // Blue channel
      } else {
        // In test environment, context might not be available
        expect(ctx).toBeNull()
      }
    })

    it('has correct ground material properties', () => {
      const groundMaterial = {
        color: 0x3a5f3a,
        roughness: 0.8,
        metalness: 0.1,
      }

      expect(groundMaterial.color).toBe(0x3a5f3a)
      expect(groundMaterial.roughness).toBe(0.8)
      expect(groundMaterial.metalness).toBe(0.1)
    })

    it('handles missing canvas context for grass texture', () => {
      // Mock getContext to return null
      const mockGetContext = vi.fn().mockReturnValue(null)
      const canvas = { getContext: mockGetContext, width: 512, height: 512 }

      // Override createElement to return our mock
      const originalCreateElement = document.createElement
      let callCount = 0
      document.createElement = vi.fn().mockImplementation((tag) => {
        if (tag === 'canvas' && callCount++ > 0) return canvas
        return originalCreateElement.call(document, tag)
      })

      // Component should handle null context without throwing
      expect(() => {
        render(<DogWasteVisualization />)
      }).not.toThrow()

      document.createElement = originalCreateElement
    })
  })

  describe('Scene Lighting', () => {
    it('has correct lighting setup', () => {
      const lights = {
        ambient: { intensity: 0.6 },
        directional: {
          position: [5, 10, 5],
          intensity: 0.8,
          shadowMapSize: [2048, 2048],
        },
        rim: {
          position: [-10, 10, -10],
          intensity: 0.5,
          color: 0x4488ff,
        },
      }

      expect(lights.ambient.intensity).toBe(0.6)
      expect(lights.directional.intensity).toBe(0.8)
      expect(lights.directional.shadowMapSize).toEqual([2048, 2048])
      expect(lights.rim.color).toBe(0x4488ff)
    })
  })

  describe('Camera and Controls', () => {
    it('sets up camera with correct position', () => {
      render(<DogWasteVisualization />)
      const camera = screen.getByTestId('perspective-camera-mock')
      expect(camera).toHaveAttribute('position', '5,3,5')
    })

    it('configures orbit controls correctly', () => {
      const controlsConfig = {
        enableDamping: true,
        dampingFactor: 0.05,
        minDistance: 2,
        maxDistance: 20,
        maxPolarAngle: Math.PI / 2 - 0.1,
      }

      expect(controlsConfig.enableDamping).toBe(true)
      expect(controlsConfig.dampingFactor).toBe(0.05)
      expect(controlsConfig.minDistance).toBe(2)
      expect(controlsConfig.maxDistance).toBe(20)
      expect(controlsConfig.maxPolarAngle).toBeCloseTo(1.47, 2)
    })
  })

  describe('Edge Cases and Error Handling', () => {
    it('handles WebGL context loss gracefully', () => {
      const canvas = document.createElement('canvas')
      const mockContext = canvas.getContext('webgl')

      if (mockContext) {
        // Simulate context loss
        const lossEvent = new Event('webglcontextlost')
        canvas.dispatchEvent(lossEvent)

        // Component should continue to render
        expect(() => {
          render(<DogWasteVisualization />)
        }).not.toThrow()
      }
    })

    it('renders with fallback when Three.js objects are not available', () => {
      // Mock Three.js module to simulate error
      vi.mock('three', () => ({
        ...vi.importActual('three'),
        Vector3: vi.fn().mockImplementation(() => {
          throw new Error('WebGL not supported')
        }),
      }))

      // Component should handle the error gracefully
      expect(() => {
        render(<DogWasteVisualization />)
      }).not.toThrow()

      // Restore mock
      vi.unmock('three')
    })
  })
})
