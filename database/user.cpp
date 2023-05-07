#include "user.h"
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
#include <future>

using namespace Poco::Data::Keywords;
using Poco::Data::Session;
using Poco::Data::Statement;

namespace database
{

    void User::init()
    {
        try
        {

            Poco::Data::Session session = database::Database::get().create_session();
            for (auto &hint : database::Database::get_all_hints()) {
                Statement create_stmt(session);
                create_stmt << "CREATE TABLE IF NOT EXISTS `User` (`id` INT NOT NULL AUTO_INCREMENT,"
                            << "`first_name` VARCHAR(256) NOT NULL,"
                            << "`last_name` VARCHAR(256) NOT NULL,"
                            << "`login` VARCHAR(256) NOT NULL,"
                            << "`password` VARCHAR(256) NOT NULL,"
                            << "`email` VARCHAR(256) NULL,"
                            << "`birth_date` VARCHAR(1024) NULL,"
                            << "PRIMARY KEY (`id`),KEY `fn` (`first_name`),KEY `ln` (`last_name`));",
                        now;
            }
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

    Poco::JSON::Object::Ptr User::toJSON() const
    {
        Poco::JSON::Object::Ptr root = new Poco::JSON::Object();

        root->set("id", _id);
        root->set("first_name", _first_name);
        root->set("last_name", _last_name);
        root->set("email", _email);
        root->set("birth_date", _birth_date);
        root->set("login", _login);
        root->set("password", _password);

        return root;
    }

    User User::fromJSON(const std::string &str)
    {
        User user;
        Poco::JSON::Parser parser;
        Poco::Dynamic::Var result = parser.parse(str);
        Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

        user.id() = object->getValue<long>("id");
        user.first_name() = object->getValue<std::string>("first_name");
        user.last_name() = object->getValue<std::string>("last_name");
        user.email() = object->getValue<std::string>("email");
        user.birth_date() = object->getValue<std::string>("birth_date");
        user.login() = object->getValue<std::string>("login");
        user.password() = object->getValue<std::string>("password");

        return user;
    }

    std::optional<long> User::auth(std::string &login, std::string &password)
    {
        try
        {
            std::vector<long> result;
            std::vector<std::string> hints = database::Database::get_all_hints();

            std::vector<std::future<std::vector<long>>> futures;

            for (const std::string &hint : hints)
            {
                auto handle = std::async(std::launch::async,
                                         [login, password, hint]() mutable -> std::vector<long>
                {
                    std::vector<long> result;

                    Poco::Data::Session session = database::Database::get().create_session();
                    Poco::Data::Statement select(session);
                    long id;
                    select << "SELECT id FROM User where login=? and password=?" + hint,
                            into(id),
                            use(login),
                            use(password),
                            range(0, 1); //  iterate over result set one row at a time

                    select.execute();
                    Poco::Data::RecordSet rs(select);

                    bool more = rs.moveFirst();
                    while (more)
                    {
                        long a;
                        a = rs[0].convert<long>();
                        result.push_back(a);
                        more = rs.moveNext();
                    }
                    return result; });
                    futures.emplace_back(std::move(handle));
            }

            for (std::future<std::vector<long>> &res : futures)
            {
                std::vector<long> v = res.get();
                std::copy(std::begin(v),
                          std::end(v),
                          std::back_inserter(result));
            }
            if (result.empty()) return result[0];
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
    std::optional<User> User::read_by_id(long id)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            User a;
            std::string sharding_hint = database::Database::sharding_hint(id);
            select << "SELECT id, first_name, last_name, email, birth_date,login,password FROM User where id=?" + sharding_hint,
                into(a._id),
                into(a._first_name),
                into(a._last_name),
                into(a._email),
                into(a._birth_date),
                into(a._login),
                into(a._password),
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

    std::vector<User> User::search(std::string first_name, std::string last_name)
    {
        try
        {
            std::vector<User> result;

            // get all hints for shards
            std::vector<std::string> hints = database::Database::get_all_hints();

            std::vector<std::future<std::vector<User>>> futures;
            first_name + "%";
            last_name + "%";
            // map phase in parallel
            for (const std::string &hint : hints)
            {
                auto handle = std::async(std::launch::async, [first_name, last_name, hint]() mutable -> std::vector<User>
                {
                    std::vector<User> result;
                    Poco::Data::Session session = database::Database::get().create_session();
                    Statement select(session);
                    select << "SELECT id, first_name, last_name, email, birth_date, login, password FROM User where first_name LIKE ? and last_name LIKE ?" + hint,
                            use(first_name),
                            use(last_name),
                            range(0, 1); //  iterate over result set one row at a times

                    select.execute();
                    Poco::Data::RecordSet rs(select);
                    bool more = rs.moveFirst();
                    while (more)
                    {
                        User a;
                        a._id = rs[0].convert<long>();
                        a._first_name = rs[1].convert<std::string>();
                        a._last_name = rs[2].convert<std::string>();
                        a._login = rs[3].convert<std::string>();
                        a._password = rs[4].convert<std::string>();
                        a._email = rs[5].convert<std::string>();
                        a._birth_date = rs[6].convert<std::string>();
                        result.push_back(a);
                        more = rs.moveNext();
                    }
                    return result;
                });
                futures.emplace_back(std::move(handle));
            }


            for (std::future<std::vector<User>> &res : futures)
            {
                std::vector<User> v = res.get();
                std::copy(std::begin(v),
                          std::end(v),
                          std::back_inserter(result));
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

    std::vector<User> User::searchbylogin(std::string login)
    {
        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Statement select(session);
            std::vector<User> result;
            User a;
            login += "%";
            select << "SELECT id, first_name, last_name, email, birth_date, login, password FROM User where login LIKE ?",
                    into(a._id),
                    into(a._first_name),
                    into(a._last_name),
                    into(a._email),
                    into(a._birth_date),
                    into(a._login),
                    into(a._password),
                    use(login),
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
    } //to do

    long User::db_size()
    {
        long result = 0;
        std::vector<std::string> hints = database::Database::get_all_hints();
        for (const std::string &hint : hints)
        {

            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement select(session);
            long a;
            std::string select_str = "SELECT COUNT(*) FROM User";
            select_str += hint;
            select << select_str,
                    into(a),
                    range(0, 1);

            select.execute();
            Poco::Data::RecordSet rs(select);

            if (rs.moveFirst()) {
                result += a;
            }
        }
        return result;
    }

    void User::save_to_mysql()
    {

        try
        {
            Poco::Data::Session session = database::Database::get().create_session();
            Poco::Data::Statement insert(session);
            long db_size = User::db_size();
            db_size += 1;
            std::string sharding_hint = database::Database::sharding_hint(db_size);

            insert << "INSERT INTO User (first_name,last_name,email,birth_date,login,password) VALUES(?, ?, ?, ?, ?, ?)" + sharding_hint,
                use(_first_name),
                use(_last_name),
                use(_email),
                use(_birth_date),
                use(_login),
                use(_password);

            insert.execute();

            Poco::Data::Statement select(session);
            select << "SELECT LAST_INSERT_ID()" + sharding_hint,
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

    const std::string &User::get_login() const
    {
        return _login;
    }

    const std::string &User::get_password() const
    {
        return _password;
    }

    std::string &User::login()
    {
        return _login;
    }

    std::string &User::password()
    {
        return _password;
    }

    long User::get_id() const
    {
        return _id;
    }

    const std::string &User::get_first_name() const
    {
        return _first_name;
    }

    const std::string &User::get_last_name() const
    {
        return _last_name;
    }

    const std::string &User::get_email() const
    {
        return _email;
    }

    const std::string &User::get_birth_date() const
    {
        return _birth_date;
    }

    long &User::id()
    {
        return _id;
    }

    std::string &User::first_name()
    {
        return _first_name;
    }

    std::string &User::last_name()
    {
        return _last_name;
    }

    std::string &User::email()
    {
        return _email;
    }

    std::string &User::birth_date()
    {
        return _birth_date;
    }
}