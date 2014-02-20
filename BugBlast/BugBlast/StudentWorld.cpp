#include "StudentWorld.h"

GameWorld* createStudentWorld()
{
	return new StudentWorld();
}

StudentWorld::StudentWorld()
{
	actors.clear();
	numZumis = 0;
}

StudentWorld::~StudentWorld()
{
	for (int i = 0; i < actors.size(); i++)
		delete actors[i];
}

int StudentWorld::init()
{
	//Initialize the data structures used to keep track of your game’s world
	actors.clear();
	numZumis = 0;

	//Load the current maze details from a level data file (each level has its own data 
	//file that specifies what the maze looks like for that level, as well as all the game 
	//parameters for the level – such as what the bonus is for completing the level 
	//quickly).
	Level lev;

	//If the current level is 0 and the first level data file level00.dat can not be 
	//found, immediately return GWSTATUS_NO_FIRST_LEVEL. 
	Level::LoadResult result = lev.loadLevel(getLevelFile(getLevel()));
	if	(getLevel() == 0 && (result == Level::load_fail_file_not_found))	
		return GWSTATUS_NO_FIRST_LEVEL;

	//If the current level is not 0 and the level data file for the current level can 
	//not be found, this means the Player has completed all levels and won, so 
	//immediately return GWSTATUS_PLAYER_WON. 
	if	(getLevel() != 0 && (result == Level::load_fail_file_not_found))	
		return GWSTATUS_PLAYER_WON;

	//If the level data file exists but is improperly formatted, immediately return 
	//GWSTATUS_LEVEL_ERROR.
	if (result != Level::load_fail_file_not_found && result == Level::load_fail_bad_format)
		return GWSTATUS_LEVEL_ERROR;

	ProbOfGoodieOverall = lev.getOptionValue(optionProbOfGoodieOverall);
	ProbOfExtraLifeGoodie = lev.getOptionValue(optionProbOfExtraLifeGoodie);
	ProbOfWalkThruGoodie = lev.getOptionValue(optionProbOfWalkThruGoodie);
	ProbOfMoreSprayersGoodie = lev.getOptionValue(optionProbOfMoreSprayersGoodie);
	TicksPerSimpleZumiMove = lev.getOptionValue(optionTicksPerSimpleZumiMove);
	TicksPerComplexZumiMove = lev.getOptionValue(optionTicksPerComplexZumiMove);
	GoodieLifetimeInTicks = lev.getOptionValue(optionGoodieLifetimeInTicks);
	LevelBonus = lev.getOptionValue(optionLevelBonus);
	WalkThruLifetimeTicks = lev.getOptionValue(optionWalkThruLifetimeTicks);
	BoostedSprayerLifetimeTicks = lev.getOptionValue(optionBoostedSprayerLifetimeTicks);
	MaxBoostedSprayers = lev.getOptionValue(optionMaxBoostedSprayers);


	//Allocate and insert a valid Player object into the game world.
	//Allocate and insert any other Simple and Complex Zumi objects, Brick objects, or 
	//Exit objects into the game world, as required by the specification in the current 
	//level’s data file. 
	for (int x = 0; x < 15; x++)
	{
		for (int y = 0; y < 15; y++)
		{
			switch (lev.getContentsOf(x, y))
			{
			case 0: // empty
				break;
			case 1: // exit
				actors.push_back(new Exit(this, x, y));
				break;
			case 2: // player
				actors.push_back(new Player(this, x, y));
				break;
			case 3: // simple_zumi
				// to add code here
				break;
			case 4: // complex_zumi
				// to add code here
				break;
			case 5: // perma_brick
				actors.push_back(new PermaBrick(this, x, y));
				break;
			case 6: // destroyable_brick
				actors.push_back(new DestructBrick(this, x, y));
				break;
			}
		}
	}

	//Return GWSTATUS_CONTINUE_GAME.
	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
	//It must ask all of the actors that are currently active in the game world to do 
	//something (e.g., ask a Simple Zumi to move itself, ask a Bug Sprayer to count 
	//down and possibly release Bug Spray into the maze, give the Player a chance to 
	//move up, down, left or right, or drop a Bug Sprayer in the maze, etc.). 
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->doSomething();

		//If an actor does something that causes the Player to die, then the move() 
		//method should immediately return GWSTATUS_PLAYER_DIED.
		if (!actors[i]->isAlive() && actors[i]->isPlayer())
			return GWSTATUS_PLAYER_DIED;
	
		//If the Player steps onto the same square as an Exit (after first clearing the 
		//level of all Zumi) and completes the current level, then the move() method 
		//should immediately: 


			//Increase the Player’s score by the remaining bonus for the level. 

			//Return a value of GWSTATUS_FINISHED_LEVEL. 
	}
	//It must then delete any actors that have died during this tick (e.g., a Simple Zumi 
	//that was destroyed by Bug Spray and so should be removed from the game world, 
	//or a Goodie that disappeared because the Player picked it up). 
	for (int i = 0; i < actors.size(); i++)
	{
		if (!actors[i]->isAlive())
		{
			delete actors[i];
			if (actors[i]->getID == IID_SIMPLE_ZUMI || actors[i]->getID == IID_COMPLEX_ZUMI)
			{
				numZumis--;
			}
		}
	}

	//It must reduce the level’s bonus points by one during each tick. So, for example, 
	//if level #0 has a starting bonus of 1000, then after the first tick, the bonus would 
	//drop to 999. On the second tick it would drop to 998, etc. This declining bonus 
	//value incentivizes the Player to complete the level as quickly as possible to get the 
	//biggest bonus score. The level bonus may not go below a value of zero. 
	LevelBonus--;
		
	//It must update the status text on the top of the screen with the latest information 
	//(e.g., the user’s updated score, the remaining bonus score for the level, etc.). 
	setGameStatText("Score: " + std::to_string(getScore()) + 
		" Level: " + std::to_string(getLevel()) + 
		" Lives: " + std::to_string(getLives()) + 
		" Bonus: " + std::to_string(LevelBonus));

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
	//Every actor in the entire maze (the Player and every Zumi, Goodies, Bug Sprayers, Bug 
	//Spray, Bricks, the Exit, etc.) must be deleted and removed from your StudentWorld’s 
	//container of active objects, resulting in an empty maze.
}

int StudentWorld::getNumZumis() const
{
	return numZumis;
}

std::string StudentWorld::getLevelFile(unsigned int num)
{
	switch (num)
	{
	case 0:
		return "level00.dat";
	case 1:
		return "level01.dat";
	case 2:
		return "level02.dat";
	case 3:
		return "level03.dat";
	case 4:
		return "level04.dat";
	case 5:
		return "level05.dat";
	case 6:
		return "level06.dat";
	case 7:
		return "level07.dat";
	case 8:
		return "level08.dat";
	case 9:
		return "level09.dat";
	case 10:
		return "level10.dat";
	default:
		return "";
	}
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp