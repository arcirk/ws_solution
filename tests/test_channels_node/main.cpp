//
// Created by arcady on 25.07.2021.
//
#include <utility>
#include <vector>
#include <iostream>
#include <arc_json.h>
#include <unordered_set>
#include <mutex>
#include <string>

#include <boost/variant.hpp>

#include <tuple>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <cstdio>
#include <string>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/algorithm/string.hpp>


std::string get_sha1(const std::string& p_arg)
{
    boost::uuids::detail::sha1 sha1;
    sha1.process_bytes(p_arg.data(), p_arg.size());
    unsigned hash[5] = {0};
    sha1.get_digest(hash);

    // Back to string
    char buf[41] = {0};

    for (int i = 0; i < 5; i++)
    {
        std::sprintf(buf + (i << 3), "%08x", hash[i]);
    }

    return std::string(buf);
}
//std::mutex mutex_;


//typedef struct channel{
//
//    std::string name;
//    UUID uuid;
//    std::unordered_set<UUID> parent;
//
//    channel(std::string name_, UUID uuid_)
//    : name(std::move(name_)),
//      uuid(uuid_)
//    {
//    }
//
//}channel;

//typedef std::map<boost::uuids::uuid, channel*> channels_child;

//typedef struct _channel : channel {
//    channels_child child_node;
//    _channel(std::string name, boost::uuids::uuid uuid)
//    : channel(std::move(name), uuid), child_node{}
//    {
//    }
//
//}_channel;

//typedef std::map<boost::uuids::uuid, _channel*> channels_tree;

//std::unordered_set<_channel*> _channels;

boost::uuids::uuid new_uuid(){
    return boost::uuids::random_generator()();
}

//void read_channels(channels_tree& tree)
//{
//    arc_json::ws_json json;
//
//    json.from_file("/home/arcady/websockets_src/ws_solution/ws_server/app/config/channels.json");
//
//    if (!json.is_parse()){
//        return;
//    }
//    boost::uuids::uuid _uuid = new_uuid();
//
//    if (json.is_object()){
//        std::string name ;
//        boost::uuids::uuid id{};
//        for (_Value::ConstMemberIterator itr = json.MemberBegin_(); itr < json.MemberEnd_(); itr++) {
//            if (itr->name.GetString() == "name"){
//                name = itr->value.GetString();
//            }else if (itr->name.GetString() == "uuid"){
//                id = arc_json::string_to_uuid(itr->value.GetString());
//            }else if (itr->name.GetString() == "child"){
//
//            }
//        }
//
//        tree.insert(std::pair<boost::uuids::uuid, _channel*>(id, new _channel(name, id)));
//
//    }
//
//}

//void join(channel* ch){
//    std::lock_guard<std::mutex> lock(mutex_);
//    //_channels.insert(ch);
//}

void getValue(_Value::ConstValueIterator itr, arc_json::content_value * val){

    if (itr->IsBool()){
        val->value = itr->GetBool();
    }else if (itr->IsInt()){
        val->value = itr->GetInt();
    }else if (itr->IsString()){
        val->value = itr->GetString();
    }
}

