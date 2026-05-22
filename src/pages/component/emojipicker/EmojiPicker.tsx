import { useState, useMemo, useRef } from "react";
import styles from "./scss/emojipicker.module.scss"
interface Category {
  id: string;
  icon: string;
  label: string;
}

interface EmojiDataMap {
  [key: string]: string[];
}

interface EmojiPickerProps {
  onEmojiSelect?: (emoji: string) => void;
}

const CATEGORIES: Category[] = [
  { id: "recent", icon: "🕐", label: "Recent" },
  { id: "smileys", icon: "😊", label: "Smileys & People" },
  { id: "nature", icon: "🌿", label: "Animals & Nature" },
  { id: "food", icon: "🍔", label: "Food & Drink" },
  { id: "travel", icon: "✈️", label: "Travel & Places" },
  { id: "activity", icon: "🏃", label: "Activities" },
  { id: "objects", icon: "💡", label: "Objects" },
  { id: "symbols", icon: "🚫", label: "Symbols" },
  { id: "flags", icon: "🏳️", label: "Flags" },
];

const EMOJI_DATA: EmojiDataMap = {
  recent: ["👍", "👋", "😄", "🙂", "❤️"],
  smileys: [
    "😀","😁","😂","🤣","😃","😄","😅","😆","😉","😊","😋","😎","😍","🥰","😘",
    "😗","😙","😚","🙂","🤗","🤩","🤔","🤨","😐","😑","😶","🙄","😏","😣",
    "😥","😮","🤐","😯","😪","😫","🥱","😴","😌","😛","😜","😝","🤤","😒",
    "😓","😔","😕","🙃","🤑","😲","☹️","🙁","😖","😞","😟","😤","😢","😭",
    "😦","😧","😨","😩","🤯","😬","😰","😱","🥵","🥶","😳","🤪","😵","🥴",
    "😠","😡","🤬","😷","🤒","🤕","🤢","🤮","🤧","😇","🥳","🥸","🤠","🤡",
  ],
  nature: [
    "🐶","🐱","🐭","🐹","🐰","🦊","🐻","🐼","🐨","🐯","🦁","🐮","🐷","🐸","🐵",
    "🙈","🙉","🙊","🐔","🐧","🐦","🐤","🦆","🦅","🦉","🦇","🐺","🐗","🐴","🦄",
    "🐝","🐛","🦋","🐌","🐞","🐜","🦟","🦗","🕷","🦂","🐢","🐍","🦎","🦖","🦕",
    "🐙","🦑","🦐","🦞","🦀","🐡","🐠","🐟","🐬","🐳","🐋","🦈","🐊","🐅","🐆",
  ],
  food: [
    "🍎","🍊","🍋","🍇","🍓","🫐","🍈","🍒","🍑","🥭","🍍","🥥","🥝","🍅","🍆",
    "🥑","🥦","🥬","🥒","🌶","🫑","🧄","🧅","🥔","🍠","🥐","🥖","🫓","🥨","🧀",
    "🍕","🌭","🍔","🍟","🥪","🥙","🧆","🌮","🌯","🫔","🥗","🥘","🫕","🍝","🍜",
    "🍲","🍛","🍣","🍱","🥟","🦪","🍤","🍙","🍚","🍘","🍥","🥮","🍢","🧁","🍰",
  ],
  travel: [
    "🚗","🚕","🚙","🚌","🚎","🏎","🚓","🚑","🚒","🚐","🛻","🚚","🚛","🚜","🛵",
    "🏍","🚲","🛴","🛹","🛼","🚏","🛣","🛤","⛽","🚨","🚥","🚦","🛑","🚧","⚓",
    "🛟","⛵","🛶","🚤","🛥","🛳","⛴","🚢","✈️","🛩","🛫","🛬","🪂","💺","🚁",
    "🛸","🚀","🛰","🌍","🌎","🌏","🗺","🧭","🏔","⛰","🌋","🗻","🏕","🏖","🏜",
  ],
  activity: [
    "⚽","🏀","🏈","⚾","🥎","🏐","🏉","🎾","🥏","🎳","🏏","🏑","🏒","🥍","🏓",
    "🏸","🥊","🥋","🎽","🛹","🛼","🛷","⛸","🤸","🤼","🤺","🏇","⛷","🏂","🪂",
    "🏋","🤼","🤸","🤺","🏊","🚵","🚴","🏆","🥇","🥈","🥉","🎖","🎗","🎫","🎟",
    "🎪","🤹","🎭","🩺","🎨","🎬","🎤","🎧","🎼","🎹","🪘","🥁","🎷","🎺","🎸",
  ],
  objects: [
    "⌚","📱","💻","⌨️","🖥","🖨","🖱","🖲","💽","💾","💿","📀","📷","📸","📹",
    "📼","📞","☎️","📟","📠","📺","📻","🧭","⏱","⏲","⏰","🕰","⌛","⏳","📡",
    "🔋","🔌","💡","🔦","🕯","🪔","🧯","🛢","💵","💴","💶","💷","🪙","💰","💳",
    "💎","⚖️","🧰","🔧","🔨","⚒","🛠","⛏","⚙️","🗜","🔩","🪛","🔫","💣","🪓",
  ],
  symbols: [
    "❤️","🧡","💛","💚","💙","💜","🖤","🤍","🤎","💔","❣️","💕","💞","💓","💗",
    "💖","💘","💝","💟","☮️","✝️","☪️","🕉","☸️","✡️","🔯","🕎","☯️","☦️","🛐",
    "⛎","♈","♉","♊","♋","♌","♍","♎","♏","♐","♑","♒","♓","🆔","⚛️",
    "🉑","☢️","☣️","📴","📳","🈶","🈚","🈸","🈺","🈷️","✴️","🆚","💮","🉐","㊙️",
  ],
  flags: [
    "🏳️","🏴","🏴‍☠️","🏁","🚩","🏳️‍🌈","🏳️‍⚧️","🇺🇳","🇦🇫","🇦🇱","🇩🇿","🇦🇩","🇦🇴","🇦🇬","🇦🇷",
    "🇦🇲","🇦🇺","🇦🇹","🇦🇿","🇧🇸","🇧🇭","🇧🇩","🇧🇧","🇧🇾","🇧🇪","🇧🇿","🇧🇯","🇧🇹","🇧🇴","🇧🇦",
    "🇧🇼","🇧🇷","🇧🇳","🇧🇬","🇧🇫","🇧🇮","🇨🇻","🇰🇭","🇨🇲","🇨🇦","🇨🇫","🇹🇩","🇨🇱","🇨🇳","🇨🇴",
    "🇨🇬","🇨🇷","🇭🇷","🇨🇺","🇨🇾","🇨🇿","🇩🇰","🇩🇯","🇩🇴","🇪🇨","🇪🇬","🇸🇻","🇬🇶","🇪🇷","🇪🇪",
  ],
};

