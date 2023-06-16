#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

typedef std::vector<std::pair<std::string, std::string>> vResistorPair_t;

/// @brief  An undirected multigraph of vertices and edges
class cMultiGraph
{
public:
    struct cEdge
    {
        int v1;
        int v2;
        std::string name;
    };

    void add(
        const std::string &v1,
        const std::string &v2,
        const std::string &name);

    std::vector<cEdge> edgeList() const
    {
        return vEdge;
    }

    std::string vertexName(int i) const
    {
        return myVertexName[i];
    }

    int findVertex(const std::string &name) const;

    int vertexDegree(int vi)
    {
        return vVertexEdges[vi].size();
    }

    void displayEdges() const;

private:
    std::vector<std::string> myVertexName;

    std::vector<cEdge> vEdge;

    /// store index of all edges connected to vertices
    std::vector<std::vector<int>> vVertexEdges;
};

int cMultiGraph::findVertex(const std::string &name) const
{
    auto it = std::find(myVertexName.begin(), myVertexName.end(), name);
    if (it == myVertexName.end())
        return -1;
    return it - myVertexName.begin();
}

void cMultiGraph::add(
    const std::string &v1name,
    const std::string &v2name,
    const std::string &edgename)
{
    cEdge e;
    e.name = edgename;
    e.v1 = findVertex(v1name);
    if (e.v1 == -1)
    {
        e.v1 = myVertexName.size();
        myVertexName.push_back(v1name);
        vVertexEdges.push_back({});
    }

    e.v2 = findVertex(v2name);
    if (e.v2 == -1)
    {
        e.v2 = myVertexName.size();
        myVertexName.push_back(v2name);
        vVertexEdges.push_back({});
    }
    int ei = vEdge.size();
    vEdge.push_back(e);
    vVertexEdges[e.v1].push_back(ei);
    vVertexEdges[e.v2].push_back(ei);
}
void cMultiGraph::displayEdges() const
{
    for (auto &e : vEdge)
        std::cout << vertexName(e.v1) << " " << vertexName(e.v2)
                  << " connected by " << e.name << "\n";
}

std::vector<std::string> tokenize(const std::string &line)
{
    std::vector<std::string> ret;
    std::stringstream sst(line);
    std::string a;
    while (getline(sst, a, ' '))
        ret.push_back(a);
    return ret;
}

/// @brief read input file
/// @param[out] g graph to populate
/// @param[out] q queries
/// @param[in] fname input filename
void read(
    cMultiGraph &g,
    vResistorPair_t &q,
    const std::string &fname)
{
    std::ifstream ifs(fname);
    if (!ifs.is_open())
        throw std::runtime_error("No input");

    std::string line;
    while (getline(ifs, line))
    {
        auto vtoken = tokenize(line);
        if (!vtoken.size())
            continue;
        if (vtoken.size() > 2)
        {
            g.add(vtoken[1], vtoken[2], vtoken[0]);
        }
        else
        {
            q.push_back(std::make_pair(vtoken[0], vtoken[1]));
        }
    }
}
/** @brief Mark parallel resistors
 * @param[in] circuit gaph
 * @return vector of resistor pairs in parallel
 *
 * Resitors are parallel if they connect the same two nodes
 */
vResistorPair_t markParallel(cMultiGraph &g)
{
    vResistorPair_t ret;

    // loop over resistor pairs
    for (auto &e1 : g.edgeList())
    {
        for (auto &e2 : g.edgeList())
        {
            // check resistore are different and in alphabetical order
            if (e1.name >= e2.name)
                continue;

            // check that they connect the same two nodes
            if ((e1.v1 == e2.v1 && e1.v2 == e2.v2) ||
                (e1.v1 == e2.v2 && e1.v2 == e2.v1))
                ret.push_back(
                    std::make_pair(e1.name, e2.name));
        }
    }
    return ret;
}
/** @brief Mark series resistors
 * @param[in] circuit gaph
 * @return vector of resistor pairs in series
 *
 * Resitors are series if they share a node that is not connected to any other resitors
 */
vResistorPair_t markSeries(cMultiGraph &g)
{
    vResistorPair_t ret;

    // loop over resistor pairs
    for (auto &e1 : g.edgeList())
    {
        for (auto &e2 : g.edgeList())
        {
            // resistor not in series with itslf
            if (e1.name == e2.name)
                continue;

            // find shared node
            int sharedVertex = -1;
            if (e1.v1 == e2.v1 || e1.v1 == e2.v2)
                sharedVertex = e1.v1;
            else if (e1.v2 == e2.v1 || e1.v2 == e2.v2)
                sharedVertex = e1.v2;
            if (sharedVertex == -1)
                continue;

            // check no other resitor connected to shred node
            if (g.vertexDegree(sharedVertex) != 2)
                continue;

            // store pair in alphabetical order
            if (e1.name < e2.name)
                ret.push_back(std::make_pair(e1.name, e2.name));
            else
                ret.push_back(std::make_pair(e1.name, e2.name));
        }
    }
    return ret;
}
bool isMarked(
    const std::pair<std::string, std::string> &query,
    vResistorPair_t marks)
{
    std::pair<std::string, std::string> p;
    if (query.first < query.second)
        p = std::make_pair(query.first, query.second);
    else
        p = std::make_pair(query.second, query.first);
    return (std::find(marks.begin(), marks.end(), p) != marks.end());
}

main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Usage: resistor <path to data file>\n";
        exit(1);
    }

    cMultiGraph g;          // the circuit graph
    vResistorPair_t q;      // the queries

    read(g, q, argv[1]);    // read the input file
    g.displayEdges();       // display input

    vResistorPair_t vParallel = markParallel(g);    // find resistor pairs in parallel   
    vResistorPair_t vSeries = markSeries(g);        // find resistor pairs in series

    // loop over queries
    for (auto &rp : q)
    {
        if (isMarked(rp, vParallel))
            std::cout << rp.first << " and "
                      << rp.second << " parallel\n";
        else if (isMarked(rp, vSeries))
            std::cout << rp.first << " and "
                      << rp.second << "  series\n";
        else
            std::cout << rp.first << " and "
                      << rp.second << "  nothing\n";
    }

    return 0;
}
