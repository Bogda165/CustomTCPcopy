//
// Created by User on 28/10/2024.
//

#ifndef PKS_PROJECT_MYWINDOW_H
#define PKS_PROJECT_MYWINDOW_H
#include "WindowF.h"
#include "../Sendable/Sendable.h"
#include <memory>
#include <mutex>
#include "../Sender/Sender.h"



template<typename Container, class SendableT>
class MyWindow: public WindowF<std::unique_ptr<SendableT>>, public Sender<Container> {
    static_assert(std::is_base_of<Sendable, SendableT>::value, "T must inherit Sendable");
    static_assert(std::is_same<typename std::iterator_traits<typename Container::iterator>::value_type, std::unique_ptr<SendableT>>::value,
                  "Container must hold std::unique_ptr<Sendable>");
private:
    MyWindow(): WindowF<std::unique_ptr<SendableT>>(), Sender<Container>(){}
    MyWindow(Container c): WindowF<std::unique_ptr<SendableT>>(), Sender<Container>(c){}

public:

    bool Condition() override {
        try {
            this->addToBuffer(seq_n, obj);
        }catch (const std::out_of_range &e) {
            return  false;
        }

        return true;
    }

    void sendThroughWindow(int seq_n, std::unique_ptr<SendableT> obj) {
        try {
            this->addToBuffer(seq_n, obj);
        }catch (const std::out_of_range &e) {

        }
    }



};


#endif //PKS_PROJECT_MYWINDOW_H
