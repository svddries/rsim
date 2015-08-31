#ifndef ERA_WEBSOCKET_SCENE_GRAPH_H_
#define ERA_WEBSOCKET_SCENE_GRAPH_H_

#include <era/geometry/math.h>
#include <string>
#include <map>

namespace era
{

class WebsocketSceneGraph
{

public:

    WebsocketSceneGraph();

    ~WebsocketSceneGraph();

    void InitComm(int port);

    void PollRequests();

    void addNode(const std::string& node_id, const std::string& parent, const Transform3& pose);

    void setData(const std::string& node_id, const std::string& field, const std::string& data);

    void setPose(const std::string& node_id, const Transform3& pose);

    std::string CreateDifferenceMessage(uint64_t since_revision) const;

private:

    struct Field
    {
        std::string data;
        uint64_t revision;
    };

    struct Node
    {
        void setField(const std::string& field, const std::string& data, uint64_t rev)
        {
            Field& f = fields[field];
            f.data = data;
            f.revision = rev;
            revision = rev;
        }

        std::map<std::string, Field> fields;
        uint64_t revision;
    };

    class IO;

    std::map<std::string, Node> data_;

    uint64_t revision_;

    IO* io_;

};

} // end namespace era

#endif
