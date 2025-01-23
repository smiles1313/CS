#include "Actor.h"
#include "StudentWorld.h"

//------------------------------------ Actor member funcs ----------------------------------------------
Actor::Actor(int imageID, int startX, int startY, StudentWorld* sWorld)                     // Actor constructor
: GraphObject(imageID, startX, startY), m_x(getX()), m_y(getY()), m_world(sWorld), m_isAlive(true), m_hitPoints(0)
{
    setVisible(true);
}

bool Actor::isDamageable() const
{
    return false;
}

void Actor::move(int dir)
{
    switch (dir)
    {
        case left:
            moveTo(getX() - 1, getY()); // move one square to the left
            break;
            
        case right:
            moveTo(getX() + 1, getY()); // move one square to the right
            break;
            
        case up:
            moveTo(getX(), getY() + 1); // move one square up
            break;
            
        case down:
            moveTo(getX(), getY() - 1); // move one square down
            break;
    }
}

bool Actor::allowsMarble() const
{
    return false;
}

bool Actor::isSwallowable() const
{
    return false;
}

bool Actor::countsInFactoryCensus() const
{
    return false;
}

bool Actor::allowsAgentColocation() const
{
    return false;
}

bool Actor::stopsPea() const
{
    return false;
}

bool Actor::bePushedBy(Actor* a, int x, int y)
{
    return false;
}

bool Actor::isPushable() const
{
    return false;
}

bool Actor::canPushMarbles() const
{
    return false;
}

bool Actor::isStealable() const
{
    return false;
}

bool Actor::givesExtraLife() const
{
    return false;
}

bool Actor::restoresPlayerHealth() const
{
    return false;
}

bool Actor::givesAmmo() const
{
    return false;
}


// Getter methods
StudentWorld* Actor::getWorld() const
{
    return m_world;
}

bool Actor::isAlive() const
{
    return m_isAlive;
}


int Actor::getHitPoints() const
{
    return m_hitPoints;
}

void Actor::setHitPoints(int h)
{
    m_hitPoints = h;
}

// Setter methods
void Actor::setDead()  // automatically sets dead actor to be invisible as well
{
    m_isAlive = false;
    setVisible(false);
}

void Actor::getDirectionTrajectory(int dir, int& dx, int& dy)  // reference vars for dx and dy, so the thing that calls this func can use its values
{
    dx = 0;
    dy = 0;
    
    switch (dir)
    {
        case left:
            dx = -1;
            break;
            
        case right:
            dx = 1;
            break;
            
        case up:
            dy = 1;
            break;
            
        case down:
            dy = -1;
            break;
            
        default:
            break;
    }
}

// Other methods that change things in the Actor
void Actor::decHitPoints(int amt)
{
    m_hitPoints -= amt;
}

void Actor::damage(int damageAmt)
{
    decHitPoints(damageAmt);
}


//------------------------------------ Wall member funcs ----------------------------------------------
Wall::Wall(int startX, int startY, StudentWorld* sWorld)                                    // Wall constructor
     : Actor(IID_WALL, startX, startY, sWorld)
{
}

void Wall::doSomething()  // only implement to avoid Wall from becoming an abstract class
{
    return;
}

bool Wall::stopsPea() const
{
    return true;
}

//------------------------------------ Agent member funcs ----------------------------------------------
    Agent::Agent(int imageID, int startX, int startY, StudentWorld* sWorld, int startDir)
        : Actor(imageID, startX, startY, sWorld)
    {
    }
    
//------------------------------------ Avatar member funcs ----------------------------------------------
Avatar::Avatar(int startX, int startY, StudentWorld* sWorld)                                // Avatar constructor
    : Agent(IID_PLAYER, startX, startY, sWorld, right), m_peas(20)
{
    setHitPoints(20);  // sets avatar health
}

