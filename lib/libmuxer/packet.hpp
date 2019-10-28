//
// Created by qiniu on 2019/10/24.
//

#ifndef ADDAUDIOFORSOURCE_PACKET_HPP
#define ADDAUDIOFORSOURCE_PACKET_HPP

namespace muxer{
    class MediaPacket {
    private:
        AVPacket* pAvPacket_= nullptr;
        AVCodecParameters* pAvCodecPar_ = nullptr;
        AVCodecContext* pAvCodecContext_ = nullptr;

    };
    class MediaFrame{
        AVFrame* pAvFrame_= nullptr;
    };
}



#endif //ADDAUDIOFORSOURCE_PACKET_HPP
