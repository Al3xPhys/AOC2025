#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>
#include <set>
#include <cmath>

using namespace std;

/* read in a list of 3d coordinates, calculate euclidiean distance between each one. */

//  define a struct for each 3d point called "Junction"
struct Junction
{
    /* each Junction has x,y,z coordinates and an id string */
    double x, y, z;
    uint64_t id;

    double distance_to(const Junction &other) const
    {
        /* calculate euclidean distance to another Junction */
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return sqrt((dx * dx) + (dy * dy) + (dz * dz));
    }
};

struct JunctionList
/* junction list containing multiple Junction structs */
{
    vector<Junction> junction_list;

    void add_junction(const Junction &junc)
    {
        junction_list.push_back(junc);
    }
};

struct Network
{
    // junctions can be added to the network, and connections between junctions can be defined
    vector<Junction> junctions;

    bool is_in_network(uint64_t junction_id) const
    {
        for (const auto &junc : junctions)
        {
            if (junc.id == junction_id)
            {
                return true;
            }
        }
        return false;
    }

    void add_junction(const Junction &junc)
    {
        junctions.push_back(junc);
    }
};

struct JunctionPair
{
    uint64_t id1, id2;
    double distance;
};

// helper function to find which network (by index) contains a junction ID
int find_network_containing(const vector<Network> &networks, uint64_t junction_id)
{
    for (int i = 0; i < networks.size(); ++i)
    {
        if (networks[i].is_in_network(junction_id))
        {
            return i;
        }
    }
    return -1; // not in any network
}

// helper function to get junction by ID from junction list
Junction get_junction_by_id(const JunctionList &jlist, uint64_t id)
{
    for (const auto &junc : jlist.junction_list)
    {
        if (junc.id == id)
        {
            return junc;
        }
    }
    // Should never happen if id is valid
    return Junction();
}

JunctionList parse_file()
{
    /*Read in a list of 3d coordinates, in the form "xx,yy,zz" per line, */
    ifstream infile("input.txt"); // Use input.txt for full data
    if (!infile.is_open())
    {
        cerr << "Error opening file!" << endl;
        exit(1);
    }

    string line;
    JunctionList junction_list; // JunctionList to hold Junction structs

    while (getline(infile, line))
    {
        stringstream ss(line); // stringstream to parse each line
        string sx, sy, sz;     // strings to hold each coordinate temporarily
        getline(ss, sx, ',');  // parse by comma
        getline(ss, sy, ',');
        getline(ss, sz, ',');

        // need to convert line into three doubles
        Junction junc;
        junc.x = stod(sx);
        junc.y = stod(sy);
        junc.z = stod(sz);
        junc.id = junction_list.junction_list.size(); // assign id based on current size of vector

        junction_list.add_junction(junc); // add Junction to list
    }

    infile.close();

    return junction_list;
}

int main()
{
    JunctionList junction_list = parse_file();

    // calcualte all pairwise distances and store in a vector of JunctionPair structs
    vector<JunctionPair> all_pairs;
    for (int i = 0; i < junction_list.junction_list.size(); ++i)
    {
        for (int j = i + 1; j < junction_list.junction_list.size(); ++j)
        {
            const auto &junc1 = junction_list.junction_list[i];
            const auto &junc2 = junction_list.junction_list[j];
            double dist = junc1.distance_to(junc2);
            all_pairs.push_back({junc1.id, junc2.id, dist});
        }
    }

    // sort all pairs by distance
    sort(all_pairs.begin(), all_pairs.end(),
         [](const JunctionPair &a, const JunctionPair &b)
         { return a.distance < b.distance; });

    // initialize each junction in its own network
    vector<Network> networks;
    for (const auto &junc : junction_list.junction_list)
    {
        Network new_network;
        new_network.add_junction(junc);
        networks.push_back(new_network);
    }

    // process the N shortest edges (even if they create cycles)
    const int MAX_EDGES_TO_PROCESS = 1000; // process 1000 shortest edges
    int edges_processed = 0;
    int connections_made = 0;

    for (const auto &pair : all_pairs)
    {
        if (edges_processed >= MAX_EDGES_TO_PROCESS)
        {
            break;
        }

        edges_processed++; // count this edge towards the limit

        int net1 = find_network_containing(networks, pair.id1);
        int net2 = find_network_containing(networks, pair.id2);

        Junction j1 = get_junction_by_id(junction_list, pair.id1);
        Junction j2 = get_junction_by_id(junction_list, pair.id2);

        if (net1 == net2)
        {
            continue;
        }


        // merge net2 into net1
        for (const auto &junc : networks[net2].junctions)
        {
            networks[net1].add_junction(junc);
        }

        // clear net2
        networks[net2].junctions.clear();
        connections_made++;

        // count current circuits
        int active_circuits = 0;
        for (const auto &net : networks)
        {
            if (!net.junctions.empty())
                active_circuits++;
        }
    }


    // any empty networks can be removed
    vector<Network> final_networks;
    for (auto &net : networks)
    {
        if (!net.junctions.empty())
        {
            final_networks.push_back(net);
        }
    }
    networks = final_networks;

    // print out the size of each network
    // cout << "\n=== Network Sizes ===" << endl;
    // for (int i = 0; i < networks.size(); ++i)
    // {
    //     cout << "Network #" << (i + 1) << " size: " << networks[i].junctions.size() << " junctions" << endl;
    // }

    // get the three largest network sizez and multiply their sizes together
    vector<size_t> network_sizes;
    for (const auto &net : networks)
    {
        network_sizes.push_back(net.junctions.size());
    }
    sort(network_sizes.begin(), network_sizes.end(), greater<size_t>());
    size_t product_of_top_three = 1;
    ;
    for (int i = 0; i < min(3, static_cast<int>(network_sizes.size())); ++i)
    {
        product_of_top_three *= network_sizes[i];
    }
    cout << "Product of the three largest network sizes: " << product_of_top_three << endl;

    return 0;
}