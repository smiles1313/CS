#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <vector>

#include "hashmap.h"
#include "geodb.h"
#include "router.h"
#include "tour_generator.h"
#include "stops.h"
#include "tourcmd.h"

using namespace std;


void print_tour(vector<TourCommand>& tcs)
{
    double total_dist = 0;
    std::string direction;
    double street_distance = 0;

    cout << "Starting tour...\n";

    for (size_t i = 0; i < tcs.size(); ++i)
    {
        if (tcs[i].get_command_type() == TourCommand::commentary)
        {
            cout << "Welcome to " << tcs[i].get_poi() << "!\n";
            cout << tcs[i].get_commentary() << "\n";
        }
        else if (tcs[i].get_command_type() == TourCommand::turn)
        {
            cout << "Take a " << tcs[i].get_direction() << " turn on " << tcs[i].get_street() << endl;
        }
        else if (tcs[i].get_command_type() == TourCommand::proceed)
        {
            total_dist += tcs[i].get_distance();
            if (direction.empty())
                direction = tcs[i].get_direction();
            street_distance += tcs[i].get_distance();
            if (i+1 < tcs.size() && tcs[i+1].get_command_type() == TourCommand::proceed
                && tcs[i+1].get_street() == tcs[i].get_street() && tcs[i].get_street() != "a path")
            {
                continue;
            }

            cout << "Proceed " << std::fixed << std::setprecision(3) << street_distance << " miles " << direction << " on " << tcs[i].get_street() << endl;
            street_distance = 0;
            direction.clear();
        }
    }

    cout << "Your tour has finished!\n";
    cout << "Total tour distance: " << std::fixed << std::setprecision(3) << total_dist << " miles\n";
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cout << "usage: BruinTour mapdata.txt stops.txt\n";
        return 1;
    }

    GeoDatabase geodb;
    if (!geodb.load(argv[1]))
    {
        cout << "Unable to load map data: " << argv[1] << endl;
        return 1;
    }

    Router router(geodb);
    TourGenerator tg(geodb, router);

    Stops stops;
    if (!stops.load(argv[2]))
    {
        cout << "Unable to load tour data: " << argv[2] << endl;
        return 1;
    }

    std::cout << "Routing...\n\n";

    vector<TourCommand> tcs = tg.generate_tour(stops);
    if (tcs.empty())
        cout << "Unable to generate tour!\n";
    else
        print_tour(tcs);
}




/*
void foo()
{

    // Define a hashmap that maps strings to doubles and has a maximum
    // load factor of 0.2. It will initially have 10 buckets when empty.
    HashMap<double> nameToGPA(0.2);
    // Add new items to the hashmap
    nameToGPA.insert("Carey", 3.5); // Carey has a 3.5 GPA
    nameToGPA.insert("David", 2.99); // David needs to up his game
    
    // You can also use brackets like C++'s unordered_map!
    nameToGPA["Annie"] = 3.85; // Adds Annie, who has the highest GPA of all.
    // Since adding Annie would push the load factor from 0.2 to 0.3, exceeding the
    // maximum load factpr of 0.2, adding Annie caused the hashmap to increase the
    // number of buckets, forcing a rehash of all items.
    double* davidsGPA = nameToGPA.find("David");
    if (davidsGPA != nullptr)
        *davidsGPA = 3.1; // after a re-grade of David's exam, update 2.99 -> 3.1
    nameToGPA.insert("Carey", 4.0); // Carey deserves a 4.0
    // sees if linda is in the map; if not, creates a new entry for linda in map
    cout << nameToGPA["Linda"] << endl; // prints zero
}
*/

