#ifndef Modules_h
#define Modules_h

#include <Simpit.h>
#include <SimpitBuilder.h>

class Modules
{
private:
    static void MessageTypeCapacity(byte &incoming);
    static void RegisterMessageTypes(SimpitBuilder *builder);
    static void InitSimpit(Simpit *simpit);

public:
    static Simpit* BuildSimpit(Stream &serial);
    static void Update(Simpit *simpit);
};

#endif