//
// Created by arcady on 14.08.2021.
//

#include <arc_json.h>
#include "../include/base.h"


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
                result.append(	",\n role TEXT DEFAULT client");
                result.append(	",\n Performance TEXT");
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
                break;
            case tables::eSubscribers:
                result =		   ",\n channel    TEXT (36) NOT NULL REFERENCES Channels (Ref)";
                result.append(	",\n user_uuid  TEXT (36) REFERENCES Users (Ref)");
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

    int sqlite3_db::execute(const std::string &query, const std::string &table_name, std::string &json, std::string &error) {

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

        auto * obj_json = new arc_json::ws_json();

        obj_json->set_array();

        while ((rc = sqlite3_step(pStmt)) == SQLITE_ROW)
        {
            unsigned int col_count = sqlite3_data_count(pStmt);

            _Value row(rapidjson::Type::kObjectType);
            row.SetObject();

            for (j = 0; j < col_count; j++)
            {

                std::string sz_col_type = "TEXT";
                if (j<col_types.size())
                {
                    sz_col_type = col_types.at(j);
                }

                const char* p;

                arc_json::content_value val;

                val.key = reinterpret_cast<const char*>(sqlite3_column_name(pStmt, j));

                if (table_name == "Users" && val.key == "hash")
                    continue;

                if ((sz_col_type == "TEXT") || (sz_col_type == "TEXT (36)"))
                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        val.value = std::string(p);
                    else
                        val.value = "";
                }
                else if (sz_col_type == "INTEGER")
                {
                    val.value = (long int)sqlite3_column_int(pStmt, j);
                }
                else if (sz_col_type == "REAL")
                {
                    val.value = sqlite3_column_double(pStmt, j);
                }
                else

                {
                    p = reinterpret_cast<const char*>(sqlite3_column_text(pStmt, j));
                    if (p != NULL)
                        val.value = std::string(p);
                    else
                        val.value = "";
                }

                obj_json->addObjectMember(&row, val);

            }

            obj_json->push_back(row);
            i++;
        }

        sqlite3_finalize(pStmt);


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

    bool sqlite3_db::incert(tables tableType, std::vector<arc_json::content_value> values, std::string& err) {

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

                if (iter->value.type() == typeid(std::string)){
                    std::string value = boost::get<std::string>(iter->value);
                    values_.append("'" + value + "'");
                    if (iter != --values.end())
                        values_.append(",\n");
                }else if (iter->value.type() == typeid(long int)){
                    long int res = boost::get<long int>(iter->value);
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

    bool sqlite3_db::update(tables tableType, std::vector<arc_json::content_value> &sets, std::vector<arc_json::content_value> &where, std::string &err) {

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

                if (iter->value.type() == typeid(std::string)){
                    std::string value = boost::get<std::string>(iter->value);
                    _set.append(" = '" + value + "'");
                    if (iter != --sets.end())
                        _set.append(",\n");
                }else if (iter->value.type() == typeid(long int)){
                    long int res = boost::get<long int>(iter->value);
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

                if (iter->value.type() == typeid(std::string)){
                    std::string value = boost::get<std::string>(iter->value);
                    _where.append(" = '" + value + "'");
                    if (iter != --where.end())
                        _where.append(" AND \n");
                }else if (iter->value.type() == typeid(long int)){
                    long int res = boost::get<long int>(iter->value);
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

        std::string query = "select _id, Ref from Users where Ref = '" + arc_json::uuid_to_string(
                const_cast<boost::uuids::uuid &>(first)) + "' OR Ref = '" + arc_json::uuid_to_string(const_cast<boost::uuids::uuid &>(second)) + "' order by _id;";

        std::vector<std::map<std::string, std::string>> table;

        std::string err;

        int result = execute(query, "Users", table, err);

        if (result <= 1) //минимум 2 записи должно быть
            return "error";

        if (!err.empty()){
            std::cerr << err << std::endl;
            return "error";
        }

        std::string hash = arc_json::get_hash(table[0].at("Ref"), table[1].at("Ref"));

        return hash;
    }

    bool sqlite3_db::save_message(const std::string &message, const boost::uuids::uuid &first,
                                  const boost::uuids::uuid &second) {

        std::string hash = get_channel_token(first, second);
        if (hash != "error"){
            std::vector<arc_json::content_value> values;
            values.push_back(arc_json::content_value("FirstField", arc_json::uuid_to_string(
                    const_cast<boost::uuids::uuid &>(first))));
            values.push_back(arc_json::content_value("SecondField", arc_json::uuid_to_string(
                    const_cast<boost::uuids::uuid &>(second))));
            boost::uuids::uuid ref = boost::uuids::random_generator()();
            values.push_back(arc_json::content_value("Ref", arc_json::uuid_to_string(ref)));
            values.push_back(arc_json::content_value("message", message));
            values.push_back(arc_json::content_value("token", hash));

            long int dt = arc_json::current_date_seconds();
            values.push_back(arc_json::content_value("date", dt));

            std::string err;

            bool result = incert(eMessages, values, err);

            if (!result)
                if (!err.empty())
                    std::cerr << err << std::endl;

            return result;

        } else

        return false;
    }

    int sqlite3_db::get_save_messages(std::string &json, const std::string &token, std::string& err, int top, int start_date, int end_date) {

        std::string query = "select * from Messages where token = '" + token + "'";

        if ((start_date + end_date) != 0){
            if ((start_date - end_date) > 0){
                //err = "Start date cannot be greater than end date!";
                err = "Дата начала не может быть больше даты окончания!";
                return 0;
            } else
                query.append(" and date >= '" + std::to_string(start_date) + "' and date <= '" + std::to_string(end_date) + "'");
        }

        if (top > 0){
            query.append(" order by token DESC");
            query.append(" limit '" + std::to_string(top) + "'");
        }

        query.append(";");

        err = "";

        int result = execute(query, "Messages", json, err);

        return result;

    }



}