/*
int main()
{
    GeoDatabase geodb;
    Router r(geodb);
    TourGenerator tourBot(geodb, r);
    Stops stops;
    
    GeoPoint p;
    GeoPoint test("34.0728395", "-118.4516549");
    vector<GeoPoint> vGP;  // empty vector
    string street;         // empty string
    
    geodb.load("/Users/s.miles1313/Desktop/mapdata.txt");
    stops.load("/Users/s.miles1313/Desktop/stops.txt");
    
    vector<TourCommand> tCommands = tourBot.generate_tour(stops);
    // for (auto& command : tCommands)
    //    cerr << command << endl;
    

    
//------------------- ROUTER TESTS ---------------------

    // vGP = r.route(GeoPoint("34.0446433", "-118.4975646"), GeoPoint("34.0372630", "-118.4889476"));  // routes actual path (along 21 street)
    // vGP = r.route(GeoPoint("34.0372630", "-118.4889476"), GeoPoint("34.0446433", "-118.4975646"));  // routes above path, but backwards
    // vGP = r.route(GeoPoint("34.0446433", "-118.4975646"), GeoPoint("34.0446433", "-118.4975646"));  // same pt, so returns (only the 1) pt
    // vGP = r.route(GeoPoint("34.0725855", "-118.4513525"), GeoPoint("34.0718478", "-118.4511253"));  // (1st) midpoint to end of a street
    // cerr << endl;
    //
    // vGP = r.route(GeoPoint("34.0728142", "-118.4513468"), GeoPoint("34.0718478", "-118.4511253"));  // POI (Delta Terrace) to end of a street
                                                                                            // THIS GOES POI -> midpoint -> street segments!

    // for (auto& point : vGP)
    //     cerr << point.to_string() << endl;
    //
    

// Delta Terrace|34.0728142 -118.4513468
// Rieber Hall|34.0724560 -118.4514651
//
// Midpoint: 34.0725855,-118.4513525
// Midpoint: 34.0725855,-118.4513525
// Midpoint: 34.0721072,-118.4511615

    
//------------------- GEODB TESTS ---------------------
    // geodb.get_poi_location("Rieber Hall", p);
    // cerr << p.to_string() << endl;
    
    // vGP = geodb.get_connected_points(test);
    //
    // for (auto it = vGP.begin(); it != vGP.end(); ++it)
    // {
    //     cerr << (*it).to_string() << endl;
    // }
    //
    // street = geodb.get_street_name(GeoPoint("34.0726607", "-118.4514721"), GeoPoint("34.0725855", "-118.4513525"));
    // cerr << street << endl;;
    
//----------------- GEODB LOAD TESTS -------------------
    // foo();
//------------------------------------------------------
}
 
*/
 
 
 
//---------------- TOURGENERATOR TESTS - WITH OWN MAIN ------------------
// int main() {
//     GeoDatabase g;
//     g.load("/Users/s.miles1313/Desktop/mapdata.txt");
//     Stops s = Stops();
//     s.load("/Users/s.miles1313/Desktop/stops.txt");
//
//     Router r(g);
//
//     TourGenerator tg(g, r);
//     vector<TourCommand> instructions = tg.generate_tour(s);
//
//     for (const auto& instruction : instructions)
//     {
//         if (instruction.get_command_type() == TourCommand::TOUR_COMMAND::invalid) {
//             cout << "unlucky";
//         }
//         else if (instruction.get_command_type() == TourCommand::TOUR_COMMAND::turn) {
//             cout << "Turn " << instruction.get_direction() << " onto " << instruction.get_street() << endl;
//         }
//         else if (instruction.get_command_type() == TourCommand::TOUR_COMMAND::proceed) {
//             GeoPoint p1;
//             GeoPoint p2;
//             instruction.get_points(p1, p2);
//             cout << instruction.get_distance() << " miles " << instruction.get_direction() << " on " << instruction.get_street() << ", from " << p1.to_string() << " to " << p2.to_string() << endl;
//         }
//         else if (instruction.get_command_type() == TourCommand::TOUR_COMMAND::commentary) {
//             cout << instruction.get_poi() << " - " << instruction.get_commentary() << endl;
//         }
//
//     }
//
// }
//-----------------------------------------------------------------------


