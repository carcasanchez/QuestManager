# The Quest for the perfect Quest

## Introduction: a design quest-ion
Hello there.   
Im Carlos, game develop student, and former leader of the amateur DuckDev team.   
Let's talk about the Quest managing in videogames   
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
The first point we need to make is concrete our quest design. ¿What states our quests have?¿How many types of quest we have?¿Can we complete the quest in different ways? ¿How a quest is completed, and what happens then? Templatize our quests and following a pattern is important. 

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

## Introducing the concept of Event
Quest managing is all about checking. But, ¿checking what and when? Here's the Event class.     
The Event is the atomic factor with we will work. The event by itself ony has one variable: an enum with the type of event it is.    
The magic comes with the heritage. By creating child classes of Event, we have the specific tools we need to check everithing we want to check.    

¿Wanna see if you talked to a specific NPC? Create a TalkEvent class with a pointer to that specific character.   
¿Slay some enemies? KillEvent class that contains an enum with the enemy type and an integer with the number of slayed.   

In our case, where we only need to check if our character has in a concrete position in the map. Since working with points is a little akward, we will take use of our Collision Manager (because yes, we have a Collision manager), making a CollisionEvent that contains a Collider (a rectangle in the map). Don't worry about collisions: our Manager is already completed, and you don't need to touch it if you don't want it.

¿Remember the Trigger and Steps I have commented before? Those are Events. Our Quest class rewrites as:

Class Quest   
{   
  int id;   
  Event* trigger;   
  vector <Event*> steps;   
  int gold;   
};   

Since the steps are fixed variables, we can store them in a std::vector.

#Page under construction
