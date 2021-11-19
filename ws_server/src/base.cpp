//
// Created by arcady on 14.08.2021.
//

#include <arcirk.h>
#include "../include/base.h"

//ToDo: исправить код получения типа данных колонки col_count
//ToDo: убрать аргумент в execute table_name, более не нужен
//ToDo: удалить код и таблицы подписчиков, таблица не нужна
namespace arc_sqlite {

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

    sqlite3_db::sqlite3_db() {
        database_file = "";
;    }

    sqlite3_db::~sqlite3_db() {
        close();
    }

    std::string sqlite3_db::get_database_file() {
        return database_file;
    }

    bool sqlite3_db::open(const std::string& file) {

        if (!file.empty()){
            database_file = file;
        }

        //char* err = 0;
        bool result = false;

        if (sqlite3_open(database_file.c_str(), &db)) {
            std::cerr << "Ошибка создания базы данных : " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
        }
        else
        {
            result = true;
        }

        return result;
    }

    void sqlite3_db::close() {
        sqlite3_close(db);
    }

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
//            case lttables::eBarcodes:
//                result =		",\n Barcode            TEXT (128) UNIQUE";
//                result.append(	",\n SeriaRef           TEXT (36)");
//                result.append(	",\n OwnerRef           TEXT (36)");
//                result.append(	",\n CharacteristicsRef TEXT (36)");
//                break;
//            case lttables::eCharacteristics:
//                result =		",\n OwnerRef    TEXT (36)";
//                break;
//            case lttables::eSeries:
//                result =		",\n OwnerRef    TEXT (36)";
//                break;
//            case lttables::eStorage:
//                result =		",\n AddressStorage INTEGER";
//                break;
//            case lttables::eStorageCells:
//                result =		",\n Barcode     TEXT (165) UNIQUE";
//                result.append(	",\n OwnerRef    TEXT (36)");
//                break;
//            case lttables::eDocuments:
//                result =		",\n NumberDoc		TEXT (50)";
//                result.append(	",\n DataDoc		INTEGER");
//                result.append(	",\n TypeDoc		INTEGER");
//                result.append(	",\n Device			TEXT (50)");
//                result.append(	",\n DeletionMark	INTEGER   DEFAULT (0)");
//                result.append(	",\n Blocking		INTEGER   DEFAULT (0)");
//                break;
//            case lttables::eDocumentTablePart:
//                result =		",\n Barcode			TEXT"; //При условиии SeriaRef=null and CharacteristicsRef=null хранится в формате 2012221122,554566655,455546554
//                result.append(	",\n OwnerRef			TEXT (36)");
//                result.append(	",\n Quantity			REAL (10, 3) DEFAULT (0)");
//                result.append(	",\n SourceQuantity		REAL (10, 3) DEFAULT (0)");
//                result.append(	",\n Coefficient		REAL (10, 3) DEFAULT (0)");
//                result.append(	",\n GoodRef			TEXT (36)");
//                result.append(	",\n SeriaRef			TEXT (36)");
//                result.append(	",\n CharacteristicsRef	TEXT (36)");
//                break;
//            case lttables::eDevices:
//                result =		",\n Device      TEXT";
//                break;
            default:
                break;
        }

        return result;
    }

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
//            case tables::eSeries:
//                return "Series";
//                break;
//            case tables::eStorage:
//                return "Storage";
//                break;
//            case tables::eStorageCells:
//                return "StorageCells";
//                break;
//            case tables::eDocuments:
//                return "Documents";
//                break;
//            case tables::eDocumentTablePart:
//                return "DocumentTablePart";
//                break;
//            case tables::eDevices:
//                return "Devices";
//                break;
            default:
                return "";
                break;
        }
    }

    int sqlite3_db::exec(const std::string& query, std::string& error) {

        sqlite3_stmt* pStmt;
        char* err = 0;
        bool result = false;
        int i = 0;

        if (sqlite3_exec(db, query.c_str(), 0, 0, &err))
        {
            std::cerr << "Ошибка SQL запроса : " << err << std::endl;
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

        return i;
    }

    int sqlite3_db::execute(const std::string &query, const std::string &table_name, std::string &json, std::string &error
                            , bool header, std::map<std::string, arcirk::bVariant> fields ) {

//if(!header)
//{
//    [
//        rows
//    ]
//}
//else
//{
//    columns:[]
//    rows[]
//}
        sqlite3_stmt* pStmt;
        char* err = 0;
        int i = 0;
        int rc;
        unsigned int j;

        auto * obj_json = new arcirk::bJson();

        _Value _header(rapidjson::Type::kArrayType);
        _header.SetArray();
        _Value _rows(rapidjson::Type::kArrayType);
        _rows.SetArray();

        if (!header)
            obj_json->set_array();
        else{
            obj_json->set_object();
            if (!table_name.empty()){
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
                    std::string p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, 1));
                    arcirk::bVariant column_name = p;
                    obj_json->push_back(_header, column_name);
                }
            }
        }

        if (sqlite3_exec(db, query.c_str(), 0, 0, &err))
        {
            std::cerr << "Ошибка SQL запроса : " << err << std::endl;
            sqlite3_free(err);
        }

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL)) {
            sqlite3_finalize(pStmt);
        }

        while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            unsigned int col_count = sqlite3_data_count(pStmt);

            _Value row(rapidjson::Type::kObjectType);
            row.SetObject();

            std::vector<std::string> m_cols{};

            for (j = 0; j < col_count; j++)
            {

                int iColType = sqlite3_column_type(pStmt, j);

                const char* p;

                arcirk::content_value val;

                //td::string __col = reinterpret_cast<const char*>(sqlite3_column_name(pStmt, j));
                //bVariant column_name = __col; //reinterpret_cast<const char*>(sqlite3_column_name(pStmt, j));

                val.key = reinterpret_cast<const char*>(sqlite3_column_name(pStmt, j));// boost::get<std::string>(column_name);

//                if (std::find(m_cols.begin(), m_cols.end(), __col) == m_cols.end())
//                {
//                    m_cols.push_back(__col);
//                    obj_json->push_back(_header, column_name);
//                }


                if (table_name == "Users" && val.key == "hash")
                    continue;

                if (iColType == SQLITE3_TEXT)// ((sz_col_type == "TEXT") || (sz_col_type == "TEXT (36)"))
                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        val.value = std::string(p);
                    else
                        val.value = "";
                }
                else if (iColType == SQLITE_INTEGER) //(sz_col_type == "INTEGER")
                {
                    val.value = (long int)sqlite3_column_int(pStmt, j);
                }
                else if (iColType == SQLITE_FLOAT) //(sz_col_type == "REAL")
                {
                    val.value = sqlite3_column_double(pStmt, j);
                }
                else if (iColType == SQLITE_NULL) //(sz_col_type == "REAL")
                {
                    val.value = "null";
                }
                else

                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        val.value = std::string(p);
                    else
                        val.value = "";
                }

                obj_json->addMember(&row, val);

            }

            obj_json->push_back(_rows, row);

            i++;
        }

        sqlite3_finalize(pStmt);

        if (!header)
            obj_json->copy_from(_rows);
        else{
            obj_json->addMember("columns", _header);
            obj_json->addMember("rows", _rows);
            for (auto k = fields.begin(); k != fields.end(); ++k) {
                obj_json->addMember(k->first, k->second);
            }
        }

        json = obj_json->to_string();

        return i;
    }

    int sqlite3_db::execute(const std::string &query, const std::string &table_name, std::vector<std::map<std::string, std::string>> &table, std::string &error) {

        sqlite3_stmt* pStmt;
        char* err = 0;
        int i = 0;
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
            std::cerr << "Ошибка SQL запроса : " << err << std::endl;
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

//                if ((sz_col_type == "TEXT") || (sz_col_type == "TEXT (36)"))
//                {
//                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
//                    if (p != NULL)
//                        val = std::string(p);
//                    else
//                        val = "";
//                }
////                else if (sz_col_type == "INTEGER")
////                {
////                    val.value = sqlite3_column_int(pStmt, j);
////                }
////                else if (sz_col_type == "REAL")
////                {
////                    val.value = sqlite3_column_double(pStmt, j);
////                }
//                else
//
//                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        val = std::string(p);
                    else
                        val = "";
               // }

                row.insert(std::pair<std::string , std::string>(key, val));
            }

            table.push_back(row);

            i++;
        }

        sqlite3_finalize(pStmt);

        return i;
    }

    bool sqlite3_db::insert(tables tableType, std::vector<arcirk::content_value> values, std::string& err) {

        std::string table = get_table_name(tableType);

        std::string query = "INSERT INTO ";
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
        query.append("\nVALUES\n");
        query.append(values_ + ";");

        err = "";

        exec(query, err);

        if (!err.empty())
            return false;


        return true;

    }

    bool sqlite3_db::update(tables tableType, std::vector<arcirk::content_value> &sets, std::vector<arcirk::content_value> &where, std::string &err) {

        std::string table = get_table_name(tableType);

        std::string query = "UPDATE ";
        query.append(table);

        std::string _set = "\n SET ";
        std::string _where = "\n WHERE ";
        try {
            for (auto iter = sets.begin(); iter < sets.end(); iter++) {
                _set.append(iter->key);
//                if (iter != --sets.end())
//                    _set.append(",\n");

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
//                if (iter != --where.end())
//                    _where.append(",\n");

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
                                  std::string &ref, bool active) {

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

            bool result = insert(eMessages, values, err);

            if (!result)
                if (!err.empty())
                    std::cerr << err << std::endl;

            return result;

        } else

        return false;
    }

    int sqlite3_db::get_save_messages(std::string &json, const std::string &token, std::string& err, int top
                                      , int start_date, int end_date, std::map<std::string, arcirk::bVariant> fields) {

        std::string query = "select * from Messages where token = '" + token + "'";

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
            query.append(" limit '" + std::to_string(top) + "'");
        }

        query.append(";");

        err = "";

        int result = execute(query, "Messages", json, err, true, fields);

        return result;

    }

    int sqlite3_db::execute(const std::string &query, const std::string &table_name, std::vector<std::map<std::string, arcirk::bVariant>> &table, std::string &error){

        sqlite3_stmt* pStmt;
        char* err = 0;
        int i = 0;
        int rc;


        if (sqlite3_exec(db, query.c_str(), 0, 0, &err))
        {
            std::cerr << "Ошибка SQL запроса : " << err << std::endl;
            sqlite3_free(err);
        }

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL))
        {
            sqlite3_finalize(pStmt);
        }

        while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            unsigned int col_count = sqlite3_data_count(pStmt);

            std::map<std::string, arcirk::bVariant> row;

            for (unsigned int j = 0; j < col_count; j++)
            {

                int iColType = sqlite3_column_type(pStmt, j);

                const char* p;

                std::string key = reinterpret_cast<const char*>(sqlite3_column_name(pStmt, j));
                arcirk::bVariant value;

                if (table_name == "Users" && key == "hash")
                    continue;

                if (iColType == SQLITE3_TEXT)// ((sz_col_type == "TEXT") || (sz_col_type == "TEXT (36)"))
                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        value = std::string(p);
                    else
                        value = "";
                }
                else if (iColType == SQLITE_INTEGER) //(sz_col_type == "INTEGER")
                {
                    value = (int)sqlite3_column_int(pStmt, j);
                }
                else if (iColType == SQLITE_FLOAT) //(sz_col_type == "REAL")
                {
                    value = sqlite3_column_double(pStmt, j);
                }
                else if (iColType == SQLITE_NULL) //(sz_col_type == "REAL")
                {
                    value = "null";
                }
                else

                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
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

        return i;
    }

    void sqlite3_db::get_columns_arr(const std::string &table_name, std::vector<std::string> &arr) {
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

    }

    int sqlite3_db::get_unread_messages(const std::string& recipient, std::string& result, std::string &error) {

        sqlite3_stmt* pStmt;
        char* err = 0;
        int i = 0;
        int rc;

        std::string query = arcirk::str_sample("SELECT FirstField AS sender,"
                          "  SecondField AS recipient,"
                          "  token,"
                          "  sum(unreadMessages) AS unreadMessages"
                          " FROM Messages "
                          " WHERE SecondField = '%1%' AND"
                          "  unreadMessages > '0' "
                          " GROUP BY FirstField;", recipient);

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

        if (sqlite3_exec(db, query.c_str(), 0, 0, &err))
        {
            std::cerr << "Ошибка SQL запроса : " << err << std::endl;
            sqlite3_free(err);
        }

        if (sqlite3_prepare_v2(db, query.c_str(), -1, &pStmt, NULL))
        {
            sqlite3_finalize(pStmt);
        }

        while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW) {
            unsigned int col_count = sqlite3_data_count(pStmt);

            _Value row(rapidjson::kObjectType);
            row.SetObject();

            for (unsigned int j = 0; j < col_count; j++) {

                int iColType = sqlite3_column_type(pStmt, j);
                const char* p = reinterpret_cast<const char*>(sqlite3_column_name(pStmt, j));
                //_Value key(p, json.GetAllocator());
                std::string key = p;
                arcirk::bVariant value;

                if (iColType == SQLITE3_TEXT)
                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        value = std::string(p);
                    else
                        value = "";
                }
                else if (iColType == SQLITE_INTEGER)
                {
                    value = (int)sqlite3_column_int(pStmt, j);
                }
                else if (iColType == SQLITE_NULL) //(sz_col_type == "REAL")
                {
                    value = "";
                }
                else

                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
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

    }



}

