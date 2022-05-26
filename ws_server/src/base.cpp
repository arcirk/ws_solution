//
// Created by arcady on 14.08.2021.
//

#include <arcirk.h>
#include "../include/base.h"

#ifdef _WINDOWS
#pragma warning(disable:4100)
#endif

namespace arc_sqlite {

#ifndef USE_QT_CREATOR
    static int callback(void* data, int argc, char** argv, char** azColName)
    {
        int i;
        std::fprintf(stderr, "%s: ", (const char*)data);

        for (i = 0; i < argc; i++) {
            printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        }

        printf("\n");
        return 0;
    }
#endif

    sqlite3_db::sqlite3_db() {
        database_file = "";
        qtWrapper = nullptr;
        useWrapper = false;
    }

#ifdef USE_QT_CREATOR
    void sqlite3_db::set_qt_wrapper(SqlInterface *wrapper) {
        qtWrapper = wrapper;
        useWrapper = true;
    }
#else
    void sqlite3_db::set_qt_wrapper(SqlWrapper *wrapper) {
        qtWrapper = wrapper;
        useWrapper = true;
    }
#endif

    sqlite3_db::~sqlite3_db() {        
        close();
    }

    std::string sqlite3_db::get_database_file() {
        return database_file;
    }

    void sqlite3_db::verify_tables()
    {
        //
    }

    bool sqlite3_db::open(const std::string& file) {

        bool result = false;
#ifndef USE_QT_CREATOR


        if (!file.empty()){
            database_file = file;
        }

        //char* err = 0;


        if (sqlite3_open(database_file.c_str(), &db)) {
            std::cerr << "Ошибка создания базы данных : " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
        }
        else
        {
            result = true;
        }
#endif

        return result;
    }

    void sqlite3_db::close() {
#ifndef USE_QT_CREATOR
        sqlite3_close(db);
#else
//      if(useWrapper)
//          qtWrapper->
#endif
    }
#ifndef USE_QT_CREATOR
    void sqlite3_db::check_database_table(tables tableType) {

        std::string mQery = createQuery(tableType);

        exec(mQery);
    }

    std::string sqlite3_db::createQuery(tables tableType) {

        std::string myQuery = get_query_template(tableType);
        std::string tableNameTemplate = get_table_name_template();

        std::string tableName = get_table_name(tableType);

        myQuery.replace(myQuery.find(tableNameTemplate), tableNameTemplate.length(), tableName);
        myQuery.append(");");

        return myQuery;
    }

    std::string sqlite3_db::get_query_template(tables tableType) {

        std::string myQueryTemplate = "CREATE TABLE IF NOT EXISTS %TABLE_NAME% (\n";
        myQueryTemplate.append(" _id           INTEGER PRIMARY KEY AUTOINCREMENT,\n");
        myQueryTemplate.append(" FirstField    TEXT,\n");
        myQueryTemplate.append(" SecondField   TEXT,\n");
        myQueryTemplate.append(" Ref           TEXT (36) UNIQUE NOT NULL");

        myQueryTemplate.append(get_columns_for_query(tableType));

        return myQueryTemplate;
    }

    std::string sqlite3_db::get_columns_for_query(tables tableType) {

        std::string result;

        switch (tableType)
        {
            case tables::eUsers:
                result =		",\n hash    TEXT UNIQUE NOT NULL";
                result.append(	",\n role TEXT DEFAULT user");
                result.append(	",\n Performance TEXT");
                result.append(	",\n channel TEXT (36) NOT NULL DEFAULT '00000000-0000-0000-0000-000000000000'");
                result.append(	",\n cache TEXT");
                result.append(	",\n webdav TEXT");
                break;
            case tables::eChannels:
                result = ",\n Parent      TEXT (36) NOT NULL DEFAULT '00000000-0000-0000-0000-000000000000'";
                //result =		",\n token    TEXT NOT NULL";
//                result.append(",\n public   INTEGER");
//                result.append(",\n Parent   TEXT (36)");
                break;
            case tables::eMessages:
                result =		 ",\n message  TEXT";
                result.append(",\n token    TEXT UNIQUE NOT NULL");
                result.append(",\n date     INTEGER");
                result.append(",\n contentType TEXT      DEFAULT HTML");
                result.append(",\n unreadMessages  INTEGER   DEFAULT (0)"); //не прочитанные сообщения всегда у получателя
                break;
            case tables::eSubscribers:
                result =		   ",\n channel    TEXT (36) NOT NULL";
                result.append(	",\n user_uuid  TEXT (36)");
                break;
            case tables::eTechnicalInformation:
                result =          ",\n infobase     TEXT (100)";
                result.append(",\n date         INTEGER");
                result.append(",\n status       INTEGER");
                result.append(",\n message      TEXT");
                result.append(",\n userName     TEXT (250)");
                break;
            default:
                break;
        }

        return result;
    }
#endif
    std::string sqlite3_db::get_table_name(tables tableType) {
        switch (tableType)
        {
            case tables::eUsers:
                return "Users";
                break;
            case tables::eMessages:
                return "Messages";
                break;
            case tables::eChannels:
                return "Channels";
                break;
            case tables::eSubscribers:
                return "Subscribers";
                break;
            case tables::eTechnicalInformation:
                return "TechnicalInformation";
                break;
            default:
                return "";
                break;
        }
    }

