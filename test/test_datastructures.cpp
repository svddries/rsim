#include <era/datastructures/graph.h>
#include <era/datastructures/array_set.h>

#include <string>
#include <iostream>

int main(int argc, char **argv)
{
    {
        era::Graph<std::string, std::string> g;

        era::Id n1 = g.addNode("n1");
        era::Id n2 = g.addNode("n2");
        era::Id n3 = g.addNode("n3");

        era::Id e1 = g.addEdge(n1, n2, "a");
        era::Id e2 = g.addEdge(n1, n3, "b");
        era::Id e3 = g.addEdge(n2, n1, "c");
        era::Id e4 = g.addEdge(n3, n2, "d");

        g.node(n2) = "N2";

        g.removeNode(n1);

        g.print();
    }

    // - - - - - - - - - - - - - - - - - -

    {
        era::Graph<int, int> g;

        int N = 3000;
        std::vector<era::Id> node_ids(N);

        for(int i = 0; i < N; ++i)
            node_ids[i] = g.addNode(i);

        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                g.addEdge(node_ids[i], node_ids[j], i * 10 + j);
            }
        }

        std::cout << "Building graph done" << std::endl;

        for(int i = 0; i < N; ++i)
        {
            g.removeNode(node_ids[i]);
        }

        std::cout << "Removing nodes done" << std::endl;
    }

    return 0;
}
