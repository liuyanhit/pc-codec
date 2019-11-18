//
// Created by qiniu on 2019/10/24.
//

#include "packet.hpp"

using namespace muxer;

MediaPacket::MediaPacket(IN const AVStream &pAvStream, IN const AVPacket* pAvPacket) {
    pAvCodecPar_ = pAvStream.codecpar;
    MediaType mediaType = static_cast<MediaType>(pAvCodecPar_->codec_type);
    if (mediaType != MEDIA_AUDIO && mediaType != MEDIA_VIDEO) {
        mediaType = MEDIA_DATA;
    }
    Media(mediaType);
    codecType_ = static_cast<CodecType>(pAvCodecPar_->codec_id);
    nWidth_ = pAvCodecPar_->width;
    nHeight_ = pAvCodecPar_->height;
    nSampleRate_ = pAvCodecPar_->sample_rate;
    nChannels_ = pAvCodecPar_->channels;
    pAvPacket_ = const_cast<AVPacket*>(pAvPacket);
}

AVPacket* MediaPacket::AvPacket() const {
    return const_cast<AVPacket*> (pAvPacket_);
}

void MediaPacket::Media(MediaType type) {
    mediaType_ = type;
}

MediaType MediaPacket::Media() const {
    return mediaType_;
}

CodecType MediaPacket::Codec() const {
    return codecType_;
}

void MediaPacket::Codec(CodecType type) {
    codecType_ = type;
}

int MediaPacket::Width() const {
    return nWidth_;
}

int MediaPacket::Height() const {
    return nHeight_;
}

void MediaPacket::Width(int _nValue) {
    nWidth_ = _nValue;
}

void MediaPacket::Height(int _nValue) {
    nHeight_ = _nValue;
}

int MediaPacket::SampleRate() const
{
    return nSampleRate_;
}

int MediaPacket::Channels() const
{
    return nChannels_;
}

void MediaPacket::SampleRate(int _nValue)
{
    nSampleRate_ = _nValue;
}

void MediaPacket::Channels(int _nValue)
{
    nChannels_ = _nValue;
}

AVFrame* MediaFrame::AvFrame() const
{
    return pAvFrame_;
}