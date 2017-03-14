#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1QuestManager.h"


j1QuestManager::j1QuestManager() : j1Module()
{
	name = ("quest");
}

j1QuestManager::~j1QuestManager()
{
	//Clear three quest lists
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
		sleepQuests.erase(it);

	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
		activeQuests.erase(it);
	
	for (std::list <Quest*>::iterator it = closedQuests.begin(); it != closedQuests.end(); it++)
		closedQuests.erase(it);
}

bool j1QuestManager::Awake(pugi::xml_node& config)
{
	LOG("Loading QuestManager data");
	path = config.child_value("path");
	return true;
}

bool j1QuestManager::Start()
{
	
	return true;
}