void Avatar::doSomething()
{
    if (getHitPoints() <= 0)  // player has no more health - dies
    {
        getWorld()->playSound(SOUND_PLAYER_DIE);
        setDead();
        getWorld()->decLives(); // decrease player life by 1
        return;
    }
    
     int key;
     if (getWorld()->getKey(key))
        {
            // user hit a key this tick!
            switch (key)
            {
                case KEY_PRESS_LEFT:
                    setDirection(left);
                    if (getWorld()->canAgentMoveTo(getX(), getY(), -1, 0, this))  // if agent is not blocked, move that direction
                        move(left);
                    break;
                    
                case KEY_PRESS_RIGHT:
                    setDirection(right);
                    if (getWorld()->canAgentMoveTo(getX(), getY(), 1, 0, this))
                        move(right);
                    break;
                    
                case KEY_PRESS_UP:
                    setDirection(up);
                    if (getWorld()->canAgentMoveTo(getX(), getY(), 0, 1, this))
                        move(up);
                    break;
                    
                case KEY_PRESS_DOWN:
                    setDirection(down);
                    if (getWorld()->canAgentMoveTo(getX(), getY(), 0, -1, this))
                        move(down);
                    break;

                case KEY_PRESS_SPACE:
                    if (m_peas != 0 && getWorld()->doesStopPea(getX(), getY()) == false) // if can fire pea (pea does NOT get stopped), do so
                    {
                        int dx, dy;
                        getDirectionTrajectory(getDirection(), dx, dy);
                        getWorld()->playSound(SOUND_PLAYER_FIRE); // play firing sound
                        getWorld()->addActor(new Pea(getX()+dx, getY()+dy, getWorld(), getDirection())); // dynam alloc a pea in front of avatar
                        m_peas--;
                    }
                    break;
                    
                case KEY_PRESS_ESCAPE:
                    getWorld()->playSound(SOUND_PLAYER_DIE);
                    setDead(); // sets self to dead
                    getWorld()->decLives(); // decrease player life by 1
                    
            }
        }
}

void Avatar::damage(int damageAmt)
{
    getWorld()->playSound(SOUND_PLAYER_IMPACT);
    decHitPoints(damageAmt);
}

bool Avatar::isDamageable() const
{
    return true;
}

bool Avatar::canPushMarbles() const
{
    return true;
}

int Avatar::getAmmo() const
{
    return m_peas;
}

void Avatar::addPeas(int exPeas)
{
    m_peas += exPeas;
}

//------------------------------------ Robot member funcs ----------------------------------------------
    Robot::Robot(int imageID, int startX, int startY, StudentWorld* sWorld, int points, int startDir)
            : Agent(imageID, startX, startY, sWorld, startDir), m_points(points)
    {
        // sets the tick interval for all robots in derived classes
        
        m_curTick = 0;
        
        int levelNumber = getWorld()->getLevel();
            
        int ticks = (28 - levelNumber) / 4; // levelNumber is the current level number (0, 1, 2, etc.)
        if (ticks < 3)
            ticks = 3; // no Robot moves more frequently than this
            
        m_maxTick = ticks;
    }

    void Robot::doSomething()
    {
        checkHitPoints();
        
        if (isAlive() == false)
            return;
        
        if (getCurTick() < getMaxTick())  // robot is tired - taking a little break this tick
        {
            increaseCurTickByOne();
            return;
        }
        
        bool didThisExecute = false;
        doFirstAdditionalEffect(didThisExecute);
        
        if (didThisExecute == false) // if did not do first additional effect, then do the second one
            doSecondAdditionalEffect();
    }

    void Robot::checkHitPoints()
    {
        if (getHitPoints() <= 0)    // When its hit points reach zero, it must “die” and be removed from the game
        {
            setDead();
            getWorld()->playSound(SOUND_ROBOT_DIE);  // all robots play this death sound when dies
            getWorld()->increaseScore(getBonusPoints());
            return;
        }
    }

    bool Robot::isDamageable() const
    {
        return true;
    }

    void Robot::damage(int damageAmt)
    {
        getWorld()->playSound(SOUND_ROBOT_IMPACT);  // all robots play this impact sound when hit
        decHitPoints(damageAmt);
    }

    bool Robot::canPushMarbles() const  // no robot can push marbles - only the avatar is the agent allowed to do that
    {
        return false;
    }

    bool Robot::needsClearShot() const
    {
        return true;
    }

    int Robot::shootingSound() const
    {
        return SOUND_ENEMY_FIRE;
    }

    void Robot::increaseCurTickByOne()
    {
        m_curTick++;
    }

    void Robot::resetCurTick()
    {
        m_curTick = 0;
    }

    int Robot::getMaxTick() const
    {
        return m_maxTick;
    }

    int Robot::getCurTick()
    {
        return m_curTick;
    }

    void Robot::reverseDirection()
    {
        int dir = getDirection();

        switch (dir)
        {
            case left:
                setDirection(right);
                break;
                
            case right:
                setDirection(left);
                break;
                
            case up:
                setDirection(down);
                break;
                
            case down:
                setDirection(up);
                break;
        }
    }

    int Robot::getBonusPoints()
    {
        return m_points;
    }

//------------------------------------ Ragebot member funcs ----------------------------------------------
    RageBot::RageBot(int startX, int startY, int startDir, StudentWorld* sWorld)
        : Robot(IID_RAGEBOT, startX, startY, sWorld, 100, startDir)
    {
        setHitPoints(10);
    }

    void RageBot::doFirstAdditionalEffect(bool& didThisExecute)
    {
        resetCurTick();  // ragebot acts this tick - does its ragebot things
            
        int dx, dy;
        getDirectionTrajectory(getDirection(), dx, dy);
            
        if (getWorld()->existsClearShotToPlayer(getX(), getY(), dx, dy)) // if all clear, ragebot shoots its shot
        {
            getWorld()->playSound(shootingSound()); // play shooting sound
            getWorld()->addActor(new Pea(getX()+dx, getY()+dy, getWorld(), getDirection())); // dynam alloc a pea in front of ragebot
        }
            
        else if (getWorld()->canAgentMoveTo(getX(), getY(), dx, dy, this))  // if robot can move, move
            move(getDirection());
            
        else  // robot is obstructed - can not shoot or move, so reverse direction
            reverseDirection();
        
        didThisExecute = true; // always will be true for Ragebot
    }

    void RageBot::doSecondAdditionalEffect()  // implement so ragebot does not become an abstract class
    {
        return;
    }


