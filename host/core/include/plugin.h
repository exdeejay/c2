#ifndef PLUGIN_H
#define PLUGIN_H

class Controller;

class Plugin {
public:
    virtual ~Plugin() {}
    virtual void init(Controller& ctrl) {}
    virtual void destroy(Controller& ctrl) {}
};

#endif