import React, { useRef, useEffect } from 'react';

interface OTPInputProps {
  onChange: (otp: string) => void;
}

const OTPInput: React.FC<OTPInputProps> = ({ onChange }) => {
  const inputRefs = useRef<(HTMLInputElement | null)[]>([]);

  const updateOTP = () => {
    const otp = inputRefs.current.map((input) => input?.value || '').join('');
    onChange(otp);
  };

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>, index: number) => {
    const value = e.target.value.replace(/\D/g, '');
    if (!value) return;

    e.target.value = value[0]; // only 1 digit
    if (index < inputRefs.current.length - 1) {
      inputRefs.current[index + 1]?.focus();
    }

    updateOTP();
  };

const handleKeyDown = (e: React.KeyboardEvent<HTMLInputElement>, index: number) => {
  const isControlKey = e.ctrlKey || e.metaKey; // For Mac Cmd + V

  if (e.key === 'Backspace') {
    if (!e.currentTarget.value && index > 0) {
      inputRefs.current[index - 1]?.focus();
    }
    setTimeout(updateOTP, 0);
    return;
  }

  // Allow navigation keys
  const allowedKeys = ['ArrowLeft', 'ArrowRight', 'Tab'];
  if (allowedKeys.includes(e.key)) return;

  // Allow Ctrl/Cmd + V
  if (isControlKey && e.key.toLowerCase() === 'v') return;

  // Block anything that's not a digit
  if (!/^[0-9]$/.test(e.key)) {
    e.preventDefault();
  }
};


  const handlePaste = (e: React.ClipboardEvent<HTMLInputElement>) => {
    e.preventDefault();
    const pasted = e.clipboardData.getData('text').replace(/\D/g, '');
    if (!pasted) return;

    const digits = pasted.slice(0, 6).split('');

    digits.forEach((digit, i) => {
      const input = inputRefs.current[i];
      if (input) {
        input.value = digit;
      }
    });

    inputRefs.current[digits.length]?.focus();
    updateOTP();
  };

  useEffect(() => {
    // Clear inputs on mount
    inputRefs.current.forEach((input) => {
      if (input) input.value = '';
    });
    updateOTP();
  }, []);

  return (
    <>
      {[...Array(6)].map((_, index) => (
        <input
          key={index}
          type="text"
          maxLength={1}
          inputMode="numeric"
          placeholder="-"
          pattern="[0-9]*"
          ref={(el) => {(inputRefs.current[index] = el)}}
          onChange={(e) => handleChange(e, index)}
          onKeyDown={(e) => handleKeyDown(e, index)}
          onPaste={handlePaste} // 👈 ensure it's on every input
        />
      ))}
    </>
  );
};

export default OTPInput;
