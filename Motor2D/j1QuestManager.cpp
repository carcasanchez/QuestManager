#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1QuestManager.h"
#include "j1FileSystem.h"
#include "j1GameLayer.h"


j1QuestManager::j1QuestManager() : j1Module()
{
	name = ("quest");
}

j1QuestManager::~j1QuestManager()
{
	//TODO: Clear the three Quest lists to avoid memory leaks
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

	//TODO 1: fill the data inside the XML (Motor2D/game/data.zip/quests/questData.xml)

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


	else for (pugi::xml_node quest = questDataFile.child("quests").first_child(); quest; quest = quest.next_sibling("quest"))
	{
		//Load quest data from XML
		Quest* new_quest = new Quest();
		new_quest->id = quest.attribute("id").as_int();
		new_quest->reward = quest.attribute("reward").as_int();

		//TODO 4: use createEvent to create a trigger for the new quest. Give quest.child("trigger") as an argument
		new_quest->trigger = createEvent(quest.child("trigger"));

		pugi::xml_node step;
		for (step = quest.child("step"); step; step = step.next_sibling("step"))
		{
			//TODO 5: use createEvent to push back Steps into the new quest
			new_quest->steps.push_back(createEvent(step));
		}

		//TODO 6: put the quest into the sleepQuests list.
		sleepQuests.push_back(new_quest);	
	}
	


	return ret;
}

Event * j1QuestManager::createEvent(pugi::xml_node &it)
{
	//Event factory method
	int type = it.attribute("type").as_int();
	
	switch (type)
	{		
		case (COLLISION_EVENT):
			//TODO 2: Create a new CollisionEvent (give it its type as an argument)
			CollisionEvent* ret = new CollisionEvent(COLLISION_EVENT);
			
			//Take collider data from XML.
			SDL_Rect rect;			
			rect.x = it.child("collider").attribute("x").as_int();
			rect.y = it.child("collider").attribute("y").as_int();
			rect.w = it.child("collider").attribute("w").as_int();
			rect.h = it.child("collider").attribute("h").as_int();			
			
			//TODO 3: create a new collider for 'ret'. Call the Collision Manager method AddCollider, and give it rect and COLLIDER_EVENT
			ret->col = App->collisions->AddCollider(rect, COLLIDER_EVENT);

			return ret;


			//BONUS: add more cases if you want to create different Events.
	}

}

//=========== Collision Callbacks
bool j1QuestManager::TriggerCollisionCallback(Collider* c)
{
	//Iterates all Triggers of sleep quests.
	for (std::list <Quest*>::iterator it = sleepQuests.begin(); it != sleepQuests.end(); it++)
	{

		
		//Check if It is a CollisionEvent
		if ((*it)->trigger->type == COLLISION_EVENT)
		{

			//TODO 7: Check if the collider given is a Trigger in any sleep quest, and active it.
			if (((CollisionEvent*)(*it)->trigger)->col == c)
			{
				LOG("Quest Triggered");
				activeQuests.push_back((*it));
				sleepQuests.erase(it);

				//Mark the collider c to delete in the next frame
				c->to_delete=true;
				return true;
			}
		}
		
	}

	return false;
}

bool j1QuestManager::StepCollisionCallback(Collider * c)
{
	//Iterates all Steps of all active quests
	for (std::list <Quest*>::iterator it = activeQuests.begin(); it != activeQuests.end(); it++)
	{
		
		//Check if It is a CollisionEvent
		if ((*it)->steps[0]->type == COLLISION_EVENT)
		{
			//TODO 8: Same as Triggers, but we check the first step now. 
			//If the Step is found, erase it.

			CollisionEvent* ev = ((CollisionEvent*)(*it)->steps[0]);
			if (ev->col == c)
			{
				LOG("Step Completed");
				
				//Erase the first step of the steps vector
				(*it)->steps.erase((*it)->steps.begin());

				//Mark the collider c to delete in the next frame
				c->to_delete = true;

				//Close the quest if there's no more steps and add reward
				if ((*it)->steps.size() == 0)
				{
					LOG("Quest completed");
					closedQuests.push_back((*it));
					activeQuests.erase(it);
					App->game->gold += (*it)->reward;
					LOG("Player Gold: %i", App->game->gold);
				}

				return true;
			}
		}
	}
	return false;
}


//BONUS: create more Trigger and Step callbacks for each type of Event. Search for the correct place to call them.

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




