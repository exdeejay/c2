export function forwardEvents(eventNames, from, to, ...args: any[]) {
    for (let eventName of eventNames) {
        from.on(eventName, function () {
            to.emit(eventName, ...args);
        });
    }
}
