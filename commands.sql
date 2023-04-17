CREATE DATABASE IF NOT EXISTS stud;
use stud;
show tables;

create table IF NOT EXISTS Chat
(
    id          int auto_increment
        primary key,
    receiver_id int(100)                              null,
    sender_id   int(100)                              null,
    message     varchar(1000) collate utf8_unicode_ci null
);

create index IF NOT EXISTS ri
    on Chat (receiver_id);

create index IF NOT EXISTS si
    on Chat (sender_id);

create table IF NOT EXISTS User
(
    id         int auto_increment
        primary key,
    first_name varchar(256) collate utf8_unicode_ci not null,
    last_name  varchar(256) collate utf8_unicode_ci not null,
    email      varchar(256) collate utf8_unicode_ci null,
    birth_date varchar(10) collate utf8_unicode_ci  null,
    login      varchar(10) collate utf8_unicode_ci  null,
    password   varchar(10) collate utf8_unicode_ci  null
);

create index IF NOT EXISTS fn
    on User (first_name);

create index IF NOT EXISTS ln
    on User (last_name);

create table IF NOT EXISTS Wall
(
    id            int auto_increment
        primary key,
    name          varchar(256) collate utf8_unicode_ci  not null,
    login         varchar(10) collate utf8_unicode_ci   not null,
    description   varchar(256) collate utf8_unicode_ci  null,
    data          varchar(1000) collate utf8_unicode_ci null,
    creation_date varchar(10) collate utf8_unicode_ci   null,
    comments      longtext collate utf8_unicode_ci      null
);

INSERT INTO stud.Chat (receiver_id, sender_id, message)
VALUES (1, 2, 'dada');

INSERT INTO stud.User (first_name, last_name, email, birth_date, login, password)
VALUES ('Ilya', 'Chernenko', 'chern@gmail.com', '01.02.00', 'coolgut', 'secret');

INSERT INTO stud.Wall (name, login, description, data, creation_date, comments)
VALUES ('random', 'coolgut', 'coolimage', 'imagineimage', '11.03.2023', 'sad;');