//------------------------------------ (Regular)ThiefBot member funcs ----------------------------------------------
    ThiefBot::ThiefBot(int imageID, int startX, int startY, StudentWorld* sWorld, int points) // points will be 10 for this - passed in when create
        : Robot(imageID, startX,  startY, sWorld, points, right)
    {
        setHitPoints(5);
        m_distanceBeforeTurning = randInt(1, 6);
        m_countDistance = 0;
        m_goodieType = 0; // indicates that robot is NOT holding goodie
    }

    void ThiefBot::checkHitPoints()
    {
        if (getHitPoints() <= 0)    // When its hit points reach zero, it must “die” and be removed from the game
        {
            // Checks if carrying a goodie when it dies; if it is, the goodie becomes its tombstone
            if (m_goodieType != 0)
            {
                if (m_goodieType == 1)
                    getWorld()->addActor(new ExtraLifeGoodie(getX(), getY(), getWorld()));
                
                if (m_goodieType == 2)
                    getWorld()->addActor(new RestoreHealthGoodie(getX(), getY(), getWorld()));
                
                if (m_goodieType == 3)
                    getWorld()->addActor(new AmmoGoodie(getX(), getY(), getWorld()));
                
                m_goodieType = 0;
            }
    
            setDead();
            getWorld()->playSound(SOUND_ROBOT_DIE);
            getWorld()->increaseScore(getBonusPoints());
        }
        
        // else, if hitpoints is not <= 0, this function does nothing
    }

    void ThiefBot::doFirstAdditionalEffect(bool& didThisExecute) // will be false, so can doSecondAdditionalEffect
    {
        return;
    }

    void ThiefBot::doSecondAdditionalEffect()
    {
        resetCurTick();  // thiefbot acts this tick

        Actor* potentialLoot = getWorld()->getColocatedStealable(getX(), getY());  // if there is a goodie, a pointer to it is returned - else nullptr
        
        if (potentialLoot != nullptr)
        {
            int randPickUp = randInt(1, 10);
            if (randPickUp == 1)  // 1/10 chance the thiefbot picks up goodie
            {
                if (potentialLoot->givesExtraLife())
                    m_goodieType = 1;
                
                if (potentialLoot->restoresPlayerHealth())
                    m_goodieType = 2;
                
                if (potentialLoot->givesAmmo())
                    m_goodieType = 3;
                
                potentialLoot->setDead(); // StudentWorld's removeDeadActors() func will remove this goodie that was picked up next tick
                getWorld()->playSound(SOUND_ROBOT_MUNCH);
                return; // thiefbot should do nothing more this tick
            }
        }
        // otherwise, potentialLoot is nullptr so m_goodieType stays as 0
        
        
        int dx, dy;
        getDirectionTrajectory(getDirection(), dx, dy);
        
        if (m_countDistance < m_distanceBeforeTurning)  // if not yet moved distanceBeforeTurning squares in its current direction
        {
            if (getWorld()->canAgentMoveTo(getX(), getY(), dx, dy, this))  // attempt to move in direction
            {
                move(getDirection());
                m_countDistance++; // increment num times thiefbot moves in this direction
                return; // thiefbot should do nothing more this tick
            }
        }
        
        
        // ThiefBot has either moved distanceBeforeTurning squares in a straight line or encountered an obstruction
        changeDistanceBeforeTurning();  // select new distanceBeforeTurning (random)
        changeDirectionRandomly();      // select new direction randomly
        
        int dirHolder = getDirection(); // holds new direction (from changeDirectionBeforeTurning()) in case bot can not move in any 4 directions
        bool triedRight = false;
        bool triedLeft = false;
        bool triedUp = false;
        bool triedDown = false;
        int DX, DY;
        
        for(;;)
        {
            alreadyTriedDirection(getDirection(), triedRight, triedLeft, triedUp, triedDown); // updates the bools to true if already tried that dir
            getDirectionTrajectory(getDirection(), DX, DY);
            
            if (getWorld()->canAgentMoveTo(getX(), getY(), DX, DY, this))  // attempt to move in the new direction
            {
                move(getDirection());
                return; // thiefbot should do nothing more this tick
            }
            
            else  // if could not move in new direction, set dir to one that has not been tried yet
            {
                if (triedRight == false)
                {
                    setDirection(right);
                    break;
                }
                
                if (triedLeft == false)
                {
                    setDirection(left);
                    break;
                }
                
                if (triedUp == false)
                {
                    setDirection(up);
                    break;
                }
                
                if (triedDown == false)
                {
                    setDirection(down);
                    break;
                }
                
                else  // tried to move in all 4 directions - but obstructed in all 4 directions
                {
                    setDirection(dirHolder);  // set direction back to the random direction selected earlier
                    return;  // break from loop - thiefbot should do nothing more this tick
                }
            }
            
        }
    }

    bool ThiefBot::countsInFactoryCensus() const
    {
        return true;
    }

    void ThiefBot::changeDistanceBeforeTurning()
    {
        m_distanceBeforeTurning = randInt(1, 6);  // randomly selects b/w 1-6
        m_countDistance = 0; // resets how many blocks walked in certain distance
    }

    void ThiefBot::changeDirectionRandomly()  // randomly choose new direction (could be the same as current direction too - spec says it's ok)
    {
        int rand = randInt(1, 4);
        
        switch (rand)
        {
            case 1:
                setDirection(left);
                break;
                
            case 2:
                setDirection(right);
                break;

            case 3:
                setDirection(up);
                break;
                
            default:  // aka, if rand == 4
                setDirection(down);
        }
    }

    void ThiefBot::alreadyTriedDirection(int dir, bool& R, bool& L, bool& U, bool& D)  // ref vars R, L, U, & D so it can be used after this func call
    {
        if (dir == right)
            R = true;
        
        else if (dir == left)
            L = true;
        
        else if (dir == up)
            U = true;
        
        else if (dir == D)
            D = true;
    }

//------------------------------------ MeanThiefBot member funcs ----------------------------------------------
    MeanThiefBot::MeanThiefBot (int startX, int startY, StudentWorld* sWorld)
        : ThiefBot(IID_MEAN_THIEFBOT, startX, startY, sWorld, 20) // bonus points for destroying a MeanThiefBot is 20
    {
        setHitPoints(8);  // health of the meanthiefbot
    }
    
    void MeanThiefBot::doFirstAdditionalEffect(bool& didThisExecute)
    {
        resetCurTick();  // meanthiefbot acts this tick

        int dx, dy;
        getDirectionTrajectory(getDirection(), dx, dy);
        
        if (getWorld()->existsClearShotToPlayer(getX(), getY(), dx, dy)) // if all clear, ragebot shoots its shot
        {
            getWorld()->playSound(shootingSound()); // play shooting sound
            getWorld()->addActor(new Pea(getX()+dx, getY()+dy, getWorld(), getDirection())); // dynam alloc a pea in front of ragebot
            didThisExecute = true;
        }
        // if didThisExecute does NOT get set to true, then the meanthiefbot will do thiefbot's doSecondAdditionalEffect() in Robot::doSomething()
    }


