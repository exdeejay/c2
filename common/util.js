function forwardEvents(eventNames, from, to) {
    for (let eventName of eventNames) {
        from.on(eventName, function () {
            to.emit(eventName, ...arguments);
        });
    }
}

exports.forwardEvents = forwardEvents;