// Emoji name map for text-based search
const EMOJI_NAMES: Record<string, string[]> = {
  "😀": ["grinning","smile","happy","face"],
  "😁": ["beaming","grin","smile","happy"],
  "😂": ["joy","laugh","crying","funny","lol"],
  "🤣": ["rolling","floor","laughing","rofl","funny"],
  "😃": ["grinning","open","mouth","happy"],
  "😄": ["smile","open","mouth","happy","laugh"],
  "😅": ["sweat","grin","nervous","laugh"],
  "😆": ["laugh","squint","happy","haha"],
  "😉": ["wink","flirty","cheeky"],
  "😊": ["smile","blush","happy","sweet"],
  "😋": ["yum","tasty","tongue","food"],
  "😎": ["cool","sunglasses","smug"],
  "😍": ["heart eyes","love","crush","adore"],
  "🥰": ["smiling","hearts","love","affection"],
  "😘": ["kiss","blow","love","wink"],
  "🙂": ["slightly","smile","neutral","okay"],
  "🙃": ["upside","down","sarcasm","silly"],
  "😐": ["neutral","expressionless","meh"],
  "😑": ["expressionless","blank","annoyed"],
  "😶": ["no mouth","silent","speechless"],
  "🙄": ["eye roll","whatever","disbelief"],
  "😏": ["smirk","smug","flirt"],
  "😒": ["unamused","unhappy","meh"],
  "😓": ["downcast","sweat","nervous"],
  "😔": ["pensive","sad","disappointed"],
  "😕": ["confused","uncertain","meh"],
  "😣": ["persevere","struggle","pain"],
  "😥": ["sad","relieved","tear","cry"],
  "😮": ["open mouth","surprised","shock"],
  "😯": ["hushed","surprised","shock"],
  "😪": ["sleepy","tired","drool"],
  "😫": ["tired","exhausted","stressed"],
  "🥱": ["yawn","tired","bored","sleepy"],
  "😴": ["sleeping","zzz","tired","rest"],
  "😌": ["relieved","peaceful","calm","serene"],
  "😛": ["tongue","cheeky","silly"],
  "😜": ["wink","tongue","silly","playful"],
  "😝": ["squint","tongue","silly"],
  "🤤": ["drool","hungry","want","desire"],
  "😤": ["triumph","huff","snort","angry"],
  "😢": ["cry","sad","tear","upset"],
  "😭": ["sob","cry","loud","tears","sad"],
  "😦": ["frowning","open","surprised"],
  "😧": ["anguished","shocked","worried"],
  "😨": ["fearful","scared","afraid"],
  "😩": ["weary","tired","frustrated"],
  "🤯": ["mind blown","exploding","shocked","wow"],
  "😬": ["grimace","nervous","awkward"],
  "😰": ["cold sweat","anxious","nervous"],
  "😱": ["scream","fear","shocked","horror"],
  "🥵": ["hot","sweating","overheated","flushed"],
  "🥶": ["cold","freezing","blue","shiver"],
  "😳": ["flushed","embarrassed","shocked"],
  "🤪": ["zany","crazy","wacky","silly"],
  "😵": ["dizzy","knocked","spiral","faint"],
  "🥴": ["woozy","drunk","unwell","dizzy"],
  "😠": ["angry","mad","grumpy"],
  "😡": ["pouting","angry","red","furious"],
  "🤬": ["swearing","cursing","angry","mad"],
  "😷": ["mask","sick","ill","covid"],
  "🤒": ["thermometer","sick","fever","ill"],
  "🤕": ["bandage","hurt","injured","pain"],
  "🤢": ["nauseated","sick","gross","vomit"],
  "🤮": ["vomit","sick","gross","puke"],
  "🤧": ["sneezing","tissue","sick","cold"],
  "😇": ["angel","halo","innocent","good"],
  "🥳": ["party","celebrate","birthday","festive"],
  "🥸": ["disguise","glasses","mustache","incognito"],
  "🤠": ["cowboy","hat","western","yeehaw"],
  "🤡": ["clown","creepy","funny","circus"],
  "🤔": ["thinking","hmm","ponder","wonder"],
  "🤨": ["raised eyebrow","skeptical","suspicious"],
  "🤗": ["hugging","hug","warm","embrace"],
  "🤩": ["star struck","wow","amazing","starstruck"],
  "👍": ["thumbs up","like","good","approve","yes"],
  "👋": ["wave","hello","hi","bye","hand"],
  "❤️": ["heart","love","red","like","favorite"],
  "🐶": ["dog","puppy","pet","animal","woof"],
  "🐱": ["cat","kitten","pet","animal","meow"],
  "🐭": ["mouse","rodent","animal"],
  "🐹": ["hamster","pet","animal"],
  "🐰": ["rabbit","bunny","animal","hop"],
  "🦊": ["fox","animal","sly","clever"],
  "🐻": ["bear","animal","teddy"],
  "🐼": ["panda","animal","bamboo","china"],
  "🐨": ["koala","animal","australia"],
  "🐯": ["tiger","animal","stripe","fierce"],
  "🦁": ["lion","animal","king","roar"],
  "🐮": ["cow","animal","moo","farm"],
  "🐷": ["pig","animal","oink","farm"],
  "🐸": ["frog","animal","green","jump"],
  "🐵": ["monkey","animal","primate"],
  "🦋": ["butterfly","insect","colorful","wings"],
  "🐢": ["turtle","animal","slow","shell"],
  "🐍": ["snake","reptile","hiss"],
  "🐙": ["octopus","sea","tentacles"],
  "🦈": ["shark","fish","sea","danger"],
  "🍎": ["apple","fruit","red","food"],
  "🍊": ["orange","fruit","citrus","food"],
  "🍋": ["lemon","fruit","yellow","sour","food"],
  "🍇": ["grapes","fruit","purple","wine","food"],
  "🍓": ["strawberry","fruit","red","sweet","food"],
  "🍕": ["pizza","food","italian","cheese","slice"],
  "🍔": ["burger","food","hamburger","fast food"],
  "🍟": ["fries","food","fast food","potato"],
  "🌮": ["taco","food","mexican"],
  "🍣": ["sushi","food","japanese","fish"],
  "🍜": ["noodles","ramen","food","soup"],
  "🍰": ["cake","food","sweet","dessert","birthday"],
  "🧁": ["cupcake","food","sweet","dessert"],
  "☕": ["coffee","hot","drink","cafe","morning"],
  "🍺": ["beer","drink","alcohol","cheers"],
  "🚗": ["car","vehicle","drive","transport","red"],
  "✈️": ["airplane","fly","travel","plane","flight"],
  "🚀": ["rocket","space","launch","astronaut"],
  "🏠": ["house","home","building"],
  "🌍": ["earth","globe","world","planet","africa"],
  "🌎": ["earth","globe","world","planet","america"],
  "🌏": ["earth","globe","world","planet","asia"],
  "⚽": ["soccer","football","sport","ball","kick"],
  "🏀": ["basketball","sport","ball","nba"],
  "🏈": ["football","sport","nfl","american"],
  "⚾": ["baseball","sport","ball"],
  "🎾": ["tennis","sport","ball","court"],
  "🏆": ["trophy","win","award","champion","gold"],
  "🥇": ["gold","medal","first","winner","award"],
  "🎮": ["game","controller","video","play","gaming"],
  "🎨": ["art","palette","paint","creative","color"],
  "🎵": ["music","note","song","melody"],
  "🎶": ["music","notes","song","melody"],
  "📱": ["phone","mobile","cell","device","smartphone"],
  "💻": ["laptop","computer","device","tech"],
  "⌨️": ["keyboard","type","computer","input"],
  "💡": ["bulb","idea","light","bright"],
  "🔋": ["battery","charge","energy","power"],
  "📷": ["camera","photo","picture","snapshot"],
  "❤️": ["heart","love","red","like"],
  "🧡": ["orange","heart","love"],
  "💛": ["yellow","heart","love","happy"],
  "💚": ["green","heart","love","nature"],
  "💙": ["blue","heart","love","calm"],
  "💜": ["purple","heart","love"],
  "🖤": ["black","heart","dark","love"],
  "💔": ["broken","heart","sad","heartbreak"],
  "💕": ["hearts","love","pink","two"],
  "💯": ["hundred","100","perfect","score","yes"],
  "🔥": ["fire","hot","flame","lit","trending"],
  "⭐": ["star","favorite","rating","shiny"],
  "🌟": ["glowing","star","sparkle","shine"],
  "✨": ["sparkles","magic","shine","glitter"],
  "🎉": ["party","celebrate","confetti","fun"],
  "🎊": ["confetti","celebrate","party","fun"],
  "🙏": ["pray","thanks","please","hands","hope"],
  "👏": ["clap","applause","bravo","hands"],
  "💪": ["muscle","strong","flex","power","arm"],
  "🤝": ["handshake","deal","agree","greet"],
  "👀": ["eyes","look","see","watch","stare"],
  "💬": ["speech","chat","talk","bubble","message"],
  "💭": ["thought","think","bubble","mind"],
  "❓": ["question","ask","doubt","unknown"],
  "❗": ["exclamation","important","alert","warning"],
  "✅": ["check","done","correct","yes","tick"],
  "❌": ["cross","no","wrong","incorrect","cancel"],
  "🚫": ["no","forbidden","stop","blocked","ban"],
  "⚠️": ["warning","caution","alert","danger"],
  "🏳️": ["white","flag","surrender"],
  "🏴": ["black","flag","pirate"],
  "🚩": ["red","flag","warning","stop"],
};

