//
// Created by 35148 on 2024/7/19.
//
#include "http_session.h"
namespace hh{
    namespace http{
        HttpSession::HttpSession(Socket::ptr sock, bool owner):
        hh::SocketStream (sock,owner){
        }
        HttpRequest::ptr HttpSession::recvRequest() {
            HttpRequestParser::ptr parser(new HttpRequestParser);
            uint64_t buff_size = HttpRequestParser::getHttpRequestBufferSize();
            //uint64_t buff_size = 100;
            std::shared_ptr<char> buffer(
                    new char[buff_size], [](char* ptr){
                        delete[] ptr;
                    });
            char* data = buffer.get();
            int offset = 0;
            do {
                int len = read(data + offset, buff_size - offset);
                if(len <= 0) {
                    close();
                    return nullptr;
                }
                len += offset;
                size_t nparse = parser->execute(data, len);
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

        int HttpSession::sendResponse(HttpResponse::ptr rsp){
            std::string string = rsp->toString();
            return writeFixSize(string.c_str(),string.size());
        }
    }
}