//------------------------------------ ThiefBotFactory member funcs ----------------------------------------------
    ThiefBotFactory::ThiefBotFactory(int startX, int startY, StudentWorld* sWorld, ProductType type)
        : Actor(IID_ROBOT_FACTORY, startX, startY, sWorld), m_thiefBotType(type)
    {
        
    }

    void ThiefBotFactory::doSomething()
    {
        int count = 0;
        if (getWorld()->doFactoryCensus(getX(), getY(), 3, count)) // if returns true, do the following
        {
            int random = randInt(1, 50);
            if (random == 7) // pick an arbitrary number - if random number is equal to that (1/50 chance), then create thiefbot of type ProductType
            {
                if (getThiefBotType() == REGULAR)
                    getWorld()->addActor(new ThiefBot(IID_THIEFBOT, getX(), getY(), getWorld(), 10));
                
                else        // getThiefBotType() == MEAN
                    getWorld()->addActor(new MeanThiefBot (getX(), getY(), getWorld()));
                
                getWorld()->playSound(SOUND_ROBOT_BORN);  // play sound once a new ThiefBot of some type is made
            }
        }
        
    }

    bool ThiefBotFactory::stopsPea() const
    {
        return true;
    }

    ThiefBotFactory::ProductType ThiefBotFactory::getThiefBotType() const
    {
        return m_thiefBotType;
    }


//------------------------------------ Pea member funcs ----------------------------------------------
Pea::Pea(int startX, int startY, StudentWorld* sWorld, int dir)
    : Actor(IID_PEA, startX, startY, sWorld), m_createdThisTick(true)
{
    setDirection(dir);  // starting direction will be the same as the actor that dynam allocs the pea
}

void Pea::doSomething()
{
    if (isAlive() == false)  // if pea is dead, do nothing
        return;
    
    // pea does nothing the first tick it is created (otherwise, it spawns too far from agent that allocated it)
    if (m_createdThisTick == true)  // after first tick, change flag to false so pea actually does something after every tick
    {
        m_createdThisTick = false;
        return;
    }
    
    else
    {
        if (getWorld()->damageSomething(this, 2)) // if on same square as: marble, a robot, or the player  - ifDamageable() - then damage
            setDead(); // set state to dead - also automatically sets visibility to false
        
        else if (getWorld()->doesStopPea(getX(), getY()))  // else if pea is on same square as something that stops it (and does not take damage)
            setDead();
            
        else  // else, move pea 1 step in direction
            move(getDirection());
    }
    
}

bool Pea::allowsAgentColocation() const
{
    return true;
}


//------------------------------------ Exit member funcs ----------------------------------------------
Exit::Exit(int startX, int startY, StudentWorld* sWorld)
    : Actor(IID_EXIT, startX, startY, sWorld)
{
    setVisible(false);  // exit should start out invisible
}

void Exit::doSomething()
{
    // if the last crystal is picked up, then call revealExit
    if (getWorld()->getNumCrystals() == 0)
    {
        makeVisible();
        getWorld()->reduceCrystalsByOne(); // this sets m_numCrystals = -1 so that this func does NOT get called again in current level
    }
    
    // if the exit is visible AND player on same square as exit
    if (isVisible() && getX() == getWorld()->getPlayer()->getX() && getY() == getWorld()->getPlayer()->getY())
    {
        getWorld()->playSound(SOUND_FINISHED_LEVEL);
        getWorld()->increaseScore(2000);   // increase Player's score by 2000
        
        // If there are any bonus points left, the player must receive the bonus points for completing the level quickly.
        getWorld()->increaseScore(getWorld()->getBonusPoints());
        
        // Indicate that player completed current level
        getWorld()->setCurrLevelCompletionStatus(true);
    }
}

void Exit::makeVisible()  // will only be called/done (at most) once in a level
{
    setVisible(true);
    getWorld()->playSound(SOUND_REVEAL_EXIT);  // plays the exit reveal sound
}

bool Exit::allowsAgentColocation() const
{
    return true;
}

//------------------------------------ Obtain member funcs ----------------------------------------------
Obtain::Obtain(int imageID, int startX, int startY, StudentWorld* sWorld)
    : Actor(imageID, startX, startY, sWorld), m_sound(SOUND_GOT_GOODIE), m_points(0)  // points set to 0 as a default
{
    setDirection(none);
}

