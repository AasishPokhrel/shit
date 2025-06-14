import React, { useEffect, useRef, useCallback } from "react";

interface MatrixBackgroundProps {
  className?: string;
}

export const MatrixBackground: React.FC<MatrixBackgroundProps> = ({ className = "" }) => {
  const canvasRef = useRef<HTMLCanvasElement>(null);
  const animationIdRef = useRef<number>();
  const lastFrameTimeRef = useRef<number>(0);

  const FPS = 30; // Limit to 30fps for better performance
  const FRAME_INTERVAL = 1000 / FPS;

  const draw = useCallback((ctx: CanvasRenderingContext2D, canvas: HTMLCanvasElement, drops: number[]) => {
    // Check for reduced motion preference
    if (window.matchMedia && window.matchMedia('(prefers-reduced-motion: reduce)').matches) {
      return;
    }

    const chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789💩🔥⚡🚀💻📦🎉";
    const charArray = chars.split("");
    const fontSize = 14;

    // Black background with slight transparency for trail effect
    ctx.fillStyle = "rgba(0, 0, 0, 0.05)";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    // Green text
    ctx.fillStyle = "#00FF00";
    ctx.font = `${fontSize}px monospace`;

    for (let i = 0; i < drops.length; i++) {
      const text = charArray[Math.floor(Math.random() * charArray.length)];
      ctx.fillText(text, i * fontSize, drops[i] * fontSize);

      // Reset drop to top randomly
      if (drops[i] * fontSize > canvas.height && Math.random() > 0.975) {
        drops[i] = 0;
      }
      drops[i]++;
    }
  }, []);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;

    const ctx = canvas.getContext("2d");
    if (!ctx) return;

    // Set canvas size
    const updateCanvasSize = () => {
      canvas.width = window.innerWidth;
      canvas.height = window.innerHeight;
    };
    updateCanvasSize();

    const fontSize = 14;
    const columns = Math.floor(canvas.width / fontSize);
    const drops: number[] = [];

    // Initialize drops
    for (let i = 0; i < columns; i++) {
      drops[i] = 1;
    }

    const animate = (currentTime: number) => {
      // Frame rate limiting
      if (currentTime - lastFrameTimeRef.current >= FRAME_INTERVAL) {
        draw(ctx, canvas, drops);
        lastFrameTimeRef.current = currentTime;
      }
      
      animationIdRef.current = requestAnimationFrame(animate);
    };

    const handleResize = () => {
      updateCanvasSize();
      // Recalculate columns
      const newColumns = Math.floor(canvas.width / fontSize);
      drops.length = newColumns;
      for (let i = 0; i < newColumns; i++) {
        if (drops[i] === undefined) drops[i] = 1;
      }
    };

    // Start animation
    animationIdRef.current = requestAnimationFrame(animate);
    window.addEventListener("resize", handleResize);

    return () => {
      if (animationIdRef.current) {
        cancelAnimationFrame(animationIdRef.current);
      }
      window.removeEventListener("resize", handleResize);
    };
  }, [draw]);

  return (
    <canvas
      ref={canvasRef}
      className={`fixed inset-0 -z-10 opacity-20 ${className}`}
      style={{ pointerEvents: "none" }}
    />
  );
};