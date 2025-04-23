#ifndef Module_Exec_Actions_Groups_h
#define Module_Exec_Actions_Groups_h

#include <SimpitRefactored.h>
#include <KerbalSimpitRefactoredMessageTypes.h>
#include "ModuleBase.h"
#include "IncomingMessageSubscriber.h"

enum struct ExecActionsGroupsModuleFlags : byte
{
    None = 0,
    ag1 = 1 << 0,
    ag2 = 1 << 1,
    ag3 = 1 << 2,
    ag4 = 1 << 3,
    ag5 = 1 << 4,
    Stage = 1 << 5,
    Abort = 1 << 6,   
};

class ExecActionsGroupsModule : public ModuleBase,
    public IncomingMessageSubscriber<Vessel::Incoming::CustomActionGroups>
{
    private:
        uint16_t bits;
        ExecActionsGroupsModuleFlags flags;
        static const byte PROGMEM EXEC_ACTION_GROUP_BITS_MAP[10];
        bool ag1_on;
        bool ag2_on;
        bool ag3_on;
        bool ag4_on;
        bool ag5_on;

    protected:
        virtual bool _connect() const override;
        virtual byte _alloc() const override;
        virtual void _register(Simpit *simpit) override;
        virtual void _subscribe(Simpit *simpit) override;
        virtual void _unsubscribe(Simpit *simpit) override;
        virtual void _update(Simpit *simpit) override;

    public:
        ExecActionsGroupsModule();

        virtual void Process(void *sender, Vessel::Incoming::CustomActionGroups *data) override;
};
#endif