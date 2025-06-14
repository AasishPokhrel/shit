/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,ts,jsx,tsx}",
  ],
  darkMode: ['media'], // Auto dark mode based on system preference
  theme: {
    extend: {
      colors: {
        // Terminal color palette
        terminal: {
          black: "#000000",
          green: "#00FF00",
          amber: "#FFA500",
          red: "#FF0000",
          cyan: "#00FFFF",
          white: "#FFFFFF",
          gray: "#808080",
        },
        // Enhanced shit color palette
        shit: {
          50: "#fefce8",
          100: "#fef9c3", 
          200: "#fef08a",
          300: "#fde047",
          400: "#facc15",
          500: "#eab308",
          600: "#ca8a04",
          700: "#a16207",
          800: "#854d0e",
          900: "#713f12",
          950: "#422006",
          brown: "#8B4513",
          gold: "#FFD700",
        },
        // Matrix theme
        matrix: {
          bg: "#000000",
          green: "#00FF00",
          darkgreen: "#008000",
          glow: "#00FF0040",
        },
      },
      fontFamily: {
        mono: ["'Courier New'", "'SF Mono'", "'Monaco'", "'Inconsolata'", "'Roboto Mono'", "monospace"],
        terminal: ["'Courier New'", "monospace"],
        sans: ["Inter", "system-ui", "sans-serif"],
        display: ["Poppins", "system-ui", "sans-serif"],
      },
      animation: {
        // Existing animations
        "bounce-gentle": "bounce-gentle 2s infinite",
        "wiggle": "wiggle 1s ease-in-out infinite", 
        "pulse-slow": "pulse 3s cubic-bezier(0.4, 0, 0.6, 1) infinite",
        "float": "float 3s ease-in-out infinite",
        // New shit-themed animations
        "shit-float": "shit-float 4s ease-in-out infinite",
        "shit-spin": "shit-spin 3s linear infinite", 
        "shit-bounce": "shit-bounce 2s ease-in-out infinite",
        "matrix-rain": "matrix-rain 3s linear infinite",
        "glitch": "glitch 0.3s ease-in-out infinite alternate-reverse",
        "typewriter": "typewriter 2s steps(20) 1s both",
        "counter-up": "counter-up 3s ease-out forwards",
        "explode": "explode 0.5s ease-out forwards",
        "terminal-blink": "terminal-blink 1s step-end infinite",
        // Chaotic but great animations
        "chaos-shake": "chaos-shake 0.5s ease-in-out infinite",
        "rainbow-text": "rainbow-text 3s linear infinite",
        "wobble": "wobble 1s ease-in-out infinite",
        "slide-crazy": "slide-crazy 8s linear infinite",
        "scale-pulse": "scale-pulse 2s ease-in-out infinite",
        "color-shift": "color-shift 4s ease-in-out infinite",
      },
      keyframes: {
        // Existing keyframes  
        "bounce-gentle": {
          "0%, 100%": {
            transform: "translateY(-5%)",
            animationTimingFunction: "cubic-bezier(0.8, 0, 1, 1)",
          },
          "50%": {
            transform: "translateY(0)",
            animationTimingFunction: "cubic-bezier(0, 0, 0.2, 1)",
          },
        },
        wiggle: {
          "0%, 100%": { transform: "rotate(-3deg)" },
          "50%": { transform: "rotate(3deg)" },
        },
        float: {
          "0%, 100%": { transform: "translateY(0px)" },
          "50%": { transform: "translateY(-10px)" },
        },
        // New shit-themed keyframes
        "shit-float": {
          "0%, 100%": { transform: "translateY(0px) rotate(0deg)" },
          "25%": { transform: "translateY(-15px) rotate(5deg)" },
          "50%": { transform: "translateY(-25px) rotate(0deg)" },
          "75%": { transform: "translateY(-10px) rotate(-5deg)" },
        },
        "shit-spin": {
          "0%": { transform: "rotate(0deg)" },
          "100%": { transform: "rotate(360deg)" },
        },
        "shit-bounce": {
          "0%, 100%": { transform: "translateY(0) scale(1)" },
          "50%": { transform: "translateY(-20px) scale(1.1)" },
        },
        "matrix-rain": {
          "0%": { transform: "translateY(-100vh)" },
          "100%": { transform: "translateY(100vh)" },
        },
        glitch: {
          "0%": {
            transform: "translate(0)",
            filter: "hue-rotate(0deg)",
          },
          "20%": {
            transform: "translate(-2px, 2px)",
            filter: "hue-rotate(90deg)",
          },
          "40%": {
            transform: "translate(-2px, -2px)",
            filter: "hue-rotate(180deg)",
          },
          "60%": {
            transform: "translate(2px, 2px)",
            filter: "hue-rotate(270deg)",
          },
          "80%": {
            transform: "translate(2px, -2px)",
            filter: "hue-rotate(360deg)",
          },
          "100%": {
            transform: "translate(0)",
            filter: "hue-rotate(0deg)",
          },
        },
        typewriter: {
          "0%": { width: "0%" },
          "100%": { width: "100%" },
        },
        "counter-up": {
          "0%": { transform: "scale(0.8) translateY(20px)", opacity: "0" },
          "100%": { transform: "scale(1) translateY(0)", opacity: "1" },
        },
        explode: {
          "0%": { transform: "scale(1)", opacity: "1" },
          "50%": { transform: "scale(1.5)", opacity: "0.7" },
          "100%": { transform: "scale(2)", opacity: "0" },
        },
        "terminal-blink": {
          "0%, 50%": { opacity: "1" },
          "51%, 100%": { opacity: "0" },
        },
        // Chaotic but great keyframes
        "chaos-shake": {
          "0%, 100%": { transform: "translateX(0)" },
          "10%": { transform: "translateX(-2px) rotate(1deg)" },
          "20%": { transform: "translateX(2px) rotate(-1deg)" },
          "30%": { transform: "translateX(-1px) rotate(0.5deg)" },
          "40%": { transform: "translateX(1px) rotate(-0.5deg)" },
          "50%": { transform: "translateX(-0.5px) rotate(0.2deg)" },
          "60%": { transform: "translateX(0.5px) rotate(-0.2deg)" },
        },
        "rainbow-text": {
          "0%": { color: "#ff0000" },
          "16.66%": { color: "#ff8000" },
          "33.33%": { color: "#ffff00" },
          "50%": { color: "#00ff00" },
          "66.66%": { color: "#0080ff" },
          "83.33%": { color: "#8000ff" },
          "100%": { color: "#ff0000" },
        },
        "wobble": {
          "0%, 100%": { transform: "rotate(0deg)" },
          "25%": { transform: "rotate(3deg) scale(1.05)" },
          "75%": { transform: "rotate(-3deg) scale(0.95)" },
        },
        "slide-crazy": {
          "0%": { transform: "translateX(-100vw) rotate(0deg)" },
          "25%": { transform: "translateX(100vw) rotate(180deg)" },
          "50%": { transform: "translateX(-100vw) rotate(360deg)" },
          "75%": { transform: "translateX(100vw) rotate(540deg)" },
          "100%": { transform: "translateX(-100vw) rotate(720deg)" },
        },
        "scale-pulse": {
          "0%, 100%": { transform: "scale(1)" },
          "50%": { transform: "scale(1.2) rotate(5deg)" },
        },
        "color-shift": {
          "0%": { filter: "hue-rotate(0deg) saturate(100%)" },
          "25%": { filter: "hue-rotate(90deg) saturate(150%)" },
          "50%": { filter: "hue-rotate(180deg) saturate(200%)" },
          "75%": { filter: "hue-rotate(270deg) saturate(150%)" },
          "100%": { filter: "hue-rotate(360deg) saturate(100%)" },
        },
      },
      backdropBlur: {
        xs: "2px",
      },
    },
  },
  plugins: [],
};