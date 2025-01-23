#ifndef ACTOR_H_
#define ACTOR_H_

#include <cstdlib>
#include "GraphObject.h"
class StudentWorld;


//----------------------- Actor class -----------------------------
class Actor : public GraphObject
{
public:
    Actor(int imageID, int startX, int startY, StudentWorld* sWorld);
    virtual void doSomething() = 0;  // pvf, so Actor is an abstract class - derived classes will reimplement
    void move(int dir);     // move actor in specified dir
    
    // "Can this actor allow a specific action" methods
    virtual bool isDamageable() const;
    virtual bool allowsMarble() const;
    virtual bool isSwallowable() const;
    virtual bool countsInFactoryCensus() const;
    virtual bool allowsAgentColocation() const;
    virtual bool stopsPea() const;
    virtual bool bePushedBy(Actor* a, int x, int y);
    virtual bool isPushable() const;
    virtual bool canPushMarbles() const;    // Return true if an actor can push marbles (which means it's the player).
    virtual bool isStealable() const;
    virtual bool givesExtraLife() const;
    virtual bool restoresPlayerHealth() const;
    virtual bool givesAmmo() const;

        
    // Getter methods
    StudentWorld* getWorld() const;
    bool isAlive() const;
    virtual int getHitPoints() const;
    
    // Setter methods
    void setDead();
    virtual void setHitPoints(int amt);
    void getDirectionTrajectory(int dir, int& dx, int& dy);  // is used to transform direction (right, left, etc) to something
                                                             // that can be added to x,y coordinates

    // Other methods that change things in the Actor
    void decHitPoints(int amt);    // decrease the health of an actor by amt - called by damage
    virtual void damage(int damageAmt);  // damages actor
    
private:
    int m_x;  // x coord of actor
    int m_y;  // y coord of actor
    StudentWorld* m_world;   // pointer to StudentWorld
    bool m_isAlive; // indicates state of actor (alive or dead)
    int m_hitPoints; // health

    
};

//----------------------- Wall class -----------------------------
class Wall : public Actor
{
public:
    Wall(int startX, int startY, StudentWorld* sWorld);
    virtual void doSomething();
    virtual bool stopsPea() const;      // stops pea without taking damage
};

//----------------------- Agent class -----------------------------
 class Agent : public Actor
 {
 public:
     Agent(int imageID, int startX, int startY, StudentWorld* sWorld, int startDir);
 };

//----------------------- Avatar class -----------------------------
class Avatar : public Agent  // Player (Avatar) class
{
public:
    Avatar(int startX, int startY, StudentWorld* sWorld);
    virtual void doSomething();
    virtual bool isDamageable() const;
    virtual bool canPushMarbles() const;
    
    // Getter methods
    int getAmmo() const; // gets player ammo
    
    // Setter methods
    void addPeas(int exPeas); // adds peas to current amount
    
    // Additional methods that change things
    virtual void damage(int damageAmt);
    
private:
    int m_peas; // the num of peas/ammo player has left
    
};

//----------------------- Robot class -----------------------------
class Robot : public Agent
{
public:
    Robot(int imageID, int startX, int startY, StudentWorld* sWorld, int points, int startDir);
    virtual void doSomething();
    virtual void checkHitPoints();
    virtual void doFirstAdditionalEffect(bool& didThisExecute) = 0;  // pvfs, so Robot is an abstract class - bool to know if do 2nd additional effect
    virtual void doSecondAdditionalEffect() = 0;                     // these Additional Effect funcs will be overriden with specific robot's actions
    virtual bool isDamageable() const;
    virtual void damage(int damageAmt);
    virtual bool canPushMarbles() const;
    virtual bool needsClearShot() const;
    virtual int shootingSound() const;
    
    // Setter function
    void setTickCalculation();
    void increaseCurTickByOne();
    void resetCurTick();
    
    // Getter function
    int getMaxTick() const;
    int getCurTick();
    void reverseDirection();
    int getBonusPoints();
    
private:
    int m_points;  // this is the num of bonus points the player gets for destroying the robot
    int m_maxTick;  // interval of ticks that robot moves
    int m_curTick;  // current tick that robot is on
    
};

//------------------ RageBot class - subclass of Robot ------------------------
class RageBot : public Robot
{
public:
    RageBot(int startX, int startY, int startDir, StudentWorld* sWorld);
    virtual void doFirstAdditionalEffect(bool& didThisExecute);
    virtual void doSecondAdditionalEffect();
};

