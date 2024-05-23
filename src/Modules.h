#ifndef Modules_h
#define Modules_h

#include <Simpit.h>
#include <SimpitBuilder.h>

class Modules
{
private:
    static void SimpitAlloc(byte &incoming);
    static void SimpitRegister(SimpitBuilder *builder);
    static void SimpitInit(Simpit *simpit);

public:
    static Simpit* BuildSimpit(Stream &serial);
    static void Update(Simpit *simpit);
};

#endif