    int sqlite3_db::exec(const std::string& query, std::string& error) {

        int i = 0;

        if(useWrapper){
#ifndef USE_QT_CREATOR
            char * err = nullptr;
            i = qtWrapper->exec(query.c_str(), &err);
            error = std::string(err);
#else
            QString err;
            i = qtWrapper->exec(QString::fromStdString(query), err);
            error = err.toStdString();
#endif
        }else{
#ifndef USE_QT_CREATOR
            sqlite3_stmt* pStmt;
            char* err = 0;
            bool result = false;


            if (sqlite3_exec(db, query.c_str(), 0, 0, &err))
            {
                std::cerr << "sqlite3_db::exec: " << "Ошибка SQL запроса : " << err << std::endl;
                error = err;
                sqlite3_free(err);
            }
            if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, nullptr))
            {
                sqlite3_finalize(pStmt);
                return 0;
            }

            int rc;

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
            {
                i++;

            }

            sqlite3_finalize(pStmt);
#else
            QString err;
            i = qtWrapper->exec(QString::fromStdString(query), err);
            error = err.toStdString();
#endif
        }

        return i;
    }


    int sqlite3_db::execute(const std::string &query, const std::string &table_name, std::string &json, std::string &error
                            , bool header, std::map<std::string, arcirk::bVariant> fields ) {

        int i = 0;

        if(useWrapper){
#ifndef USE_QT_CREATOR
            char * err = nullptr;
            char * result = nullptr;
            i = qtWrapper->execute(query.c_str(), &result, &err, header);
            error = std::string(err);
            json = std::string(result);
#else
            QString err;
            QString result;
            i = qtWrapper->execute(query, result, err, header);
            error = err.toStdString();
            json = result.toStdString();
#endif
            if(header && fields.size() > 0){
                auto obj_json = arcirk::bJson();
                obj_json.parse(json);
                if(obj_json.is_parse()){
                    for (auto k = fields.begin(); k != fields.end(); ++k) {
                        obj_json.addMember(k->first, k->second);
                    }
                }
                json = obj_json.to_string();
            }

        }else {
#ifndef USE_QT_CREATOR
            sqlite3_stmt *pStmt;
            char *err = 0;
            int rc;
            unsigned int j;

            auto obj_json = arcirk::bJson();

            _Value _header(rapidjson::Type::kArrayType);
            _header.SetArray();
            _Value _rows(rapidjson::Type::kArrayType);
            _rows.SetArray();

            if (!header)
                obj_json.set_array();
            else {
                obj_json.set_object();
                if (!table_name.empty()) {
                    std::string table_info = "PRAGMA table_info(" + table_name + ")";

                    if (sqlite3_prepare_v2(db, table_info.c_str(), -1, &pStmt, NULL)) {
                        sqlite3_finalize(pStmt);
                    }

                    while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW) {
//                    0 == cid
//                    1 == name
//                    2 == type
                        std::string p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, 1));
                        arcirk::bVariant column_name = p;
                        obj_json.push_back(_header, column_name);
                    }
                }
            }

            if (sqlite3_exec(db, query.c_str(), 0, 0, &err)) {
                std::cerr << "sqlite3_db::execute: " << "Ошибка SQL запроса : " << err << std::endl;
                sqlite3_free(err);
            }

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL)) {
                sqlite3_finalize(pStmt);
            }

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW) {
                unsigned int col_count = sqlite3_data_count(pStmt);

                _Value row(rapidjson::Type::kObjectType);
                row.SetObject();

                std::vector<std::string> m_cols{};

                for (j = 0; j < col_count; j++) {

                    int iColType = sqlite3_column_type(pStmt, j);

                    const char *p;

                    arcirk::content_value val;

                    val.key = reinterpret_cast<const char *>(sqlite3_column_name(pStmt, j));

                    if (table_name == "Users" && val.key == "hash")
                        continue;

                    if (iColType == SQLITE3_TEXT)// ((sz_col_type == "TEXT") || (sz_col_type == "TEXT (36)"))
                    {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            val.value = std::string(p);
                        else
                            val.value = "";
                    } else if (iColType == SQLITE_INTEGER) //(sz_col_type == "INTEGER")
                    {
                        val.value = (long int) sqlite3_column_int(pStmt, j);
                    } else if (iColType == SQLITE_FLOAT) //(sz_col_type == "REAL")
                    {
                        val.value = sqlite3_column_double(pStmt, j);
                    } else if (iColType == SQLITE_NULL) //(sz_col_type == "REAL")
                    {
                        val.value = "null";
                    } else {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            val.value = std::string(p);
                        else
                            val.value = "";
                    }

                    obj_json.addMember(&row, val);

                }

                obj_json.push_back(_rows, row);

                i++;
            }

            sqlite3_finalize(pStmt);

            if (!header)
                obj_json.copy_from(_rows);
            else {
                obj_json.addMember("columns", _header);
                obj_json.addMember("rows", _rows);
                for (auto k = fields.begin(); k != fields.end(); ++k) {
                    obj_json.addMember(k->first, k->second);
                }
            }
            json = obj_json.to_string();
