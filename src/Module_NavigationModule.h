#ifndef Module_NavigationModule_h
#define Module_NavigationModule_h

#include <SimpitRefactored.h>
#include "ModuleBase.h"

enum struct NavigationModuleFlags : byte
{
    None = 0,
    MapReturn = 1 << 0,
    CycleShipMinus = 1 << 1,
    CycleMapMinus = 1 << 2,
    CycleShipPlus = 1 << 3,
    CycleMapPlus = 1 << 4,
    CycleNav = 1 << 5,
    Map = 1 << 6,
    Nav = 1 << 7
};

class NavigationModule : public ModuleBase
{
    private:
        NavigationModuleFlags flags;

        bool navigation_navball_on;
        bool navigation_map_on;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        NavigationModule();
};
#endif