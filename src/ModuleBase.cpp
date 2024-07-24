#include "ModuleBase.h"

ModuleBase::ModuleBase(const __FlashStringHelper *name)
{
    this->Name = name;
}

void ModuleBase::Initialize(byte &incomingMessageHandlerCapacity)
{
    this->_connected = this->_connect();

    if(this->_connected == false)
    {
        return;
    }

    incomingMessageHandlerCapacity += this->_alloc();
}

void ModuleBase::Register(Simpit *simpit)
{
    if(this->_connected == false)
    {
        return;
    } 

    this->_register(simpit);
}

void ModuleBase::Subscribe(Simpit *simpit)
{
    if(this->_connected == false)
    {
        return;
    } 

    this->_subscribe(simpit);
}

void ModuleBase::Unsubscribe(Simpit *simpit)
{
    if(this->_connected == false)
    {
        return;
    } 

    this->_unsubscribe(simpit);
}

void ModuleBase::Update(Simpit *simpit)
{
    if(this->_connected == false)
    {
        return;
    } 

    this->_update(simpit);
}

bool ModuleBase::IsConnected()
{
    return this->_connected;
}