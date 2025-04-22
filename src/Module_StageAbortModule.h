#ifndef Module_StageAbortModule_h
#define Module_StageAbortModule_h

#include <SimpitRefactored.h>
#include <KerbalSimpitRefactoredMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct StageAbortModuleFlags : byte
{
    None = 0,
    Stage = 1 << 0,
    Abort = 1 << 1,
};

class StageAbortModule : public ModuleBase
{
    private:
        StageAbortModuleFlags flags;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        StageAbortModule();
};
#endif