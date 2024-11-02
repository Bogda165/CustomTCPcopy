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
class MyWindow: public WindowF<SendableT>, public Sender<Container> {
    static_assert(std::is_base_of<Sendable, SendableT>::value, "T must inherit Sendable");
    static_assert(std::is_same<typename std::iterator_traits<typename Container::iterator>::value_type, std::unique_ptr<SendableT>>::value,
                  "Container must hold std::unique_ptr<Sendable>");

public:
    MyWindow(): WindowF<SendableT>(), Sender<Container>(){}
    MyWindow(Container c): WindowF<SendableT>(), Sender<Container>(c){}

    OUT_OF_RANGE Condition(std::unique_ptr<Sendable> obj) override {
        try {
            //priority for example ask or syn or fin
            if (obj->getSequenceNumber() < 0) {
                std::cout << "Pmessage" << std::endl;
                return OUT_OF_RANGE::IN;
            }
            this->addToBuffer(obj->getSequenceNumber(), std::move(obj));
        }catch (const out_of_range_exc &e) {
            return e.getCode();
        }

        return OUT_OF_RANGE::IN;
    }

    void moveFromWindowToContainer() {
        for (auto it = this->buffer->begin(); it != this->buffer->end(); it ++) {
            std::unique_ptr<Sendable> obj = it->second->clone();
            this->addToContainerLF(std::move(obj));
        }
    }

    // add stop mutex
    std::thread reSender(int timeout) {
        return std::thread([this, timeout]() {
            while(true) {
                {
                    std::cout << "ReSender" << std::endl;
                    std::scoped_lock lock(*this->buffer_m, *this->container_m);
                    std::cout << "ReSender blocked" << std::endl;
                    moveFromWindowToContainer();
                    std::cout << "ReSender unblocked" << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::seconds(timeout));
            }
        });
    }
};


#endif //PKS_PROJECT_MYWINDOW_H
