#ifndef MAI_HL_CLASS_HTTP_REQUEST_FACTORY_H
#define MAI_HL_CLASS_HTTP_REQUEST_FACTORY_H
#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/Exception.h"
#include "Poco/ThreadPool.h"
#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <iostream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "handlers/chat_handler.h"
#include "handlers/user_handler.h"
#include "handlers/wall_handler.h"

[[maybe_unused]] static bool startsWith(const std::string &str, const std::string &prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

class HTTPRequestFactory : public HTTPRequestHandlerFactory
{
public:
    HTTPRequestFactory(const std::string &format) : _format(format)
    {
    }

    HTTPRequestHandler *createRequestHandler(
            const HTTPServerRequest &request)
    {
        std::cout << "request:" << request.getURI()<< std::endl;
        if (hasSubstr(request.getURI(),"/send") ||
            hasSubstr(request.getURI(),"/chat"))
            return new ChatHandler(_format);
        if (hasSubstr(request.getURI(),"/user") ||
            hasSubstr(request.getURI(),"/searchbynames") ||
            hasSubstr(request.getURI(),"/auth") ||
            hasSubstr(request.getURI(),"/searchbylogin"))
            return new UserHandler(_format);
        if (hasSubstr(request.getURI(),"/dopost") ||
            hasSubstr(request.getURI(),"/getpost") ||
            hasSubstr(request.getURI(),"/changepost") ||
            hasSubstr(request.getURI(),"/checkcomments") ||
            hasSubstr(request.getURI(),"/addcomment")
                )
            return new WallHandler(_format);
        return 0;
    }

private:
    std::string _format;
};
#endif //MAI_HL_CLASS_HTTP_REQUEST_FACTORY_H