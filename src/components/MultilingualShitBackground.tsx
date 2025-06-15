import React, { useEffect, useState, useCallback } from "react";

interface ShitWord {
  id: number;
  text: string;
  x: number;
  y: number;
  size: number;
  opacity: number;
  speed: number;
  color: string;
  type: 'word' | 'sentence' | 'emoji';
  language: string;
}

export const MultilingualShitBackground: React.FC = () => {
  const [words, setWords] = useState<ShitWord[]>([]);

  const shitWords = {
    english: {
      words: ['shit', 'crap', 'poop', 'turd', 'dump', 'dung', 'waste', 'stool'],
      sentences: [
        'Holy shit!',
        'What the shit?',
        'No shit!',
        'Shit happens',
        'That\'s the shit!',
        'Bullshit!',
        'This is some good shit',
      ]
    },
    spanish: {
      words: ['mierda', 'caca', 'popó', 'porquería', 'excremento'],
      sentences: [
        '¡Qué mierda!',
        '¡No jodas!',
        '¡Está de la mierda!',
        'Pura mierda',
      ]
    },
    french: {
      words: ['merde', 'caca', 'crotte', 'étron', 'bouse'],
      sentences: [
        'Merde alors!',
        'C\'est de la merde!',
        'Quelle merde!',
        'Oh merde!',
      ]
    },
    german: {
      words: ['scheiße', 'kacke', 'mist', 'kot', 'dreck'],
      sentences: [
        'Was für ein Scheiß!',
        'Scheiße nochmal!',
        'Das ist Scheiße!',
        'Verdammte Scheiße!',
      ]
    },
    italian: {
      words: ['merda', 'cacca', 'stronzo', 'escremento'],
      sentences: [
        'Che merda!',
        'Madonna che schifo!',
        'È una merda!',
        'Merda santa!',
      ]
    },
    portuguese: {
      words: ['merda', 'cocô', 'bosta', 'porcaria', 'estrume'],
      sentences: [
        'Que merda!',
        'Puta merda!',
        'É uma bosta!',
        'Merda mesmo!',
      ]
    },
    russian: {
      words: ['говно', 'дерьмо', 'какашка', 'кал', 'срань'],
      sentences: [
        'Вот дерьмо!',
        'Какая хрень!',
        'Полное говно!',
        'Чертово дерьмо!',
      ]
    },
    japanese: {
      words: ['うんち', 'うんこ', 'クソ', '糞', 'ふん'],
      sentences: [
        'クソったれ！',
        'なんてこった！',
        'マジかよ！',
        'ふざけんな！',
      ]
    },
    chinese: {
      words: ['屎', '粪', '大便', '狗屎', '臭屎', '打卡', '合影', '史前留名', '火钳刘明'],
      sentences: [
        '什么鬼！',
        '狗屎！',
        '见鬼！',
        '慕名前来打卡',
        '名留青史',
        '大家都来都来留下一根脚毛',
        '我是史学家，这就是历史性的一个项目',
        '排泄点合影',
        '该死！',
      ]
    },
    korean: {
      words: ['똥', '대변', '분변', '똥꼬', '응가'],
      sentences: [
        '젠장!',
        '망할!',
        '개똥!',
        '뭐야 이거!',
      ]
    },
    hindi: {
      words: ['गू', 'टट्टी', 'हगना', 'मल', 'पाखाना'],
      sentences: [
        'क्या बकवास!',
        'ये क्या गंदगी है!',
        'बकवास बात!',
        'घटिया चीज़!',
      ]
    },
    arabic: {
      words: ['خرا', 'براز', 'غائط', 'قذر', 'وسخ'],
      sentences: [
        'يا إلهي!',
        'ما هذا الهراء!',
        'هذا سخيف!',
        'يا للقرف!',
      ]
    }
  };

  const shitEmojis = [
    '💩', '🚽', '🪠', '🧻', '🤢', '🤮', '🦨', '💨', '🫣', '😵‍💫',
    '🤯', '😱', '🙄', '🤦‍♂️', '🤷‍♀️', '💸', '🗑️', '♻️', '🚮', '🧽'
  ];

  const colors = [
    '#8B4513', '#654321', '#D2691E', '#CD853F', '#DEB887',
    '#F4A460', '#D2B48C', '#BC8F8F', '#A0522D', '#8B7355',
    '#6B4423', '#5D4037', '#8D6E63', '#A1887F', '#BCAAA4'
  ];

  const generateWord = useCallback((): ShitWord => {
    const languages = Object.keys(shitWords);
    const randomLang = languages[Math.floor(Math.random() * languages.length)];
    const langData = shitWords[randomLang as keyof typeof shitWords];
    
    let text: string;
    let type: 'word' | 'sentence' | 'emoji';
    
    const rand = Math.random();
    if (rand < 0.4) {
      // 40% words
      text = langData.words[Math.floor(Math.random() * langData.words.length)];
      type = 'word';
    } else if (rand < 0.7) {
      // 30% sentences
      text = langData.sentences[Math.floor(Math.random() * langData.sentences.length)];
      type = 'sentence';
    } else {
      // 30% emojis
      text = shitEmojis[Math.floor(Math.random() * shitEmojis.length)];
      type = 'emoji';
    }

    return {
      id: Date.now() + Math.random(),
      text,
      x: Math.random() * window.innerWidth,
      y: -50,
      size: type === 'emoji' ? (Math.random() * 20 + 20) : (Math.random() * 16 + 12),
      opacity: Math.random() * 0.6 + 0.1,
      speed: Math.random() * 1 + 0.5,
      color: type === 'emoji' ? 'transparent' : colors[Math.floor(Math.random() * colors.length)],
      type,
      language: randomLang,
    };
  }, []);

  useEffect(() => {
    // Initialize with some words
    const initialWords = Array.from({ length: 30 }, () => generateWord());
    setWords(initialWords.map(word => ({
      ...word,
      y: Math.random() * window.innerHeight,
    })));
  }, [generateWord]);

  useEffect(() => {
    const animateWords = () => {
      setWords(prevWords => {
        const updatedWords = prevWords.map(word => ({
          ...word,
          y: word.y + word.speed,
          x: word.x + Math.sin(word.y * 0.01) * 0.5,
        }));

        // Remove words that are off screen
        const visibleWords = updatedWords.filter(word => word.y < window.innerHeight + 100);

        // Add new words occasionally
        if (Math.random() < 0.05 && visibleWords.length < 50) {
          visibleWords.push(generateWord());
        }

        return visibleWords;
      });
    };

    const interval = setInterval(animateWords, 100);
    return () => clearInterval(interval);
  }, [generateWord]);

  const handleResize = useCallback(() => {
    setWords(prevWords => 
      prevWords.filter(word => word.x < window.innerWidth)
    );
  }, []);

  useEffect(() => {
    window.addEventListener('resize', handleResize);
    return () => window.removeEventListener('resize', handleResize);
  }, [handleResize]);

  return (
    <div className="fixed inset-0 -z-20 pointer-events-none overflow-hidden">
      {words.map(word => (
        <div
          key={word.id}
          className="absolute select-none font-mono"
          style={{
            left: `${word.x}px`,
            top: `${word.y}px`,
            fontSize: `${word.size}px`,
            color: word.color,
            opacity: word.opacity,
            transform: `rotate(${Math.sin(word.y * 0.01) * 5}deg)`,
            textShadow: word.type === 'emoji' ? 'none' : '1px 1px 2px rgba(0,0,0,0.3)',
            filter: word.type === 'emoji' ? 'none' : 'blur(0.5px)',
          }}
        >
          {word.text}
        </div>
      ))}
    </div>
  );
};