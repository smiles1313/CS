//
//  tour_generator.cpp
//  cs32Project4

#include "tour_generator.h"
#include "geotools.h"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

//------------------------ Helper funcs ------------------------
string angleToDirection(double angle)  // will never be called if angle is <= 1 degree or >= 359 degrees
{
    if (angle >= 0 && angle < 22.5)
        return "east";
    
    else if (angle >= 22.5 && angle < 67.5)
        return "northeast";
    
    else if (angle >= 67.5 && angle < 112.5)
        return "north";
    
    else if (angle >= 112.5 && angle < 157.5)
        return "northwest";
    
    else if (angle >= 157.5 && angle < 202.5)
        return "west";
    
    else if (angle >= 202.5 && angle < 247.5)
        return "southwest";
    
    else if (angle >= 247.5 && angle < 292.5)
        return "south";
    
    else if (angle >= 292.5 && angle < 337.5)
        return "southeast";
    
    else // if angle >= 337.5  - but also < 359 implicitly
        return "east";
}

string angleToLeftRight(double angle)
{
    if (angle >= 1 && angle < 180)
        return "left";
    
    else // if angle is >= 180 and <= 359
        return "right";
}

//-------------------------------------------------------------


TourGenerator::TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router)
    : m_geodb(geodb), m_router(router)
{}

TourGenerator::~TourGenerator()
{}

vector<TourCommand> TourGenerator::generate_tour(const Stops& stops) const
{
    vector<TourCommand> instructions;  // creates empty vector to hold instructions
    vector<TourCommand> empty;         // creates an empty vector to return if no route possible or an unknown POI is passed in as a stop
    
    for (int i = 0; i < stops.size(); i++) // For each point of interest P held by the Stops object.
    {
        string poi_key;
        GeoPoint poi_val;
        string talkingPts;
        TourCommand c;  // creates Commentary TourCommand object
        
        stops.get_poi_data(i, poi_key, talkingPts);
        c.init_commentary(poi_key, talkingPts);   // specifies what should be said about current poi
        instructions.push_back(c);  // adds commentary about poi to end of instructions vector
        
        m_geodb.get_poi_location(poi_key, poi_val); // sets poi_val
        
        
        if (i < (stops.size() - 1))  // if there is another poi after this one (aka, if not the last item in vector)
        {
            vector<GeoPoint> stopToStopPath;
            string nextPoi;
            GeoPoint nextPoi_val;
            string nextTalkingPts;  // dummy holder - ignore
            stops.get_poi_data(i + 1, nextPoi, nextTalkingPts);  // gets the next poi's geopoint KEY
            
            m_geodb.get_poi_location(nextPoi, nextPoi_val); // transforms the next poi's key to get the actual GeoPoint val
            
            stopToStopPath = m_router.route(poi_val, nextPoi_val); // generate a pt-to-pt route between the cur and next poi geopoints
            
            if (stopToStopPath.empty())  // returns an empty vector if no route possible for 0 thru n-1 stops (where n is total # of stops)
                return empty;            // also for if unknown POI is a stop - since .route calls .get_connected_points, which returns empty vector
            
            // using the path, stored in stopToStopPath, generate a sequence of TourCommands with proceed and turn instructions
                for (int k = 0; k < (stopToStopPath.size() - 1); k++) // adds a proceed tour command - size() - 1 since accessing [k+1]
            {
                //------- Proceed -------
                TourCommand p;  // creates Proceed TourCommand object
                GeoPoint gp1 = stopToStopPath[k];     // start of segment
                GeoPoint gp2 = stopToStopPath[k + 1];   // end of segment
                string dir;      // holds direction
                string street;   // holds street name
                double distance; // holds distance (in miles)
                
                dir = angleToDirection(angle_of_line(gp1, gp2));  // sets (inter-/)cardinal direction
                street = m_geodb.get_street_name(gp1, gp2);       // sets street name to corresponding geopts
                distance = distance_earth_miles(gp1, gp2);
                
                p.init_proceed(dir, street, distance, gp1, gp2);  // specifies proceed info
                instructions.push_back(p);  // adds proceed info to end of instructions vector
                
                
                //--------- Turn ---------
                // Add a turn tour command to the output vector after each proceed tour command IF AND ONLY IF:
                        // there is a GeoPoint p3 directly after p2 on the path (which is always true due to if statement), AND
                if (k < (stopToStopPath.size() - 2))  // need to check because trying to access at index [k + 2]
                    {
                    GeoPoint gp3 = stopToStopPath[k + 2];   // end of NEXT segment
                    string possibleNewStreet = m_geodb.get_street_name(gp2, gp3);
                    
                    // if the street/path name of first segment != second segment's
                    if (street != possibleNewStreet && possibleNewStreet != "")  // if the street is different AND not empty (aka there is no street)
                    {
                        double angle = angle_of_turn(gp1, gp2, gp3); ;  // holds the angle created by first segment and second segment
                        
                        if (angle >= 1 && angle <= 359)    // if angle is >= 1 degree and <= 359 degrees
                        {
                            // THEN, add turn command
                            TourCommand t;  // creates Turn TourCommand object
                            t.init_turn(angleToLeftRight(angle), possibleNewStreet);
                            instructions.push_back(t);  // adds turn info to end of instructions vector
                        }
                    }
                }
            }
        }
    }
    return instructions; // will be empty if no commands were added - aka no route possible or an unknown poi was passed in as a stop
}
//-------------------------------------------------------------------------------------------------------------------------