void Obtain::doSomething()
{
    if (!isAlive())
        return;
    
    if (getX() == getWorld()->getPlayer()->getX() && getY() == getWorld()->getPlayer()->getY()) // if crystal or goodie on same square as player
    {
        getWorld()->increaseScore(m_points);
        setDead();
        getWorld()->playSound(m_sound);
        doAdditionalEffect();
    }
}

bool Obtain::allowsAgentColocation() const
{
    return true;
}

bool Obtain::isStealable() const
{
    return true;
}

// setter methods
void Obtain::setPoints(int p)
{
    m_points = p;
}

//------------------------------------ Crystal member funcs ----------------------------------------------
Crystal::Crystal(int startX, int startY, StudentWorld* sWorld)
    : Obtain(IID_CRYSTAL, startX, startY, sWorld)
{
    setPoints(50);  // set crystal's bonus points to 50 (player gets 50 bonus points if obtain crystal)
}

void Crystal::doAdditionalEffect()
{
    getWorld()->reduceCrystalsByOne();  // calls setter method in StudentWorld - reduces num of crystals in world by one
}

bool Crystal::isStealable() const
{
    return false;
}

//------------------------------------ Extra Life Goodie member funcs ----------------------------------------------
ExtraLifeGoodie::ExtraLifeGoodie(int startX, int startY, StudentWorld* sWorld)
    : Obtain(IID_EXTRA_LIFE, startX, startY, sWorld)
{
    setPoints(1000);  // sets extra life goodie's bonus points to 1000
}

void ExtraLifeGoodie::doAdditionalEffect()
{
    getWorld()->incLives();    // calls StudentWorld to increase player lives by 1
}

bool ExtraLifeGoodie::givesExtraLife() const
{
    return true;
}


//------------------------------------ Restore Health Goodie member funcs ----------------------------------------------
RestoreHealthGoodie::RestoreHealthGoodie(int startX, int startY, StudentWorld* sWorld)
    : Obtain(IID_RESTORE_HEALTH, startX, startY, sWorld)
{
    setPoints(500); // sets restore health goodie's bonus points to 500
}

void RestoreHealthGoodie::doAdditionalEffect()
{
    getWorld()->getPlayer()->setHitPoints(20);  // calls StudentWorld to restore player health to full value (20)
}

bool RestoreHealthGoodie::restoresPlayerHealth() const
{
    return true;
}


//------------------------------------ Ammo Goodie member funcs ----------------------------------------------
AmmoGoodie::AmmoGoodie(int startX, int startY, StudentWorld* sWorld)
    : Obtain(IID_AMMO, startX, startY, sWorld)
{
    setPoints(100); // sets ammo goodie's bonus points to 100
}

void AmmoGoodie::doAdditionalEffect()
{
    getWorld()->getPlayer()->addPeas(20);  // calls StudentWorld and its player getter method to add 20 peas/ammo to player
}

bool AmmoGoodie::givesAmmo() const
{
    return true;
}


//------------------------------------ Pit member funcs ----------------------------------------------
Pit::Pit(int startX, int startY, StudentWorld* sWorld)
    :Actor(IID_PIT, startX, startY, sWorld)
{
}

void Pit::doSomething()
{
    if (isAlive() == false)  // if pit is dead, does nothing
        return;
    
    if (getWorld()->swallowSwallowable(this))   // if on same square as a marble, the marble gets deleted in StudentWorld...
        setDead(); // ... and the Pit will be deleted this tick by the removeDeadActors() function in StudentWorld
}

bool Pit::allowsMarble() const
{
    return true;
}

//------------------------------------ Marble member funcs ----------------------------------------------
Marble::Marble(int startX, int startY, StudentWorld* sWorld)
    : Actor(IID_MARBLE, startX, startY, sWorld)
{
    setHitPoints(10);
}

void Marble::doSomething()
{
    // When a marble is attacked (i.e., a pea collides with it), it loses 2 hit points - DONE IN PEA DEFINITION

    if (getHitPoints() <= 0)    // When its hit points reach zero, it must “die” and be removed from the game
        setDead();
        //remove marble from game - do this in StudentWorld with the removeDeadActors() function in the move() func.
}

bool Marble::isDamageable() const
{
    return true;
}

bool Marble::isSwallowable() const
{
    return true;
}

bool Marble::bePushedBy(Actor* a, int x, int y)
{
    if (a->canPushMarbles())
    {
        if (getWorld()->canMarbleMoveTo(x, y))    // attempt to push marble
            return true;
    }
    
    return false;
}

bool Marble::isPushable() const
{
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------
