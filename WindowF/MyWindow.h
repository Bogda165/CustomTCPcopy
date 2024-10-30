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
template <typename T>
int WindowF<T>::max_buffer_size = 5;

template<typename Container, class SendableT>
class MyWindow: public WindowF<std::unique_ptr<SendableT>>, public Sender<Container> {
    static_assert(std::is_base_of<Sendable, SendableT>::value, "T must inherit Sendable");
    static_assert(std::is_same<typename std::iterator_traits<typename Container::iterator>::value_type, std::unique_ptr<SendableT>>::value,
                  "Container must hold std::unique_ptr<Sendable>");

public:
    MyWindow(): WindowF<std::unique_ptr<SendableT>>(), Sender<Container>(){}
    MyWindow(Container c): WindowF<std::unique_ptr<SendableT>>(), Sender<Container>(c){}

    bool Condition(std::unique_ptr<Sendable> obj) override {
        try {
            //priority for example ask or syn or fin
            if (obj->getSequenceNumber() < 0) {
                std::cout << "Pmessage" << std::endl;
                return true;
            }
            this->addToBuffer(obj->getSequenceNumber(), std::move(obj));
        }catch (const std::out_of_range &e) {
            return  false;
        }

        return true;
    }
};


#endif //PKS_PROJECT_MYWINDOW_H
