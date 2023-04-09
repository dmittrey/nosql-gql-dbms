# NoSQL GQL syntax dbms

## Модуль dbms

Модуль представляет из себя трехслойную архитектуру системы управления базами данных. Путь к файлу кодируется с помощью параметра передаваемого в cli. Также передается адрес и порт.

## Модуль GQLclient

Модуль представляет клиента который принимает запросы на языке GraphQL и посылает их серверу на адрес и порт который указаны при запуске клиента.

## Модуль protos

В файле dbms.proto указана конфигурация передваемых сообщений и API сервиса.

# Технологии

- Flex
- Bison
- gRPC
- Protobuf
- CMake


