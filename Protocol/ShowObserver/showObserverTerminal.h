//
// Created by User on 21/11/2024.
//

#ifndef PKS_PROJECT_SHOWOBSERVERTERMINAL_H
#define PKS_PROJECT_SHOWOBSERVERTERMINAL_H
#include <memory>
#include <unordered_map>
#include <utility>
#include <MySocket.h>
#include "showObserver.h"

class ShowObserverTerminal: public ShowObserver{
private:
    std::shared_ptr<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>> messages;
    std::shared_ptr<std::mutex> messages_m;

public:
    ShowObserverTerminal(std::shared_ptr<std::unordered_map<int, std::pair<Header, std::variant<Data, FileData>>>> messages, std::shared_ptr<std::mutex> messages_m): messages(messages), messages_m(messages_m) {

    }

     bool func(int message_id, int message_size) override {
        std::lock_guard<std::mutex> lock(*messages_m);
        auto& obj = (*messages)[message_id];
        bool _return = false;

        std::visit([message_id, &_return, message_size](auto&& _obj){
            _return = (_obj.dataAmount() == message_size);
            std::cout << "Size while checking: " <<_obj.dataAmount() << message_size << std::endl;
            if (_return) {
                std::cout << "Message with id: " << message_id << std::endl;
                //change
                std::cout << _obj.toString() << std::endl;
            }
        }, obj.second);

        if(_return) {
            messages->erase(message_id);
        }
        return _return;
    }

    std::shared_ptr<ShowObserver> getSharedPtr()override {
        return shared_from_this();
    }

    ~ShowObserverTerminal() = default;
};


#endif //PKS_PROJECT_SHOWOBSERVERTERMINAL_H