const searchEmojis = (query: string): string[] => {
  if (!query.trim()) return [];
  const q = query.toLowerCase().trim();
  const all = Object.values(EMOJI_DATA).flat();
  const unique = [...new Set(all)];

  return unique.filter((emoji) => {
    // Match by emoji character directly
    if (emoji.includes(q)) return true;
    // Match by name/keyword
    const names = EMOJI_NAMES[emoji];
    if (names) return names.some((n) => n.includes(q));
    return false;
  });
};

export default function EmojiPicker({ onEmojiSelect }: EmojiPickerProps) {
  const [search, setSearch] = useState<string>("");
  const [activeCategory, setActiveCategory] = useState<string>("recent");
  const [recentEmojis, setRecentEmojis] = useState<string[]>(EMOJI_DATA.recent);
  const [hoveredEmoji, setHoveredEmoji] = useState<string | null>(null);
  const scrollRef = useRef<HTMLDivElement>(null);

  const filteredEmojis = useMemo<string[] | null>(() => {
    if (!search.trim()) return null;
    return searchEmojis(search);
  }, [search]);

  const handleSelect = (emoji: string): void => {
    setRecentEmojis((prev) => {
      const filtered = prev.filter((e) => e !== emoji);
      return [emoji, ...filtered].slice(0, 20);
    });
    onEmojiSelect?.(emoji);
  };

  const handleCategoryChange = (id: string): void => {
    setActiveCategory(id);
    setSearch("");
    scrollRef.current?.scrollTo({ top: 0 });
  };

  const displayEmojis: string[] =
    filteredEmojis ??
    (activeCategory === "recent" ? recentEmojis : EMOJI_DATA[activeCategory] ?? []);

  const activeLabel =
    CATEGORIES.find((c) => c.id === activeCategory)?.label ?? "Smileys & People";

  return (
  <div className={styles.container}>

      {/* Search */}
      <div className={styles.searchWrapper}>
        <div className={styles.searchBox}>
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none"
            stroke="#8e8e93" strokeWidth="2.5" strokeLinecap="round" strokeLinejoin="round">
            <circle cx="11" cy="11" r="8" />
            <path d="m21 21-4.35-4.35" />
          </svg>
          <input
            className={styles.searchInput}
            value={search}
            onChange={(e: React.ChangeEvent<HTMLInputElement>) => setSearch(e.target.value)}
            placeholder="Search"
          />
          {search && (
            <button className={styles.searchClear} onClick={() => setSearch("")}>
              ✕
            </button>
          )}
        </div>
      </div>

      {/* Category Tabs */}
      <div className={styles.tabs}>
        {CATEGORIES.map((cat) => {
          const isActive = activeCategory === cat.id && !search;
          return (
            <button
              key={cat.id}
              title={cat.label}
              onClick={() => handleCategoryChange(cat.id)}
              className={[
                styles.tab,
                cat.id === "recent" ? styles.recentTab : "",
                isActive ? styles.active : "",
              ].join(" ")}
            >
              {cat.id === "recent" ? (
                <svg width="18" height="18" viewBox="0 0 24 24" fill="none"
                  stroke={isActive ? "#007AFF" : "#555"}
                  strokeWidth="2" strokeLinecap="round" strokeLinejoin="round">
                  <circle cx="12" cy="12" r="10" />
                  <polyline points="12 6 12 12 16 14" />
                </svg>
              ) : (
                cat.icon
              )}
            </button>
          );
        })}
      </div>

      {/* Section Label */}
      <div className={styles.sectionLabel}>
        {search
          ? "Search results"
          : activeCategory === "recent"
          ? "Frequently Used"
          : activeLabel}
      </div>

      {/* Emoji Grid */}
      <div className={styles.gridScroll} ref={scrollRef}>
        {displayEmojis.length === 0 ? (
          <div className={styles.emptyState}>No emoji found</div>
        ) : (
          <div className={styles.grid}>
            {displayEmojis.map((emoji, i) => (
              <button
                key={`${emoji}-${i}`}
                className={styles.emojiBtn}
                onClick={() => handleSelect(emoji)}
                title={emoji}
              >
                {emoji}
              </button>
            ))}
          </div>
        )}
      </div>

    </div>
  );
}
