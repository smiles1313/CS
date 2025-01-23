//
//  geodb.cpp
//  cs32Project4

#include "geodb.h"
#include "geotools.h"
#include <iostream> // needed for any I/O
#include <fstream>  // needed in addition to <iostream> for file I/O
#include <sstream>  // needed in addition to <iostream> for string stream I/O
#include <string>

using namespace std;

 GeoDatabase::GeoDatabase()
 {}
    
GeoDatabase::~GeoDatabase()
 {}

bool GeoDatabase::load(const string& map_data_file)
 {
      ifstream inf(map_data_file);    // Open the file for input
    
      if (!inf)
          return false; // data failed to load
    
      int count = 0;
      string line;
      string name;  // name of street or POI
      GeoPoint s1;
      GeoPoint s2;
      GeoPoint mid;

      while (getline(inf, line))  // while end of file is not yet reached
      {
          istringstream iss(line);  // of the line
          string segment;
          vector<string> segments;  // for street geopoints
          int numOfPOI;
          string longitude; // for POI long
          string latitude; // for POI lat
          
          // count = 1  reads the key
          // count = 2  reads the coords
          // count = 3  reads the POIs
          count++;
          
          if (count == 1)
              name = line;
          
          if (count == 2)
          {
              while (iss >> segment)
              {
                  segments.push_back(segment);  // pos[0] = long1, [1] = lat1, [2] = long2, [3] = lat2
              }
              
              s1 = GeoPoint(segments[0], segments[1]);  // set s1
              s2 = GeoPoint(segments[2], segments[3]);  // set s2
              
              // Insert start + end AND end + start geopoints on the line to map to street name
              m_gpToStreet.insert(s1.to_string() + s2.to_string(), name);
              m_gpToStreet.insert(s2.to_string() + s1.to_string(), name);
              
              // Insert start and end geopoints on the line to map to each other
              m_gpTogp[s1.to_string()].push_back(s2);  // pushes s2 into value vector for key/node s1
              m_gpTogp[s2.to_string()].push_back(s1);  // pushes s1 into value vector for key/node s2
          }
          
          if (count == 3)
          {
              iss >> numOfPOI;
              
              mid = midpoint(s1, s2);  // set midpoint
              
              if (numOfPOI > 0)  // calculate and store midpoint of street segment
              {
                  // Make bilateral connection bw s1 and mid
                  m_gpTogp[s1.to_string()].push_back(mid);   // pushes mid into value vector for key/node s1
                  m_gpTogp[mid.to_string()].push_back(s1);   // pushes s1 into value vector for key/node mid
                  
                  // Assigns street name for s1 + mid and mid + s1
                  m_gpToStreet.insert(s1.to_string() + mid.to_string(), name);  // CHECK BY CERR AND SEE WHAT NAME IS BY THIS TIME!!
                  m_gpToStreet.insert(mid.to_string() + s1.to_string(), name);

                  // Make bilateral connection bw s2 and mid
                  m_gpTogp[s2.to_string()].push_back(mid);   // pushes mid into value vector for key/node s2
                  m_gpTogp[mid.to_string()].push_back(s2);   // pushes s2 into value vector for key/node mid
                  
                  // Assigns street name for s2 + mid and mid + s2
                  m_gpToStreet.insert(s2.to_string() + mid.to_string(), name);  // CHECK BY CERR AND SEE WHAT NAME IS BY THIS TIME!!
                  m_gpToStreet.insert(mid.to_string() + s2.to_string(), name);
              }
              
              while (!segments.empty())  // clears out the vector from count == 2 if statement
                  segments.pop_back();
                  
              while (numOfPOI > 0)
              {
                  getline(inf, line); // Read the next line from 'inf'

                  istringstream line_stream(line); // Create a stringstream from the line

                  getline(line_stream, name, '|'); // Read until '|' into 'name'
                  line_stream >> latitude >> longitude; // Read latitude and longitude from line_stream
                  GeoPoint poi = GeoPoint(latitude, longitude);
                  
                  m_nameToPOI.insert(name, poi);
                  
                  // Make bidirectional connection bw POI and midpoint
                  m_gpTogp[poi.to_string()].push_back(mid);   // pushes mid into value vector for key/node poi
                  m_gpTogp[mid.to_string()].push_back(poi);   // pushes poi into value vector for key/node mid
                  
                  // Assign street name "a path" for both keys
                  m_gpToStreet.insert(poi.to_string() + mid.to_string(), "a path");
                  m_gpToStreet.insert(mid.to_string() + poi.to_string(), "a path");
                  
                  --numOfPOI;
              }
              // and reset count to 0
              count = 0;
          }
      }
          return true;
 }

bool GeoDatabase::get_poi_location(const string& poi, GeoPoint& point) const
 {
     const GeoPoint* temp = m_nameToPOI.find(poi);  // holds nullptr or the GP of poi if found
     
     if (temp == nullptr)
         return false;
     
     else
     {
         point = *temp;
         return true;
     }
 }
    
vector<GeoPoint> GeoDatabase::get_connected_points(const GeoPoint& pt) const
 {
    vector<GeoPoint> gp;  // empty vector
     
     if (m_gpTogp.find(pt.to_string()) != nullptr)
         return *(m_gpTogp.find(pt.to_string()));
     
    return gp;  // returns empty vector if not find
 }
    
string GeoDatabase::get_street_name(const GeoPoint& pt1, const GeoPoint& pt2) const
 {
    const std::string* result = m_gpToStreet.find(pt1.to_string() + pt2.to_string());
    
    if (result != nullptr)
        return *result;
     
     else
         return "";
 }
