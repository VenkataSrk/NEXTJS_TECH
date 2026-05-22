import { useEffect, useRef, useState } from "react";
import styles from "./scss/composemail.module.scss";
import { generateAIMessage } from "../../base/service/nlp_service";
import Aicon from "../../assets/images/inbox/aiicon.svg";
import Formal from "../../assets/images/rephraseicon/formal.svg";
import Friendly from "../../assets/images/rephraseicon/friendly.svg";
import Concise from "../../assets/images/rephraseicon/concise.svg";
import Elaborate from "../../assets/images/rephraseicon/elaborate.svg";
import Tick from "../../assets/images/inbox/tick.svg";
import Undo from "../../assets/images/inbox/undo.svg";
import Retry from "../../assets/images/inbox/retry.svg";

type Tone = "Formal" | "Friendly" | "Concise" | "Elaborate";

type RephraseOp = {
  originalText: string;
  originalFragment: DocumentFragment;
  insertedNode: HTMLElement;
  tone: string;
};

interface AIRephraseTooltipProps {
  editorElementId: string;
  onContentChange?: (html: string) => void;
}

const TONE_OPTIONS: { label: Tone; icon: string }[] = [
  { label: "Formal", icon: Formal },
  { label: "Friendly", icon: Friendly },
  { label: "Concise", icon: Concise },
  { label: "Elaborate", icon: Elaborate },
];

