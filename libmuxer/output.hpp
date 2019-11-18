//
// Created by qiniu on 2019/10/31.
//

#ifndef PC_CODEC_OUTPUT_HPP
#define PC_CODEC_OUTPUT_HPP

#include "common.hpp"
namespace muxer{
    typedef const std::function<int(IN const std::shared_ptr<MediaPacket>&)> EncoderHandlerType;
    class AvEncoder{
    public:
        AvEncoder();
        ~AvEncoder();
        int Encode(IN std::shared_ptr<MediaFrame> &pFrame,IN EncoderHandlerType& callback);

    private:
        int Init(IN const std::shared_ptr<MediaFrame>& pFrame);
        int EncodeAAC(IN std::shared_ptr<MediaFrame>& pFrame, IN EncoderHandlerType& callback);
        int EncodeH264(IN std::shared_ptr<MediaFrame>& pFrame, IN EncoderHandlerType& callback);

    private:
        AVCodecContext* pAvEncoderContext_ = nullptr;
        bool bIsEncoderAvailable_ = false;


    };

    class output {

    };
}



#endif //PC_CODEC_OUTPUT_HPP
