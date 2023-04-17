#ifndef WALL_H
#define WALL_H

#include <string>
#include <vector>
#include "Poco/JSON/Object.h"
#include <optional>

namespace database
{
    class Wall{
    private:
        long _id;
        std::string _name;
        std::string _login;
        std::string _description;
        std::string _data;
        std::string _creation_date;
        std::string _comments;

    public:

        static Wall fromJSON(const std::string & str);

        long get_id() const;
        const std::string &get_name() const;
        const std::string &get_login() const;
        const std::string &get_description() const;
        const std::string &get_data() const;
        const std::string &get_creation_date() const;
        const std::string &get_comments() const;

        long& id();
        std::string &name();
        std::string &login();
        std::string &description();
        std::string &data();
        std::string &creation_date();
        std::string &comments();

        static void init();
        static std::optional<Wall> read_by_id(long id);
        void save_to_mysql();
        void edit_post(long id);
        void add_comments(std::string comment);
        static std::optional<Wall> get_comments(long id);


        Poco::JSON::Object::Ptr toJSON() const;

        Poco::JSON::Object::Ptr toJSONComments() const;
    };
}

#endif // WALL_H
