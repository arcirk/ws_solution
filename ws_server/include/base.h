//
// Created by arcady on 14.08.2021.
//

#ifndef WS_SOLUTION_BASE_H
#define WS_SOLUTION_BASE_H

#include <arcirk.h>

#ifdef USE_QT_CREATOR
#include "../../sqlwrapper/sqlinterface.h"
#else
#include "../sqlite/sqlite3.h"
#include "_sqlwrapper.h"
#endif


namespace arc_sqlite {

    enum tables
    {
        eUsers = 0,
        eMessages,
        eChannels,
        eSubscribers,
        eTechnicalInformation,
        eContainers,
        eCertUsers,
        eWSConf,
        eServers
    };
#ifndef USE_QT_CREATOR
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
#endif

    class sqlite3_db{

    public:
        sqlite3_db();
        ~sqlite3_db();

        bool open(const std::string& file="");

        void close();

        std::string get_database_file();

        void verify_tables();

#ifndef USE_QT_CREATOR
        void check_database_table(tables tableType);

        static std::string createQuery(tables tableType);

        static std::string get_query_template(tables tableType);
#endif
        int exec(const std::string& query, std::string& error = (std::string &) "");

        int exec_qt(const std::string& i_query, std::string& err);

        int exec_qt(const std::string& i_query, std::string& tableResult, std::string& err);

        int execute(const std::string& query, const std::string &table_name, std::string& json = (std::string &) "", std::string& error = (std::string &) "",
                    bool header = false, std::map<std::string, arcirk::bVariant> fields = std::map<std::string, arcirk::bVariant>());

        int execute(const std::string &query, const std::string &table_name, std::vector<std::map<std::string, std::string>> &table, std::string &error);

        int execute(const std::string &query, const std::string &table_name, std::vector<std::map<std::string, arcirk::bVariant>> &table, std::string &error);

        bool insert(tables tableType, std::vector<arcirk::content_value> values, std::string &err,
                    const std::string &not_ref);

        bool update(tables tableType, std::vector<arcirk::content_value> &sets, std::vector<arcirk::content_value> &where, std::string& err = (std::string &) "");

        std::string get_channel_token(const boost::uuids::uuid& first, const boost::uuids::uuid& second);

        bool save_message(const std::string &message, const boost::uuids::uuid &first, const boost::uuids::uuid &second,
                          std::string &ref, bool active, const std::string &firstName);

        int get_save_messages(std::string &json, const std::string& token,
                              std::string& err, int top = 10, int start_date = 0,
                              int end_date = 0,
                              std::map<std::string, arcirk::bVariant> fields = std::map<std::string, arcirk::bVariant>());

        void get_columns_arr(const std::string& table_name, std::vector<std::string> &arr);

        int get_unread_messages(const std::string& recipient, std::string& result, std::string &err);

        void sync_users(const std::string& resp);

        bool isOpen();

#ifdef USE_QT_CREATOR
    void set_qt_wrapper(SqlInterface * wrapper);

#else
        void set_qt_wrapper(SqlWrapper * wrapper);
#endif


        bool is_use_wrapper() const{
#ifndef USE_QT_CREATOR
            return useWrapper;
#else
            if(qtWrapper->driver() != "QODBC")
                return false;
            else
                return useWrapper;
#endif
        };
//#ifndef USE_QT_CREATOR
        bool export_tables();
//#endif
    private:
        bool useWrapper;
        std::string database_file;
#ifndef USE_QT_CREATOR
        sqlite3* db{};
#endif
        static std::string get_table_name(tables tableType);
#ifdef USE_QT_CREATOR
    SqlInterface * qtWrapper;
#else
        SqlWrapper * qtWrapper;        
        static std::string get_columns_for_query(tables tableType);
        static std::string get_table_name_template() { return "%TABLE_NAME%"; };
#endif


    };

}

#endif //WS_SOLUTION_BASE_H
