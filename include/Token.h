/*
 * Copyright 2009-2017 Alibaba Cloud All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ALIBABACLOUD_CORE_TOKEN_H_
#define ALIBABACLOUD_CORE_TOKEN_H_

#ifdef _WIN32
#ifndef NLS_TOKEN_API
#define NLS_TOKEN_API _declspec(dllexport)
#endif
#else
#define NLS_TOKEN_API
#endif

#include <string>

namespace NlsSdkToken {
    class NLS_TOKEN_API NlsToken {
    public:

        virtual ~NlsToken();
        NlsToken();

        /**
            * @brief 获取tokenId.
            * @note 调用之前, 请先设置请求参数. token值有效期默认24小时.
            * @return 成功则返回0, token可以获取token值; 失败返回-1, token返回失败原因
            */
         int getNlsSdkToken(std::string& token);

        /**
            * @brief 设置阿里云账号的KeySecret
            * @param KeySecret	Secret字符串
            * @return void
            */
        void setKeySecret(std::string KeySecret);

        /**
            * @brief 设置阿里云账号的KeyId
            * @param KeyId	Id字符串
            * @return void
            */
        void setAccessKeyId(std::string accessKeyId);

        /**
            * @brief 设置域名
            * @param domain	域名字符串
            * @return void
            */
        void setDomain(std::string domain);

        /**
            * @brief 设置API版本
            * @param serverVersion	API版本字符串
            * @return void
            */
        void setServerVersion(std::string serverVersion);

        /**
            * @brief 设置服务路径
            * @param serverResourcePath	服务路径字符串
            * @return void
            */
        void setServerResourcePath(std::string serverResourcePath);

    private:
        std::string accessKeySecret_;
        std::string accessKeyId_;

        std::string domain_;
        std::string serverVersion_;
        std::string serverResourcePath_;
    };

}


#endif //ALIBABACLOUD_CORE_TOKEN_H_
