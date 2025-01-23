#include "StudentWorld.h"
#include "GameConstants.h"
#include "Level.h" // so can use Level class
#include "Actor.h" // so can add actors 
#include <string>
using namespace std;

// For formatDisplay() helper function
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw

//-------------------------------- Non-member Helper Functions ---------------------------------------
string formatDisplay(int score, int level, int lives, int health, int ammo, int bonus)  // formats display on top of screen (called by setDisplayText)
{
    ostringstream oss;
    
    oss.fill('0');
    oss << "Score: " << setw(7) << score;
    oss << "  Level: " << setw(2) << level;
    oss.fill(' ');
    oss << "  Lives: " << setw(2) << lives;
    oss << "  Health: " << setw(3) << health << "%";
    oss << "  Ammo: " << setw(3) << ammo;
    oss << "  Bonus: " << setw(4) << bonus;
    
    string s = oss.str();
    return s;
}


string nextLevel(int numLevel)  // called to format the next level string (called in loadLevel())
{
    ostringstream oss;
    
    oss.fill('0');
    oss << "level" << setw(2) << numLevel << ".txt";
    
    string s = oss.str();
    return s;
}

//----------------------------------------------------------------------------------------------------

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_avatarExists = false; // no avatar/player created yet
    m_numCrystals = 0;  // initialize to 0 so can add to it
    m_levelComplete = false; // flag, only turns true when player completes current level
    m_bonus = 1000; // bonus starts at 1000 for each level
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::loadLevel()
{
    string curLevel = nextLevel(getLevel());
    Level lev(assetPath());
    Level::LoadResult result = lev.loadLevel(curLevel);
    
    if (curLevel == "level99.txt" || result == Level::load_fail_file_not_found)
    {
        cerr << "Could not find level00.txt data file\n";
        return GWSTATUS_PLAYER_WON; // player won game (completed all available levels)
    }
    
    else if (result == Level::load_fail_bad_format)
    {
        cerr << "Your level was improperly formatted\n";
        return GWSTATUS_LEVEL_ERROR; // failed to load level
    }
    
    else if (result == Level::load_success)
        cerr << "Successfully loaded level\n";

    for (int y = 0; y < VIEW_HEIGHT; y++)
    {
        for (int x = 0; x < VIEW_WIDTH; x++)
        {
            Level::MazeEntry item = lev.getContentsOf(x, y);
            switch (item)
            {
                case Level::empty:
                    break;
                    
                case Level::player:
                    m_PLAYER = new Avatar(x, y, this);  // creates the player
                    m_avatarExists = true; // avatar is alive and visible
                    break;
                    
                case Level::wall:
                    m_listACTORS.push_back(new Wall(x, y, this)); // adds new wall to list
                    break;
                    
                case Level::exit:
                    m_listACTORS.push_back(new Exit(x, y, this)); // adds exit to list
                    break;
                    
                case Level::horiz_ragebot:
                    m_listACTORS.push_back(new RageBot(x, y, 0, this));  // adds new horiz ragebot - dir (0) is right
                    break;
                    
                case Level::vert_ragebot:
                    m_listACTORS.push_back(new RageBot(x, y, 270, this));  // adds new vert ragebot - dir (270) is down
                    break;
                    
                case Level::thiefbot_factory:
                    m_listACTORS.push_back(new ThiefBotFactory(x, y, this, ThiefBotFactory::REGULAR)); // adds new regular thiefbotfactory
                    break;
                    
                case Level::mean_thiefbot_factory:
                    m_listACTORS.push_back(new ThiefBotFactory(x, y, this, ThiefBotFactory::MEAN)); // adds new mean thiefbotfactory
                    break;
                    
                case Level::marble:
                    m_listACTORS.push_back(new Marble(x, y, this)); // adds new marble
                    break;
                    
                case Level::pit:
                    m_listACTORS.push_back(new Pit(x, y, this)); // adds new pit
                    break;
                    
                case Level::crystal:
                    m_listACTORS.push_back(new Crystal(x, y, this)); // adds new crystal
                    m_numCrystals++; // increase num of crystals by 1
                    break;
                    
                case Level::restore_health:
                    m_listACTORS.push_back(new RestoreHealthGoodie(x, y, this)); // adds new rhGoodie
                    break;
                    
                case Level::extra_life:
                    m_listACTORS.push_back(new ExtraLifeGoodie(x, y, this)); // adds new elGoodie
                    break;
                    
                case Level::ammo:
                    m_listACTORS.push_back(new AmmoGoodie(x, y, this)); // adds new ammoGoodie
                    break;
            }
        }
    }
    return 0;  // successful level load
}

void StudentWorld::setDisplayText()
{
     int score = getScore();
     int level = getLevel();
     int lives = getLives();
     int health = ( m_PLAYER->getHitPoints() / 20.0 ) * 100;  // makes it into percentage form - health out of 20 hit points
     int ammo = m_PLAYER->getAmmo();
     unsigned int bonus = getBonusPoints();

     string s = formatDisplay(score, level, lives, health, ammo, bonus);
    
    // Finally, update the display text at the top of the screen with newly created stats
     setGameStatText(s); // calls our provided GameWorld::setGameStatText
}
 
