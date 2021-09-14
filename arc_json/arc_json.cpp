#include "arc_json.h"

namespace arc_json{

    bool ws_json::from_file(const std::string &filename) {

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

    bool ws_json::getMember(const std::string &member, std::string &result) {

        if (!IsObject())
            return false;

        _Value::ConstMemberIterator itr = this->FindMember(member.c_str());

        if (itr == this->MemberEnd()) {
            result = "";
            return false;
        } else {
            if (itr->value.IsString()) {
                result = itr->value.GetString();
            } else
                result = "";
        }

        return true;
    }

    std::string ws_json::to_string(_Document *doc) {
        _StringBuffer stringBuffer;
        rapidjson::Writer<_StringBuffer> writer(stringBuffer);
        doc->Accept(writer);
        return stringBuffer.GetString();
    }

    bool ws_json::parse(const std::string &jsonText) {

        is_parse_ = !this->Parse(jsonText.c_str()).HasParseError();

        if (!is_parse_) {
            std::cerr << "ws_json::ws_json ERROR: encountered a JSON parsing error" << std::endl;
            return false;
        }

        return true;
    }

    bool ws_json::getMember(const std::string &member, boost::variant<int, bool, std::string> &value) {

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

    bool ws_json::getMembers(const std::string &member, std::vector<content_value> &value) {
        return getMembers(this, member, value);
    }

    bool ws_json::getMembers(_Document *doc, const std::string &member, std::vector<content_value> &value) {
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

    bool ws_json::getMembers(const std::string &member, std::vector <std::vector<content_value>> &value) {
        //Массив объектов
        _Value::ConstMemberIterator itr = this->FindMember(member.c_str());

        if (itr == this->MemberEnd()) {
            return false;
        } else {
            if (itr->value.IsArray()) {
                _Value::ConstArray a = itr->value.GetArray();

                for (_Value::ConstValueIterator row = a.Begin(); row != a.end(); row++) {
                    if (row->IsObject()) {
                        std::vector <content_value> _valuearr;

                        for (_Value::ConstMemberIterator col = row->MemberBegin(); col != row->MemberEnd(); col++) {

                            content_value _value;

                            _value.key = col->name.GetString();

                            if (col->value.IsInt()) {
                                long int intVal = col->value.GetInt();
                                _value.value = intVal;
                            } else if (col->value.IsString()) {

                                std::string strVal = col->value.GetString();
                                _value.value = strVal;
                            } else if (col->value.IsBool()) {
                                bool boolVal = col->value.GetBool();
                                _value.value = boolVal;
                            } else {
                                _value.value = "";
                            }

                            _valuearr.push_back(_value);
                        }

                        value.push_back(_valuearr);
                    }
                }

            }
        }
        return true;
    }

    void ws_json::addMember(_Document *doc, content_value &val) {

        _Value::ConstMemberIterator itr = doc->FindMember(val.key.c_str());

        if (itr != doc->MemberEnd()) {
            std::cerr << "error: the key '" << val.key.c_str() << "' already exists" <<std::endl;
            return;
        }
        _Value key(val.key.c_str(), doc->GetAllocator());

        if (val.value.type() == typeid(std::string)) {
            _Value value(boost::get<std::string>(val.value).c_str(), doc->GetAllocator());
            doc->AddMember(key, value, doc->GetAllocator());
        } else if (val.value.type() == typeid(bool)) {
            doc->AddMember(key, boost::get<bool>(val.value), doc->GetAllocator());
        } else if (val.value.type() == typeid(long int)) {
            doc->AddMember(key, boost::get<long int>(val.value), doc->GetAllocator());
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
    }

    void ws_json::addObjectMember(_Value* object, content_value val) {

        _Value key(val.key.c_str(), this->GetAllocator());

        if (val.value.type() == typeid(std::string)) {
            _Value value(boost::get<std::string>(val.value).c_str(), this->GetAllocator());
            object->AddMember(key, value, this->GetAllocator());
        } else if (val.value.type() == typeid(bool)) {
            object->AddMember(key, boost::get<bool>(val.value), this->GetAllocator());
        } else if (val.value.type() == typeid(long int)) {
            object->AddMember(key, boost::get<long int>(val.value), this->GetAllocator());
        }
    }

    void ws_json::set_object(_Document * doc) {
        doc->SetObject();
    }

    void ws_json::set_array(_Document *doc) {
        doc->SetArray();
    }

    void ws_json::push_back(_Value &value) {
        this->PushBack(value, this->GetAllocator());
    }

    bool ws_json::getMember(const std::string &member, int &result) {
        if (!IsObject())
            return false;

        _Value::ConstMemberIterator itr = this->FindMember(member.c_str());

        if (itr == this->MemberEnd()) {
            result = 0;
            return false;
        } else {
            if (itr->value.IsInt()) {
                result = itr->value.GetInt();
            } else
                result = 0;
        }

        return true;
    }

    unsigned int ws_json::get_member_count() {
        return this->MemberCount();
    }

    std::string ws_json::getStringMember(const std::string &member) {

        if (!IsObject())
            return "";

        _Value::ConstMemberIterator itr = this->FindMember(member.c_str());

        if (itr == this->MemberEnd()) {
            return "";
        } else {
            if (itr->value.IsString()) {
                return itr->value.GetString();
            }
        }
        return "";
    }

    void ws_json::addObject(_Document * doc, std::vector<content_value> &values) {

        if (!doc->IsObject()){
            std::cerr << "Документ должен быть типом 'Object'!";
            return;
        }

        for (auto iter : values){
            addMember(doc, iter);
        }

    }
    void ws_json::addObject(std::vector<content_value> &values){
        addObject(this, values);
    }
}
