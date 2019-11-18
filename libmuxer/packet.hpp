//
// Created by qiniu on 2019/10/24.
//

#ifndef ADDAUDIOFORSOURCE_PACKET_HPP
#define ADDAUDIOFORSOURCE_PACKET_HPP

#include "common.hpp"

namespace muxer {
    class MediaPacket {
    public:
        MediaPacket(IN const AVStream &pAvStream, IN const AVPacket *pAvPacker);

        // stream and codec
        MediaType Media() const;

        void Media(IN MediaType);

        CodecType Codec() const;

        void Codec(IN CodecType);

        //video
        int Width() const;

        int Height() const;

        void Width(int);

        void Height(int);

        // audio
        int SampleRate() const;

        int Channels() const;

        void SampleRate(int);

        void Channels(int);

        // get raw AV structs
        AVPacket *AvPacket() const;

        AVCodecParameters *AvCodecParameters() const; // used by receiver and decoder
        AVCodecContext *AvCodecContext() const;       // used by encoder and sender
    private:
        AVPacket *pAvPacket_ = nullptr;
        AVCodecParameters *pAvCodecPar_ = nullptr;
        AVCodecContext *pAvCodecContext_ = nullptr;

        MediaType mediaType_;
        CodecType codecType_;

        int nWidth_ = -1, nHeight_ = -1;
        int nSampleRate_ = -1, nChannels_ = -1;
    };

    class MediaFrame {
    public:
        explicit MediaFrame(IN const AVFrame* pAvFrame);
        MediaFrame();
        MediaFrame(IN int nSamples,IN int nChannels,IN AVSampleFormat format,IN bool bSilence = false);
        MediaFrame(IN int nWidth,IN int nHeight,IN AVPixelFormat format,IN int nColor= -1,IN int nAlpha=0);
        MediaFrame(const MediaFrame&);
        ~MediaFrame();
        AVFrame* AvFrame() const;


        MediaType Media() const;
        void Media(IN MediaType)const;
        CodecType Codec()const ;
        void Codec(IN CodecType);

    private:
        AVFrame *pAvFrame_ = nullptr;
    };
}  // namespace muxer

#endif  // ADDAUDIOFORSOURCE_PACKET_HPP
