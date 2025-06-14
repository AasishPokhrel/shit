/**
 * DogWasteVisualization Component
 *
 * This component creates a realistic 3D visualization of dog feces using Three.js and React Three Fiber.
 * The visualization is designed to be anatomically accurate with proper colors, textures, and shape.
 *
 * Key features:
 * - Realistic coiled pile shape with 2.5 spiral rotations
 * - Dark brown color scheme (#5D4037 primary with variations)
 * - Wet/glossy appearance using MeshStandardMaterial
 * - Natural irregularities and bulges for organic look
 * - Interactive orbit controls for viewing
 * - Proper shadows and lighting
 */

import { useRef, useMemo } from 'react'
import { Canvas } from '@react-three/fiber'
import { OrbitControls, PerspectiveCamera } from '@react-three/drei'
import * as THREE from 'three'

/**
 * Creates the spiral path for the tube geometry
 * @param rotations - Number of spiral rotations (2.5 for realistic appearance)
 * @param segments - Number of segments in the path (higher = smoother)
 * @param totalHeight - Total height of the pile
 * @param baseRadius - Starting radius at the base
 * @returns Array of Vector3 points forming the spiral path
 */
function createSpiralPath(
  rotations: number = 2.5,
  segments: number = 100,
  totalHeight: number = 2,
  baseRadius: number = 1.8
): THREE.Vector3[] {
  const points: THREE.Vector3[] = []

  for (let i = 0; i <= segments; i++) {
    const t = i / segments // Progress along the spiral (0 to 1)
    const angle = t * Math.PI * 2 * rotations // Current angle in the spiral

    // Taper the spiral radius from wide base to narrow top
    const spiralRadius = baseRadius * (1 - t * 0.8)

    // Add natural irregularities using sine waves
    const wobble = Math.sin(t * 15) * 0.05 + Math.sin(t * 23) * 0.03
    const r = spiralRadius + wobble

    // Calculate position in 3D space
    const x = Math.cos(angle) * r
    const z = Math.sin(angle) * r
    let y = t * totalHeight

    // Base should spread slightly where it touches the ground
    if (t < 0.05) {
      y = y + 0.1 * (1 - t / 0.05)
    }

    // Add slight curve to the top portion
    const topCurve = t > 0.8 ? Math.sin((t - 0.8) * 5) * 0.1 : 0

    points.push(new THREE.Vector3(x + topCurve, y, z))
  }

  return points
}

/**
 * Creates custom tube geometry with varying radius
 * This is necessary because Three.js TubeGeometry doesn't natively support varying radius
 * @param curve - The curve path for the tube
 * @param tubeSegments - Number of segments along the tube
 * @param radialSegments - Number of segments around the tube
 * @returns BufferGeometry for the tube
 */
