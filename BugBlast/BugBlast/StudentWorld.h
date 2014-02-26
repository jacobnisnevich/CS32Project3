#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include "Level.h"
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
	StudentWorld();
	virtual ~StudentWorld();
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	int getNumZumis() const;
	void setNumZumis(int num);
	int getNumSprayers() const {return m_numSprayers;}
	void setNumSprayers(int num) {m_numSprayers = num;}
	int getMaxSprayer() const {return m_maxSprayers;}
	void setMaxSprayer(int num) {m_maxSprayers = num;}
	std::string getLevelFile(unsigned int num);
	std::vector<GameObject*>* getActors();

	int getLevelBonus() {return LevelBonus;}
	int getGoodieProb() {return ProbOfGoodieOverall;}
	int getExtraLifeProb() {return ProbOfExtraLifeGoodie;}
	int getWalkThruProb() {return ProbOfWalkThruGoodie;}
	int getMoreSprayersProb() {return ProbOfMoreSprayersGoodie;}
	int getGoodieLifetime() {return GoodieLifetimeInTicks;}
	int getWalkThruLifetimeTicks() {return WalkThruLifetimeTicks;}
	int getBoostedSprayerLifetimeTicks() {return BoostedSprayerLifetimeTicks;}
private:
	// level options
	int ProbOfGoodieOverall;
	int ProbOfExtraLifeGoodie;
	int ProbOfWalkThruGoodie;
	int ProbOfMoreSprayersGoodie;
	int TicksPerSimpleZumiMove;
	int TicksPerComplexZumiMove;
	int GoodieLifetimeInTicks;
	int LevelBonus;
	int WalkThruLifetimeTicks;
	int BoostedSprayerLifetimeTicks;
	int MaxBoostedSprayers;

	// level objects
	std::vector<GameObject*> actors;
	int m_numZumis;
	int m_numSprayers;
	int m_maxSprayers;
};

#endif // STUDENTWORLD_H_