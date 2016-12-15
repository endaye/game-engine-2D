#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "UObject.h"

//Addisional include files
#ifndef _CONTEXT_H
#include "context.h"
#endif // !_CONTEXT_H

//Enumeration
enum SystemType
{
	Sys_Invalid,
	Sys_Window,
	Sys_Game,
	Sys_Input,
	Sys_Graphics,
	Sys_GameTimer,

	Sys_MAX
};

//Structs
struct SystemData
{
	SystemData();
	SystemData(const SystemType& type);

	SystemType systemType;
};

class System : public UObject
{
	friend class Engine;
public:
	SystemType GetType() const { return m_SystemType; }
	
protected:
	System(const SystemData& data);
	virtual ~System();

	virtual bool Initialize() { return UObject::Initialize(); }
	virtual bool Update(Context& context)	{ return UObject::Update(context); }
	virtual bool ShutDown()					{ return UObject::ShutDown(); }

	SystemType GetType() { return m_SystemType; }

protected:
	SystemType m_SystemType;

};

#endif // !_SYSTEM_H