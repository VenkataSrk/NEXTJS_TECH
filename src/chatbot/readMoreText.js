import React, { useState, useEffect } from 'react';
import parse from "html-react-parser";

const ReadMore = ({ text }) => {
  const [showFullText, setShowFullText] = useState(true);

  const displayText =
    text?.$$typeof === Symbol.for('react.element') ? text.props.children : text;

  function urlify(text) {
    if (text !== undefined) {
      const urlRegex = /(https?:\/\/[^\s<]+)/g;
      return text.replace(urlRegex, (url) =>
        `<a href="${url}" target="_blank" rel="noopener noreferrer" >${url}</a>`
      );
    }
    return text;
  }

  const stringText = typeof displayText === 'string' ? displayText : '';
  const remainingText = stringText.slice(300).length > 50;
  const truncatedText =
    stringText.length > 300 && remainingText
      ? stringText.slice(0, 300) + '...'
      : stringText;

  useEffect(() => {
    if (displayText.includes('<table')) {
      setShowFullText(true);
    }
  }, []);

  const contentToRender = (showFullText ? stringText : truncatedText)
  .replace(
    /<a\s+(?![^>]*target=)[^>]*href=/g,
    '<a target="_blank" rel="noopener noreferrer" href='
  )

  const finalContent = displayText.includes('<a') ? contentToRender : urlify(contentToRender);  
  return <p dangerouslySetInnerHTML={{__html: finalContent}} />
};

export default ReadMore;
