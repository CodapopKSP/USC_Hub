#ifndef ModuleBase_h
#define ModuleBase_h

#include <KerbalSimpitRefactoredMessageTypes.h>
#include <SimpitRefactored.h>

class ModuleBase
{
private:
    bool _connected;

protected:
    /// @brief Test module connection over wire
    /// @return 
    virtual bool _connect() const = 0;

    /// @brief Allocate the total number of unique incoming simpit message types 
    /// this module can possibly recieve.
    /// @return 
    virtual byte _alloc() const = 0;

    /// @brief Register all allocated incoming message subscribers. Called once after communication with KSP is established.
    /// @param simpit 
    virtual void _register(Simpit *simpit) = 0;

    /// @brief Subscribe to any required incoming message.
    /// This method is called automatically when the flight scene is activated.
    /// @param simpit 
    virtual void _subscribe(Simpit *simpit) = 0;

    /// @brief Unsubscribe to any required incoming message
    /// This method is called automatically when the flight scene is deactivated.
    /// @param simpit 
    virtual void _unsubscribe(Simpit *simpit) = 0;
    
    virtual void _update(Simpit *simpit) = 0;

public:
    const __FlashStringHelper *Name;

    ModuleBase(const __FlashStringHelper *name);

    void Initialize(byte &incomingMessageHandlerCapacity);

    void Register(Simpit *simpit);

    void Subscribe(Simpit *simpit);

    void Unsubscribe(Simpit *simpit);

    void Update(Simpit *simpit);

    bool IsConnected();
};

#endif