int StudentWorld::init()
{
    if (loadLevel() != 0)
       return loadLevel(); // level not load
    
    // else, the level DID load
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::ask_all_actors_to_doSomething()
{
                                // Note: have to do player/avatar separately, since it is not in the list
    (m_PLAYER)->doSomething();  // If player is dead, this function (ask_all_actors_to_doSomething()) wouldn't even be called
        
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->isAlive())
            (*it)->doSomething();
    }

}

int StudentWorld::move()
{
    // Update the Game Status Line
    setDisplayText();
    
    ask_all_actors_to_doSomething();
    
    if (m_PLAYER->isAlive() == false) // if player died during (last) tick
        return GWSTATUS_PLAYER_DIED;
    
    if (getCurrLevelCompletionStatus() == true) // if player completed current level
        return GWSTATUS_FINISHED_LEVEL;  // note: bonus is added to player in exit implementation

    removeDeadActors(); // remove newly dead actors after each tick
    
    if (getBonusPoints() > 0)  // bonus points should not go below zero
        reduceBonusByOne();  // reduce current bonus for the level by 1
        
	return GWSTATUS_CONTINUE_GAME;
}


void StudentWorld::cleanUp()
{
    list<Actor*>::iterator it = m_listACTORS.begin();
        
    while (it!= m_listACTORS.end())
    {
            delete *it;  // deletes the ACTOR pointed to by this Actor* in the list
            it = m_listACTORS.erase(it);  // deletes the actor POINTER in list
    }
    
    // also need to delete the player since dynamically allocated
    if (m_PLAYER != nullptr)  // but need to check it's not nullptr (would happen if level load causes player to automatically win, for instance)
        delete m_PLAYER;
    
    // reset all the private member variables of StudentWorld
    m_PLAYER = nullptr;
    m_avatarExists = false;
    m_levelComplete = false;
    m_bonus = 1000;
    m_numCrystals = 0;
}

void StudentWorld::removeDeadActors()  // removes all dead actors (except for the player) from each tick
{
    list<Actor*>::iterator it = m_listACTORS.begin();
        
    while (it!= m_listACTORS.end())
    {
            if ((*it)->isAlive() == false) // if actor is dead
            {
                delete *it;  // deletes the ACTOR pointed to by this Actor* in the list
                it = m_listACTORS.erase(it);  // deletes the actor POINTER in list
            }
            else
                it++;
    }
}

bool StudentWorld::canAgentMoveTo(int x, int y, int dx, int dy, Actor* a) // can a robot or player move to a location
{
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->getX() == x + dx && (*it)->getY() == y + dy && (*it)->allowsAgentColocation() == false)  // if agent is obstructed
        {
            if ((*it)->isPushable() && (*it)->bePushedBy(a, x + (2 * dx), y + (2 * dy))) // check if it is a marble that can be pushed in dir of...
                                                                                         // ...player (note: only a player is allowed to push marble)
            {
               // if bePushedBy is true, then marble can be pushed by this actor and is unobstructed
                (*it)->move(a->getDirection());  // move marble in direction the player is facing
                return true; // so player knows to move too
            }
                
            return false; // object is in the way
        }
    }
    
    if (m_PLAYER->getX() == x + dx && m_PLAYER->getY() == y + dy)  // check if player is not in way - only could happen if a is a robot
        return false; // player is in way of agent (robot)

    return true; // nothing in way, or it allows agent colocation
}

bool StudentWorld::canMarbleMoveTo(int x, int y)  // sees if a marble can move to a certain spot when pushed
{                                                 // this is called in bePushedBy(), which is called by canAgentMoveTo()
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->allowsMarble() == false)  // note: another marble's allowsMarble() == false
            return false;
    }
    
    return true;
}


bool StudentWorld::doesStopPea(int x, int y)  // if an object stops a pea (without taking damage itself)
{
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->getX() == x && (*it)->getY() == y && (*it)->stopsPea())
            return true; // pea gets stopped
    }
    
    return false; // pea does not get stopped
}


void StudentWorld::addActor(Actor* a)
{
    m_listACTORS.push_back(a); // adds new actor to list
}


bool StudentWorld::damageSomething(Actor* a, int damageAmt)
{
    // if a (always a pea - since only pea will call it) is on same spot as something that can be damaged - (e.g. robots, marbles)
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->getX() == a->getX() && (*it)->getY() == a->getY() && (*it)->isDamageable())
        {
            (*it)->damage(damageAmt);
            return true; // damaged object
        }
    }
    
    // now need to check if avatar can be damaged (since it is outside of the list)
    if (m_PLAYER->getX() == a->getX() && m_PLAYER->getY() == a->getY())  // already know player is damageable
    {
        m_PLAYER->damage(damageAmt);
        return true; // damaged avatar/player
    }
    
    return false;
}

