//
// Created by qiniu on 2019/10/24.
//

#ifndef ADDAUDIOFORSOURCE_INPUT_HPP
#define ADDAUDIOFORSOURCE_INPUT_HPP

#include <memory>
#include <functional>
#include "common.hpp"

namespace muxer{
    typedef const std::function<int(const std::unique_ptr<MediaPacket>)> PacketHandlerType;

    class AvReceiver{
public:
    AvReceiver();
    ~AvReceiver();
    int Receive(IN const std::string&url,IN PacketHandlerType);
};
}



#endif //ADDAUDIOFORSOURCE_INPUT_HPP
