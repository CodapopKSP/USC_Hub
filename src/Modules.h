#ifndef Modules_h
#define Modules_h

#include <Simpit.h>
#include <KerbalSimpitMessageTypes.h>

class Modules
{
private:
    static void HandleSceneChanged(void *sender, Environment::Incoming::SceneChange *data);

public:
    static Simpit* Initialize(Stream &serial);
    static void Update(Simpit *simpit);
};

#endif