function createVariableRadiusTube(
  curve: THREE.CatmullRomCurve3,
  tubeSegments: number = 200,
  radialSegments: number = 16
): THREE.BufferGeometry {
  const geometry = new THREE.BufferGeometry()
  const vertices: number[] = []
  const normals: number[] = []
  const uvs: number[] = []
  const indices: number[] = []
  const colors: number[] = []

  // Color variations for realistic appearance
  const darkColors = [0x5d4037, 0x3e2723, 0x4e342e, 0x6d4c41]
  const color = new THREE.Color()

  // Generate vertices along the curve
  for (let i = 0; i <= tubeSegments; i++) {
    const t = i / tubeSegments
    const point = curve.getPointAt(t)
    const tangent = curve.getTangentAt(t)

    // Calculate radius at this point - tapers from base to tip
    const baseRadius = 0.3
    const tipRadius = 0.1
    const bulge = Math.sin(t * 8) * 0.04 + Math.sin(t * 13) * 0.03
    let radius = baseRadius - (baseRadius - tipRadius) * t + bulge

    // Make the start thicker (spread on ground)
    if (t < 0.05) {
      radius = radius * (1 + (1 - t / 0.05) * 0.3)
    }

    // Rounded end instead of sharp point
    if (t > 0.95) {
      const endFactor = (t - 0.95) / 0.05
      radius = radius * (1 - endFactor * 0.5)
    }

    // Create normal and binormal vectors for tube construction
    const normal = new THREE.Vector3(0, 1, 0)
    const binormal = new THREE.Vector3().crossVectors(tangent, normal).normalize()
    normal.crossVectors(binormal, tangent).normalize()

    // Generate vertices around the tube circumference
    for (let j = 0; j <= radialSegments; j++) {
      const v = (j / radialSegments) * Math.PI * 2
      const sin = Math.sin(v)
      const cos = Math.cos(v)

      // Calculate vertex position
      const x = point.x + radius * (cos * binormal.x + sin * normal.x)
      const y = point.y + radius * (cos * binormal.y + sin * normal.y)
      const z = point.z + radius * (cos * binormal.z + sin * normal.z)

      vertices.push(x, y, z)

      // Calculate normal for lighting
      const nx = cos * binormal.x + sin * normal.x
      const ny = cos * binormal.y + sin * normal.y
      const nz = cos * binormal.z + sin * normal.z
      normals.push(nx, ny, nz)

      // UV coordinates for texture mapping
      uvs.push(i / tubeSegments, j / radialSegments)

      // Assign color variation based on position
      const colorIndex = Math.floor(Math.random() * darkColors.length)
      color.setHex(darkColors[colorIndex])
      colors.push(color.r, color.g, color.b)
    }
  }

  // Generate triangle indices for the tube mesh
  for (let i = 0; i < tubeSegments; i++) {
    for (let j = 0; j < radialSegments; j++) {
      const a = i * (radialSegments + 1) + j
      const b = a + radialSegments + 1
      const c = a + 1
      const d = b + 1

      indices.push(a, b, c)
      indices.push(b, d, c)
    }
  }

  // Set geometry attributes
  geometry.setIndex(indices)
  geometry.setAttribute('position', new THREE.Float32BufferAttribute(vertices, 3))
  geometry.setAttribute('normal', new THREE.Float32BufferAttribute(normals, 3))
  geometry.setAttribute('uv', new THREE.Float32BufferAttribute(uvs, 2))
  geometry.setAttribute('color', new THREE.Float32BufferAttribute(colors, 3))
  geometry.computeBoundingSphere()

  return geometry
}

/**
 * Creates a bump texture for surface detail
 * @returns CanvasTexture with noise pattern
 */
function createBumpTexture(): THREE.CanvasTexture {
  const canvas = document.createElement('canvas')
  canvas.width = 256
  canvas.height = 256
  const ctx = canvas.getContext('2d')

  // Check if context is available (might not be in test environment)
  if (ctx) {
    // Generate noise pattern for bump map
    for (let x = 0; x < 256; x++) {
      for (let y = 0; y < 256; y++) {
        const value = Math.random() * 50 + 100
        ctx.fillStyle = `rgb(${value}, ${value}, ${value})`
        ctx.fillRect(x, y, 1, 1)
      }
    }
  }

  const texture = new THREE.CanvasTexture(canvas)
  texture.wrapS = THREE.RepeatWrapping
  texture.wrapT = THREE.RepeatWrapping
  texture.repeat.set(4, 4)

  return texture
}

/**
 * Main pile mesh component
 */
function PileMesh() {
  const meshRef = useRef<THREE.Mesh>(null)

  // Create geometry and materials using memoization for performance
  const { geometry, material } = useMemo(() => {
    // Create spiral path
    const points = createSpiralPath()
    const curve = new THREE.CatmullRomCurve3(points)

    // Create custom tube geometry
    const geometry = createVariableRadiusTube(curve)

    // Create material with realistic properties
    const material = new THREE.MeshPhysicalMaterial({
      color: 0x5d4037, // Primary dark brown
      roughness: 0.6, // Slightly rough surface
      metalness: 0.05, // Minimal metallic appearance
      clearcoat: 0.3, // Wet/glossy appearance
      clearcoatRoughness: 0.4,
      bumpMap: createBumpTexture(),
      bumpScale: 0.015,
      vertexColors: true, // Enable per-vertex colors for variation
      envMapIntensity: 0.8, // Environmental reflection
    })

    return { geometry, material }
  }, [])

  return <mesh ref={meshRef} geometry={geometry} material={material} castShadow receiveShadow />
}

