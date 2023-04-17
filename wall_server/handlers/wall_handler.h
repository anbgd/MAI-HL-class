#ifndef HL_SERVER_WALL_HANDLER_H
#define HL_SERVER_WALL_HANDLER_H

#include "Poco/Net/HTTPServer.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPServerRequest.h"
#include "Poco/Net/HTTPServerResponse.h"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTMLForm.h"
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
#include <iostream>
#include <fstream>

using Poco::DateTimeFormat;
using Poco::DateTimeFormatter;
using Poco::ThreadPool;
using Poco::Timestamp;
using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerParams;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Net::NameValueCollection;
using Poco::Net::ServerSocket;
using Poco::Util::Application;
using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

#include "../../database/wall.h"
#include "../../helper.h"

static bool hasSubstr(const std::string &str, const std::string &substr)
{
    if (str.size() < substr.size())
        return false;
    for (size_t i = 0; i <= str.size() - substr.size(); ++i)
    {
        bool ok{true};
        for (size_t j = 0; ok && (j < substr.size()); ++j)
            ok = (str[i + j] == substr[j]);
        if (ok)
            return true;
    }
    return false;
}

class WallHandler : public HTTPRequestHandler
{
private:

public:
    WallHandler(const std::string &format) : _format(format)
    {
    }


    void handleRequest(HTTPServerRequest &request,
                       HTTPServerResponse &response)
    {
        HTMLForm form(request, request.stream());
        try
        {
            if (hasSubstr(request.getURI(), "/dopost"))
            {
                database::Wall wall;
                wall.name() = form.get("name");
                wall.login() = form.get("login");
                wall.description() = form.get("description");
                wall.data() = form.get("data");
                wall.creation_date() = form.get("creation_date");
                wall.comments() = form.get("comments");

                bool check_result = true;
                std::string message;
                std::string reason;

                if (check_result)
                {
                    wall.save_to_mysql();
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    ostr << wall.get_id();
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    std::ostream &ostr = response.send();
                    ostr << message;
                    response.send();
                    return;
                }

            }
            else if (hasSubstr(request.getURI(), "/getpost"))
            {
                long id = atol(form.get("id").c_str());

                std::optional<database::Wall> result = database::Wall::read_by_id(id);
                if (result)
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(result->toJSON(), ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/not_found");
                    root->set("title", "Internal exception");
                    root->set("status", "404");
                    root->set("detail", "wall not found");
                    root->set("instance", "/wall");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }
            }
            else if (hasSubstr(request.getURI(), "/changepost"))
            {
                database::Wall wall;
                long id = atol(form.get("id").c_str());

                std::optional<database::Wall> result = database::Wall::read_by_id(id);
                if (form.has("name")) {
                    wall.name() = form.get("name");
                } else {
                    wall.name() = result->name();
                }
                if (form.has("login")) {
                    wall.login() = form.get("login");
                } else {
                    wall.login() = result->login();
                }
                if (form.has("description")) {
                    wall.description() = form.get("description");
                } else {
                    wall.description() = result->description();
                }
                if (form.has("data")) {
                    wall.data() = form.get("data");
                } else {
                    wall.data() = result->data();
                }
                if (form.has("creation_date")) {
                    wall.creation_date() = form.get("creation_date");
                } else {
                    wall.creation_date() = result->creation_date();
                }
                if (form.has("comments")) {
                    wall.comments() = form.get("comments");
                } else {
                    wall.comments() = result->comments();
                }

                bool check_result = true;
                std::string message;
                std::string reason;

                if (check_result)
                {
                    wall.edit_post(id);
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(wall.toJSON(), ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    std::ostream &ostr = response.send();
                    ostr << message;
                    response.send();
                    return;
                }

            }
            else if (hasSubstr(request.getURI(), "/checkcomments"))
            {
                long id = atol(form.get("id").c_str());

                std::optional<database::Wall> result = database::Wall::get_comments(id);
                if (result)
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(result->toJSONComments(), ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
                    root->set("type", "/errors/not_found");
                    root->set("title", "Internal exception");
                    root->set("status", "404");
                    root->set("detail", "wall not found");
                    root->set("instance", "/wall");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(root, ostr);
                    return;
                }


            }
            else if (hasSubstr(request.getURI(), "/addcomment"))
            {
                long id = atol(form.get("id").c_str());

                std::optional<database::Wall> result = database::Wall::read_by_id(id);

                database::Wall wall;
                wall.name() = result->name();
                wall.login() = result->login();
                wall.description() = result->description();
                wall.data() = result->data();
                wall.creation_date() = result->creation_date();
                wall.comments() = result->comments() + ";" + form.get("comments");

                bool check_result = true;
                std::string message;
                std::string reason;

                if (check_result)
                {
                    wall.edit_post(id);
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_OK);
                    response.setChunkedTransferEncoding(true);
                    response.setContentType("application/json");
                    std::ostream &ostr = response.send();
                    Poco::JSON::Stringifier::stringify(wall.toJSONComments(), ostr);
                    return;
                }
                else
                {
                    response.setStatus(Poco::Net::HTTPResponse::HTTP_NOT_FOUND);
                    std::ostream &ostr = response.send();
                    ostr << message;
                    response.send();
                    return;
                }

            }
        }
        catch (...)
        {
        }

        response.setStatus(Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        response.setChunkedTransferEncoding(true);
        response.setContentType("application/json");
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();
        root->set("type", "/errors/not_found");
        root->set("title", "Internal exception");
        root->set("status", Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_FOUND);
        root->set("detail", "request not found");
        root->set("instance", "/wall");
        std::ostream &ostr = response.send();
        Poco::JSON::Stringifier::stringify(root, ostr);
    }

private:
    std::string _format;
};

#endif //HL_SERVER_WALL_HANDLER_H