#else
            QString err;
            QString result;
            i = qtWrapper->execute(query, result, err, header);
            error = err.toStdString();
            json = result.toStdString();

            if(header && fields.size() > 0){
                auto obj_json = arcirk::bJson();
                obj_json.parse(json);
                if(obj_json.is_parse()){
                    for (auto k = fields.begin(); k != fields.end(); ++k) {
                        obj_json.addMember(k->first, k->second);
                    }
                }
                json = obj_json.to_string();
            }
 #endif
        }

        return i;
    }

    int sqlite3_db::execute(const std::string &query, const std::string &table_name, std::vector<std::map<std::string, std::string>> &table, std::string &error) {
        int i = 0;

        if(useWrapper){
#ifndef USE_QT_CREATOR
            char * err = nullptr;
            char * chTable = nullptr;
            i = qtWrapper->execute(query.c_str(), &chTable,  &err);
            if(err){
                error = std::string(err);
                if(error == "no error")
                    error = "";
            }
#else
            QString err;
            QString chTable;
            i = qtWrapper->execute(query, chTable,  err);
#endif

            if(i > 0){
#ifndef USE_QT_CREATOR
                std::string json = std::string(chTable);
#else
                std::string json = chTable.toStdString();
#endif
                auto b_json = arcirk::bJson();
                b_json.parse(json);
                if(b_json.is_parse()){
                    b_json.getArray(table);
                }
            }


        }else{
#ifndef USE_QT_CREATOR
            sqlite3_stmt* pStmt;
            char* err = nullptr;

            int rc;

            std::vector<std::string> col_types;
            std::string table_info = "PRAGMA table_info(" + table_name + ")";

            if (sqlite3_prepare_v2(db, table_info.c_str(), -1, &pStmt, NULL))
            {
                sqlite3_finalize(pStmt);
            }

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
            {
                const char* p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 2));
                col_types.emplace_back(p);
            }

            if (sqlite3_exec(db, query.c_str(), 0, 0, &err))
            {
                std::cerr << "sqlite3_db::execute: " << "Ошибка SQL запроса : " << err << std::endl;
                sqlite3_free(err);
            }

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL))
            {
                sqlite3_finalize(pStmt);
            }

            unsigned int j;

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
            {
                unsigned int col_count = sqlite3_data_count(pStmt);

                //std::vector<arc_json::content_value> row;
                std::map<std::string, std::string> row;

                for (j = 0; j < col_count; j++)
                {

                    std::string sz_col_type = "TEXT";
                    if (j<col_types.size())
                    {
                        sz_col_type = col_types.at(j);
                    }

                    const char* p;

                    //arc_json::content_value val;

                    std::string key = reinterpret_cast<const char*>(sqlite3_column_name(pStmt, j));
                    std::string val;

                    if (table_name == "Users" && key == "hash")
                        continue;

                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        val = std::string(p);
                    else
                        val = "";

                    row.insert(std::pair<std::string , std::string>(key, val));
                }

                table.push_back(row);

                i++;
            }

            sqlite3_finalize(pStmt);
#else
            QString err;
            QString chTable;
            i = qtWrapper->execute(query, chTable,  err);
            if(i > 0){
                std::string json = chTable.toStdString();
                auto b_json = arcirk::bJson();
                b_json.parse(json);
                if(b_json.is_parse()){
                    b_json.getArray(table);
                }
            }
