#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1QuestManager.h"
#include "j1CollisionManager.h"
#include "j1FileSystem.h"


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
	//Load the path of QuestData file from Config
	LOG("Loading QuestManager data");
	path = config.child("data").attribute("file").as_string();
	return true;
}

bool j1QuestManager::Start()
{
	bool ret = true;

	//Load QuestData File
	pugi::xml_document	questDataFile;
	char* buff;
	int size = App->fs->Load(path.c_str(), &buff);
	pugi::xml_parse_result result = questDataFile.load_buffer(buff, size);
	RELEASE(buff);

	if (result == NULL)
	{
		LOG("Could not load questData xml file. Pugi error: %s", result.description());
		ret = false;
	}


	return ret;
}



//=============================================

Quest::~Quest()
{
	//Destroy each event of each quest
	delete trigger;
	for (vector <Event*>::iterator it = steps.begin(); it != steps.end(); it++)
	{
		steps.erase(it);
	}
}





