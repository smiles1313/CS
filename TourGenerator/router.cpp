//
//  router.cpp
//  cs32Project4
//
//  Created by Sophia Miles on 3/12/24.
//

#include "router.h"
#include <queue>  // to use priority_queue
#include <cmath>  // to use the sqrt function
#include <algorithm> // for reversing path

using namespace std;

//-------------------- Helper Struct/Funcs Implementation --------------------
struct CompareDistance
{
    const GeoPoint& destination; // member var that holds the endpt
    CompareDistance(const GeoPoint& dest)  // constructor - takes in the endpt
        : destination(dest)
    {}
    
    double distance(const GeoPoint& cur, const GeoPoint& end) const  // distance from current GeoPoint to the endpoint
    {
        return sqrt((end.latitude - cur.latitude) * (end.latitude - cur.latitude) + (end.longitude - cur.longitude) * (end.longitude - cur.longitude));  // distance formula
    }
    
    bool operator()(const GeoPoint& a, const GeoPoint& b) const  // defines a () overload class that returns true if
    {
        double distA = distance(a, destination);
        double distB = distance(b, destination);
        return distA > distB;  // makes this a min heap - so if there is smaller distance to destination, that point has higher priority
    }
};


//-------------------- Router Class Implementations --------------------

 Router::Router(const GeoDatabaseBase& geo_db)
    : m_geodb(geo_db)
 {}

 Router::~Router()
 {}

 vector<GeoPoint>Router::route(const GeoPoint& pt1, const GeoPoint& pt2) const
 {
     vector<GeoPoint> path;  // creates empty vector
     // vector<GeoPoint> temp;
     HashMap<GeoPoint> m_locOfPrevGP;  // map that associates a given geopoint G (key) to the previous geopoint P (value) in the route
     priority_queue<GeoPoint, std::vector<GeoPoint>, CompareDistance> pq((CompareDistance(pt2))); // priority queue by shortest distance to pt2 (end)
     

     // Special case: if the points are the same
     if (pt1.to_string() == pt2.to_string())
     {
         path.push_back(pt1);  // doesn't matter which point to add to vector - it's the same
         return path;    // return the vector with just the one element
     }
     
     
     // Insert starting point into priority queue
     pq.push(pt1);

     while (!pq.empty())
     {
         GeoPoint current = pq.top();
         pq.pop();
         if (current.latitude == pt2.latitude && current.longitude == pt2.longitude)  // if reached the endpoint
         {
             // Reconstruct path
             while (!(current.latitude == pt1.latitude && current.longitude == pt1.longitude))  // while not back at starting point
             {
                 path.push_back(current);
                 current = *(m_locOfPrevGP.find(current.to_string()));
             }
             path.push_back(pt1); // Add start point
             std::reverse(path.begin(), path.end()); // Reverse to get correct order
             return path;
         }

         // Look up connected points and iterate through them
         vector<GeoPoint> connectedPoints = m_geodb.get_connected_points(current); // Get connected points
         for (const auto& next : connectedPoints) {
             if (m_locOfPrevGP.find(next.to_string()) == nullptr) // if next is not already mapped to a previous waypoint - to avoid duplicates
             {
                 m_locOfPrevGP[next.to_string()] = current;
                 pq.push(next);
             }
         }
     }
     return path;            // returns empty if no path exists
 }

