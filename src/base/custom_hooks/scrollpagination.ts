export function attachScrollPagination(elementOrRef: any, apiCall: any) {
    let container: HTMLElement | null = null;
  
    if (typeof elementOrRef === 'string') {
      container = document.getElementById(elementOrRef);
    } else if (elementOrRef?.current) {
      container = elementOrRef.current;
    } else {
      container = elementOrRef;
    }
  
    if (!container) {
      console.warn('Container not found');
      return () => { };
    }
  
      let isLoading = false;
      let lastScrollTop = 0;
      let triggered = false;
    
      const onScroll = async () => {
        const { scrollTop, scrollHeight, clientHeight } = container!;
        // if (scrollTop <= lastScrollTop) return;
        // lastScrollTop = scrollTop;
    
        if (
          scrollHeight - scrollTop - clientHeight < 100 &&
          !isLoading &&
          !triggered
        ) {
          triggered = true;
          isLoading = true;
          try {
            await apiCall();
          } finally {
            isLoading = false;
            triggered = false;
          }
        }
      };
    
      container.addEventListener('scroll', onScroll);  
    
      return () => {
        container?.removeEventListener('scroll', onScroll);
      };
    }
  