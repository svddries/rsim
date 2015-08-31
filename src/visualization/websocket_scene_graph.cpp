#include "era/visualization/websocket_scene_graph.h"

#include <sstream>

// io
#include <set>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "era/config/data.h"
#include "era/config/yaml.h"
#include "era/config/reader.h"

#include "era/config/emitter.h"

namespace era
{

// ----------------------------------------------------------------------------------------------------

typedef websocketpp::server<websocketpp::config::asio> server;

class WebsocketSceneGraph::IO
{

public:

    IO(int port, WebsocketSceneGraph* wsg) : server_(new server), wsg_(wsg)
    {
        server_->set_reuse_addr(true);   // For now, until we know how to correctly close the connection

        server_->set_message_handler(bind(&WebsocketSceneGraph::IO::on_message, this, websocketpp::lib::placeholders::_1, websocketpp::lib::placeholders::_2));
        server_->set_open_handler(bind(&WebsocketSceneGraph::IO::on_open, this, websocketpp::lib::placeholders::_1));

        server_->init_asio();
        server_->listen(port);
        server_->start_accept();
    }

    ~IO()
    {
        server_->stop_listening();
        for (auto& c : connections_) {
            try{
                server_->close(c, websocketpp::close::status::normal, "");   // a-sync call; how do we wait?
            }catch(websocketpp::lib::error_code ec){
                std::cout<<"lib::error_code "<<ec<<std::endl;
            }
        }

        delete server_;
    }

    void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg)
    {
//        std::cout << msg->get_payload() << std::endl;

        // Parse request
        era::config::Data data;
        era::config::yaml::parseString(msg->get_payload(), data);
        era::config::Reader r(data);

        // Get requested revision
        int rev = 0;
        if (!r.value("rev", rev))
            std::cout << "WARNING: received request without 'rev' field." << std::endl;

        // Send scene graph update (since revision 'rev')
        send(wsg_->CreateDifferenceMessage(rev));
    }

    void on_open(websocketpp::connection_hdl hdl)
    {
        connections_.insert(hdl);
    }

    void send(const std::string& msg)
    {
        for(auto& c : connections_)
        {
            try
            {
                server_->send(c, msg, websocketpp::frame::opcode::text);
            }
            catch (websocketpp::exception& e)
            {
                std::cout << e.what() << std::endl;
                connections_.erase(c);
            }
        }
    }

    void poll()
    {
        server_->poll();
    }

protected:

    server* server_;

    std::set<websocketpp::connection_hdl,std::owner_less<websocketpp::connection_hdl>> connections_;

    WebsocketSceneGraph* wsg_;

};

// ----------------------------------------------------------------------------------------------------

std::string poseToString(const Transform3& p)
{
    std::stringstream s;
    s << "[" << p.t.x << "," << p.t.y << "," << p.t.z << "]";
    return s.str();
}

// ----------------------------------------------------------------------------------------------------

WebsocketSceneGraph::WebsocketSceneGraph() : revision_(0), io_(nullptr)
{
}

// ----------------------------------------------------------------------------------------------------

WebsocketSceneGraph::~WebsocketSceneGraph()
{
    delete io_;
}

// ----------------------------------------------------------------------------------------------------

void WebsocketSceneGraph::InitComm(int port)
{
    delete io_;
    io_ = new WebsocketSceneGraph::IO(port, this);
}

// ----------------------------------------------------------------------------------------------------

void WebsocketSceneGraph::PollRequests()
{
    io_->poll();
}

// ----------------------------------------------------------------------------------------------------

void WebsocketSceneGraph::addNode(const std::string& node_id, const std::string& parent, const Transform3& pose)
{
    ++revision_;

    Node& n = data_[node_id];
    n.setField("parent", "\"" + parent + "\"", revision_);
    n.setField("pose", poseToString(pose), revision_);
}

// ----------------------------------------------------------------------------------------------------

void WebsocketSceneGraph::setData(const std::string& node_id, const std::string& field, const std::string& data)
{
    ++revision_;
    Node& n = data_[node_id];
    n.setField(field, data, revision_);
}

// ----------------------------------------------------------------------------------------------------

void WebsocketSceneGraph::setPose(const std::string& node_id, const Transform3& pose)
{
    ++revision_;
    Node& n = data_[node_id];
    n.setField("pose", poseToString(pose), revision_);
}

// ----------------------------------------------------------------------------------------------------

std::string WebsocketSceneGraph::CreateDifferenceMessage(uint64_t since_revision) const
{
    std::stringstream msg;
    msg << "{\"updates\":[";

    bool first = true;
    for(auto& it : data_)
    {
        const std::string& id = it.first;
        const Node& n = it.second;

        if (n.revision <= since_revision)
            continue;

        if (!first)
            msg << ",";
        else
            first = false;

        msg << "{\"id\":\"" << id << "\"";

        for(auto& it_field : n.fields)
        {
            const Field& f = it_field.second;
            if (f.revision <= since_revision)
                continue;

            msg << ",\"" << it_field.first << "\":" << f.data;
        }

        msg << "}";
    }

    msg << "],\"rev\":" << revision_ << "}";

    return msg.str();
}

} // end namespace era
