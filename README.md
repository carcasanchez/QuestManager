# The Quest for the perfect Quest

## Introduction: a design quest-ion
Hello there.   
Im Carlos, game develop student, and former leader of the amateur DuckDev team.   
Let's talk about the Quest managing in videogames.   
Quest managing is a constant interrogant in gamedev forums. There's a lot of questions about quest managing, and even worse: there's a lot of answers. We have no magical algorithm of how to make the perfect quest system, as the needs of each game are different.  

I have done some little research work for my project, and I think i'm ready to share my new acquired knowledge.   

I have prepared a little tutorial of my approach to this problem. Although It only covers a very specific scenario, the great advantage (I guess) of the system I've built is it's scalability: you can expand it to reach every field you need for your game.   
Another goal of this system is being data-driven. By modifying a simple xml file, you can custom existing quests and add new ones, without even open the code.   
Disclaimer: This is a code tutorial. I won't talk about quest design and filosophy. That is a complete different field that I will last for years if I want to cover it. There's a lot of useful and inspiring videos and articles about quest design. I only focus in how make quests work.  
Let's stop talking. Let's start working.

## Code organization
This tutorial is built in C++. The solution is from Visual Studio, but you can work with it in other software if you take the .h and .cpp files in the Motor2D folder.
The code is separated in different Modules that works in different areas (one for the renderer, other for the entities...). The Quest Manager is a module itself.  
There's a extern pointer, called App, from you can access each module and use their methods.   
Don't worry if it seems a little overwhelming: we will go step by step. 


## System Explanation
Our game is a Zelda-esque adventure, controlled with WASD. Don't blame the map so much: it's enough for what we will doing here.   
The first point we need to make is concrete our quest design. What states our quests have? How many types of quest we have?¿Can we complete the quest in different ways? How a quest is completed, and what happens then? Templatize our quests and following a pattern is important. 

For the purpose of this tutorial, I have reduced this design to the minimum: 
-Our quests have three states: sleep (the player haven't found the quest yet), active (the player is doing the quest right now), and closed (the quest is finished).
-We can have more than one quest active, but they don't interact between them.
-Our quests consist only in reach a certain point in the map.
-Quests have a Trigger variable to check if it has been activated.
-Quests are divided in Steps (immediate objectives). When a step is complete, player goes to the next. A quest is finished when there's no more steps to do.
-When we finish a quest, the player gains a certain amount of gold as a reward.

So, our primitive c++ Quest class could be:

Class Quest   
{   
  Id   
  Trigger   
  List of Steps   
  Amount of gold   
};

But there's a thing or two missing, ¿right? ¿Where's the Quest type?¿And the state?    
Don't wory. I will explain those soon. Remember that this is a generic Quest class. Let's keep it simple.    
And, don't be confused with the Id: that's only a number to identify the quest if you need it.   


## The Quest Manager Module
There's no so much difficulty here. The Manager inherits from class Module (as the rest of the Managers), stores all quests and organizes them.   
We will return later, but there's an important concept: the Three Quest Lists.   
¿Weren't you wondering why Quests don't have a Type variable by themselves? Well, we don't need it (by now). The manager has three Quest lists, one for each type (Sleep, Active and Closed). Initially, all are stored in the Sleep list. When player activates one quest, the manager moves it to the Active list, and same when a quest is completed.  
By this metod, we avoid future overlapping errors and save time by iterating only one quest type at time.

QuestManager : public Module     
{   
  list <Quest*> Sleep;   
  list <Quest*> Active;    
  list <Quest*> Closed;     
}


## Introducing the concept of Event
Quest managing is all about checking. But, ¿checking what and when? Here's the Event class.     
The Event is the atomic factor with we will work. The event by itself ony has one variable: an enum with the type of event it is.    
The magic comes with the heritage. By creating child classes of Event, we have the specific tools we need to check everithing we want to check.    

Class Event    
{   
  enum EVENT_TYPE;    
}    
 
Wanna see if you talked to a specific NPC? Create a TalkEvent class with a pointer to that specific character.   
Slay some enemies? KillEvent class that contains an enum with the enemy type and an integer with the number of slayed.   

In our case, where we only need to check if our character has in a concrete position in the map. Since working with points is a little akward, we will take use of our Collision Manager (because yes, we have a Collision manager), making a CollisionEvent that contains a Collider (a rectangle in the map). Don't worry about collisions: our Manager is already completed, and you don't need to touch it if you don't want it.

Class CollisionEvent : public Event   
{   
  Collider* col;   
}   


¿Remember the Trigger and Steps I have commented before? Those are Events. Our Quest class rewrites as:

Class Quest      
{   
  int id;   
  Event* trigger;   
  vector <Event*> steps;   
  int gold;   
};   

Since the steps are fixed variables, we can store them in a std::vector.

Ok, so, we have Quests that have Events, but they don't do anything by themselves. How we check when an event is triggered?

# Calling the Callbacks

If you take a look to the Module class, you will see a cute function called OnCollisionCallback. This adorable virtual method is called by the Collision Manager when a collider is hit. Every collider has a pointer to a Module (called "listener"), and every Module has it's own OnCollisionCallback that decides what to do whit that collider. We apply some of this system here.  
There are two thing that we want to check: the Trigger and the Steps (both of them, part of the QuestManager class).   
Why we separate both in different variables and use different Callbacks? Because we want to do different things which each one. 

Trigger Callback iterates the Sleep list, cheking the Trigger variable of each Quest and, if the conditions are happening, move that Quest to the Active list. 
Step Callback does that with the Active list, but cheking the first of the Steps (formely, the current objective inside that quest). If that Step happens to be triggered, we remove it from the vector. Then, if there's no more steps in the vector, we move the quest to the Closed list and add the gold reward to the player pocket.   
The Closed list is never iterated. Sorry, Closed list. Maybe you will found a better place in different modules.   
   
Important: each type of Event demands different checks, so we need two different Callbacks (Trigger and Step) for every type of event we have created.   

Take this example:

class TalkEvent: public Event  
{   
   NPC* talk_to.   
}

We have created a TalkEvent with a pointer to the NPC we need to talk. Assuming that all Events inside our quests are TalkEvents, the process would be:

-The Talk_TriggerCallback is called each time we talk with a NPC, and receives a pointer to that NPC.  
-The Talk_TriggerCallback checks each Trigger of each Sleep quest, comparing the NPC related to that Trigger.
-If one Trigger NPC and the NPC we have just talked are the same, the Quest is moved to the Active list and stops iteration. If not, Talk_StepCallback is called (and receives the same NPC as argument).   
-If the Talk_StepCallback is called, do the same as the Talk_TriggerCallback, but comparing the NPC to the ones of each first Step of each vector of each Active quest. 
-If we found a Quest whose current Step has the NPC we are interested in, delete that Step from the vector (by doing this, the next Step in the vector automatically becomes the first Step)   
-If there's no more Steps left in the vector, we move the Quest to the Closed list and add the gold reward to the player pocket.   


Since we want to check if some Collider has been hit, we have the Collision_TriggerCallback and the Collision_StepCallback. Both are called from the CollisionManager when a Collider is hit by the player, and receives a pointer to that Collider. The Collision Callbacks compares the meant Collider to the ones linked in the CollisionEvents.  
Be careful when checking Events in Callbacks! Be sure that, before accesing the Event data, its type is the correct.

#TODO's
#Page under construction
