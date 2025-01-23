//
//  router.h
//  cs32Project4
//
//  Created by Sophia Miles on 3/12/24.
//

#ifndef router_h
#define router_h

#include "base_classes.h"
#include "hashmap.h"
#include <vector>

class Router: public RouterBase
{
public:
 Router(const GeoDatabaseBase& geo_db);
 virtual ~Router();
 virtual std::vector<GeoPoint> route(const GeoPoint& pt1, const GeoPoint& pt2) const;
    
private:
    const GeoDatabaseBase& m_geodb;
};

#endif /* router_h */
