#include "j1EntityManager.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1GameLayer.h"
#include "p2Point.h"
#include "Entity.h"
#include "Player.h"

j1EntityManager::j1EntityManager()
{
	
}

bool j1EntityManager::Awake(pugi::xml_node & config)
{
	bool ret = true;


	pugi::xml_node entityAttributes = config.child("entityAttributes");
	std::string folder = entityAttributes.attribute("folder").as_string("/");
	pugi::xml_node entity = entityAttributes.first_child();

	for (int tmp = 0; !entity.empty(); tmp++)
	{
		dir.insert(std::pair<ENTITY_TYPE, std::string >(ENTITY_TYPE(tmp), (folder + entity.attribute("file").as_string(".xml"))));
		entity = entity.next_sibling();
	}

	return ret;
}

bool j1EntityManager::PreUpdate()
{
	bool ret = true;

	// check for dead entities
	std::list<Entity*>::iterator item = entities.begin();
	while (item != entities.end())
	{
		if ((*item)->life <= 0)
		{
			(*item)->OnDeath();
			item = entities.erase(item); //calls destroyer
		}
		else
		{
			++item;
		}

		// reasign ids
		if(item != entities.end())
			(*item)->id = item;
	}

	return ret;
}

bool j1EntityManager::Update(float dt)
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities.begin(); item != entities.end(); item++)
	{
		(*item)->Update(dt);
	}

	return ret;
}

bool j1EntityManager::PostUpdate()
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities.begin(); item != entities.end(); item++)
	{
		(*item)->Draw();
	}

	return ret;
}

bool j1EntityManager::CleanUp()
{
	bool ret = true;

	for (std::list<Entity*>::iterator item = entities.begin(); item != entities.end(); item++)
	{
		entities.erase(item);
	}

	return ret;
}

Entity* j1EntityManager::Create(ENTITY_TYPE type, int x, int y)
{
	Entity* ret = nullptr;

	switch (type)
	{
	case LINK:
		ret = new Player();
		break;
	case NPC1:
		break;
	default:
		break;
	}

	if (ret != nullptr)
	{
		ret->Spawn(dir[type], iPoint(x,y));

		if (type == LINK)
		{
			entities.push_front(ret);
			App->game->playerId = ret->id = entities.begin();

		}
		else
		{
			entities.push_back(ret);
			ret->id = entities.end();
		}
	}

	return ret;
}