#include "ModuleProvider.h"

#include <Simpit.h>

ModuleProvider::ModuleProvider()
{
    _count = 0;
    _modules = nullptr;
}

void ModuleProvider::Add(ModuleBase* module)
{
    ModuleBase** newModules = (ModuleBase**)malloc(sizeof(ModuleBase**));

    if(_count > 0)
    {
        for(int i=0; i<_count; i++)
        {
            newModules[i] = _modules[i];
        }
    }

    if(_modules != nullptr)
    {
        free(_modules);
    }

    _modules = newModules;
    _modules[_count++] = module;
}

void ModuleProvider::HandleSceneChanged(void *sender, Environment::Incoming::SceneChange *data)
{
    for(int i=0; i<_count; i++)
    {
        _modules[i]->HandleSceneChanged((Simpit*)sender, data);
    }
}