#endif
        }
        return i;
    }

    bool sqlite3_db::insert(tables tableType, std::vector<arcirk::content_value> values, std::string &err,
                            const std::string &not_ref) {

        std::string table = get_table_name(tableType);

        std::string query = "INSERT INTO ";
        if(useWrapper)
            query.append("dbo.");
        query.append(table);

        std::string into = "\n(";
        std::string values_ = "\n(";
        try {
            for (auto iter = values.begin(); iter < values.end(); iter++) {
                into.append(iter->key);
                if (iter != --values.end())
                    into.append(",\n");

                if (iter->value.is_string()){
                    std::string value = iter->value.get_string();
                    values_.append("'" + value + "'");
                    if (iter != --values.end())
                        values_.append(",\n");
                }else if (iter->value.is_int()){
                    long int res = iter->value.get_int();
                    std::string value = std::to_string(res);
                    values_.append("'" + value + "'");
                    if (iter != --values.end())
                        values_.append(",\n");
                }

            }
        }catch (std::exception& e){
            err = e.what();
            return false;
        }

        into.append("\n)");
        values_.append("\n)");
        query.append(into);
        if(!not_ref.empty()) {
            if (!useWrapper)
                query.append("\nSELECT\n");
            else
                query.append("\nVALUES\n");
        }else
            query.append("\nVALUES\n");
        query.append(values_);

        if(!not_ref.empty()){
            if(!useWrapper)
            query.append(arcirk::str_sample("WHERE NOT EXISTS \n"
                         " (SELECT [Ref] FROM %1% WHERE [Ref]='%2%')", table, not_ref));
            else{
                std::string query_ = arcirk::str_sample("IF NOT EXISTS \n"
                                                        "    (   SELECT  [Ref]\n"
                                                        "        FROM    dbo.%1% \n"
                                                        "        WHERE   [Ref]='%2%' \n"
                                                        "    )\n"
                                                        "BEGIN\n", table, not_ref);
                query_.append(query);
                query_.append("\nEND");
                query = query_;
            }

        }

        query.append(";");

        err = "";

        exec(query, err);

        if (!err.empty())
            return false;


        return true;

    }

    bool sqlite3_db::update(tables tableType, std::vector<arcirk::content_value> &sets, std::vector<arcirk::content_value> &where, std::string &err) {

        std::string table = get_table_name(tableType);

        std::string query = "UPDATE ";
        if(useWrapper)
            query.append("dbo.");
        query.append(table);

        std::string _set = "\n SET ";
        std::string _where = "\n WHERE ";
        try {
            for (auto iter = sets.begin(); iter < sets.end(); iter++) {
                _set.append(iter->key);

                if (iter->value.is_string()){
                    std::string value = iter->value.get_string();
                    _set.append(" = '" + value + "'");
                    if (iter != --sets.end())
                        _set.append(",\n");
                }else if (iter->value.is_int()){
                    long int res = iter->value.get_int();
                    std::string value = std::to_string(res);
                    _set.append(" = '" + value + "'");
                    if (iter != --sets.end())
                        _set.append(",\n");
                }

            }

            for (auto iter = where.begin(); iter < where.end(); iter++) {
                _where.append(iter->key);

                if (iter->value.is_string()){
                    std::string value =  iter->value.get_string();
                    _where.append(" = '" + value + "'");
                    if (iter != --where.end())
                        _where.append(" AND \n");
                }else if (iter->value.is_int()){
                    long int res = iter->value.get_int();
                    std::string value = std::to_string(res);
                    _where.append(" = '" + value + "'");
                    if (iter != --where.end())
                        _where.append(" AND \n");
                }

            }
        }catch (std::exception& e){
            err = e.what();
            return false;
        }

        _set.append("\n");
        _where.append("\n");
        query.append(_set);
        query.append(_where + ";");

        err = "";

        exec(query, err);

        if (!err.empty())
            return false;

        return true;
    }

    std::string sqlite3_db::get_channel_token(const boost::uuids::uuid &first, const boost::uuids::uuid &second) {

        std::string query = "select _id, Ref from Users where Ref = '" + arcirk::uuid_to_string(
                const_cast<boost::uuids::uuid &>(first)) + "' OR Ref = '" + arcirk::uuid_to_string(const_cast<boost::uuids::uuid &>(second)) + "' order by _id;";

        std::vector<std::map<std::string, std::string>> table;

        std::string err;

        int result = execute(query, "Users", table, err);

        if (result <= 1) //минимум 2 записи должно быть
            return "error";

        if (!err.empty()){
            std::cerr << err << std::endl;
            return "error";
        }

        std::string hash = arcirk::get_hash(table[0].at("Ref"), table[1].at("Ref"));

        return hash;
    }

    bool sqlite3_db::save_message(const std::string &message, const boost::uuids::uuid &first,
                                  const boost::uuids::uuid &second,
                                  std::string &ref, bool active, const std::string &firstName) {

        std::string hash = get_channel_token(first, second);

        if (hash != "error"){
            std::vector<arcirk::content_value> values;
            values.push_back(arcirk::content_value("FirstField", arcirk::uuid_to_string(
                    const_cast<boost::uuids::uuid &>(first))));
            values.push_back(arcirk::content_value("SecondField", arcirk::uuid_to_string(
                    const_cast<boost::uuids::uuid &>(second))));
            boost::uuids::uuid _ref = boost::uuids::random_generator()();
            ref = arcirk::uuid_to_string(_ref);
            values.push_back(arcirk::content_value("Ref", ref));
            values.push_back(arcirk::content_value("message", message));
            values.push_back(arcirk::content_value("token", hash));

            long int dt = arcirk::current_date_seconds();
            values.push_back(arcirk::content_value("date", dt));

            //не прочитанные сообщения всегда у получателя
            values.push_back(arcirk::content_value("unreadMessages", (int)!active));

            std::string err;

            bool result = insert(eMessages, values, err, "");

            if (!result)
                if (!err.empty())
                    std::cerr << err << std::endl;

            if(result && !active){
                try {
                    //сохраним в кеше получателя, при первом подключении отправитель добавится в список активных чатов
                    std::string cache_query = arcirk::str_sample(
                            "select cache from Users where Ref = '%1%'", boost::to_string(second));
                    std::vector<std::map<std::string, arcirk::bVariant>> table;
                    std::string error;

                    int i = execute(cache_query, "Users", table, error);

                    if (i > 0) {
                        std::string json = arcirk::base64_decode(table[0].at("cache").to_string());
                        auto b_json = arcirk::bJson();
                        b_json.parse(json);
                        std::string uuid_sender = boost::to_string(first);
                        bool is_exists = false;
                        if (b_json.is_parse()) {
                            auto chats = b_json.FindMember("chats");
                            if(!chats->value.IsObject()){
                                return result;
                            }
                            auto obg_chats = chats->value.GetObject();
                            auto arr_chats = obg_chats.FindMember("rows");

                            if (arr_chats->value.IsArray()) {
                                auto arr = arr_chats->value.GetArray();
                                for (auto itr = arr.Begin(); itr < arr.End(); ++itr) {
                                    auto uuid = itr->GetObject().FindMember("uuid");
                                    if (uuid->value.IsString()) {
                                        if (uuid->value.GetString() == uuid_sender) {
                                            is_exists = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (!is_exists) {
                                _Value obj(rapidjson::kObjectType);
                                obj.SetObject();
                                b_json.addMember(&obj, arcirk::content_value("uuid", arcirk::bVariant(uuid_sender)));
                                b_json.addMember(&obj, arcirk::content_value("name", firstName));
                                b_json.addMember(&obj, arcirk::content_value("draft", arcirk::bVariant(std::string())));
                                b_json.addMember(&obj, arcirk::content_value("unreadMessages", arcirk::bVariant(1)));
                                b_json.addMember(&obj, arcirk::content_value("active", arcirk::bVariant(false)));

                                arr_chats->value.GetArray().PushBack(obj, b_json.GetAllocator());
                                std::string sz_result = arcirk::base64_encode(b_json.to_string());
                                cache_query = arcirk::str_sample("update Users set cache='%1%' where Ref = '%2%'", sz_result,
                                                                 boost::to_string(second));
                                std::string err = "";
                                exec(cache_query, err);
                                if(!err.empty())
                                    std::cerr << "sqlite3_db::save_message: " << "error update user cache: " << err << std::endl;
                            }
                        }
                    }
                }catch (std::exception& e){
                    std::cerr << "sqlite3_db::save_message: " << "save_message error: " <<  e.what() << std::endl;
                }
            }
            return result;

        } else{
            std::cerr << "sqlite3_db::save_message: " << "ошибка получения хеша чата!" << std::endl;
            return false;
        }


    }

    int sqlite3_db::get_save_messages(std::string &json, const std::string &token, std::string& err, int top
                                      , int start_date, int end_date, std::map<std::string, arcirk::bVariant> fields) {

        std::string query = "select _top_ * from Messages where token = '" + token + "'";

        if ((start_date + end_date) != 0){
            if ((start_date - end_date) > 0){
                //err = "Start date cannot be greater than end date!";
                err = "Дата начала не может быть больше даты окончания!";
                return 0;
            } else
                query.append(" and date >= '" + std::to_string(start_date) + "' and date <= '" + std::to_string(end_date) + "'");
        }

        query.append(" order by date DESC");

        if (top > 0){
            if(!useWrapper){
                query.append(" limit '" + std::to_string(top) + "'");
                query.replace(query.find("_top_"), query.length(), "");
            }else{
                query.replace(query.find("_top_"), query.length(), "top " +std::to_string(top));
            }

        }

        query.append(";");

        err = "";

        int result = execute(query, "Messages", json, err, true, fields);

        return result;

    }

    int sqlite3_db::execute(const std::string &query, const std::string &table_name, std::vector<std::map<std::string, arcirk::bVariant>> &table, std::string &error){
        int i = 0;

        if(useWrapper){
#ifndef USE_QT_CREATOR
            char * chTable = nullptr;
            char * err = nullptr;
            i = qtWrapper->execute(query.c_str(), &chTable,  &err);
#else
            QString chTable;
            QString err;
            i = qtWrapper->execute(query, chTable,  err);
#endif

            if(i > 0){
#ifndef USE_QT_CREATOR
                std::string json = std::string(chTable);
#else
                std::string json = chTable.toStdString();
#endif
                auto b_json = arcirk::bJson();
                b_json.parse(json);
                if(b_json.is_parse()){
                    b_json.getArray(table);
                }
            }else{
#ifndef USE_QT_CREATOR
                if(err){
                    error = std::string(err);
                }
#else
                error = err.toStdString();
#endif
            };

        }else {
#ifndef USE_QT_CREATOR
            sqlite3_stmt *pStmt;
            char *err = 0;
            int rc;


            if (sqlite3_exec(db, query.c_str(), 0, 0, &err)) {
                std::cerr << "sqlite3_db::execute: " << "Ошибка SQL запроса : " << err << std::endl;
                sqlite3_free(err);
            }

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL)) {
                sqlite3_finalize(pStmt);
            }

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW) {
                unsigned int col_count = sqlite3_data_count(pStmt);

                std::map<std::string, arcirk::bVariant> row;

                for (unsigned int j = 0; j < col_count; j++) {

                    int iColType = sqlite3_column_type(pStmt, j);

                    const char *p;

                    std::string key = reinterpret_cast<const char *>(sqlite3_column_name(pStmt, j));
                    arcirk::bVariant value;

                    if (table_name == "Users" && key == "hash")
                        continue;

                    if (iColType == SQLITE3_TEXT)// ((sz_col_type == "TEXT") || (sz_col_type == "TEXT (36)"))
                    {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            value = std::string(p);
                        else
                            value = "";
                    } else if (iColType == SQLITE_INTEGER) //(sz_col_type == "INTEGER")
                    {
                        value = (int) sqlite3_column_int(pStmt, j);
                    } else if (iColType == SQLITE_FLOAT) //(sz_col_type == "REAL")
                    {
                        value = sqlite3_column_double(pStmt, j);
                    } else if (iColType == SQLITE_NULL) //(sz_col_type == "REAL")
                    {
                        value = "null";
                    } else {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            value = std::string(p);
                        else
                            value = "";
                    }

                    row.insert(std::pair<std::string, arcirk::bVariant>(key, value));

                }

                table.push_back(row);

                i++;
            }

            sqlite3_finalize(pStmt);
#else

            QString chTable;
            QString err;
            i = qtWrapper->execute(query, chTable,  err);
            if(i > 0){
                std::string json = chTable.toStdString();
                auto b_json = arcirk::bJson();
                b_json.parse(json);
                if(b_json.is_parse()){
                    b_json.getArray(table);
                }
            }else{
                error = err.toStdString();
            };
#endif
        }

        return i;
    }

    void sqlite3_db::get_columns_arr(const std::string &table_name, std::vector<std::string> &arr) {

        if(useWrapper){

            std::string query = arcirk::str_sample("SELECT name FROM sys.columns WHERE object_id = OBJECT_ID('dbo.%1%')", table_name);
#ifndef USE_QT_CREATOR
            char * err = nullptr;
            char * result = nullptr;
            int i = qtWrapper->execute(query.c_str(), &result, &err);
#else
            QString err;
            QString result;
            int i = qtWrapper->execute(query.c_str(), result, err);
#endif

            if(i > 0){
#ifndef USE_QT_CREATOR
                std::string b_result = std::string(result);
#else
                std::string b_result = result.toStdString();
#endif
                auto json = arcirk::bJson();
                json.parse(b_result);
                if(json.is_parse() && json.IsArray()){
                    _Value v_arr = json.GetArray();
                    for (auto itr = v_arr.Begin(); itr != v_arr.End(); ++itr) {
                        auto colItr = itr->FindMember("name");
                        if(colItr != itr->MemberEnd()){
                            arr.push_back(colItr->value.GetString());
                        }
                    }
                }
            }
        }else{
#ifndef USE_QT_CREATOR
            sqlite3_stmt* pStmt;
            int rc;
            std::string table_info = "PRAGMA table_info(" + table_name + ")";

            if (sqlite3_prepare_v2(db, table_info.c_str(), -1, &pStmt, NULL))
            {
                sqlite3_finalize(pStmt);
            }

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
            {
//                    0 == cid
//                    1 == name
//                    2 == type
                std::string column_name = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 1));
                arr.push_back(column_name);
            }
#else
            std::string query = arcirk::str_sample("SELECT name FROM sys.columns WHERE object_id = OBJECT_ID('dbo.%1%')", table_name);
            QString err;
            QString result;
            int i = qtWrapper->execute(query.c_str(), result, err);
            if(i > 0){
                std::string b_result = result.toStdString();
                auto json = arcirk::bJson();
                json.parse(b_result);
                if(json.is_parse() && json.IsArray()){
                    _Value v_arr = json.GetArray();
                    for (auto itr = v_arr.Begin(); itr != v_arr.End(); ++itr) {
                        auto colItr = itr->FindMember("name");
                        if(colItr != itr->MemberEnd()){
                            arr.push_back(colItr->value.GetString());
                        }
                    }
                }
            }
#endif
        }

    }

    int sqlite3_db::get_unread_messages(const std::string& recipient, std::string& result, std::string &error) {

        int i = 0;

        std::string dbo = useWrapper ? "dbo." : "";
        std::string query = arcirk::str_sample("SELECT FirstField AS sender,"
                          "  SecondField AS recipient,"
                          "  token,"
                          "  sum(unreadMessages) AS unreadMessages"
                          " FROM %1%Messages "
                          " WHERE SecondField = '%2%' AND"
                          "  unreadMessages > '0' "
                          " GROUP BY FirstField, SecondField, token;", dbo, recipient);

        if(useWrapper){
#ifndef USE_QT_CREATOR
            char * res = nullptr;
            char * err = nullptr;
            i = qtWrapper->execute(query.c_str(), &res, &err, true);
            if(i > 0){
                result = std::string(res);
            }
            error = std::string(err);
#else
            QString res;
            QString err;
            i = qtWrapper->execute(query.c_str(), res, err, true);
            if(i > 0){
                result = res.toStdString();
            }
            error = err.toStdString();
#endif

            }else{
#ifndef USE_QT_CREATOR
            sqlite3_stmt *pStmt;
            char *err = nullptr;

            int rc;
            auto json = arcirk::bJson();
            json.SetObject();
            _Value header(rapidjson::kArrayType);
            header.SetArray();
            header.PushBack("sender", json.GetAllocator());
            header.PushBack("recipient", json.GetAllocator());
            header.PushBack("token", json.GetAllocator());
            header.PushBack("unreadMessages", json.GetAllocator());
            json.addMember("columns", header);

            _Value rows(rapidjson::kArrayType);
            rows.SetArray();

            if (sqlite3_exec(db, query.c_str(), 0, 0, &err)) {
                std::cerr << "sqlite3_db::get_unread_messages: " << "Ошибка SQL запроса : " << err << std::endl;
                error = err;
                sqlite3_free(err);
                return 0;
            }

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL)) {
                sqlite3_finalize(pStmt);
            }

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW) {
                unsigned int col_count = sqlite3_data_count(pStmt);

                _Value row(rapidjson::kObjectType);
                row.SetObject();

                i++;

                for (unsigned int j = 0; j < col_count; j++) {

                    int iColType = sqlite3_column_type(pStmt, j);
                    const char *p = reinterpret_cast<const char *>(sqlite3_column_name(pStmt, j));
                    //_Value key(p, json.GetAllocator());
                    std::string key = p;
                    arcirk::bVariant value;

                    if (iColType == SQLITE3_TEXT) {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            value = std::string(p);
                        else
                            value = "";
                    } else if (iColType == SQLITE_INTEGER) {
                        value = (int) sqlite3_column_int(pStmt, j);
                    } else if (iColType == SQLITE_NULL) //(sz_col_type == "REAL")
                    {
                        value = "";
                    } else {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            value = std::string(p);
                        else
                            value = "";
                    }

                    json.addMember(&row, arcirk::content_value(key, value));

                }

                rows.PushBack(row, json.GetAllocator());

            }

            json.addMember("rows", rows);

            result = json.to_string();
