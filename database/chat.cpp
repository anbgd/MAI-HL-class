#include "chat.h"
#include "database.h"
#include "../config/config.h"

#include <Poco/Data/MySQL/Connector.h>
#include <Poco/Data/MySQL/MySQLException.h>
#include <Poco/Data/SessionFactory.h>
#include <Poco/Data/RecordSet.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Dynamic/Var.h>

#include <sstream>
#include <exception>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void Chat::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Chat` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`receiver_id` INT(100) NOT NULL,"
                        << "`sender_id` INT(100) NOT NULL,"
                        << "`message` VARCHAR(1000) NOT NULL,"
                        << "PRIMARY KEY (`id`),KEY `ri` (`receiver_id`),KEY `si` (`sender_id`));",
                    now;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    Poco::JSON::Object::Ptr Chat::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("receiver_id", _receiver_id);
        root->set("sender_id", _sender_id);
        root->set("message", _message);

        return root;
    }

    Chat Chat::fromJSON(const std::string &str)
    {
        Chat chat;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        chat.id() = object->getValue<long>("id");
        chat.receiver_id() = object->getValue<long>("receiver_id");
        chat.sender_id() = object->getValue<long>("sender_id");
        chat.message() = object->getValue<std::string>("message");

        return chat;
    }

    std::optional<Chat> Chat::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Chat a;
            select << "SELECT id, receiver_id, sender_id, message  FROM Chat where id=?",
                    into(a._id),
                    into(a._receiver_id),
                    into(a._sender_id),
                    into(a._message),
                    use(id),
                    range(0, 1); //  iterate over result set one row at a time

            select.execute();
            Poco::Data::RecordSet rs(select);
            if (rs.moveFirst()) return a;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;

        }
        return {};
    }

    std::vector<Chat> Chat::read_by_sender(long sender)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            std::vector<Chat> result;
            Chat a;
            select << "SELECT id, receiver_id, sender_id, message FROM Chat where sender_id=?",
                    into(a._id),
                    into(a._receiver_id),
                    into(a._sender_id),
                    into(a._message),
                    use(sender),
                    range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;

        }
        return {};
    }

    std::vector<Chat> Chat::read_all()
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<Chat> result;
            Chat a;
            select << "SELECT id, receiver_id, sender_id, message FROM Chat",
                    into(a._id),
                    into(a._receiver_id),
                    into(a._sender_id),
                    into(a._message),
                    range(0, 1); //  iterate over result set one row at a time

            while (!select.done())
            {
                if (select.execute())
                    result.push_back(a);
            }
            return result;
        }

        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    void Chat::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Chat (receiver_id,sender_id,message) VALUES(?, ?, ?)",
                    use(_receiver_id),
                    use(_sender_id),
                    use(_message);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()",
                    into(_id),
                    range(0, 1); //  iterate over result set one row at a time

            if (!select.done())
            {
                select.execute();
            }
            std::cout << "inserted:" << _id << std::endl;
        }
        catch (Poco::Data::MySQL::ConnectionException &e)
        {
            std::cout << "connection:" << e.what() << std::endl;
            throw;
        }
        catch (Poco::Data::MySQL::StatementException &e)
        {

            std::cout << "statement:" << e.what() << std::endl;
            throw;
        }
    }

    long Chat::get_id() const
    {
        return _id;
    }


    long &Chat::id()
    {
        return _id;
    }

    long &Chat::receiver_id()
    {
        return _receiver_id;
    }

    long &Chat::sender_id()
    {
        return _sender_id;
    }

    std::string &Chat::message(){
        return _message;
    }


}