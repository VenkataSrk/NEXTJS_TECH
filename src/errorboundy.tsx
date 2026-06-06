import { Component } from "react";

class ErrorBoundary extends Component<{ children: React.ReactNode }> {
  state = { hasError: false };

  static getDerivedStateFromError() {
    return { hasError: true };
  }

  componentDidCatch(error: any, errorInfo: any) {
    console.error("Error:", error, errorInfo);
    // Reload the page on error
    window.location.reload();
  }

  render() {
    if (this.state.hasError) {
      return null; // Or show a simple loading or error message
    }

    return this.props.children;
  }
}

export default ErrorBoundary;
