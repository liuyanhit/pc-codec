//
// Created by qiniu on 2019/10/24.
//

#ifndef ADDAUDIOFORSOURCE_COMMON_HPP
#define ADDAUDIOFORSOURCE_COMMON_HPP

#include <mutex>
#include <string>
#include <unordered_map>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "spdlog/spdlog.h"

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

namespace muxer {
    typedef enum {
        MEDIA_VIDEO = AVMEDIA_TYPE_VIDEO,
        MEDIA_AUDIO = AVMEDIA_TYPE_AUDIO,
        MEDIA_DATA = AVMEDIA_TYPE_DATA
    } MediaType;

    typedef enum {
        // video
        CODEC_H264 = AV_CODEC_ID_H264,
        CODEC_VC1 = AV_CODEC_ID_VC1,

        // Audio
        CODEC_MP3 = AV_CODEC_ID_MP3,
        CODEC_AAC = AV_CODEC_ID_AAC,
        CODEC_WAV1 = AV_CODEC_ID_WMAV1,
        CODEC_WAV2 = AV_CODEC_ID_WMAV2,

        // others
        CODEC_FLV_METADATA = AV_CODEC_ID_FFMETADATA,
        CODEC_UNKNOWN = AV_CODEC_ID_NONE
    } CodecType;

    class OptionMap {
    public:
        virtual bool GetOption(IN const std::string &key, OUT std::string &value);

        virtual bool GetOption(IN const std::string &key, OUT int &value);

        virtual bool GetOption(IN const std::string &key);

        virtual void SetOption(IN const std::string &flag);

        virtual void SetOption(IN const std::string &key,
                               IN const std::string &value);

        virtual void SetOption(IN const std::string &key, IN int val);

        virtual void DelOption(IN const std::string &key);

        virtual void GetOptions(IN const OptionMap &opts);

    protected:
        std::unordered_map<std::string, std::string> params_;
        std::mutex paramsLck_;

        std::unordered_map<std::string, int> intParas_;
        std::mutex intParamsLck_;
    };

    typedef OptionMap Options;
}  // namespace muxer
#include "packet.hpp"


#endif  // ADDAUDIOFORSOURCE_COMMON_HPP