#else
            QString res;
            QString err;
            i = qtWrapper->execute(query.c_str(), res, err, true);
            if(i > 0){
                result = res.toStdString();
            }
            error = err.toStdString();
#endif
        }

        return i;
    }

    bool sqlite3_db::export_tables() {


        if(!useWrapper)
            return false;

        if(!qtWrapper->isOpen()){
            return false;
        }


#ifdef USE_QT_CREATOR

    return qtWrapper->exportTablesToSqlServer();

#else
        std::map<std::string, tables> t;
        t.insert(std::pair<std::string, tables>("Users", tables::eUsers));
        t.insert(std::pair<std::string, tables>("Messages", tables::eMessages));
        t.insert(std::pair<std::string, tables>("Channels", tables::eChannels));
        t.insert(std::pair<std::string, tables>("Subscribers", tables::eSubscribers));
        t.insert(std::pair<std::string, tables>("TechnicalInformation", tables::eTechnicalInformation));

        if(!open("base/db.sqlite"))
            return false;

        sqlite3_stmt *pStmt;
        char *err = 0;
        int rc;
        unsigned int j;

        for(auto itr = t.begin(); itr != t.end(); ++itr){
            std::string table_name = itr->first;
            std::string query = arcirk::str_sample("select * from %1%;", table_name);

            if (sqlite3_exec(db, query.c_str(), 0, 0, &err)) {
                std::cerr << "sqlite3_db::export_table_to_SqlWrapper: " << "Ошибка SQL запроса : " << err << std::endl;
                sqlite3_free(err);
            }

            if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL)) {
                sqlite3_finalize(pStmt);
            }

            while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW) {

                unsigned int col_count = sqlite3_data_count(pStmt);

                std::vector<arcirk::content_value> row;
                std::string Ref;

                for (j = 0; j < col_count; j++) {

                    int iColType = sqlite3_column_type(pStmt, j);

                    const char *p;

                    arcirk::content_value val;

                    val.key = reinterpret_cast<const char *>(sqlite3_column_name(pStmt, j));
                    if(val.key == "_id")
                        continue;

                    if (iColType == SQLITE3_TEXT)// ((sz_col_type == "TEXT") || (sz_col_type == "TEXT (36)"))
                    {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            val.value = std::string(p);
                        else
                            val.value = "";
                    } else if (iColType == SQLITE_INTEGER) //(sz_col_type == "INTEGER")
                    {
                        val.value = (long int) sqlite3_column_int(pStmt, j);
                    } else if (iColType == SQLITE_FLOAT) //(sz_col_type == "REAL")
                    {
                        val.value = sqlite3_column_double(pStmt, j);
                    } else if (iColType == SQLITE_NULL) //(sz_col_type == "REAL")
                    {
                        val.value = "null";
                    } else {
                        p = reinterpret_cast<const char *>(sqlite3_column_text(pStmt, j));
                        if (p != NULL)
                            val.value = std::string(p);
                        else
                            val.value = "";
                    }

                    if(val.key == "Ref")
                        Ref = val.value.to_string();

                    row.push_back(val);

                }

                std::string error;
                insert(t[table_name], row, error, Ref);
            }
            sqlite3_finalize(pStmt);
            std::cout << "export table " << itr->first << std::endl;
        }
        close();
#endif
        return true;
    }

    void sqlite3_db::sync_users(const std::string &resp){

        auto doc = arcirk::bJson();
        doc.parse(resp);
        if(!doc.is_parse())
            return;

        std::vector<std::vector<arcirk::content_value>> arr;
        doc.getMembers("rows", arr);

        for (auto itr : arr) {
            std::string err;
            std::string ref;

            std::vector<arcirk::content_value> _ref;

            for(auto item : itr){
                if(item.key == "Ref"){
                    ref = item.value.get_string();
                    _ref.push_back(arcirk::content_value("Ref", ref));
                    break;
                }
            }
            itr.push_back(arcirk::content_value("hash", "0"));
            bool result = insert(tables::eUsers, itr, err, ref);
            if(result && !ref.empty()){
                itr.resize(itr.size() - 1);
                update(tables::eUsers, itr, _ref, err);
            }




        }
    }


}

