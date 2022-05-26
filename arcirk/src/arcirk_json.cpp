//
// Created by arcady on 05.11.2021.
//

#include "../include/arcirk.h"
#include "rapidjson/filewritestream.h"

namespace arcirk{


    bool bJson::parse(const std::string &jsonText) {

        is_parse_ = !this->Parse(jsonText.c_str()).HasParseError();

        if (!is_parse_) {
            std::cerr << "parse ERROR: encountered a JSON parsing error" << std::endl;
            return false;
        }

        return true;
    }

    bool bJson::is_parse() {
        return is_parse_;
    }

    bVariant bJson::get_member(const std::string &member) {
        bVariant result;
        getMember(member, result);
        return result;
    }

    bool bJson::getMember(const std::string &member, bVariant &value) {

        _Value::ConstMemberIterator itr = this->FindMember(member.c_str());

        if (itr == this->MemberEnd()) {
            return false;
        } else {
            if (itr->value.IsInt()) {
                int intVal = itr->value.GetInt();
                value = intVal;
            } else if (itr->value.IsString()) {
                std::string strVal = itr->value.GetString();
                if(member == "uuid_form"){
                    boost::uuids::uuid uuid_form{};
                    if (is_valid_uuid(strVal, uuid_form))
                        value = uuid_form;
                    else
                        value = strVal;
                }else
                    value = strVal;
            } else if (itr->value.IsBool()) {
                bool boolVal = itr->value.GetBool();
                value = boolVal;
            } else {
                value = 0;
            }
        }

        return true;
    }

    void bJson::set_object(_Document * doc) {
        doc->SetObject();
    }

    std::string bJson::to_string(_Document *doc) {
        _StringBuffer stringBuffer;
        rapidjson::Writer<_StringBuffer> writer(stringBuffer);
        doc->Accept(writer);
        return stringBuffer.GetString();
    }

