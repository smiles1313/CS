#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
#include <string>
#include <list>


// need the following incomplete class declarations to create pointers to these classes
class Actor;
class Agent;
class Avatar;  // aka player

// Note:  A convention used in a number of interfaces is to represent a
// direction with the adjustments to x and y needed to move one step in
// that direction:
// left:   dx is -1, dy is  0
// right:  dx is  1, dy is  0
// up:     dx is  0, dy is  1
// down:   dx is  0, dy is -1

class StudentWorld : public GameWorld
{
public:
  StudentWorld(std::string assetPath);
 ~StudentWorld();  // just calls cleanUp()
  virtual int init();  // calls loadLevel()
  virtual int move();
  virtual void cleanUp(); // cleans up player and all actors in list
    
  int loadLevel();  // loads next level - uses helper function nextLevel()
  void setDisplayText();  // updates text at top of screen - calls GameWorld::setGameStatText - uses helper function formatDisplay()
  void ask_all_actors_to_doSomething(); // call the player and all other actor's doSomething() from Actor
  void removeDeadActors();  // remove all actors that died this tick
  
  bool canAgentMoveTo(int x, int y, int dx, int dy, Actor* a);  // is the player or robot obstructed by an object (like a wall)
                                                                // if the object is pushable (i.e. a marble), see if can push the marble
  bool doesStopPea(int x, int y);  // does this object stop a pea and NOT take damage (wall & thiefbot factory)
  void addActor(Actor* a);  // add an Actor (using polymorphism)


  bool canMarbleMoveTo(int x, int y);    // Can a marble move to x,y?

  bool damageSomething(Actor* a, int damageAmt);   // Try to cause damage to something at a's location.  (a is only ever going to be a pea.)                                                       // Return true if something stops a - (something at this location prevents
                                                   // a pea from continuing).

  bool swallowSwallowable(Actor* a);     // Swallow any swallowable object at a's location.  (a is only ever going to be a pit.)

  bool existsClearShotToPlayer(int x, int y, int dx, int dy);    // If a pea were at x,y moving in direction dx,dy, could it hit the
                                                                        // player without encountering any obstructions?

  bool doFactoryCensus(int x, int y, int distance, int& count);  // If a factory is at x,y, how many items of the type that should be
                                                                        // counted are in the rectangle bounded by x-distance,y-distance and
                                                                        // x+distance,y+distance?  Set count to that number and return true
                                                                        // ONLY if count is less than 3. Else, return false. Or, if an item
                                                                        //  is on the factory itself, return false regardless of count.
                                                                        // (The items counted are only ever going ThiefBots.)
    

  Actor* getColocatedStealable(int x, int y); // If an item that can be stolen is at x,y, return a pointer to it;
                                              // otherwise, return a null pointer.  (Stealable items are only ever going be goodies)
    
  // Getter methods
    Avatar* getPlayer();  // gets the pointer to the player private member variable
    int getBonusPoints(); // gets the bonus points the player has - private member var
    int getNumCrystals(); // gets the number of crystals left on level - private member var
    bool getCurrLevelCompletionStatus();  // gets whether or not level is complete - private member var

    
  // Setter methods
    void reduceBonusByOne();  // used to reduce the 1000 bonus by 1 each tick
    void reduceCrystalsByOne();  // called to reduce the num of crystals on level whenever player receives one
    void setCurrLevelCompletionStatus(bool b);  // sets level completion status
    
private:
    int m_bonus;            // keeps track of the bonus the player will receive
    int m_numCrystals;      // keeps track of how many crystals are left on level
    bool m_avatarExists;    // keeps track of whether or not the avatar/player was created
    bool m_levelComplete;   // keeps track of whether or not the avatar/player completed the current level
    Avatar* m_PLAYER;       // a pointer to a dynamically allocated avatar/player (added in StudentWorld::loadLevel())
    std::list<Actor*> m_listACTORS; // a list of pointers to all other dynam alloc actors (added in StudentWorld::loadLevel())
    
};

#endif // STUDENTWORLD_H_
