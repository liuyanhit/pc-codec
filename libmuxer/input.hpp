//
// Created by qiniu on 2019/10/24.
//

#ifndef ADDAUDIOFORSOURCE_INPUT_HPP
#define ADDAUDIOFORSOURCE_INPUT_HPP

#include <functional>
#include <memory>
#include "common.hpp"

namespace muxer {

    class Input : public OptionMap {

    };

    class Stream : public Input {
    public:
        Stream(IN const std::string &name);

        ~Stream();

        void Start(IN const std::string &url);

    private:
        std::atomic<bool> bReceiverExit_;

    };

    typedef const std::function<int(const std::unique_ptr<MediaPacket>)>
            PacketHandlerType;

    class AvReceiver {
    public:
        AvReceiver();

        ~AvReceiver();

        int Receive(IN const std::string &url, IN PacketHandlerType);

    private:
        struct StreamInfo {
            struct AVStream *pAvStream;
            int64_t nFirstPts = -1;
            std::chrono::high_resolution_clock::time_point start;
        };

        static int AvInterruptCallback(void *pContext);

        static bool EmulateFramerate(IN int64_t nPts, OUT StreamInfo &stream);

    private:
        struct AVFormatContext *pAvContext_ = nullptr;
        std::chrono::high_resolution_clock::time_point start_;
        std::vector<StreamInfo> streams_;
    };

    typedef const std::function<int(const std::shared_ptr<MediaFrame> &)> FrameHandlerType;

    class AvDecoder {
    public:
        AvDecoder();

        ~AvDecoder();

        int Decode(IN const std::unique_ptr<MediaPacket> &pPacket, IN FrameHandlerType &callback);

        int Reset();

    private:
        int Init(IN const std::unique_ptr<MediaPacket> &pPakcet);

    private:
        AVCodecContext *pAvDecoderContext_ = nullptr;
        bool bIsDecoderAvailable_ = false;
    };
}  // namespace muxer

#endif  // ADDAUDIOFORSOURCE_INPUT_HPP