bool StudentWorld::swallowSwallowable(Actor* a)
{
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->getX() == a->getX() && (*it)->getY() == a->getY() && (*it)->isSwallowable())
        {
            (*it)->setDead(); // set marble to dead so removeDeadActors() will destroy it next tick
            return true; // will swallow marble, so need to also destroy pit
        }
    }
    
    return false;
}

bool StudentWorld::existsClearShotToPlayer(int x, int y, int dx, int dy)    // only called by shootable robots
{
    int playerX = getPlayer()->getX();
    int playerY = getPlayer()->getY();
    // NOTE: robot and player x is column, NOT row - and y is row, NOT column
    
//------for Horizontal Ragebots (and Mean Thiefbots) -------
    if (y == playerY) // if shootable bot is in same row as player
    {
        if (dx == -1 && dy == 0 &&  x > playerX) // if the bot is facing left & the player is to the left of it
        {
            // check if there are obstacles
            for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
            {
                // check if something else that stops a pea (with or without taking damage itself) is in between bot and player too
                if (((*it)->stopsPea() || (*it)->isDamageable()) && y == (*it)->getY() && x > (*it)->getX() && playerX < (*it)->getX())
                    return false; // no clear shot to player
            }
            return true;  // otherwise, unobstructed path to player
        }
        
        else if (dx == 1 && dy == 0 && x < playerX) // if the bot is facing right & the player is to the right of it
        {
            // check if there are obstacles
            for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
            {
                    if (((*it)->stopsPea() || (*it)->isDamageable()) && y == (*it)->getY() && x < (*it)->getX() && playerX > (*it)->getX())
                    return false; // no clear shot to player
            }
            return true;  // otherwise, unobstructed path to player
        }
    }
    
//------for Vertical Ragebots (and Mean Thiefbots) -------
    if (x == playerX) // if shootable bot is on same column as player
    {
        if (dx == 0 && dy == 1 && y < playerY) // if the bot is facing up & the player is above it
        {
            // check if there are obstacles
            for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
            {
                if (((*it)->stopsPea() || (*it)->isDamageable()) && x == (*it)->getX() && y < (*it)->getY() && playerY > (*it)->getY())
                    return false; // no clear shot to player
            }
            return true; // otherwise, unobstructed path to player
        }
        
        else if (dx == 0 && dy == -1 && y > playerY)  // if the bot is facing down & the player is below it
        {
            // check if there are obstacles
            for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
            {
                if (((*it)->stopsPea() || (*it)->isDamageable()) && x == (*it)->getX() && y > (*it)->getY() && playerY < (*it)->getY())
                    return false; // no clear shot to player
            }
            return true; // otherwise, unobstructed path to player
        }
    }
    
    return false; // bot not in same row or col as player, so no clear shot to player
}

 
bool StudentWorld::doFactoryCensus(int x, int y, int distance, int& count)
{
    // set boundaries of maze - 3 blocks each direction from the factory
    int minX = x - distance;
    int maxX = x + distance;
    int minY = y - distance;
    int maxY = y + distance;
    
    // if bounds are past boundaries of maze, just set that element to maze boundary
    if (minX < 0) minX = 0;
    if (maxX > 14) maxX = 14;
    if (minY < 0) minY = 0;
    if (maxY > 14) maxY = 14;
    
    count = 0;
    
    // check how many thiefbots of any type are within the bounds and if any thiefbot is ON the factory
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->countsInFactoryCensus() && (*it)->getX() == x && (*it)->getY() == y)
            return false; // there is a thiefbot ON factory
        
        if ((*it)->countsInFactoryCensus() && minX <= (*it)->getX() && maxX >= (*it)->getX())   // if is thiefbot type and within X bounds
        {
            if (minY <= (*it)->getY() && maxY >= (*it)->getY())  // and if is also within Y bounds
                count++;    // then increase the count
        }
    }
    if (count >= 3)  // if count is NOT less than 3, then return false
        return false;
    
    return true;  // else, less than 3 thiefbots within bounds of factory, and NO thiefbot ON factory
}

Actor* StudentWorld::getColocatedStealable(int x, int y)  // only a thiefbot type will call this function
{
    for (list<Actor*>::iterator it = m_listACTORS.begin(); it!= m_listACTORS.end(); it++)
    {
        if ((*it)->isStealable() && (*it)->getX() == x && (*it)->getY() == y)
            return *it; // there is a goodie where the thiefbot is at
    }
    
    return nullptr;  // there is no goodie for the thiefbot to pick up
}


//---------------------- Getter method implementations ----------------------
Avatar* StudentWorld::getPlayer()
{
    return m_PLAYER;
}

int StudentWorld::getBonusPoints()
{
    return m_bonus;
}

int StudentWorld::getNumCrystals()
{
    return m_numCrystals;
}

bool StudentWorld::getCurrLevelCompletionStatus()
{
    return m_levelComplete;
}


//---------------------- Setter method implementations ----------------------
void StudentWorld::reduceBonusByOne() // reduces bonus by one (done per tick)
{
    m_bonus--;
}

void StudentWorld::reduceCrystalsByOne()
{
    m_numCrystals--;
}

void StudentWorld::setCurrLevelCompletionStatus(bool b)
{
    m_levelComplete = b;
}
