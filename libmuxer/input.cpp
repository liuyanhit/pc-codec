//
// Created by qiniu on 2019/10/24.
//

#include "input.hpp"

using namespace muxer;

AvReceiver::AvReceiver() {}

AvReceiver::~AvReceiver() {
    avformat_close_input(&pAvContext_);
    pAvContext_ = nullptr;
}

int AvReceiver::Receive(const std::string &url, muxer::PacketHandlerType callback) {
    if (pAvContext_ != nullptr) {
        spdlog::warn("internal: reuse of Receiver is not recommended");
    }
    pAvContext_ = avformat_alloc_context();  //包含该视频的 url protocol layer,
    //format layer codec layer 解封装
    if (pAvContext_ == nullptr) {
        spdlog::error("av context could not be created");
        return -1;
    }
    pAvContext_->interrupt_callback.callback = AvReceiver::AvInterruptCallback;
    pAvContext_->interrupt_callback.opaque = this;
    start_ = std::chrono::high_resolution_clock::now();
    spdlog::info("input url %s", url.c_str());
    int nStatus = avformat_open_input(&pAvContext_, url.c_str(), 0, 0);
    if (nStatus < 0) {
        spdlog::error("could not get stream info %d", nStatus);
    } else {
        spdlog::info("url %s ,duration  %ld", url.c_str(), pAvContext_->duration);
    }
    bool isPic = false;
    for (unsigned int i = 0; i < pAvContext_->nb_streams; i++) {
        struct AVStream *pAvStream = pAvContext_->streams[i];
        streams_.push_back(StreamInfo{pAvStream, -1});
        spdlog::info("stream is found: avstream=%d, avcodec=%d first dts %ld",
                     pAvStream->codecpar->codec_type, pAvStream->codecpar->codec_id,
                     pAvStream->first_dts);
        isPic = (AV_CODEC_ID_MJPEG == pAvStream->codecpar->codec_id);
        if (isPic) {
            break;
        }
    }

    AVPacket avPacket;
    av_init_packet(&avPacket);
    while ((nStatus = av_read_frame(pAvContext_, &avPacket)) >= 0) {
        if (avPacket.stream_index < 0 ||
            static_cast<unsigned int>(avPacket.stream_index) >= pAvContext_->nb_streams) {
            spdlog::warn("invalid stream index in packet");
            av_packet_unref(&avPacket);
            continue;
        }
        if (static_cast<size_t >(avPacket.stream_index) < streams_.size()) {
            AVRational tb = AVRational{1, 1000};
            AVRounding r = static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX);
            avPacket.dts = av_rescale_q_rnd(avPacket.dts, streams_[avPacket.stream_index].pAvStream->time_base, tb, r);
            avPacket.pts = av_rescale_q_rnd(avPacket.pts, streams_[avPacket.stream_index].pAvStream->time_base, tb, r);


            // emulate framerate @ 1.0x speed
            if (EmulateFramerate(avPacket.pts, streams_[avPacket.stream_index]) == true) {
                int nReturn = callback(std::make_unique<MediaPacket>(*streams_[avPacket.stream_index].pAvStream,
                                                                     &avPacket));
                if (nReturn != 0) {
                    av_packet_unref(&avPacket);
                    return nReturn;
                }
            } else {
                av_packet_unref(&avPacket);
            }
        } else {
            av_packet_unref(&avPacket);
        }
    }
}

bool AvReceiver::EmulateFramerate(IN int64_t nPts, INOUT muxer::AvReceiver::StreamInfo &stream) {
    if (nPts < 0) {
        spdlog::warn("Reciver :negative pts get %ld", nPts);
        return false;
    }
    using namespace std::chrono;
    if (stream.nFirstPts < 0) {
        stream.start = high_resolution_clock::now();
        stream.nFirstPts = nPts;
    }
    auto tenSecond = 10 * 1000;
    auto now = high_resolution_clock::now();
    int64_t nPlaytime = nPts - stream.nFirstPts;
    auto nDuration = duration_cast<microseconds>(now - stream.start).count();
    if (nPlaytime > nDuration) {
        auto delay = nPlaytime - nDuration;
        if (delay > tenSecond) {
            spdlog::warn("Receive: fps emulation delay>10s %ld,skip", delay);
        } else {
            usleep(delay * 1000);
        }
    }
}

