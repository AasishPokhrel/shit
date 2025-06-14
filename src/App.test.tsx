/**
 * App Component Tests
 *
 * Tests for the main App component to ensure it renders correctly
 * and contains the DogWasteVisualization component.
 */

import { describe, it, expect } from 'vitest'
import { render } from '@testing-library/react'
import App from './App'

describe('App', () => {
  it('renders without crashing', () => {
    const { container } = render(<App />)
    expect(container).toBeTruthy()
  })

  it('renders the DogWasteVisualization component', () => {
    const { container } = render(<App />)
    // Check that the 3D canvas container is rendered
    const canvasContainer = container.querySelector('div')
    expect(canvasContainer).toBeInTheDocument()
    expect(canvasContainer).toHaveStyle({ width: '100%', height: '100vh' })
  })
})
