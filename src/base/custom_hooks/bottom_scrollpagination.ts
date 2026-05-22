import { useEffect, useRef, useCallback } from "react";

const useBottomScrollPagination = ({ onLoadMore, hasMore, delay = 500 }) => {
  const containerRef = useRef(null);
  const debounceTimer = useRef<any>(null);
  const isLoadingRef = useRef(false);

  const handleLoadMore = useCallback(() => {
    try {
      if (isLoadingRef.current || !hasMore) return;

      if (debounceTimer.current) {
        clearTimeout(debounceTimer.current);
      }

      debounceTimer.current = setTimeout(async () => {
        try {
          isLoadingRef.current = true;

          await onLoadMore();

          isLoadingRef.current = false;
        } catch (err) {
          console.error("Load more error:", err);
          isLoadingRef.current = false;
        }
      }, delay);
    } catch (err) {
      console.error("Debounce error:", err);
    }
  }, [onLoadMore, hasMore, delay]);

  useEffect(() => {
    let observer;

    try {
      if (!containerRef.current) return;

      observer = new IntersectionObserver(
        (entries) => {
          try {
            const target = entries[0];

            if (target.isIntersecting) {
              handleLoadMore();
            }
          } catch (err) {
            console.error("Observer callback error:", err);
          }
        },
        {
          root: null,
          rootMargin: "100px", // prefetch before reaching bottom
          threshold: 0.1,
        }
      );

      observer.observe(containerRef.current);
    } catch (err) {
      console.error("Observer setup error:", err);
    }

    return () => {
      try {
        if (observer && containerRef.current) {
          observer.unobserve(containerRef.current);
        }

        if (debounceTimer.current) {
          clearTimeout(debounceTimer.current);
        }
      } catch (err) {
        console.error("Cleanup error:", err);
      }
    };
  }, [handleLoadMore]);

  return containerRef;
};

export default useBottomScrollPagination;