void read_members(_Value::ConstMemberIterator itr, std::vector<arc_json::content_value*> &content){

    std::string key = itr->name.GetString();

    auto * _content = new arc_json::content_value();
    _content->key = key;

    if (itr->value.IsObject()){ // content_value key, content_value(key, array<content_value>)
        //std::cout << "object" << std::endl;
        _Value::ConstObject object = itr->value.GetObject();
        std::vector<arc_json::content_value*> child_content;

        for (_Value::ConstMemberIterator _itr = object.MemberBegin(); _itr < object.MemberEnd(); _itr++) {
            read_members(_itr, child_content);
        }
        _content->value = child_content;
        content.emplace_back(_content);
    }
    else if (itr->value.IsArray()){

        _Value::ConstArray array = itr->value.GetArray();

        std::vector<arc_json::content_value*> array_content;

        for (_Value::ConstValueIterator row = array.Begin(); row < array.End(); row++) {

            auto * obj = new arc_json::content_value ();

            if (row->IsObject()){
                obj->key = "object_item";
                std::vector<arc_json::content_value*> row_content;
                for (_Value::ConstMemberIterator _itr = row->MemberBegin(); _itr < row->MemberEnd(); _itr++){
                    read_members(_itr, row_content);
                }
                obj->value = row_content;
            }else if (row->IsArray()){
                obj->key = "array_item";
                //
            }else
            {
                getValue(row, obj);
            }
            array_content.emplace_back(obj);
        }

        _content->value = array_content;
        content.emplace_back(_content);
    }
    else{

        std::string value;
        try {
            value = itr->value.GetString();
        }catch (std::exception& e){
            std::cerr << e.what() << std::endl;
            value = "null";
        }
        auto * val = new arc_json::content_value (key, value);
        content.emplace_back(val);
    }
}

void get_settings(){

    arc_json::ws_json json;

    json.from_file("../settings.json");
    if (!json.is_parse()){
        return;
    }

    auto var_doc = new arc_json::content_value;
    var_doc->key = "root";

    std::vector<arc_json::content_value *> members;
    for (_Value::ConstMemberIterator itr = json.MemberBegin_(); itr < json.MemberEnd_(); itr++){
        read_members(itr, members);
    }

    var_doc->value = members;

    for (auto itr : members) {

        std::string member = itr->key;
        member.append(": ");

        if (itr->value.type() == typeid(std::vector<arc_json::content_value*>)){

            std::vector<arc_json::content_value*> vec = boost::get<std::vector<arc_json::content_value*>>(itr->value);

            bool is_array = false;
            if (vec.size() > 1 && (vec[0]->key == "object_item" || vec[0]->key == "array_item")){
                member.append("\n[");
                is_array = true;
            }
            std::string child ="";// "\n   {\n     ";

            if (!is_array){
                child.append("\n   {\n     ");
            }

            for (auto it = vec.begin() ; it != vec.end(); ++it) {

                arc_json::content_value * a = *it;

                if (!is_array){
                    child.append(a->key);
                    child.append(": ");
                }

                if (a->value.type() == typeid(std::string)){
                    child.append(boost::get<std::string>(a->value));
                    if (it != --vec.end()){
                        child.append("\n     ");
                    }
                } else if (a->value.type() == typeid(std::vector<arc_json::content_value*>)){
                    child.append("\n   " + a -> key + ": ");
                    std::vector<arc_json::content_value*> child_vec = boost::get<std::vector<arc_json::content_value*>>(a->value);
                    child.append("\n   {\n     ");
                    //for (auto i: child_vec) {
                    for (auto c = child_vec.begin() ; c != child_vec.end(); ++c) {
                        arc_json::content_value * i = *c;
                        child.append(i->key);
                        child.append(": ");



                        if (c != --child_vec.end()){
                            child.append("\n     ");
                        }
                    }
                    child.append("\n   }");
                }
            }

            if (!is_array){
                child.append("\n   }");
            }

            member.append(child);

            if (is_array){
                member.append("\n]");
            }
        }else
        {
            if (itr->value.type() == typeid(std::string)){
                member.append(boost::get<std::string>(itr->value));
            }
        }

        std::cout << member << std::endl;
    }

}

