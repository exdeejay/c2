import { EventEmitter } from "stream";

export function forwardEvents(eventNames: string[], from: EventEmitter, to: EventEmitter, ...args: any[]) {
    for (let eventName of eventNames) {
        from.on(eventName, function () {
            to.emit(eventName, ...args);
        });
    }
}
