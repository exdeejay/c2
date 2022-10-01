import { EventEmitter } from "stream";

export function forwardEvents(eventNames: string[], from: EventEmitter, to: EventEmitter) {
    for (let eventName of eventNames) {
        from.on(eventName, function (...args: any[]) {
            to.emit(eventName, ...args);
        });
    }
}
