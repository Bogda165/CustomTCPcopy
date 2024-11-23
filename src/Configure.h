//
// Created by User on 22/11/2024.
//

#ifndef PKS_PROJECT_CONFIGURE_H
#define PKS_PROJECT_CONFIGURE_H
#include <yaml-cpp/yaml.h>
#include <string>

class Configure {
private:
    std::string ip;
    int port;
    int chunk_len;
    constexpr static std::string name = "config.yaml";
public:
    Configure(){
        YAML::Node config = YAML::LoadFile(name);

        ip = config["ip"].as<std::string>();
        port = config["port"].as<int>();

        chunk_len = config["chunk_len"].as<int>();

    };

    void update() {
        YAML::Node config = YAML::LoadFile(name);
        chunk_len = config["chunk_len"].as<int>();
    }

    std::string getIp() const {
        return ip;
    }

    int getPort() const {
        return port;
    }

    int getChunkLen() const {
        return chunk_len;
    }

    static std::string getName() {
        return name;
    }
};


#endif //PKS_PROJECT_CONFIGURE_H