    void bJson::addMember(_Document *doc, content_value &val) {

        _Value::ConstMemberIterator itr = doc->FindMember(val.key.c_str());

        if (itr != doc->MemberEnd()) {
            std::cerr << "error: the key '" << val.key.c_str() << "' already exists" <<std::endl;
            return;
        }
        _Value key(val.key.c_str(), doc->GetAllocator());

        if (val.value.is_string()) {
            _Value value(val.value.get_string().c_str(), doc->GetAllocator());
            doc->AddMember(key, value, doc->GetAllocator());
        } else if (val.value.is_bool()) {
            doc->AddMember(key, val.value.get_bool(), doc->GetAllocator());
        } else if (val.value.is_int()) {
            int v = val.value.get_int();
            doc->AddMember(key, v, doc->GetAllocator());
        }else if (val.value.is_uuid()) {
            _Value value(val.value.to_string().c_str(), doc->GetAllocator());
            doc->AddMember(key, value, doc->GetAllocator());
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }
    void bJson::addMember(_Document *doc, content_value &val, bool updateIsExists) {

        _Value::MemberIterator itr = doc->FindMember(val.key.c_str());

        if (itr != doc->MemberEnd()) {
            if(!updateIsExists){
                std::cerr << "error: the key '" << val.key.c_str() << "' already exists" <<std::endl;
            }else{
                if (val.value.is_string()) {
                    itr->value.SetString(val.value.get_string().c_str(), doc->GetAllocator());
                } else if (val.value.is_bool()) {
                    itr->value.SetBool(val.value.get_bool());
                } else if (val.value.is_int()) {
                    itr->value.SetInt(val.value.get_int());
                }else if (val.value.is_uuid()) {
                    itr->value.SetString(val.value.to_string().c_str(), doc->GetAllocator());
                }

            }
            return;
        }

        _Value key(val.key.c_str(), doc->GetAllocator());

        if (val.value.is_string()) {
            _Value value(val.value.get_string().c_str(), doc->GetAllocator());
            doc->AddMember(key, value, doc->GetAllocator());
        } else if (val.value.is_bool()) {
            doc->AddMember(key, val.value.get_bool(), doc->GetAllocator());
        } else if (val.value.is_int()) {
            int v = val.value.get_int();
            doc->AddMember(key, v, doc->GetAllocator());
        }else if (val.value.is_uuid()) {
            _Value value(val.value.to_string().c_str(), doc->GetAllocator());
            doc->AddMember(key, value, doc->GetAllocator());
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    unsigned int bJson::get_member_count() {
        return this->MemberCount();
    }

    void bJson::set_array(_Document *doc) {
        doc->SetArray();
    }

    void bJson::addMember(_Value* object, content_value val) {

        _Value key(val.key.c_str(), this->GetAllocator());

        if (val.value.is_string()) {
            _Value value(val.value.get_string().c_str(), this->GetAllocator());
            object->AddMember(key, value, this->GetAllocator());
        } else if (val.value.is_bool()) {
            object->AddMember(key, val.value.get_bool(), this->GetAllocator());
        } else if (val.value.is_int()) {
            int v = val.value.get_int();
            object->AddMember(key, v, this->GetAllocator());
        }else if (val.value.is_uuid()) {
            _Value value(val.value.to_string().c_str(), this->GetAllocator());
            object->AddMember(key, value, this->GetAllocator());
        }
    }

    void bJson::addMember(const std::string &member, bVariant value) {
        _Value key(member.c_str(), this->GetAllocator());
        if (value.is_string()) {
            _Value val(value.get_string().c_str(), this->GetAllocator());
            this->AddMember(key, val, this->GetAllocator());
        } else if (value.is_bool()) {
            this->AddMember(key, value.get_bool(), this->GetAllocator());
        } else if (value.is_int()) {
            int v = value.get_int();
            this->AddMember(key, v, this->GetAllocator());
        }else if (value.is_uuid()) {
            _Value val(value.to_string().c_str(), this->GetAllocator());
            this->AddMember(key, val, this->GetAllocator());
        }
    }

    bool bJson::getMembers(_Document *doc, const std::string &member, std::vector<content_value> &value) {
        //Объект
        _Value::ConstMemberIterator itr = doc->FindMember(member.c_str());

        if (itr == doc->MemberEnd()) {
            return false;
        } else {
            if (itr->value.IsObject()) {


                for (_Value::ConstMemberIterator m = itr->value.MemberBegin(); m != itr->value.MemberEnd(); m++) {

                    content_value _value;

                    _value.key = m->name.GetString();

                    if (itr->value.IsInt()) {
                        long int intVal = m->value.GetInt();
                        _value.value = intVal;
                    } else if (m->value.IsString()) {
                        std::string strVal = m->value.GetString();
                        _value.value = strVal;
                    } else if (m->value.IsBool()) {
                        bool boolVal = m->value.GetBool();
                        _value.value = boolVal;
                    } else {
                        _value.value = "";
                    }

                    value.push_back(_value);
                }

            }
        }

        return true;
    }
    bool bJson::getArray(std::vector<std::map<std::string, std::string>> &values) {
        return getArray(this, values);
    }
    bool bJson::getArray(std::vector<std::map<std::string, bVariant>> &values) {
        return getArray(this, values);
    }

    bool bJson::getArray(_Document *doc, std::vector<std::map<std::string, std::string>> &values) {

        if(!doc->IsArray())
            return false;

        auto items = doc->GetArray();

        for (auto item = items.Begin(); item != items.End(); item++) {

            std::map<std::string, std::string> row;

            for (_Value::ConstMemberIterator m = item->MemberBegin(); m != item->MemberEnd(); m++) {

                std::string key = m->name.GetString();
                std::string value;

                if (m->value.IsInt()) {
                    long int intVal = m->value.GetInt();
                    value = std::to_string(intVal);
                } else if (m->value.IsString()) {
                    value = m->value.GetString();
                } else if (m->value.IsBool()) {
                    bool boolVal = m->value.GetBool();
                    value = std::to_string(boolVal);
                } else {
                    value = "";
                }

                row.insert(std::pair<std::string, std::string>(key, value));

            }

            values.push_back(row);
        }

        return true;
    }

    bool bJson::getArray(_Document *doc, std::vector<std::map<std::string, bVariant>> &values) {

        if(!doc->IsArray())
            return false;

        auto items = doc->GetArray();

        for (auto item = items.Begin(); item != items.End(); item++) {

            std::map<std::string, bVariant> row;

            for (_Value::ConstMemberIterator m = item->MemberBegin(); m != item->MemberEnd(); m++) {

                std::string key = m->name.GetString();
                bVariant value;

                if (m->value.IsInt()) {
                    long int intVal = m->value.GetInt();
                    value = intVal;
                } else if (m->value.IsString()) {
                    value = m->value.GetString();
                } else if (m->value.IsBool()) {
                    value = m->value.GetBool();
                } else {
                    value = "";
                }

                row.insert(std::pair<std::string, bVariant>(key, value));

            }

            values.push_back(row);
        }

        return true;
    }

    bool bJson::getMembers(_Document *doc, const std::string &member, std::vector<std::map<std::string, std::string>> &values) {
        //Объект
        _Value::ConstMemberIterator itr = doc->FindMember(member.c_str());

        if (itr == doc->MemberEnd()) {
            return false;
        } else {
            if (itr->value.IsObject()) {

                for (_Value::ConstMemberIterator m = itr->value.MemberBegin(); m != itr->value.MemberEnd(); m++) {

                    std::map<std::string, std::string> _value;

                    std::string key = m->name.GetString();
                    std::string value;

                    if (m->value.IsInt()) {
                        long int intVal = m->value.GetInt();
                        value = std::to_string(intVal);
                    } else if (m->value.IsString()) {
                        value = m->value.GetString();
                    } else if (m->value.IsBool()) {
                        bool boolVal = m->value.GetBool();
                        value = std::to_string(boolVal);
                    } else {
                        value = "";
                    }

                    _value.insert(std::pair<std::string, std::string>(key, value));

                    values.push_back(_value);
                }

            } else if (itr->value.IsArray()) {

                for (auto item = itr->value.Begin(); item != itr->value.End(); item++) {

                    std::map<std::string, std::string> row;

                    for (_Value::ConstMemberIterator m = item->MemberBegin(); m != item->MemberEnd(); m++) {

                        std::string key = m->name.GetString();
                        std::string value;

                        if (m->value.IsInt()) {
                            long int intVal = m->value.GetInt();
                            value = std::to_string(intVal);
                        } else if (m->value.IsString()) {
                            value = m->value.GetString();
                        } else if (m->value.IsBool()) {
                            bool boolVal = m->value.GetBool();
                            value = std::to_string(boolVal);
                        } else {
                            value = "";
                        }

                        row.insert(std::pair<std::string, std::string>(key, value));

                    }

                    values.push_back(row);
                }
            }

        }

        return true;
    }

    bool bJson::getMembers(_Document *doc, const std::string &member, std::vector<std::map<std::string, arcirk::bVariant>> &values) {
        //Объект
        _Value::ConstMemberIterator itr = doc->FindMember(member.c_str());

        if (itr == doc->MemberEnd()) {
            return false;
        } else {
            if (itr->value.IsObject()) {

                for (_Value::ConstMemberIterator m = itr->value.MemberBegin(); m != itr->value.MemberEnd(); m++) {

                    std::map<std::string, arcirk::bVariant> _value;

                    std::string key = m->name.GetString();
                    bVariant value;

                    if (itr->value.IsInt()) {
                        long int intVal = m->value.GetInt();
                        value = intVal;
                    } else if (m->value.IsString()) {
                        value = m->value.GetString();
                    } else if (m->value.IsBool()) {
                        bool boolVal = m->value.GetBool();
                        value = boolVal;
                    } else {
                        value = "";
                    }

                    _value.insert(std::pair<std::string, arcirk::bVariant>(key, value));

                    values.push_back(_value);
                }

            }

        }

        return true;
    }

    bool bJson::getMembers(_Document *doc, const std::string &member, std::vector<std::map<std::string, content_value>> &values) {

        _Value::ConstMemberIterator itr = doc->FindMember(member.c_str());

        if (itr == doc->MemberEnd()) {
            return false;
        } else {
            if (itr->value.IsObject()) {

                for (_Value::ConstMemberIterator m = itr->value.MemberBegin(); m != itr->value.MemberEnd(); m++) {

                    std::map<std::string, content_value> _value;

                    content_value item;
                    item.key = m->name.GetString();

                    if (itr->value.IsInt()) {
                        long int intVal = m->value.GetInt();
                        item.value = intVal;
                    } else if (m->value.IsString()) {
                        item.value = m->value.GetString();
                    } else if (m->value.IsBool()) {
                        bool boolVal = m->value.GetBool();
                        item.value = boolVal;
                    } else {
                        item.value = "";
                    }

                    _value.insert(std::pair<std::string, content_value>(item.key, item));

                    values.push_back(_value);
                }

            }

        }

        return true;
    }

    bool bJson::getMembers(_Document *doc, const std::string &member, std::vector<std::vector<content_value>> &values) {

        _Value::ConstMemberIterator itr = doc->FindMember(member.c_str());

        if (itr == doc->MemberEnd()) {
            return false;
        } else {
            if (itr->value.IsObject()) {

                for (_Value::ConstMemberIterator m = itr->value.MemberBegin(); m != itr->value.MemberEnd(); m++) {

                    std::vector<content_value> _value;

                    content_value item;
                    item.key = m->name.GetString();

                    if (itr->value.IsInt()) {
                        long int intVal = m->value.GetInt();
                        item.value = intVal;
                    } else if (m->value.IsString()) {
                        item.value = m->value.GetString();
                    } else if (m->value.IsBool()) {
                        bool boolVal = m->value.GetBool();
                        item.value = boolVal;
                    } else {
                        item.value = "";
                    }

                    _value.push_back(item);

                    values.push_back(_value);
                }

            }

        }

        return true;
    }

    bool bJson::getMembers(const std::string &member, std::vector<content_value> &value) {
        return getMembers(this, member, value);
    }

    bool bJson::getMembers(const std::string &member, std::vector<std::map<std::string, std::string>> &values) {
        return getMembers(this, member, values);
    }

    bool bJson::getMembers(const std::string &member, std::vector<std::map<std::string, arcirk::bVariant>> &values) {
        return getMembers(this, member, values);
    }

    bool bJson::getMembers(const std::string &member, std::vector<std::map<std::string, content_value>> &values) {
        return getMembers(this, member, values);
    }

    bool bJson::getMembers(const std::string &member, std::vector<std::vector<content_value>> &values) {
        return getMembers(this, member, values);
    }

    void bJson::addObject(_Document * doc, std::vector<content_value> &values) {

        if (!doc->IsObject()){
            std::cerr << "Документ должен быть типом 'Object'!";
            return;
        }

        for (auto iter : values){
            addMember(doc, iter);
        }

    }

    void bJson::addObject(std::vector<content_value> &values){
        addObject(this, values);
    }

    void bJson::push_back(_Value &arr, bVariant& val) {
        if (val.is_string()) {
            _Value value(val.get_string().c_str(), this->GetAllocator());
            arr.PushBack(value, this->GetAllocator());
        } else if (val.is_bool()) {
            bool v = val.get_bool();
            arr.PushBack( v, this->GetAllocator());
        } else if (val.is_int()) {
            int v = val.get_int();
            arr.PushBack( v, this->GetAllocator());
        }else if (val.is_uuid()) {
            _Value value(val.get_string().c_str(), this->GetAllocator());
            arr.PushBack( value, this->GetAllocator());
        }
    }

    void bJson::push_back(_Value &arr, _Value &val) {
        arr.PushBack(val, this->GetAllocator());
    }

    void bJson::push_back(_Value &value) {
        this->PushBack(value, this->GetAllocator());
    }

    void bJson::copy_from(_Value& val) {
        this->CopyFrom(val, this->GetAllocator());
    }

    void bJson::addMember(const std::string &key, _Value& value) {
        _Value _key(key.c_str(), this->GetAllocator());
        this->AddMember(_key, value, this->GetAllocator());
    }

    bool bJson::from_file(const std::string &filename) {

        is_parse_ = false;

        std::ifstream ifs;

        ifs.open(filename.c_str());
        if (!ifs)
        {
            std::cerr << "Ошибка открытия файла\n";
            return false;
        }
        IStreamWrapper isw{ ifs };

        if (this->ParseStream(isw).HasParseError())
            std::cerr << "ws_json::from_file ERROR: encountered a JSON parsing error" << std::endl;
        else
        {
            is_parse_ = true;
        }
        ifs.close();

        return is_parse_;

    }

    bool bJson::to_file(const std::string &filename) {
//        if(!is_parse_)
//            return false;

        std::ofstream ofs(filename);
        OStreamWrapper osw(ofs);

        Writer<OStreamWrapper> writer(osw);
        this->Accept(writer);


//        FILE* fp = fopen(filename.c_str(), "wb"); // non-Windows use "w"
//
//        char writeBuffer[65536];
//        FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
//
//        Writer<FileWriteStream> writer(os);
//        this->Accept(writer);
//
//        fclose(fp);

        return true;
    }

    std::string bJson::get_pt_member(ptree& pt, const std::string& key){
        try {
            return pt.get<std::string>(key);
        }catch (std::exception&){
            return "";
        }
    }

    bool bJson::parse_pt(const std::string& json, ptree& pt){
        try {

            std::stringstream data(json);
            boost::property_tree::json_parser::read_json(data, pt);

        }catch(std::exception&){
            return false;
        }

        return true;
    }

}