/**
 * Ground plane component with grass texture
 */
function Ground() {
  const texture = useMemo(() => {
    const canvas = document.createElement('canvas')
    canvas.width = 512
    canvas.height = 512
    const ctx = canvas.getContext('2d')

    // Check if context is available (might not be in test environment)
    if (ctx) {
      // Base grass color
      ctx.fillStyle = '#3a5f3a'
      ctx.fillRect(0, 0, 512, 512)

      // Draw individual grass blades
      for (let i = 0; i < 5000; i++) {
        const x = Math.random() * 512
        const y = Math.random() * 512
        const length = Math.random() * 20 + 5
        const angle = Math.random() * 0.3 - 0.15

        // Vary grass color slightly
        const hue = 100 + Math.random() * 40
        const lightness = 30 + Math.random() * 20
        ctx.strokeStyle = `hsl(${hue}, 50%, ${lightness}%)`
        ctx.lineWidth = Math.random() * 2 + 1

        // Draw grass blade
        ctx.beginPath()
        ctx.moveTo(x, y)
        ctx.lineTo(x + Math.sin(angle) * length, y - length)
        ctx.stroke()
      }
    }

    const texture = new THREE.CanvasTexture(canvas)
    texture.wrapS = THREE.RepeatWrapping
    texture.wrapT = THREE.RepeatWrapping
    texture.repeat.set(4, 4)

    return texture
  }, [])

  return (
    <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -0.01, 0]} receiveShadow>
      <planeGeometry args={[20, 20]} />
      <meshStandardMaterial map={texture} color={0x3a5f3a} roughness={0.8} metalness={0.1} />
    </mesh>
  )
}

/**
 * Scene component containing all 3D objects and lighting
 */
function Scene() {
  return (
    <>
      {/* Ambient light for overall illumination - reduced for more dramatic shadows */}
      <ambientLight intensity={0.3} />

      {/* Main sun light with realistic intensity and shadows */}
      <directionalLight
        position={[10, 20, 5]}
        intensity={1.5}
        color={0xffffff}
        castShadow
        shadow-mapSize={[2048, 2048]}
        shadow-camera-near={0.1}
        shadow-camera-far={50}
        shadow-camera-left={-10}
        shadow-camera-right={10}
        shadow-camera-top={10}
        shadow-camera-bottom={-10}
        shadow-bias={-0.001}
      />

      {/* Sky light for realistic outdoor lighting */}
      <hemisphereLight color={0x87ceeb} groundColor={0x362a1f} intensity={0.6} />

      {/* Subtle fill light to soften shadows */}
      <directionalLight position={[-5, 8, -5]} intensity={0.3} color={0xffd4a3} />

      {/* Main pile mesh */}
      <PileMesh />

      {/* Ground plane */}
      <Ground />

      {/* Fog for atmospheric effect - darker for black background */}
      <fog attach="fog" args={[0x000000, 50, 200]} />
    </>
  )
}

/**
 * Main visualization component
 */
export default function DogWasteVisualization() {
  return (
    <div style={{ width: '100%', height: '100vh', background: '#000000' }}>
      <div
        style={{
          position: 'absolute',
          top: 10,
          left: 10,
          color: 'white',
          background: 'rgba(0,0,0,0.5)',
          padding: '10px',
          borderRadius: '5px',
          fontSize: '14px',
          fontFamily: 'Arial, sans-serif',
          zIndex: 1,
        }}
      >
        Click and drag to rotate • Scroll to zoom
      </div>

      <Canvas shadows>
        <PerspectiveCamera makeDefault position={[5, 3, 5]} />
        <OrbitControls
          enableDamping
          dampingFactor={0.05}
          minDistance={2}
          maxDistance={20}
          maxPolarAngle={Math.PI / 2 - 0.1}
        />
        <Scene />
      </Canvas>
    </div>
  )
}
