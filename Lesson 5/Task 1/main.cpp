#include <iostream>
#include <string>

#include <pqxx/pqxx>
#include <Windows.h>

#pragma execution_character_set("utf-8")

enum class Commands {
    CREATE_TABLES = 1,
    INSERT_CLIENT,
    INSERT_PHONE,
    UPDATE_CLIENT,
    DELETE_PHONE,
    DELETE_CLIENT,
    SELECT_ALL,
    FIND_CLIENT
};

enum class ClientField {
    FIRST_NAME=1,
    LAST_NAME,
    EMAIL,
    PHONE
};

class ClientDB {
private:
    std::unique_ptr<pqxx::connection> con;

public:
    ClientDB(const std::string& dbname, const std::string& user, const std::string password, const std::string& host = "localhost", const std::string& port = "5432") {
        con = std::make_unique<pqxx::connection>(
            "host=" + host +
            " port=" + port +
            " dbname=" + dbname +
            " user=" + user +
            " password=" + password
        );
        con->prepare("insert_client", "INSERT INTO client (first_name, last_name, email) VALUES ($1, $2, $3)");
        con->prepare("insert_phone", "INSERT INTO phone (phone_number, client_id) VALUES ($1, $2)");
        con->prepare("update_client", "UPDATE client SET first_name = $2, last_name = $3, email = $4 WHERE id = $1");
        con->prepare("delete_phone", "DELETE FROM phone WHERE id = $1");
        con->prepare("delete_client", "DELETE FROM client WHERE id = $1");        
    }

    void create_tables() {
        pqxx::work tx(*con);
        tx.exec("CREATE TABLE IF NOT EXISTS client (id SERIAL PRIMARY KEY, first_name VARCHAR(40) NOT NULL, last_name VARCHAR(60) NOT NULL, email VARCHAR(80) NOT NULL)");
        tx.exec("CREATE TABLE IF NOT EXISTS phone (id SERIAL PRIMARY KEY, phone_number VARCHAR(12) NOT NULL, client_id INTEGER NOT NULL REFERENCES client(id) ON UPDATE CASCADE ON DELETE CASCADE)");
        tx.commit();
    }

    void add_client(const std::string& first_name, const std::string& last_name, const std::string& email) const {
        pqxx::work tx(*con);
        tx.exec_prepared("insert_client", first_name, last_name, email);
        tx.commit();
    }

    void add_phone(const std::string& phone, const std::string& client_id) const {
        pqxx::work tx(*con);
        tx.exec_prepared("insert_phone", phone, client_id);
        tx.commit();
    }

    void update_client(const std::string& id, const std::string& first_name, const std::string& last_name, const std::string& email) const {
        pqxx::work tx(*con);
        tx.exec_prepared("update_client", id, first_name, last_name, email);
        tx.commit();
    }

    void delete_phone(const std::string& id) const {
        pqxx::work tx(*con);
        tx.exec_prepared("delete_phone", id);
        tx.commit();
    }

    void delete_client(const std::string& id) const {
        pqxx::work tx(*con);
        tx.exec_prepared("delete_client", id);
        tx.commit();
    }

    auto select_all() const {
        pqxx::work tx(*con);
        return tx.query<int, std::string, std::string, std::string, int, std::string>("SELECT c.id, c.first_name, c.last_name, c.email, COALESCE(p.id, 0), COALESCE(p.phone_number, 'no phone') FROM client c LEFT JOIN phone p ON c.id = p.client_id");
    }

