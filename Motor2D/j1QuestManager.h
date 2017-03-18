#ifndef __j1QUEST_MANAGER_H__
#define __j1QUEST_MANAGER_H__

#include "j1Module.h"
#include <list>
#include <vector>
#include "p2Point.h"
#include "j1CollisionManager.h"

class Collider;
enum EVENT_TYPE
{
	//Define event types
	COLLISION_EVENT = 0
};

class Event
{
public:
	Event(EVENT_TYPE type) : type(type) {};
	~Event() {};

	EVENT_TYPE type;
};

class CollisionEvent : public Event
{
public:
	CollisionEvent(EVENT_TYPE type) :Event(type) {};

	~CollisionEvent() {};

	Collider* col;

};

class Quest
{
public:
	Quest() {};
	~Quest();

public:
	int id;
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
	Event* createEvent(pugi::xml_node&);

	//Callbacks for each event type
	bool TriggerCollisionCallback(Collider* c);
	bool StepCollisionCallback(Collider* c);

private:
	string path;

	list<Quest*> sleepQuests;
	list<Quest*> activeQuests;
	list<Quest*> closedQuests;

	
};

#endif