# The Quest for the perfect Quest

## Introduction: a design quest-ion
Hello there.   
I'm Carlos, game develop student, former leader of the amateur DuckDev team, and i have been working on a C++ data-driven Quest Manager for videogames.     
The purpose of the system is to automatically create sequences of events from the data written in a XML file, allowing the designers to work and iterate quickly without touching any code. The system can be expanded easily to cover any design necessity in your game.   
This web works as documentation and as tutorial. You can download an incomplete code with TODO's as an exercise, a .exe with the final result, and the complete code.

# Resources
Our game is a Zelda-esque adventure, controlled with WASD. The quest displayed is simple: go to the upper red rectangle (1), then go to the lower (2), then to the last one (3). After complete the three steps, you will receive gold as a reward. Turn off the debug with F1 (although I don't reccomend to do so, because you will need the debug mode to see the rectangles).   

# prueba
![](https://fotos.subefotos.com/b043f1e01de16d1a3a53f2c0e3808236o.gif)   

### [Handout with TODO's](https://github.com/carcasanchez/QuestManager/releases/tag/v2)    
### [Exe with the system fully working](https://github.com/carcasanchez/QuestManager/releases/tag/v1)   
### [Entire repository with code solution](https://github.com/carcasanchez/QuestManager)   


## Code organization
This tutorial is built in C++. The solution is from Visual Studio, but you can work with it in other software if you take the .h and .cpp files in the Motor2D folder.
The code is separated in different Modules that works in different areas (one for the renderer, other for the entities...). The Quest Manager is a module itself.  
There's a extern pointer, called App, from you can access each module.   
Don't worry if it seems a little overwhelming: we will go step by step. 

# Intro to the system
The first point we need to make is concrete our quest design. What states our quests have? How many types of quest we have? Can we complete the quest in different ways? How a quest is completed, and what happens then? Templatize our quests and following a pattern is important. 

For the purpose of this tutorial, I have reduced this design to the minimum:     

* Our quests have three states: sleep (the player haven't found the quest yet), active (the player is doing the quest right now), and closed (the quest is finished).   
* We can have more than one quest active, but they don't interact between them.   
* Our quests consist only in reach a certain point in the map.   
* Quests have a Trigger variable to check if it has been activated.   
* Quests are divided in Steps (immediate objectives). When a step is complete, player goes to the next. A quest is finished when there's no more steps to do.   
* When we finish a quest, the player gains a certain amount of gold as a reward.   

So, our primitive c++ Quest class could be:
 ```
Class Quest   
{   
  Id   
  Trigger   
  List of Steps   
  Amount of gold   
};
  ```
But there's a thing or two missing, right? Where's the Quest type?¿And the state?    
Don't wory. I will explain those soon. Remember that this is a generic Quest class. Let's keep it simple.    
And, don't be confused with the Id: that's only a number to identify the quest if you need it.   


# The Quest Manager Module
There's no so much difficulty here. The Manager inherits from class Module (as the rest of the Managers), stores all quests and organizes them.   

We will return later, but there's an important concept: the Three Quest Lists.   
Weren't you wondering why Quests don't have a Type variable by themselves? Well, we don't need it (by now). The manager has three Quest lists, one for each type (Sleep, Active and Closed). Initially, all are stored in the Sleep list. When player activates one quest, the manager moves it to the Active list, and same when a quest is completed.  

Doing this, we avoid future overlapping errors and save time by iterating only one quest type at time.

```
Class QuestManager : public Module     
{   
  list <Quest*> Sleep;   
  list <Quest*> Active;    
  list <Quest*> Closed;     
}
```


# Introducing the concept of Event
Quest managing is all about checking. But, ¿checking what and when? Here's the **Event class**.     
The Event is the atomic factor with we will work. The event by itself ony has one variable: an enum with the type of event it is.    
The magic comes with the heritage. By creating child classes of Event, we have the specific tools we need to check everithing we want to check.    

```
Class Event    
{   
  enum EVENT_TYPE;    
}    
```
 
Wanna see if you talked to a specific NPC? Create a TalkEvent class with a pointer to that specific character.   
Slay some enemies? KillEvent class that contains an enum with the enemy type and an integer with the number of slayed.   

In our case, where we only need to check if our character has in a concrete position in the map. Since working with points is a little akward, we will take use of our Collision Manager (because yes, we have a Collision manager), making a CollisionEvent that contains a Collider (a rectangle in the map). Don't worry about collisions: our Manager is already completed, and you don't need to touch it if you don't want it.

```
Class CollisionEvent : public Event   
{   
  Collider* col;   
}   
```


Remember the Trigger and Steps I have commented before? Those are Events. Our Quest class rewrites as:

```
Class Quest      
{   
  int id;   
  Event* trigger;   
  vector <Event*> steps;   
  int gold;   
};   
```

Since the steps are fixed variables, we can store them in a std::vector.

Ok, so, we have Quests that have Events, but they don't do anything by themselves. How we check when an event is triggered?

# Calling the Callbacks

If you take a look to the Module class, you will see a cute function called OnCollisionCallback. This adorable virtual method is called by the Collision Manager when a collider is hit. Every collider has a pointer to a Module (called "listener"), and every Module has it's own OnCollisionCallback that decides what to do whit that collider. We apply some of this system here.  
There are two thing that we want to check: the Trigger and the Steps (both of them, part of the QuestManager class).   
Why we separate both in different variables and use different Callbacks? Because we want to do different things which each one. 

* Trigger Callback iterates the Sleep list, cheking the Trigger variable of each Quest and, if the conditions are happening, move that Quest to the Active list. 
* Step Callback does that with the Active list, but cheking the first of the Steps (formely, the current objective inside that quest). If that Step happens to be triggered, we remove it from the vector. Then, if there's no more steps in the vector, we move the quest to the Closed list and add the gold reward to the player pocket.   
* The Closed list is never iterated. Sorry, Closed list. Maybe you will found a better place in different modules.   
   
**Important: each type of Event demands different checks, so we need two different Callbacks (Trigger and Step) for every type of event we have created.**   

Take this example:

```
class TalkEvent: public Event  
{   

   NPC* talk_to.   
}
```

We have created a TalkEvent with a pointer to the NPC we need to talk. Assuming that all Events inside our quests are TalkEvents, the process would be:

1. The Talk_TriggerCallback is called each time we talk with a NPC, and receives a pointer to that NPC.  
2. The Talk_TriggerCallback checks each Trigger of each Sleep quest, comparing the NPC related to that Trigger.   
3. If one Trigger NPC and the NPC we have just talked are the same, the Quest is moved to the Active list and stops iteration. If not, Talk_StepCallback is called (and receives the same NPC as argument).   
4. If the Talk_StepCallback is called, do the same as the Talk_TriggerCallback, but comparing the NPC to the ones of each first Step of each vector of each Active quest.  
5. If we found a Quest whose current Step has the NPC we are interested in, delete that Step from the vector (by doing this, the next Step in the vector automatically becomes the first Step)   
6. If there's no more Steps left in the vector, we move the Quest to the Closed list and add the gold reward to the player pocket.   

   
Since we want to check if some Collider has been hit, we have the Collision_TriggerCallback and the Collision_StepCallback. Both are called from the CollisionManager when a Collider is hit by the player, and receives a pointer to that Collider. The Collision Callbacks compares the meant Collider to the ones linked in the CollisionEvents.  
Be careful when checking Events in Callbacks! Be sure that, before accesing the Event data, its type is the correct.

# Creating quests from a XML file
This is the XML I'm using in this case, stored in "Motor2D/Game/data.zip/quests": 

```
<!-- Quest Data -->
<quests>
	<quest id = "1" reward="300">

		<trigger type ="0">
     			 <collider x = "30" y= "30" w="10" h="10"/>
		</trigger>

		<step type = "0">
			<collider x = "150" y= "30" w="10" h="10"/>
		</step>
		<step type = "0">
			<collider x = "30" y= "30" w="10" h="10"/>
		</step>


	</quest>

	<quest id = "2" reward="700">

		<trigger type ="0">
     			 <collider x = "130" y= "130" w="10" h="10"/>
		</trigger>

		<step type = "0">
			<collider x = "250" y= "130" w="10" h="10"/>
		</step>
		<step type = "0">
			<collider x = "130" y= "130" w="10" h="10"/>
		</step>


	</quest>

</quests>
```
As you see, I have two different quests, with two steps each one (written in order of execution), but you can add any amount of quests and steps as you wish.   
During the first loop of the program, it loads this XML and automatically created one Quest for each one that is written in the text file, and stores them in the Sleep list. Also, there's a method, called CreateEvent, that receives the type of event we want to create. Another time, you must make a case for each type of event, since the info received is different for each one. I have marked each Trigger and Step with a "type = 0", because the CollisionEvent type has value 0 inside the enum. Make sure that our code has a way to recognize what type of event each Trigger and Step is.   

In our ColliderEvents, the only stored data are four integers (x, y, w, and h). In the CreateEvent method, the CollisionManager receives this numbers and creates a Collider that will be linked to the Event we are created.   
Coming back to the NPC-Talk example, the CreateEvent could receive the Id of the NPC, search it and store it's direction inside the Event.    

# TODO's

Let's dive into the TODO's. TODO's are comments placed in primordial segments of code, and give in instructions to make the system work. Search them with ctrl+SHIFT+F and go in order.

## Before writting any code
### TODO 1: Fill the XML with data.
Go to questData.xml. Although you can create all the quests with all the steps you want, I suggest you to start with one quest with one step (and the trigger, of course). Remeber to put the event type in each one (in this case, "0", since we only have one type). Be careful when setting x and y values, because the collider can fall outside the map or inside a wall. The values given above are all valid.

## CreateEvent method
This method receives a xml node containing the Event's data. From here, it extracts the Event's type, and creates the corresponding event.
### TODO 2: Create a new CollisionEvent
No mistery. Make a new to the "ret" pointer, passing it COLLISION_EVENT and Voilà.
### TODO 3: Create the collider 
After making "ret" exist, fill it's "col" variable with the collider that returns the method AddCollider, located in  App->collisions. The methods receives the rect we have extracted from the XML, and the type of collider you want to create (in this case, give it a EVENT_COLLIDER).

## Create the events

### TODO 4: Create an event for the Trigger
We are inside a loop, used to create each quest written in the XML. We have a new_quest pointer, with the Id and the Reward variables already filled. We must provide it with a Trigger, using our brand new CreateEvent method. CreateEvent need a xml_node with the event data, so give it 'quest.child("trigger")' as an argument.
### TODO 5: Create steps
Same as the previous TODO, but with a difference: we must create one event for each Step stored in the XML. Give createElement the "step" node, and store the returned value inside the "Steps" vector of "new_quest".
### TODO 6: Store the new quest inside the sleepQuest list
Do you know how to store data in a vector, right?  


## Create the Callbacks
Remember: we have two callback methods (one for triggers and one for steps) for each event type. Now, we use Collision Callbacks, that receives the EVENT_COLLIDER that has been hit.   
### TODO 7: In the CollisionTriggerCallback, check if the Trigger is triggered, and activate the quest.
Now, we will iterate all quests and checking all it's triggers. Since we only have one event type, this won't be a problem, but remember to ensure that the event type we are cheking is a COLLISION_EVENT before checking it's collider! Compare each trigger collider to the given and move the quest to the Active list if the comparison gives true. 
### TODO 8: In the CollisionStepCallback, check if the first Step is triggered.
Same as with triggers, but we access to the first step of the steps vector of the quest. One more time, erase the first step if it has the same collider as the given in the argument.    
Below, you will see the code that closes the quest if there's no more steps left in the quest. If you want to create more rewards, probably create a method that checks what reward the quest has and actues in consequence will be a good idea.   

## Calling the Callbacks
The magic touch here is know where and when the callbacks should be called. In our case, it's relatively easy: when the player hits a collider of type EVENT.

### TODO 9: Call both callbacks
If c2->type is a COLLIDER_EVENT, call the CollisionTriggerCallback and give it c2 as an argument. If returns false, it means that this collider is a Step, so we call the CollisionStepCallback (with c2 argument).


# Expanding
Do you want to add more event types? Remember:
* You must create another case in the EVENT_TYPE enum and a class for the event. Give it the correct number in the xml.
* You must specify the data you will store in the XML, and what will the CreateEvent method do with this data.
* Create Trigger and StepCallbacks for this event. How their recognize if the event has been completed, and from where they are called?

You can tweak and experiment with the system as you want. Try adding fail states to the quests, activating new quests as reward for other quests, or mixing various types of event inside the same quest. The sky is the limit now!

# Farewell
I hope I've been useful or, at least, entertaining with this little tutorial. Follow me on Github and ask me any question you have about this manager, and follow me on [twitter](https://twitter.com/carcasanchez) if you have any question non related with this manager =)    
Since then, I have been Carlos, and I hope to see you soon.
