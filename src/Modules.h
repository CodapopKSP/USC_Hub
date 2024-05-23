#ifndef Modules_h
#define Modules_h

#include <SimpitBuilder.h>
#include <SimpitAddon.h>
#include <Simpit.h>

class Modules : public SimpitAddon
{
public:
    void Register(SimpitBuilder *builder);
    static void Init(Simpit *simpit);
    static void Update(Simpit *simpit);
};

#endif