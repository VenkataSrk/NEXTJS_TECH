// nlp_stt.ts
type Subscriber = (data: any) => void;
export default class NlpWebSocketService {
  private static instance: NlpWebSocketService;
  private socket: WebSocket | null = null;
  private subscribers: Subscriber[] = [];
  private constructor() { } // private to enforce singleton
  /** Get singleton instance */
  public static getInstance(): NlpWebSocketService {
    if (!NlpWebSocketService.instance) {
      NlpWebSocketService.instance = new NlpWebSocketService();
    }
    return NlpWebSocketService.instance;
  }
  /** Initialize socket */
  public connect(url: string, onOpenCallback?: () => void) {
    try {
      if (
        !this.socket ||
        this.socket.readyState === WebSocket.CLOSED ||
        this.socket.readyState === WebSocket.CLOSING
      ) {
        this.socket = new WebSocket(url);
      }
      this.socket.onopen = () => onOpenCallback?.();
      this.socket.onerror = (err) => console.error("WebSocket Error:", err);
      this.socket.onclose = (e) => console.log("WebSocket Disconnected", e);
      this.socket.onmessage = (event) => {
        try {
          console.log("event--->", event?.data)
          console.log("event---> typeof", typeof event?.data)
          if (!event.data) return;
          const data = JSON.parse(event.data);
          this.subscribers.forEach((cb) => cb(data));
        } catch (err) {
          console.error("Error parsing message:",err, event.data);
        }
      };
    } catch (error) {
      console.log("connect error",error)
    }
  }
  /** Subscribe to messages */
  public subscribe(callback: Subscriber) {
    this.subscribers.push(callback);
    return () => {
      this.subscribers = this.subscribers.filter((cb) => cb !== callback);
    };
  }
  /** Send message */
  public sendMessage(data: any) {
    if (this.socket && this.socket.readyState === WebSocket.OPEN) {
      console.log('sendMessage====>>>>', data)
      this.socket.send(data);
    } else {
      console.warn("⚠️ WebSocket not connected");
    }
  }
  /** Check connection */
  public isConnected() {
    return this.socket?.readyState === WebSocket.OPEN;
  }
  /** Close connection */
  public closeConnection() {
    this.socket?.close();
    this.socket = null;
  }
}
