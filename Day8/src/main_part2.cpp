#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <chrono>

using namespace std;

// Union-Find (Disjoint Set Union) data structure
class UnionFind
{
private:
    vector<int> parent;
    vector<int> rank;
    int num_components;

public:
    UnionFind(int n) : parent(n), rank(n, 0), num_components(n)
    {
        for (int i = 0; i < n; i++)
        {
            parent[i] = i;
        }
    }

    int find(int x)
    {
        if (parent[x] != x)
        {
            parent[x] = find(parent[x]); // path compression
        }
        return parent[x];
    }

    bool unite(int x, int y)
    {
        int root_x = find(x);
        int root_y = find(y);

        if (root_x == root_y)
        {
            return false; // already in same set
        }

        // union by rank
        if (rank[root_x] < rank[root_y])
        {
            parent[root_x] = root_y;
        }
        else if (rank[root_x] > rank[root_y])
        {
            parent[root_y] = root_x;
        }
        else
        {
            parent[root_y] = root_x;
            rank[root_x]++;
        }

        num_components--;
        return true;
    }

    int get_num_components() const
    {
        return num_components;
    }
};

struct Junction
{
    double x, y, z;

    double distance_to(const Junction &other) const
    {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return sqrt((dx * dx) + (dy * dy) + (dz * dz));
    }
};

struct JunctionPair
{
    int id1, id2;
    double distance;
};

vector<Junction> parse_file()
{
    ifstream infile("input.txt");
    if (!infile.is_open())
    {
        exit(1);
    }

    vector<Junction> junctions;
    junctions.reserve(1000); // preallocate for known size

    string line;
    while (getline(infile, line))
    {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);

        Junction junc;
        junc.x = stod(line.substr(0, pos1));
        junc.y = stod(line.substr(pos1 + 1, pos2 - pos1 - 1));
        junc.z = stod(line.substr(pos2 + 1));

        junctions.push_back(junc);
    }

    infile.close();
    return junctions;
}

int main()
{
    auto start_time = chrono::high_resolution_clock::now();

    vector<Junction> junctions = parse_file();
    const int n = junctions.size();

    // Calculate all pairwise distances
    vector<JunctionPair> all_pairs;
    all_pairs.reserve(n * (n - 1) / 2); // preallocate exact size

    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            double dist = junctions[i].distance_to(junctions[j]);
            all_pairs.push_back({i, j, dist});
        }
    }

    // Sort all pairs by distance
    sort(all_pairs.begin(), all_pairs.end(),
         [](const JunctionPair &a, const JunctionPair &b)
         { return a.distance < b.distance; });

    // initialize union-find structure
    UnionFind uf(n);

    // process edges in order of increasing distance
    for (const auto &pair : all_pairs)
    {
        if (uf.unite(pair.id1, pair.id2))
        {

            if (uf.get_num_components() == 1)
            {
                // single circuit made
                const Junction &j1 = junctions[pair.id1];
                const Junction &j2 = junctions[pair.id2];
                cout << "Product of X coordinates: " << (uint64_t(j1.x) * uint64_t(j2.x)) << endl;
                break;
            }
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end_time - start_time;
    cout << "Elapsed time: " << elapsed.count() << " seconds" << endl;

    return 0;
}