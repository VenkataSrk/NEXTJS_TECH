// components/EmblaCarousel.tsx
import React, { useCallback, useEffect, useState } from 'react';
import useEmblaCarousel from 'embla-carousel-react';
import type { EmblaCarouselType } from 'embla-carousel';


import './scss/worktualcarousel.scss';

interface EmblaCarouselProps {
  slides: React.ReactNode[];  // Accepts JSX content
  slidesPerView?: number;     // Optional number of slides shown at once (default 1)
}

const WorktualCarousel: React.FC<EmblaCarouselProps> = ({ slides, slidesPerView = 1 }) => {
  const [emblaRef, emblaApi] = useEmblaCarousel({
    loop: false,
    align: 'start',
    slidesToScroll: 1,
  });

  const [selectedIndex, setSelectedIndex] = useState(0);
  const [scrollSnaps, setScrollSnaps] = useState<number[]>([]);
  const [prevDisabled, setPrevDisabled] = useState(true);
  const [nextDisabled, setNextDisabled] = useState(true);


const onSelect = useCallback((embla: EmblaCarouselType) => {
  setSelectedIndex(embla.selectedScrollSnap());
  setPrevDisabled(!embla.canScrollPrev());
  setNextDisabled(!embla.canScrollNext());
}, []);



  useEffect(() => {
    if (!emblaApi) return;
    setScrollSnaps(emblaApi.scrollSnapList());
    onSelect(emblaApi);
    emblaApi.on('select', () => onSelect(emblaApi));
    emblaApi.on('reInit', () => onSelect(emblaApi));
  }, [emblaApi, onSelect]);

  const scrollPrev = () => emblaApi?.scrollPrev();
  const scrollNext = () => emblaApi?.scrollNext();
  const scrollTo = (index: number) => emblaApi?.scrollTo(index);

  return (
    <div className="embla">
      <div className="embla__viewport" ref={emblaRef}>
        <div className="embla__container" style={{ gap: '1rem' }}>
          {slides.map((slide, index) => (
            <div
              className="embla__slide"
              key={index}
              style={{ flex: `0 0 ${100 / slidesPerView}%` }} // Responsive width
            >
              <div className="embla__slide__inner">{slide}</div>
            </div>
          ))}
        </div>
      </div>

      {/* Navigation Buttons */}
      <button
        className="embla__button embla__button--prev"
        onClick={scrollPrev}
        disabled={prevDisabled}
      >
        ◀
      </button>
      <button
        className="embla__button embla__button--next"
        onClick={scrollNext}
        disabled={nextDisabled}
      >
        ▶
      </button>

      {/* Dots */}
      <div className="embla__dots">
        {scrollSnaps.map((_, index) => (
          <button
            key={index}
            onClick={() => scrollTo(index)}
            className={`embla__dot${index === selectedIndex ? ' is-selected' : ''}`}
          />
        ))}
      </div>
    </div>
  );
};

export default WorktualCarousel;
