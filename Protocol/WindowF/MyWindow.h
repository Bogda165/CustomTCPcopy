//
// Created by User on 28/10/2024.
//

#ifndef PKS_PROJECT_MYWINDOW_H
#define PKS_PROJECT_MYWINDOW_H
#include "WindowF.h"
#include "../Sendable/Sendable.h"
#include <memory>
#include <mutex>
#include <cmath>
#include "../Sender/Sender.h"
template <typename T>
int WindowF<T>::max_buffer_size = 1024;

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
            auto& obj = it->second;

            auto now = std::chrono::system_clock::now();

            std::chrono::duration<double> duration = obj.first - now;

            if (std::abs(duration.count()) > 10) {
                std::cout << "add to conainer once more time" << std::endl;
                this->addToContainerLF(std::move(obj.second->clone()));
                continue;
            }
            std::cout << "Time didn't come yet " <<  duration.count() << std::endl;
        }
    }

    // add stop mutex
    std::thread reSender(int timeout) {
        return std::thread([this, timeout]() {
            while(true) {
                //std::cout << "Resend" << std::endl;
                {
                    std::scoped_lock lock(*this->buffer_m, *this->container_m);
                    moveFromWindowToContainer();
                }
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        });
    }
};


#endif //PKS_PROJECT_MYWINDOW_H
