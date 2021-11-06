//
// Created by arcady on 05.11.2021.
//

#include "../include/arcirk.h"


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
            doc->AddMember(key, val.value.get_int(), doc->GetAllocator());
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
            object->AddMember(key, val.value.get_int(), this->GetAllocator());
        }else if (val.value.is_uuid()) {
            _Value value(val.value.to_string().c_str(), this->GetAllocator());
            object->AddMember(key, value, this->GetAllocator());
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

    bool bJson::getMembers(const std::string &member, std::vector<content_value> &value) {
        return getMembers(this, member, value);
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
            arr.PushBack( val.get_bool(), this->GetAllocator());
        } else if (val.is_int()) {
            arr.PushBack( val.get_int(), this->GetAllocator());
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