//
//  tour_generator.h
//  cs32Project4
//
//  Created by Sophia Miles on 3/13/24.
//


#ifndef tour_generator_h
#define tour_generator_h

#include "base_classes.h"
#include "tourcmd.h"   // also included in base_classes.h
#include "stops.h"     // also included in base_classes.h
#include <vector>

class TourGenerator: public TourGeneratorBase
{
public:
 TourGenerator(const GeoDatabaseBase& geodb, const RouterBase& router);  // in base_classes.h
 virtual ~TourGenerator();
 virtual std::vector<TourCommand> generate_tour(const Stops& stops) const;  // TourCommand defined in tourcmd.h, Stops defined in stops.h
    
private:
    const GeoDatabaseBase& m_geodb;
    const RouterBase& m_router;
};

#endif // tour_generator_h

