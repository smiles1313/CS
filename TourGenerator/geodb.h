//
//  geodb.h
//  cs32Project4
//
//  Created by Sophia Miles on 3/11/24.
//

#ifndef geodb_h
#define geodb_h

#include "base_classes.h"
#include "hashmap.h"
#include <vector>

class GeoDatabase: public GeoDatabaseBase
{
public:
 GeoDatabase();
 virtual ~GeoDatabase();
 virtual bool load(const std::string& map_data_file);
 virtual bool get_poi_location(const std::string& poi,GeoPoint& point) const;
 virtual std::vector<GeoPoint> get_connected_points(const GeoPoint& pt) const;
 virtual std::string get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const;
    
private:
    HashMap<GeoPoint> m_nameToPOI;  // string POI name key - GeoPoint POI value
    HashMap<std::string> m_gpToStreet;     // GeoPoint + GeoPoint key - string Street Name value (includes midpoints & is added bilateral)
    HashMap<std::vector<GeoPoint>> m_gpTogp;            // GeoPoint key - GeoPoint value (includes midpoints & is BILATERAL)...
                                                        // ... so start and end points mapped, AS WELL AS end to start!
};

#endif /* geodb_h */
