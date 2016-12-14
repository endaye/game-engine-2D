#ifndef _GAME_H
#define _GAME_H

#include "System.h"

//Structs
struct GameData : public SystemData
{
	
};

class Game : public System
{
public:
	Game(const GameData& data);
	virtual ~Game();
};

#endif // !_GAME_H

