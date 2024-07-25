//
// Created by 35148 on 2024/7/25.
//
#include "http_connection.h"
namespace hh{
    namespace http{

        HttpConnection::HttpConnection(Socket::ptr sock, bool owner)
        : SocketStream(sock, owner) {
        }

        HttpResponse::ptr HttpConnection::recvResponse()  {
            HttpResponseParser::ptr parser(new HttpResponseParser);
            uint64_t buff_size = HttpResponseParser::getHttpResponseBufferSize();
            //uint64_t buff_size = 100;
            std::shared_ptr<char> buffer(
                    new char[buff_size], [](char* ptr){
                        delete[] ptr;
                    });
            char* data = buffer.get();
            // 处理完请求体数据，剩余的数据长度
            int offset = 0;
            do {
                int len = read(data + offset, buff_size - offset);
                if(len <= 0) {
                    close();
                    return nullptr;
                }
                len += offset;
                size_t nparse = parser->execute(data, len, false);
                if(parser->isError()) {
                    close();
                    return nullptr;
                }
                offset = len - nparse;
                if(offset == (int)buff_size) {
                    close();
                    return nullptr;
                }
                if(parser->isFinish()) {
                    break;
                }
            } while(true);
            int64_t length = parser->getContentLength();
            if(length > 0) {
                std::string body;
                body.resize(length);

                int len = 0;
                if(length >= offset) {
                    memcpy(&body[0], data, offset);
                    len = offset;
                } else {
                    memcpy(&body[0], data, length);
                    len = length;
                }
                length -= offset;
                // 在请求体中的的数据长度-解析剩余的长度，如果>0 说明还有，需要强制读取完
                if(length > 0) {
                    if(readFixSize(&body[len], length) <= 0) {
                        close();
                        return nullptr;
                    }
                }
                parser->getData()->setBody(body);
            }

            return parser->getData();
        }

        int HttpConnection::sendRequest(HttpRequest::ptr rsp) {
            std::string str = rsp->toString();
            return writeFixSize(str.c_str(), str.size());
        }
    }
}