//
// Created by qiniu on 2019/10/24.
//

#ifndef ADDAUDIOFORSOURCE_COMMON_HPP
#define ADDAUDIOFORSOURCE_COMMON_HPP

#include <string>
#include <bits/unordered_map.h>
#include <mutex>

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef INOUT
#define INOUT
#endif

namespace muxer{
    class OptionMap{
    public:
        virtual bool GetOption(IN const std::string & key,OUT std::string &value);
        virtual bool GetOption (IN const std::string &key ,OUT int& value);
        virtual bool GetOption(IN const std::string& key);
        virtual void SetOption(IN const std::string& flag);
        virtual void SetOption(IN const std::string& key, IN const std::string& value);
        virtual void SetOption(IN const std::string& key, IN int val);
        virtual void DelOption(IN const std::string& key);
        virtual void GetOptions(IN const OptionMap& opts);

    protected:
        std::unordered_map<std::string,std::string> params_;
        std::mutex paramsLck_;

        std::unordered_map<std::string,int > intParas_;
        std::mutex intParamsLck_;
    };
    typedef OptionMap Options;
}

#include "packet.hpp"
#endif //ADDAUDIOFORSOURCE_COMMON_HPP