int main(){


    std::string line;

    std::string usr = "Arcady";
    boost::to_upper(usr);

    std::string pwd = "281974";

    std::string hash = get_sha1(std::string(usr) + pwd);

    std::cout << "user name:" << std::endl;
    std::string _usr;
    getline(std::cin, _usr);
    std::cout << "password:" << std::endl;
    boost::trim(_usr);
    boost::to_upper(_usr);

    std::string _pwd;
    getline(std::cin, _pwd);

    std::string hash1 = get_sha1(std::string(_usr) + _pwd);

    if (hash == hash1){
        std::cout << "true" << std::endl;
        std::cout << hash << std::endl;
    } else
        std::cout << "false" << std::endl;



/*    const std::string filename = "/home/arcady/ws_sockets_src/ws_server/app/config/channels.json";

    using boost::property_tree::ptree;
    ptree pt;

    boost::property_tree::json_parser::read_json(filename, pt);

    for(const ptree::value_type &v : pt)
    {
        std::cout << v.first << std::endl;



    }

    boost::property_tree::ptree &array =
            pt.get_child("child");

    for (boost::property_tree::ptree::iterator element = array.begin();
        element != array.end();
        element++)
    {
        std::cout <<  element->second.get<std::string>("name")  << std::endl;
    };*/


//    for(const auto& i : pt.get_child("child"))
//    {
//        //            std::string name;
//        //            ptree sub_pt;
//        //            std::tie(name, sub_pt) = i;
//        //
//        //            if (name != "M")
//        //                continue;
//        //            std::cout << name << std::endl;
//        //            std::cout << "\t" << sub_pt.get<std::string>("<xmlattr>.name") << std::endl;
//        //            std::cout << "\t" << sub_pt.get<std::string>("<xmlattr>.type") << std::endl;
//
//
//    }
//
//    std::cout << std::endl;

    return EXIT_SUCCESS;
/*    auto * tree = reinterpret_cast<Node<std::string> *>(new Node(std::string ("root")));

    tree->add("test");

    auto child = tree->("test");*/


    //get_settings();

/*
    std::vector<UUID> node;

    for (int i = 0; i < 7; ++i) {
        node.emplace_back(boost::uuids::random_generator()());
    }

    lstdTreeNode< UUID > tree( node[0] );
    // add some children
    tree.addChild( node[1] );
    tree.addChild( node[2]);
    // add a child to the 2nd child
    tree.getChildren()[1].addChild( node[4] );
    // add children to the 1st child of the 2nd child
    tree.getChildren()[1].getChildren()[0].addChild( node[3] );
    tree.getChildren()[1].getChildren()[0].addChild( node[6]);
    tree.addChild( node[3] );
    // remove a child
    tree.removeChild( node[3] );
    // debug print
    tree.print();

    system( "PAUSE" );
    return 0;
*/


//    const int key=5;
//    for (auto it = mtp.lower_bound(key); it != mtp.end() && it->first ==key; it++)
//    {
//        ...
//    }
//    std::multimap<int,int> mtp;
//
//    //бежим по по всем элементам
//    //it здесь - итератор
//    for(auto it=mtp.begin(); it!=mtp.end(); it++)
//    {
//        it->first;//ключ
//        it->second;//значение
//    }
//
//    //бежим по всем элементам (краткая запись)
//    //it здесь - ссылка на элемент
//    for(auto& it:mtp)
//    {
//        it.first;//ключ
//        it.second;//значение
//    }
//
//    //бежим по ключу 5
//    auto r5=mtp.equal_range(5);
//    //it здесь - итератор
//    for(auto it=r5.first; it!=r5.second; it++)
//    {
//        it->first;//ключ
//        it->second;//значение
//    }

    return 0;

/*        std::vector<arc_json::content_value> value;
        value.emplace_back(arc_json::content_value ("key1", std::string ("test")));

        arc_json::content_value val("key", value);

        if (val.value.type() == typeid(std::vector<arc_json::content_value>)) {
            std::vector<arc_json::content_value> test_val = boost::get<std::vector<arc_json::content_value>>(val.value);
            if (test_val.size() > 0) {
                arc_json::content_value result = test_val[0];
                if (result.value.type() == typeid(std::string)) {
                    std::cout << boost::get<std::string>(result.value) << std::endl;
                }
            }
        }*/

    return 0;
}