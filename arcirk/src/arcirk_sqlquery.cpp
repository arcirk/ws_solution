//
// Created by admin on 04.06.2022.
//

#include <utility>

#include "../include/arcirk.h"

//Password=CONVERT(VARBINARY(MAX), '0x2B89C2954E18E15759545A421D243E251784FA009E46F7A163926247FDB945F85F095DBB1FFF5B2B43A6ADAE27B8C46E176902412C4F8943E39528FF94E0DD5B', 1)

namespace arcirk{

    bSqlValue::bSqlValue(bSqlTypeOfComparison _typeOfComparison, content_value _field){
        typeOfComparison = _typeOfComparison;
        field = std::move(_field);
    }

    bSqlQuery::bSqlQuery(bSqlCommand command, const std::string& tableName) {
        _command = command;
        _table = tableName;
    }

    void bSqlQuery::add_field(const content_value& field) {
        _fields.push_back(field);
    }

    void bSqlQuery::add_where(const content_value& val, bSqlTypeOfComparison typeOfComparison) {
        _where.emplace_back(typeOfComparison, val);
    }

    void bSqlQuery::set_where_sample(const std::string &str) {
        _whereSample = str;
    }

    void bSqlQuery::add_field_is_exists(const content_value& field) {
        _fieldsIsExists.push_back(field);
    }

    std::string bSqlQuery::query() const {
        if(_command == SqlInsert){
            return bindQueryInsert();
        }else if(_command == SqlUpdate){
            return bindQueryUpdate();
        }else if(_command == SqlDelete){
            return bindQueryUpdate();
        }

        return {};
    }

    std::string bSqlQuery::bindQueryInsert() const {

        std::string query;
        std::string into = "\n(";
        std::string values_ = "\n(";
        query = "INSERT INTO ";
        query.append("dbo." + _table);
        for (auto iter = _fields.begin(); iter != _fields.end(); iter++) {
            into.append(iter->key);
            if (iter != --_fields.end())
                into.append(",\n");
            bVariant vValue = iter->value;
            if (vValue.is_string()){
                std::string value = vValue.get_string();
                values_.append("'" + value + "'");
                if (iter != --_fields.end())
                    values_.append(",\n");
            }else if (vValue.is_int()){
                int res = vValue.get_int();
                std::string value = std::to_string(res);
                values_.append("'" + value + "'");
                if (iter != --_fields.end())
                    values_.append(",\n");
            }
        }
        into.append("\n)");
        values_.append("\n)");
        query.append(into);
        query.append("\nVALUES\n");
        query.append(values_);

        if(!_fieldsIsExists.empty()){
            std::string _whereEx;

            for (auto iter = _fieldsIsExists.begin(); iter != _fieldsIsExists.end(); iter++) {
                _whereEx.append(iter->key);
                bVariant vValue = iter->value;
                if (vValue.is_string()){
                    std::string value = vValue.get_string();
                    _whereEx.append(" = '" + value + "'");
                    if (iter != --_fields.end())
                        _whereEx.append(" AND\n");
                }else if (vValue.is_int()){
                    int res = vValue.get_int();
                    std::string value = std::to_string(res);
                    _whereEx.append(" = '" + value + "'");
                    if (iter != --_fields.end())
                        _whereEx.append(" AND\n");
                }
            }

            std::string query_ = arcirk::str_sample("IF NOT EXISTS \n"
                                     "    (   SELECT  [Ref]\n"
                                     "        FROM    dbo.%1% \n"
                                     "        WHERE   '%2' \n"
                                     "    )\n"
                                     "BEGIN\n", _table, _whereEx);
            query_.append(query);
            query_.append("\nEND");
            query = query_;
        }

        query.append(";");

        return query;
    }

    std::string bSqlQuery::bindQueryUpdate() const {
        return {};
    }

    std::string bSqlQuery::bindQueryDelete() const {
        return {};
    }

    void bSqlQuery::fromJson(const std::string &json) {

//        auto doc = bJson();
//        doc.parse(json);
//
//        if(!doc.is_parse())
//            return;
//
//        bVariant val;
//        doc.getMember("command", val);
//        if(val.is_int())
//            _command = (bSqlCommand)val.get_int();
//
//        std::vector<std::map<std::string, bVariant>> arr_fields;
//        doc.getArray(arr_fields);
//
//        if(arr_fields.size() > 0){
//            for (auto itr : arr_fields) {
//                _fields.insert(content_value(itr.ge))
//            }
//        }


    }
}