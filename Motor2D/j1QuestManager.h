#ifndef __j1QUEST_MANAGER_H__
#define __j1QUEST_MANAGER_H__

#include "j1Module.h"
#include <list>
#include <vector>
#include "p2Point.h"

class Collider;

enum EVENT_TYPE
{
	COLLISION = 0
};

class Event
{
public:
	Event() {};
	~Event() {};

	EVENT_TYPE type;
};

class CollisionEvent : public Event
{
public:
	CollisionEvent() {};
	~CollisionEvent() {};

private:
	Collider* col;

};

class Quest
{
public:
	Quest() {};
	~Quest();

private:
	int reward;
	Event* trigger;
	vector <Event*> steps;
	
};

class j1QuestManager : public j1Module
{
public:

	j1QuestManager();
	~j1QuestManager();

	bool Awake(pugi::xml_node&);
	bool Start();



private:
	string path;

	list<Quest*> sleepQuests;
	list<Quest*> activeQuests;
	list<Quest*> closedQuests;

	//bool activateQuest(Quest*);
	//bool closeQuest(Quest*);
};

#endif