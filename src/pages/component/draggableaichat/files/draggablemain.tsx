import DraggableWindow from "./DraggableWindow";

export default function DraggApp() {


  return (
    <div>
        <DraggableWindow
          initialX={80}
          initialY={80}
          initialWidth={340}
        />
    </div>
    
  );
}
