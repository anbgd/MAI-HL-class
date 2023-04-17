#include "wall.h"
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

    void Wall::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Statement create_stmt(session);
            create_stmt << "CREATE TABLE IF NOT EXISTS `Wall` (`id` INT NOT NULL AUTO_INCREMENT,"
                        << "`name` VARCHAR(256) NOT NULL,"
                        << "`login` VARCHAR(256) NOT NULL,"
                        << "`description` VARCHAR(256) NOT NULL,"
                        << "`data` VARCHAR(256) NOT NULL,"
                        << "`creation_date` VARCHAR(256) NULL,"
                        << "`comments` VARCHAR(1024) NULL,"
                        << "PRIMARY KEY (`id`),KEY `fn` (`name`),KEY `ln` (`login`));",
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

    Poco::JSON::Object::Ptr Wall::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("name", _name);
        root->set("login", _login);
        root->set("description", _description);
        root->set("data", _data);
        root->set("creation_date", _creation_date);
        root->set("comments", _comments);

        return root;
    }

    Poco::JSON::Object::Ptr Wall::toJSONComments() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("comments", _comments);

        return root;
    }

    Wall Wall::fromJSON(const std::string &str)
    {
        Wall wall;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        wall.id() = object->getValue<long>("id");
        wall.name() = object->getValue<std::string>("name");
        wall.login() = object->getValue<std::string>("login");
        wall.description() = object->getValue<std::string>("description");
        wall.data() = object->getValue<std::string>("data");
        wall.creation_date() = object->getValue<std::string>("creation_date");
        wall.comments() = object->getValue<std::string>("comments");

        return wall;
    }

    std::optional<Wall> Wall::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Wall a;
            select << "SELECT id, name, login, description, data, creation_date, comments FROM Wall where id=?",
                    into(a._id),
                    into(a._name),
                    into(a._login),
                    into(a._description),
                    into(a._data),
                    into(a._creation_date),
                    into(a._comments),
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

    std::optional<Wall> Wall::get_comments(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            Wall a;
            select << "SELECT id, comments FROM Wall where id=?",
                    into(a._id),
                    into(a._comments),
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

    void Wall::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);

            insert << "INSERT INTO Wall (name,login,creation_date,comments,description,data) VALUES(?, ?, ?, ?, ?, ?)",
                    use(_name),
                    use(_login),
                    use(_description),
                    use(_data),
                    use(_creation_date),
                    use(_comments);

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

    void Wall::edit_post(long id)
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement update(session);

            update << "UPDATE Wall SET name = ?, login = ?, description = ?, data = ?, creation_date = ?, comments = ? WHERE id = ?",
                    use(_name),
                    use(_login),
                    use(_description),
                    use(_data),
                    use(_creation_date),
                    use(_comments),
                    use(id);

            update.execute();

            std::cout << "edited:" << _id << std::endl;

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

    const std::string &Wall::get_name() const
    {
        return _name;
    }

    const std::string &Wall::get_login() const
    {
        return _login;
    }

    std::string &Wall::name()
    {
        return _name;
    }

    std::string &Wall::login()
    {
        return _login;
    }

    long Wall::get_id() const
    {
        return _id;
    }

    const std::string &Wall::get_description() const
    {
        return _description;
    }

    const std::string &Wall::get_creation_date() const
    {
        return _creation_date;
    }

    const std::string &Wall::get_comments() const
    {
        return _comments;
    }

    const std::string &Wall::get_data() const
    {
        return _data;
    }

    long &Wall::id()
    {
        return _id;
    }

    std::string &Wall::description()
    {
        return _description;
    }

    std::string &Wall::creation_date()
    {
        return _creation_date;
    }

    std::string &Wall::comments()
    {
        return _comments;
    }

    std::string &Wall::data()
    {
        return _data;
    }



}
