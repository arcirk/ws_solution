//
// Created by arcady on 14.08.2021.
//

#ifndef WS_SOLUTION_BASE_H
#define WS_SOLUTION_BASE_H

#include "../sqlite/sqlite3.h"
#include "../arc_json/global.h"


namespace arc_sqlite {

//    struct DefTable
//    {
//
//        char** chData;
//
//        void serialize(char* data) {
//            memcpy(data, this, sizeof(DefTable));
//        }
//
//        void deserialize(char* data) {
//            memcpy(this, data, sizeof(DefTable));
//        }
//    };

    enum tables
    {
        eUsers = 0,
        eMessages,
        eChannels,
        eSubscribers,
        //eSeries,
        //eStorage,
        //eStorageCells,
        //eCharacteristics,
        //eDevices,
        eUnknown
    };

    namespace exception{
        static std::string what(int err){

            std::string result = "Не известная ошибка";

            switch (err)
            {
                case SQLITE_OK: result = "Успешное выполнение.";
                break;
                case SQLITE_ERROR: result = "Общая ошибка.";
                break;
                case SQLITE_INTERNAL: result = "Внутренняя логическая ошибка в SQLite.";
                break;
                case SQLITE_PERM: result = "Отказано в доступе.";
                break;
                case SQLITE_ABORT: result = "Процедура обратного вызова запросила прерывание.";
                break;
                case SQLITE_BUSY: result = "Файл базы данных заблокирован.";
                break;
                case SQLITE_LOCKED: result = "Таблица в базе данных заблокирована.";
                break;
                case SQLITE_NOMEM: result = "Сбой выделения памяти.";
                break;
                case SQLITE_READONLY: result = "Попытка записи базы данных только для чтения.";
                break;
                case SQLITE_INTERRUPT: result = "Операция прекращена sqlite3_interrupt ().";
                break;
                case SQLITE_IOERR: result = "Ошибка ввода-вывода.";
                break;
                case SQLITE_CORRUPT: result = "Образ диска базы данных искажен.";
                break;
                case SQLITE_NOTFOUND: result = "Неизвестный код операции.";
                break;
                case SQLITE_FULL: result = "Ошибка вставки из-за переполнения базы данных.";
                break;
                case SQLITE_CANTOPEN: result = "Невозможно открыть файл базы данных.";
                break;
                case SQLITE_PROTOCOL: result = "Ошибка протокола блокировки базы данных.";
                break;
                case SQLITE_EMPTY: result = "Только для внутреннего использования.";
                break;
                case SQLITE_SCHEMA: result = "Схема базы данных изменена.";
                break;
                case SQLITE_TOOBIG: result = "Строка или большой двоичный объект превышает ограничение размера.";
                break;
                case SQLITE_CONSTRAINT: result = "Прервано из-за нарушения ограничения.";
                break;
                case SQLITE_MISMATCH: result = "Несоответствие типов данных.";
                break;
                case SQLITE_MISUSE: result = "Неверный параметр или другое неправильное использование SQLite API.";
                break;
                case SQLITE_NOLFS: result = "Поддержка больших файлов отключена.";
                break;
                case SQLITE_AUTH: result = "Авторизация запрещена.";
                break;
                case SQLITE_FORMAT: result = "Формат не поддерживается.";
                break;
                case SQLITE_RANGE: result = "Индекс поля вне диапазона.";
                break;
                case SQLITE_NOTADB: result = "Открыт файл, который не является файлом базы данны.";
                break;
                case SQLITE_NOTICE: result = "Уведомления от sqlite3_log().";
                break;
                case SQLITE_WARNING: result = "Предупреждения из sqlite3_log().";
                break;
                case SQLITE_ROW: result = "sqlite3_step () подготовил еще одну строку.";
                break;
                case SQLITE_DONE: result = "sqlite3_step () завершил выполнение.";
                break;
                default:
                    result = "";
                    break;
            }

            return result;
        }
    }

    class sqlite3_db{

    public:
        sqlite3_db();
        ~sqlite3_db();

        bool open(const std::string& file="");

        void close();

        std::string get_database_file();

        void check_database_table(tables tableType);

        static std::string createQuery(tables tableType);

        static std::string get_query_template(tables tableType);

        int exec(const std::string& query, std::string& error = (std::string &) "");

        int execute(const std::string& query, const std::string &table_name, std::string& json = (std::string &) "", std::string& error = (std::string &) "");

        int execute(const std::string &query, const std::string &table_name, std::vector<std::map<std::string, std::string>> &table, std::string &error);

        bool incert(tables tableType, std::vector<arc_json::content_value> values, std::string& err = (std::string &) "");
        bool update(tables tableType, std::vector<arc_json::content_value> &sets, std::vector<arc_json::content_value> &where, std::string& err = (std::string &) "");

        std::string get_channel_token(const boost::uuids::uuid& first, const boost::uuids::uuid& second);

        bool save_message(const std::string &message, const boost::uuids::uuid& first, const boost::uuids::uuid& second);

        int get_save_messages(std::string &json, const std::string& token, std::string& err, int top = 10, int start_date = 0, int end_date = 0);

    private:

        std::string database_file;
        sqlite3* db{};

        static std::string get_columns_for_query(tables tableType);
        static std::string get_table_name(tables tableType);
        static std::string get_table_name_template() { return "%TABLE_NAME%"; };
    };

}

#endif //WS_SOLUTION_BASE_H
