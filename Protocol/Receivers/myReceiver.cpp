//
// Created by User on 24/10/2024.
//

#include "myReceiver.h"

// MyReceiver.cpp


bool MyReceiver::onReceive() {
    // handle what to do with the message detect if its a handshake or whatever
    std::vector<uint8_t> recv_buffer(1024, 0);
    udp::endpoint sender_endpoint;
    size_t length;
    {
        if (socket->available() > 0) {
            std::lock_guard<std::mutex> lock(*socket_m);
            length = socket->receive_from(boost::asio::buffer(recv_buffer), sender_endpoint);
        }else {
            return false;
        }
    }
    std::cout << "Received from the socket" << std::endl;

    std::cout << "Buffer";
    for (auto i: recv_buffer) {
        std::cout << static_cast<int>(i) << " ";
    }
    std::cout << std::endl;

    auto packet = std::make_unique<Packet>();
    packet->fromU8(std::vector(recv_buffer.begin(), recv_buffer.begin() + length));

    //packet->show();

    auto header = std::make_unique<Header>(packet->getHeader());
    std::cout << "Received header: ";
    header->show();
    std::cout << std::endl;

    // chech if we need data: Now 2 main types hand shake and message later add acknolegment and keep alive

    //TODO change this shit with encapsulation
    auto handShake_p = customSocket->getHandShakeStat();
    auto handShake = handShake_p.first;
    auto handShake_m = handShake_p.second;

    auto messages_p = customSocket->getMessages();
    auto messages = messages_p.first;
    auto messages_m = messages_p.second;

    //if Hand shake
     if (header->getFlags(Flags::ACK) || header->getFlags(Flags::SYN)) {
        std::lock_guard<std::mutex> lock(*handShake_m);
        std::cout << "Process handshake" << std::endl;
        handShake->handleHandShake(std::move(header), sender_endpoint);

        return true;
     }else if(header->getFlags(Flags::END)) {
         auto shower = customSocket->getShowObserver();
         shower->addToBufferLF(header->getMessageId(), header->getPacketId());
         std::cout << "Fininsh receivng  message with id: " << header->getMessageId() << std::endl;
         return true;
     }else if(header->getFlags(Flags::ACK2)) {
        try {
            auto _packet = this->customSocket->getFromBuffer(-1 * (header->getSequenceNumber() + 1));

            std::unique_ptr<Header> _header(dynamic_cast<Header*>(_packet.release()));
            if (_header) {
                if(_header->getFlags(Flags::FIN)) {
                    // I am locking a buffer here be really careful
                    auto lock = customSocket->getBufferBlock();
                    lock.release();
                    if(customSocket->getStatus() == Status::SEND) {
                        std::cout << "Finish working" << std::endl;
                        customSocket->finish();
                        //finish
                    }

                    customSocket->setStatus(Status::RECV);
                }
            }
        }catch(out_of_range_exc &e) {
            if (e.getCode() == OUT_OF_RANGE::LEFT) {
                std::cout << "ACK was already sent for this message!!!!(())((" << std::endl;
            }
        }
        std::cout << "Recv ack for " << header->getMessageId() << ": " << header->getPacketId() << std::endl;
        return true;

     }else if(header->getFlags(Flags::FIN)) {
         auto send_header = Header(Flags::ACK2, Flags::FIN);
         send_header.setSequenceNumber(header->getSequenceNumber() * -1 - 1);

         std::unique_ptr<Sendable> _packet = std::make_unique<Header>(send_header);
         this->customSocket->addToContainer(std::move(_packet));

         if(customSocket->getStatus() == Status::RECV) {
             std::cout << "Finish working" << std::endl;
             customSocket->finish();
         }
         customSocket->setStatus(Status::SEND);

         return true;
     }else {
        std::cout << "Process message" << std::endl;
        // just a usual message
        //get message id
        auto message_id = header->getMessageId();
        auto _header = header->clone();
        {
            auto data_b = packet->getChunk();
            //fix
            std::lock_guard<std::mutex> lock(*messages_m);
            auto message = messages->find(message_id);
            if (message == messages->end()) {
                // if not exist create a new message and data
                std::variant<Data, FileData> data;

                if (header->getFlags(Flags::FILE)) {
                    data = FileData();
                } else {
                    data = Data();
                }

                std::visit([&](auto& d) {
                    d.addChunk(header->getPacketId(), data_b);
                    //d.show();
                }, data);


                messages->insert(std::make_pair(message_id, std::make_pair(*header.release(), std::move(data))));

            }else {
                auto& _data = message->second.second;
                std::visit([&data_b, &header](auto& data){data.addChunk(header->getPacketId(), data_b);}, _data);
            }
        }

        {
            //send ack
            auto send_header = Header(Flags::ACK2);
            send_header.setSequenceNumber(_header->getSequenceNumber() * -1 - 1);
            send_header.setPacketId(packet->getHeader().getPacketId());
            send_header.setMessageId(packet->getHeader().getMessageId());

            std::unique_ptr<Sendable> _packet = std::make_unique<Header>(send_header);
            this->customSocket->addToContainer(std::move(_packet));
        }
        return true;
    }
}

