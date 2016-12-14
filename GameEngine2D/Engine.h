#ifndef ENGINE_H
#define ENGINE_H

//C Runtime Header Files
#ifndef _WINDOWS_
#include <Windows.h>
#endif // !_WINDOWS_

#ifndef _MAP_
#include <map>
#endif // !_MAP_

#ifndef _CONTEXT_H
#include "context.h"
#endif // !_CONTEXT_H

//Forware Declaration
enum SystemType;

class System;
class Game;

//Enumeration
enum EngineState
{
	Invaild,
	Constructing,
	Initializing,
	Running,
	ShuttingDown,
	Destroying
};

class Engine
{
public:
	Engine();
	~Engine();

	int RunLoop();

	////Memory manager
	//void* operator new(size_t size);		//重载new操作符
	//void operator delete(void* pdelete);	//重载delete操作符

	static EngineState GetEngineState() { return m_EngineState; }

private:

	int Intialize();	//返回值为int是如果发生错误可以返回error state
	int Draw(const Context& context);
	int Update(const Context& context);
	int ShutDown();

	//Add a core system to the engine
	int AddSystem(System* psys);
	
	//Retrieve a core system from the engine
	template<typename T>
	T* GetSystem(SystemType systype)
	{
		T* psys = static_cast<T*>(m_mapSystems[systype]);
		if (!psys)
		{
			//Logger::Log("System is not valid!");
			return nullptr;
		}
		return psys;
	}

	//Create the game instance
	Game* CreateGame();

	std::map<SystemType, System*> m_mapSystems;
	static EngineState m_EngineState;
};

#endif // !ENGINE_H