const AIRephraseTooltip = ({ editorElementId, onContentChange }: AIRephraseTooltipProps) => {
  const [tooltip, setTooltip] = useState({ show: false, top: 0, left: 0 });
  const [mode, setMode] = useState<"button" | "list" | "result">("button");
  const [selectedType, setSelectedType] = useState("");
  const [isRephrasing, setIsRephrasing] = useState(false);

  const tooltipRef = useRef<HTMLDivElement>(null);
  const originalSelectionRef = useRef<{ text: string; range: Range | null }>({ text: "", range: null });
  const rephraseHistoryRef = useRef<RephraseOp[]>([]);
  const selectionAtMouseDownRef = useRef("");

  const getEditor = () => document.getElementById(editorElementId);

  const syncEditorChange = () => {
    const editor = getEditor();
    if (editor && onContentChange) onContentChange(editor.innerHTML);
  };

  const buildRephraseSpan = (html: string) => {
    const span = document.createElement("span");
    span.setAttribute("data-ai-rephrased", "true");
    span.innerHTML = html;
    return span;
  };

  const closeTooltip = () => {
    setTooltip({ show: false, top: 0, left: 0 });
    setMode("button");
    setSelectedType("");
    originalSelectionRef.current = { text: "", range: null };
  };

  const handleMouseDown = (e: MouseEvent) => {
    selectionAtMouseDownRef.current = window.getSelection()?.toString() ?? "";

    if (tooltipRef.current && tooltipRef.current.contains(e.target as Node)) return;
    const editor = getEditor();
    if (editor && editor.contains(e.target as Node)) return;
    setTooltip({ show: false, top: 0, left: 0 });
  };

  const handleMouseUp = (e: MouseEvent) => {
    if (tooltipRef.current && tooltipRef.current.contains(e.target as Node)) return;

    const selection = window.getSelection();
    const currentText = selection?.toString() ?? "";
    const beforeText = selectionAtMouseDownRef.current;

    if (currentText.trim() === "" || currentText === beforeText) {
      setTooltip((prev) => ({ ...prev, show: false }));
      return;
    }

    const range = selection!.getRangeAt(0);
    const editor = getEditor();
    if (
      !editor ||
      !editor.contains(range.startContainer) ||
      !editor.contains(range.endContainer)
    ) {
      return;
    }

    const rect = range.getBoundingClientRect();

    originalSelectionRef.current = {
      text: currentText,
      range: range.cloneRange(),
    };
    setMode("button");
    setSelectedType("");
    setTooltip({
      show: true,
      top: rect.top - 60,
      left: rect.left + rect.width / 2,
    });
  };

  useEffect(() => {
    window.addEventListener("mouseup", handleMouseUp);
    window.addEventListener("mousedown", handleMouseDown);
    return () => {
      window.removeEventListener("mouseup", handleMouseUp);
      window.removeEventListener("mousedown", handleMouseDown);
    };
  }, []);

  const rephraseSelection = async (tone: string) => {
    const original = originalSelectionRef.current;
    if (!original?.range || !original?.text) return;

    setIsRephrasing(true);
    try {
      const response: any = await generateAIMessage({ text: original.text, tone });
      const aiText = typeof response === "string" ? response : (response?.body ?? "");
      if (!aiText) return;

      const originalFragment = original.range.cloneContents();
      const span = buildRephraseSpan(aiText);
      original.range.deleteContents();
      original.range.insertNode(span);

      rephraseHistoryRef.current.push({
        originalText: original.text,
        originalFragment,
        insertedNode: span,
        tone,
      });

      const newRange = document.createRange();
      newRange.selectNodeContents(span);
      originalSelectionRef.current = {
        text: span.textContent || "",
        range: newRange,
      };

      syncEditorChange();
    } catch (err) {
      console.error(err);
    } finally {
      setIsRephrasing(false);
    }
  };

  const handleSelectTone = (tone: string) => {
    setSelectedType(tone);
    setMode("result");
    rephraseSelection(tone);
  };

  const handleConfirmRephrase = () => {
    rephraseHistoryRef.current = [];
    closeTooltip();
  };

  const handleUndoRephrase = () => {
    const stack = rephraseHistoryRef.current;
    const op = stack.pop();
    if (op && op.insertedNode.parentNode) {
      const parent = op.insertedNode.parentNode;
      const restored = op.originalFragment.cloneNode(true) as DocumentFragment;
      const firstChild = restored.firstChild;
      const lastChild = restored.lastChild;
      parent.replaceChild(restored, op.insertedNode);
      syncEditorChange();

      if (firstChild && lastChild) {
        const restoredRange = document.createRange();
        restoredRange.setStartBefore(firstChild);
        restoredRange.setEndAfter(lastChild);
        originalSelectionRef.current = {
          text: op.originalText,
          range: restoredRange,
        };
      }
    }

    if (stack.length === 0) {
      // closeTooltip();
    } else {
      const prev = stack[stack.length - 1];
      const newRange = document.createRange();
      newRange.selectNodeContents(prev.insertedNode);
      originalSelectionRef.current = {
        text: prev.insertedNode.textContent || "",
        range: newRange,
      };
      setSelectedType(prev.tone);
    }
  };

  const handleRetryRephrase = async () => {
    const stack = rephraseHistoryRef.current;
    const top = stack[stack.length - 1];
    if (!top) return;

    setIsRephrasing(true);
    try {
      const response: any = await generateAIMessage({
        text: top.originalText,
        tone: top.tone,
      });
      const aiText = typeof response === "string" ? response : (response?.body ?? "");
      if (!aiText) return;

      const span = buildRephraseSpan(aiText);
      if (top.insertedNode.parentNode) {
        top.insertedNode.parentNode.replaceChild(span, top.insertedNode);
      }
      top.insertedNode = span;

      const newRange = document.createRange();
      newRange.selectNodeContents(span);
      originalSelectionRef.current = {
        text: span.textContent || "",
        range: newRange,
      };

      syncEditorChange();
    } catch (err) {
      console.error(err);
    } finally {
      setIsRephrasing(false);
    }
  };

  if (!tooltip.show) return null;

  return (
    <div
      ref={tooltipRef}
      className={`${styles.methodstooltip} ${mode === "list" ? styles.listshowicon : ""}`}
      style={{
        position: "fixed",
        top: tooltip.top,
        left: tooltip.left,
        transform: "translateX(-50%)",
        padding: "6px 10px",
        zIndex: 9999,
      }}
      onMouseDown={(e) => e.preventDefault()}
      onMouseUp={(e) => e.stopPropagation()}
      onClick={(e) => e.stopPropagation()}
    >
      <div className={styles.mainmenu}>
        {mode === "button" && (
          <div
            className={styles.iconcontent}
            onClick={(e) => {
              e.stopPropagation();
              setMode("list");
            }}
            style={{ cursor: "pointer" }}
          >
            <img src={Aicon} alt="Aicon" /> Rephrase with AI
          </div>
        )}

        {mode === "list" && (
          <div className={styles.rephraselist}>
            {TONE_OPTIONS.map(({ label, icon }) => (
              <div
                key={label}
                className={styles.iconlistphrase}
                onClick={() => handleSelectTone(label)}
              >
                <img src={icon} alt={label} /> {label}
              </div>
            ))}
          </div>
        )}

        {mode === "result" && (
          <div className={styles.resultbox}>
            <div className={styles.resultleft}>
              <div className={styles.iconcontent} style={{ cursor: "pointer" }}>
                <img src={Aicon} alt="Aicon" />
                {isRephrasing ? "Rephrasing..." : selectedType}
              </div>
              <div className={styles.resultright}>
                {!isRephrasing && <img
                  src={Tick}
                  alt="Tick"
                  style={{ cursor: "pointer" }}
                  onClick={handleConfirmRephrase}
                />}
                <button onClick={handleUndoRephrase} disabled={isRephrasing}>
                  <img src={Undo} alt="" /> Undo
                </button>
                <button onClick={handleRetryRephrase} disabled={isRephrasing}>
                  <img src={Retry} alt="" /> Try again
                </button>
              </div>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};

export default AIRephraseTooltip;
