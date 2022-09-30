#ifndef STDPLUGIN_H
#define STDPLUGIN_H

#include "plugin.h"

class StdPlugin : public Plugin {
public:
    void init(Controller& ctrl) override;
};

#endif