    template<ClientField Field>
    auto find_client(const std::string& data) const {
        pqxx::work tx(*con);
        std::string query = "SELECT c.id, c.first_name, c.last_name, c.email, COALESCE(p.id, 0), COALESCE(p.phone_number, 'no phone') FROM client c LEFT JOIN phone p ON c.id = p.client_id";
        switch (Field) {
        case ClientField::FIRST_NAME:
            return tx.query<int, std::string, std::string, std::string, int, std::string>(query + " WHERE c.first_name = '" + data + "'");
        case ClientField::LAST_NAME:
            return tx.query<int, std::string, std::string, std::string, int, std::string>(query + " WHERE c.last_name = '" + data + "'");
        case ClientField::EMAIL:
            return tx.query<int, std::string, std::string, std::string, int, std::string>(query + " WHERE c.email = '" + data + "'");
        case ClientField::PHONE:
            return tx.query<int, std::string, std::string, std::string, int, std::string>(query + " WHERE p.phone_number = '" + data + "'");
        }

    }
};

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    try {
        std::string dbname;
        std::cout << "Введите название БД: ";
        std::cin >> dbname;

        std::string user;
        std::cout << "Введите имя пользователя: ";
        std::cin >> user;

        std::string password;
        std::cout << "Введите пароль: ";
        std::cin >> password;

        ClientDB clients(dbname, user, password);

        std::string first_name, last_name, email;
        std::string phone;
        std::string client_id, phone_id;
        std::string data;

        int cmd{};
        int field{};

        do {
            std::cout << "\nКоманды:\n"
                << "1 - создать таблицы \"Клиент\" и \"Телефон\"\n"
                << "2 - добавить данные о клиенте (имя, фамилия, email)\n"
                << "3 - добавить номер телефона (+ХХХХХХХХХХХ)\n"
                << "4 - изменить данные о клиенте по id (имя, фамилия, email)\n"
                << "5 - удалить телефон по id\n"
                << "6 - удалить клиента и его телефоны по id\n"
                << "7 - вывести все данные о клиентах и телефонах\n"
                << "8 - вывести данные о клиенте по его имени, фамилии, email или номеру телефона\n"
                << "0 - выход из программы" << std::endl;
            std::cin >> cmd;
            switch (static_cast<Commands>(cmd)) {
            case Commands::CREATE_TABLES:
                clients.create_tables();
                break;
            case Commands::INSERT_CLIENT:
                std::cout << "\nВведите имя клиента: ";
                std::cin >> first_name;
                std::cout << "Введите фамилию клиента: ";
                std::cin >> last_name;
                std::cout << "Введите email клиента: ";
                std::cin >> email;
                clients.add_client(first_name, last_name, email);
                break;
            case Commands::INSERT_PHONE:
                std::cout << "\nВведите номер телефона: ";
                std::cin >> phone;
                std::cout << "Введите id клиента: ";
                std::cin >> client_id;
                clients.add_phone(phone, client_id);
                break;
            case Commands::UPDATE_CLIENT:
                std::cout << "\nВведите id клиента: ";
                std::cin >> client_id;
                std::cout << "Введите имя клиента: ";
                std::cin >> first_name;
                std::cout << "Введите фамилию клиента: ";
                std::cin >> last_name;
                std::cout << "Введите email клиента: ";
                std::cin >> email;
                clients.update_client(client_id, first_name, last_name, email);
                break;
            case Commands::DELETE_PHONE:
                std::cout << "\nВведите id телефона: ";
                std::cin >> phone_id;
                clients.delete_phone(phone_id);
                break;
            case Commands::DELETE_CLIENT:
                std::cout << "\nВведите id клиента: ";
                std::cin >> client_id;
                clients.delete_client(client_id);
                break;
            case Commands::SELECT_ALL:
                for (auto [client_id, first_name, last_name, email, phone_id, phone] : clients.select_all()) {
                    std::cout << '\n' << client_id << '\t' << first_name << '\t' << last_name << '\t' << email << '\t' << phone_id << '\t' << phone;;
                }
                break;
            case Commands::FIND_CLIENT:
                std::cout << "\nВыберите фильтр для поиска:\n"
                    << "1 - по имени\n"
                    << "2 - по фамилии\n"
                    << "3 - по email\n"
                    << "4 - по номеру телефона\n";
                std::cin >> field;
                std::cout << "\nВведите данные для поиска: ";
                std::cin >> data;
                switch (static_cast<ClientField>(field)) {
                case ClientField::FIRST_NAME:
                    for (auto [client_id, first_name, last_name, email, phone_id, phone] : clients.find_client<ClientField::FIRST_NAME>(data)) {
                        std::cout << '\n' << client_id << '\t' << first_name << '\t' << last_name << '\t' << email << '\t' << phone_id << '\t' << phone;
                    }
                    std::cout << std::endl;
                    break;
                case ClientField::LAST_NAME:
                    for (auto [client_id, first_name, last_name, email, phone_id, phone] : clients.find_client<ClientField::LAST_NAME>(data)) {
                        std::cout << '\n' << client_id << '\t' << first_name << '\t' << last_name << '\t' << email << '\t' << phone_id << '\t' << phone;
                    }
                    std::cout << std::endl;
                    break;
                case ClientField::EMAIL:
                    for (auto [client_id, first_name, last_name, email, phone_id, phone] : clients.find_client<ClientField::EMAIL>(data)) {
                        std::cout << '\n' << client_id << '\t' << first_name << '\t' << last_name << '\t' << email << '\t' << phone_id << '\t' << phone;
                    }
                    std::cout << std::endl;
                    break;
                case ClientField::PHONE:
                    for (auto [client_id, first_name, last_name, email, phone_id, phone] : clients.find_client<ClientField::PHONE>(data)) {
                        std::cout << '\n' << client_id << '\t' << first_name << '\t' << last_name << '\t' << email << '\t' << phone_id << '\t' << phone;
                    }
                    std::cout << std::endl;
                    break;
                default:
                    std::cout << "\nПоиск отменен" << std::endl;
                }
                break;
            }
        } while (cmd != 0);
    }
    catch (const std::exception& err) {
        std::cout << err.what() << std::endl;
    }
}