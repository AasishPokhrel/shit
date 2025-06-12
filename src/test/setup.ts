/**
 * Test Setup File
 *
 * This file runs before all tests and sets up the testing environment.
 * It imports jest-dom for additional DOM matchers and configures global mocks.
 */

import '@testing-library/jest-dom'

// Mock WebGL context for Three.js testing
const mockWebGLContext = {
  canvas: {
    width: 1024,
    height: 768,
    style: {},
    addEventListener: vi.fn(),
    removeEventListener: vi.fn(),
    clientWidth: 1024,
    clientHeight: 768,
    getContext: vi.fn(() => mockWebGLContext),
  },
  getParameter: vi.fn((param: number) => {
    const params: Record<number, string | number> = {
      7937: 16384, // MAX_TEXTURE_SIZE
      7938: 8192, // MAX_VIEWPORT_DIMS
      3379: 16384, // MAX_TEXTURE_SIZE
      34930: 16, // MAX_TEXTURE_IMAGE_UNITS
      35661: 16, // MAX_FRAGMENT_UNIFORM_VECTORS
      36349: 1024, // MAX_VERTEX_UNIFORM_VECTORS
      34076: 16384, // MAX_CUBE_MAP_TEXTURE_SIZE
      36347: 1024, // MAX_VERTEX_ATTRIBS
      37157: 8, // MAX_VERTEX_TEXTURE_IMAGE_UNITS
      35724: 'WebGL 1.0', // SHADING_LANGUAGE_VERSION
    }
    return params[param] || 0
  }),
  getExtension: vi.fn(),
  getShaderPrecisionFormat: vi.fn(() => ({
    precision: 23,
    rangeMin: 127,
    rangeMax: 127,
  })),
  createShader: vi.fn(() => ({})),
  shaderSource: vi.fn(),
  compileShader: vi.fn(),
  getShaderParameter: vi.fn(() => true),
  createProgram: vi.fn(() => ({})),
  attachShader: vi.fn(),
  linkProgram: vi.fn(),
  getProgramParameter: vi.fn(() => true),
  useProgram: vi.fn(),
  createBuffer: vi.fn(() => ({})),
  bindBuffer: vi.fn(),
  bufferData: vi.fn(),
  createTexture: vi.fn(() => ({})),
  bindTexture: vi.fn(),
  texParameteri: vi.fn(),
  texImage2D: vi.fn(),
  createFramebuffer: vi.fn(() => ({})),
  bindFramebuffer: vi.fn(),
  framebufferTexture2D: vi.fn(),
  createRenderbuffer: vi.fn(() => ({})),
  bindRenderbuffer: vi.fn(),
  renderbufferStorage: vi.fn(),
  framebufferRenderbuffer: vi.fn(),
  checkFramebufferStatus: vi.fn(() => 36053), // FRAMEBUFFER_COMPLETE
  viewport: vi.fn(),
  clear: vi.fn(),
  clearColor: vi.fn(),
  enable: vi.fn(),
  disable: vi.fn(),
  depthFunc: vi.fn(),
  cullFace: vi.fn(),
  frontFace: vi.fn(),
  blendFunc: vi.fn(),
  blendEquation: vi.fn(),
  pixelStorei: vi.fn(),
  generateMipmap: vi.fn(),
  getUniformLocation: vi.fn(() => ({})),
  uniformMatrix4fv: vi.fn(),
  uniform1f: vi.fn(),
  uniform2f: vi.fn(),
  uniform3f: vi.fn(),
  uniform4f: vi.fn(),
  uniform1i: vi.fn(),
  getAttribLocation: vi.fn(() => 0),
  vertexAttribPointer: vi.fn(),
  enableVertexAttribArray: vi.fn(),
  drawArrays: vi.fn(),
  drawElements: vi.fn(),
  deleteShader: vi.fn(),
  deleteProgram: vi.fn(),
  deleteBuffer: vi.fn(),
  deleteTexture: vi.fn(),
  deleteFramebuffer: vi.fn(),
  deleteRenderbuffer: vi.fn(),
  isContextLost: vi.fn(() => false),
}

// Mock HTMLCanvasElement.getContext
HTMLCanvasElement.prototype.getContext = vi.fn((contextType: string) => {
  if (contextType === 'webgl' || contextType === 'webgl2' || contextType === 'experimental-webgl') {
    return mockWebGLContext
  }
  return null
})

// Mock window.matchMedia
Object.defineProperty(window, 'matchMedia', {
  writable: true,
  value: vi.fn().mockImplementation((query) => ({
    matches: false,
    media: query,
    onchange: null,
    addListener: vi.fn(), // deprecated
    removeListener: vi.fn(), // deprecated
    addEventListener: vi.fn(),
    removeEventListener: vi.fn(),
    dispatchEvent: vi.fn(),
  })),
})

// Mock IntersectionObserver
global.IntersectionObserver = vi.fn().mockImplementation(() => ({
  observe: vi.fn(),
  unobserve: vi.fn(),
  disconnect: vi.fn(),
}))

// Mock ResizeObserver
global.ResizeObserver = vi.fn().mockImplementation(() => ({
  observe: vi.fn(),
  unobserve: vi.fn(),
  disconnect: vi.fn(),
}))
