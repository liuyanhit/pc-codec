//
// Created by qiniu on 2019/10/31.
//

#include "output.hpp"

using namespace muxer;

int AvEncoder::Init(const std::shared_ptr<MediaFrame> &pFrame) {
    if (pAvEncoderContext_ != nullptr) {
        return 0;
    }
    std::vector<std::string> codecs;
    switch (pFrame->Media()) {
        case MEDIA_AUDIO: {
            std::vector<std::string> audioCodecs{"libfdk_aac"};
            codecs = audioCodecs;
            break;
        }
        case MEDIA_VIDEO: {
            std::vector<std::string> videoCodecs{"libx264"};
            codecs = videoCodecs;
            break;
        }
        default:
            return -1;
    }

    auto openEnc = [&](std::string name) -> bool {
        AVCodec *pAvCodec = avcodec_find_encoder_by_name(name.c_str());
        if (pAvCodec == nullptr) {
            spdlog::error("could not fine the encoder %s", name.c_str());
            return false;
        }
        pAvEncoderContext_ = avcodec_alloc_context3(pAvCodec);
        if (pAvEncoderContext_ == nullptr) {
            spdlog::error("could not allocate avencoder context for encoder");
            return false;
        }

        std::shared_ptr<MediaFrame> pframe = pFrame;
//        Preset(pframe);
        AVDictionary *pAvDictonary = nullptr;
        if (avcodec_open2(pAvEncoderContext_, pAvCodec, &pAvDictonary) < 0) {
            spdlog::error("could not open encoder");
            avcodec_free_context(&pAvEncoderContext_);
            pAvEncoderContext_ = nullptr;
            av_dict_free(&pAvDictonary);
            pAvDictonary = nullptr;
            return false;
        } else {
            spdlog::info("open encoder media = %d codec = %d", pframe->Media(), pframe->Codec());
            bIsEncoderAvailable_ = true;
            av_dict_free(&pAvDictonary);
            pAvDictonary = nullptr;
            return true;
        }
    };
    auto result = std::find_if(codecs.begin(), codecs.end(), [&openEnc](std::string name) {
        spdlog::info("find the codec %s ", name.c_str());
        return openEnc(name);
    });
    if (result == codecs.end()) {
        return -1;
    }
    return 0;

}

int AvEncoder::Encode(std::shared_ptr<MediaFrame> &pFrame, muxer::EncoderHandlerType &callback) {
    if (Init(pFrame) != 0) {
        return -1;
    }
    switch (pFrame->Codec()) {
        case CODEC_AAC:
            return EncodeAAC(pFrame, callback);
        case CODEC_H264:
            return EncodeH264(pFrame, callback);
        default:
            spdlog::error("no preset ");
    }
    return -1;
}

int AvEncoder::EncodeH264(std::shared_ptr<MediaFrame> &pFrame, muxer::EncoderHandlerType &callback) {
    auto pframe = pFrame;
    pframe->AvFrame()->pict_type = AV_PICTURE_TYPE_NONE;
    do {
        bool bNeedSendAgain = false;
        int nStatus = avcodec_send_frame(pAvEncoderContext_, pframe->AvFrame());
        if (nStatus != 0) {
            if (nStatus == AVERROR(EAGAIN)) {
                spdlog::warn("internal:assert failed,we should not get EAGIN");
                bNeedSendAgain = true;
            } else {
                spdlog::error("H264 encoder: could not send frame status %d", nStatus);
                return -1;
            }
        }

        while (1) {
            auto pPacket = std::make_shared<MediaPacket>(pAvEncoderContext_);
            pPacket->Media(MEDIA_VIDEO);
            pPacket->Codec(CODEC_H264);
            pPacket->Width(pAvEncoderContext_->width);
            pPacket->Height(pAvEncoderContext_->height);
            pPacket->AvPacket()->data = nullptr;
            pPacket->AvPacket()->size = 0;

            nStatus = avcodec_receive_packet(pAvEncoderContext_, pPacket->AvPacket());
            if (nStatus == 0) {
                nStatus = callback(pPacket);
                if (nStatus < 0) {
                    return nStatus;
                }
                if (bNeedSendAgain) {
                    break;
                }
            } else if (nStatus == AVERROR(EAGAIN)) {
                return 0;
            } else {
                spdlog::error("h264 encoder: could not receive frame, status=%d", nStatus);
                return -1;
            }
        }
    } while (1);
}

int AvEncoder::EncodeAAC(std::shared_ptr<MediaFrame> &pFrame, muxer::EncoderHandlerType &callback) {

}