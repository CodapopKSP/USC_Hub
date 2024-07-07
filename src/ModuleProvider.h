#ifndef ModuleProvider_h
#define ModuleProvider_h

#include <Arduino.h>
#include "ModuleBase.h"

class ModuleProvider
{
private:
    byte _count;
    ModuleBase** _modules;

public:
    ModuleProvider();

    void Add(ModuleBase* module);

    void ModuleProvider::HandleSceneChanged(void *sender, Environment::Incoming::SceneChange *data);
};

#endif