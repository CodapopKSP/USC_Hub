#ifndef ModuleBase_h
#define ModuleBase_h

#include <KerbalSimpitMessageTypes.h>
#include <Simpit.h>

class ModuleBase
{
private:
    int _address;

public:
    ModuleBase(int address);

    virtual void HandleSceneChanged(Simpit *simpit, Environment::Incoming::SceneChange *data) = 0;
};

#endif