//------------------ ThiefBot class - subclass of Robot ------------------------ aka RegularThiefBot
class ThiefBot : public Robot
{
public:
    ThiefBot(int imageID, int startX, int startY, StudentWorld* sWorld, int score);
    virtual void checkHitPoints();  // checks if hitpoints is 0 or below - if so, does the appropiate actions
    virtual void doFirstAdditionalEffect(bool& didThisExecute);
    virtual void doSecondAdditionalEffect();
    virtual bool countsInFactoryCensus() const;
    
    void changeDistanceBeforeTurning();
    void changeDirectionRandomly();
    void alreadyTriedDirection(int dir, bool& R, bool& L, bool& U, bool& D); // R,L,U,D = right, left, up, down respectively
    
private:
    int m_distanceBeforeTurning;
    int m_countDistance;
    int m_goodieType;  // if == 0, no goodie  ::  if == 1, ExtraLifeGoodie  ::  if == 2, RestoreHealthGoodie  ::   if == 3, AmmoGoodie  
};

//------------- MeanThiefBot class - subclass of ThiefBot -------------------
class MeanThiefBot : public ThiefBot
{
public:
    MeanThiefBot(int startX, int startY, StudentWorld* sWorld);
    virtual void doFirstAdditionalEffect(bool& didThisExecute);
};


 //----------------------- ThiefBotFactory class -----------------------------
 class ThiefBotFactory : public Actor
 {
 public:
     enum ProductType { REGULAR, MEAN };

     ThiefBotFactory(int startX, int startY, StudentWorld* sWorld, ProductType type);
     virtual void doSomething();
     virtual bool stopsPea() const;
     
     // Getter method
     ProductType getThiefBotType() const;
     
 private:
     ProductType m_thiefBotType;
 };
 


//----------------------- Pea class -----------------------------
class Pea : public Actor
{
    public:
        Pea(int startX, int startY, StudentWorld* sWorld, int dir);
        virtual void doSomething();
        virtual bool allowsAgentColocation() const;

    private:
        bool m_createdThisTick;
};

//----------------------- Exit class -----------------------------
class Exit : public Actor
{
    public:
        Exit(int startX, int startY, StudentWorld* sWorld);
        virtual void doSomething();
        virtual void makeVisible();
        virtual bool allowsAgentColocation() const;

};


//----------------------- Obtain class ----------------------------- // for crystals and goodies subclasses - an abstract class

class Obtain : public Actor
{
    public:
        Obtain(int imageID, int startX, int startY, StudentWorld* sWorld);
        virtual void doSomething();
        virtual bool allowsAgentColocation() const;
        virtual bool isStealable() const;
        virtual void doAdditionalEffect() = 0; // crystal and goodies reimplement to do their specific additional effect
                                               // pvf, so this class is an abstract class
    
    // Setter function
        virtual void setPoints(int p);

    private:
        int m_points;  // the bonus points player will receive upon obtaining object (different for each type)
        int m_sound;   // same sound for crystal and goodies
    
};

//----------------------- Crystal class - subclass of Obtain -----------------------------
class Crystal : public Obtain
{
    public:
        Crystal(int startX, int startY, StudentWorld* sWorld);
        virtual void doAdditionalEffect();  // does nothing in this class (for now), but implement so not an ADT
        virtual bool isStealable() const;
};

//----------------------- Extra Life Goodie class - subclass of Obtain -----------------------------
class ExtraLifeGoodie : public Obtain
{
    public:
        ExtraLifeGoodie(int startX, int startY, StudentWorld* sWorld);
        virtual void doAdditionalEffect();
        virtual bool givesExtraLife() const;
};

//----------------------- Restore Health Goodie class - subclass of Obtain -----------------------------
class RestoreHealthGoodie : public Obtain
{
    public:
        RestoreHealthGoodie(int startX, int startY, StudentWorld* sWorld);
        virtual void doAdditionalEffect();
        virtual bool restoresPlayerHealth() const;
};

//----------------------- Ammo Goodie class - subclass of Obtain -----------------------------
class AmmoGoodie : public Obtain
{
    public:
        AmmoGoodie(int startX, int startY, StudentWorld* sWorld);
        virtual void doAdditionalEffect();
        virtual bool givesAmmo() const;
};

//----------------------- Pit class -----------------------------
class Pit : public Actor
{
public:
    Pit(int startX, int startY, StudentWorld* sWorld);
    virtual void doSomething();
    virtual bool allowsMarble() const;
};

//----------------------- Marble class -----------------------------
class Marble : public Actor
{
public:
    Marble(int startX, int startY, StudentWorld* world);
    virtual void doSomething();
    virtual bool isDamageable() const;
    virtual bool isSwallowable() const;
    virtual bool bePushedBy(Actor* a, int x, int y);
    virtual bool isPushable() const;
};

#endif // ACTOR_H_