AVCodecParameters* MediaPacket::AvCodecParameters() const
{
    return pAvCodecPar_;
}

AVCodecContext* MediaPacket::AvCodecContext() const
{
    return pAvCodecContext_;
}

AvDecoder::AvDecoder() {

}
AvDecoder::~AvDecoder() {
    if (bIsDecoderAvailable_){
        avcodec_close(pAvDecoderContext_);
    }
    if (pAvDecoderContext_!= nullptr){
        avcodec_free_context(&pAvDecoderContext_);
    }
}

int AvDecoder::Init(IN const std::unique_ptr<MediaPacket> &pPakcet) {
    if (pAvDecoderContext_==nullptr){
        AVCodec *pAvCodec = avcodec_find_decoder(static_cast<AVCodecID>(pPakcet->Codec()));
        if (pAvCodec== nullptr){
            spdlog::error("could not find the av decode for codec id %d",pPakcet->Codec());
            return -1;
        }
        pAvDecoderContext_ = avcodec_alloc_context3(pAvCodec);
        if (pAvDecoderContext_ == nullptr){
            spdlog::error("could not allocate av codec context");
            return -1;
        }

        if (pPakcet->AvCodecParameters()!=nullptr){
            if (avcodec_parameters_to_context(pAvDecoderContext_,pPakcet->AvCodecParameters())<0){
                spdlog::error("could not copy decoder context");
                return -1;
            }
        }
        if (avcodec_open2(pAvDecoderContext_,pAvCodec, nullptr)<0){
            spdlog::error("could not open decoder");
            return -1;
        }else{
            spdlog::info("open decoder stream stream %d codec %d",pPakcet->Media(),pPakcet->Codec());
            bIsDecoderAvailable_ = true;
        }
    }
    return 0;
}

int AvDecoder::Decode(IN const std::unique_ptr<MediaPacket> &pPacket,IN muxer::FrameHandlerType &callback) {
    if (Init(pPacket)<0){
        return -1;
    }
    do{
        bool bNeedSendAgain = false;
        int nStatus = avcodec_send_packet(pAvDecoderContext_,pPacket->AvPacket());
        if (nStatus!=0){
            if (nStatus==AVERROR(EAGAIN)){
                spdlog::warn("decoder internal :assert failed we should not ger EAGAIN");
                bNeedSendAgain = true;
            }else{
                spdlog::error("decoder:could not send frame status %d \n",nStatus);
                return -1;
            }
        }
        while(1){
            auto pFrame = std::make_shared<MediaFrame>();
            pFrame->Media(pPacket->Media());
            pFrame->Codec(pPacket->Codec());

            nStatus = avcodec_receive_frame(pAvDecoderContext_,pFrame->AvFrame());
            if (nStatus == 0){
                int nStatus=callback(pFrame);
                if (nStatus<0){
                    return nStatus;
                }
                if (bNeedSendAgain){
                    break;
                }
            }else if (nStatus == AVERROR(EAGAIN)){
                return  0;
            }else {
                spdlog::error("decoder : could not receive frame status %d",nStatus);
                return -1;
            }

        }

    }while(1);
}


void Stream::Start(IN const std::string &url) {
    if (url.empty()) {
        bReceiverExit_.store(true);
        spdlog::error("the url is empty");
    }
    auto recv = [this, url] {
        while (bReceiverExit_.load() == false) {
            auto avReceiver = std::make_unique<AvReceiver>();
            auto vDecoder = std::make_unique<AvDecoder>();
            auto aDecoder = std::make_unique<AvDecoder>();

            auto receiveHook=[&](IN const std::unique_ptr<MediaPacket> pPacket)->int {
                if (bReceiverExit_.load()==true){
                    return -1;
                }
                auto decodeHook=[&](IN const std::shared_ptr<MediaFrame> pFrame )->int{
                    if (bReceiverExit_.load()==true){
                        return -1;
                    }
                };
                int nStatus =0;
                if(pPacket->Media()==MEDIA_VIDEO){
                    nStatus= vDecoder->Decode(pPacket,decodeHook);
                } else if (pPacket->Media()==MEDIA_AUDIO){
                    nStatus=aDecoder->Decode(pPacket,decodeHook);
                }
                return nStatus;
            };
            if (avReceiver->Receive(url,receiveHook)==0){

            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    };
}