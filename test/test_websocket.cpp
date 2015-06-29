#include <iostream>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <set>
#include <signal.h>

std::set<websocketpp::connection_hdl,std::owner_less<websocketpp::connection_hdl>> connections_;

typedef websocketpp::server<websocketpp::config::asio> server;

server* server_;
bool running_ = true;

void on_message(websocketpp::connection_hdl hdl, server::message_ptr msg) {
    std::cout << msg->get_payload() << std::endl;
    server_->send(hdl, "Bla die bla", websocketpp::frame::opcode::text);
}

void on_open(websocketpp::connection_hdl hdl) {
    connections_.insert(hdl);
}

void signalHandler(int sig)
{
    running_ = false;
}

int main()
{
    server viz_server;
    server_ = &viz_server;

    viz_server.set_reuse_addr(true);   // For now, until we know how to correctly close the connection

    viz_server.set_message_handler(&on_message);
    viz_server.set_open_handler(&on_open);

    viz_server.init_asio();
    viz_server.listen(9002);
    viz_server.start_accept();

    signal(SIGINT, signalHandler);

    double x = 0;
    while(running_)
    {
        viz_server.poll();

        std::vector<double> ranges = {1, 1, 1, 1, 1, 2, 1.5, 1, 1, 1, 1, 1, 1, 1, 1, 1};
        for(double& r : ranges)
            r += sin(x);

        std::stringstream ss;
        ss << "{\"angle_min\":-2,\"angle_max\":2,\"ranges\":[";

        ss << ranges[0];
        for(unsigned int i = 1; i < ranges.size(); ++i)
            ss << "," << ranges[i];

        ss << "]}";

        for(auto& c : connections_)
        {
            try
            {
                viz_server.send(c, ss.str(), websocketpp::frame::opcode::text);
            }
            catch (websocketpp::exception& e)
            {
                std::cout << e.what() << std::endl;
                connections_.erase(c);
            }
        }

        usleep(10000);

        x+= 0.1;
    }

    std::cout << "Closing connections" << std::endl;

    viz_server.stop_listening();
    for (auto& c : connections_) {
        try{
            viz_server.close(c, websocketpp::close::status::normal, "");   // a-sync call; how do we wait?
        }catch(websocketpp::lib::error_code ec){
            std::cout<<"lib::error_code "<<ec<<std::endl;
        }
    }

    std::cout << "Stopped" << std::endl;

    return 0;

//    print_server.run();
}
