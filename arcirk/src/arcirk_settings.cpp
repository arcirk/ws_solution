//
// Created by arcady on 03.01.2022.
//

#include "../include/arcirk.h"
#ifdef _WINDOWS
#include <filesystem>
#include <tchar.h>
#else
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>
#endif

#define ARR_SIZE(x) (sizeof(x) / sizeof(x[0]))

#ifdef _WINDOWS
using namespace std::filesystem;
#else
using namespace boost::filesystem;
#endif

const std::string bFieldsStr[] = {
        "ServerHost",
        "ServerPort",
        "User",
        "Hash",
        "ServerName",
        "AppName",
        "AutoConnect",
        "SaveHash",
        "ServerWorkingDirectory",
        "ClientWorkingDirectory",
        "isLocalInstallation",
        "RunAsService",
        "UseLocalWebDavDirectory",
        "LocalWebDavDirectory",
        "WebDavHost",
        "WebDavUser",
        "WebDavPwd",
        "WebDavSSL"};

namespace arcirk{

    bConf::bConf(const std::string& file_name, bool public_settings, bool no_save_def){

        init(public_settings);

#ifdef _WINDOWS
        separator = '\\';
#else
        separator = boost::filesystem::path::separator;
#endif

        if(!public_settings){
            output_directory = get_conf_directory() + separator + "config";
        }else
            output_directory = parent_path() + separator + "config"; //boost::dll::program_location().parent_path().string() + separator + "config";

        bool result = verify_directory(output_directory);

        if(!result){
            std::cerr << "Error verify config directory!" << std::endl;
            return;
        }

        if(!file_name.empty())
            output_filename = file_name;

        std::string fileName = output_directory + separator + output_filename;

        if (!exists(path(fileName))){
            if(public_settings)
                m_vec[bConfFields::AppName] = "server";
            if(!no_save_def)
            save();
        }
        else
            parse();

    }

    bVariant & bConf::operator[] (bConfFields index) {
        return m_vec[index];
    }

    bVariant const &bConf::operator[](bConfFields index) const {
        return m_vec[index];
    }

    bVariant bConf::get(bConfFields index) const {
        return m_vec[index];
    }

    void bConf::set(bConfFields index, const bVariant &value) {
        m_vec[index] = value;
    }

    std::string bConf::to_string() const{
        bJson m_doc{};
        m_doc.set_object();
        int fCount = sizeof(bFieldsStr) / sizeof(bFieldsStr[0]);

        for (int i = 0; i < fCount; ++i) {
            std::string key = bFieldsStr[i];
            m_doc.addMember(content_value(key, m_vec[i]));
        }

        return m_doc.to_string();
    }

    void bConf::init(bool server) {

        m_vec.clear();
        std::vector<bConfFields> m_bool;
        m_bool.push_back(bConfFields::AutoConnect);
        m_bool.push_back(bConfFields::RunAsService);
        m_bool.push_back(bConfFields::SaveHash);
        m_bool.push_back(bConfFields::UseLocalWebDavDirectory);
        m_bool.push_back(bConfFields::WebDavSSL);
        m_bool.push_back(bConfFields::isLocalInstallation);

        int fCount = sizeof(bFieldsStr) / sizeof(bFieldsStr[0]);

        for (int i = 0; i < fCount; ++i) {
            auto ind = (bConfFields)i;
            //std::string key = bFieldsStr[i];

            if (ind == bConfFields::ServerPort){
                //m_doc.addMember(content_value(key, 0));
                m_vec.push_back(0);
            }
            else{
                if(std::find(m_bool.begin(), m_bool.end(), ind) == m_bool.end())
                    m_vec.push_back("");
                else
                    m_vec.push_back(false);
            }
        }

        m_vec[bConfFields::ServerHost] = "localhost";
        m_vec[bConfFields::ServerPort] = 8080;
        m_vec[bConfFields::User] = "admin";
        m_vec[bConfFields::ServerName] = "NoName";
        m_vec[bConfFields::AppName] = "unknown application";
        if(server)
            m_vec[bConfFields::ServerWorkingDirectory] = parent_path(); //boost::dll::program_location().parent_path().string();
        else
            m_vec[bConfFields::ClientWorkingDirectory] = parent_path(); //boost::dll::program_location().parent_path().string();
    }

    void bConf::save() {

        bJson m_doc{};
        m_doc.set_object();
//        std::string separator;
//#ifdef _WINDOWS
//        separator = '\\';
//#else
//        separator = boost::filesystem::path::separator;
//#endif
        int fCount = sizeof(bFieldsStr) / sizeof(bFieldsStr[0]);

        for (int i = 0; i < fCount; ++i) {
            std::string key = bFieldsStr[i];
            m_doc.addMember(content_value(key, m_vec[i]));
        }
        std::string fileName = output_directory + separator + output_filename;

        m_doc.to_file(fileName);
    }

    bool bConf::parse() {
//        std::string separator;
//#ifdef _WINDOWS
//        separator = '\\';
//#else
//        separator = boost::filesystem::path::separator;
//#endif
        bJson m_doc{};
        std::string fileName = output_directory + separator + output_filename;

        if (exists(path(fileName)))
            m_doc.from_file(fileName);
        else
            return false;

        if(!m_doc.is_parse())
            return false;

        int fCount = sizeof(bFieldsStr) / sizeof(bFieldsStr[0]);
        for (int i = 0; i < fCount; ++i) {
            std::string key = bFieldsStr[i];
            bVariant val = m_doc.get_member(key);
            m_vec[i] = val;
        }

        return true;
    }

    std::string bConf::get_field_alias(bConfFields val){
        return bFieldsStr[val];
    }

#ifdef _WINDOWS
    LPTSTR bConf::ExtractFilePath(LPCTSTR FileName, LPTSTR buf)
    {
        int i, len = lstrlen(FileName);
        for(i=len-1; i>=0; i--)
        {
            if(FileName[i] == _T('\\'))
                break;
        }
        lstrcpyn(buf, FileName, i+2);
        return buf;
    }
#endif

    std::string bConf::parent_path() {
#ifdef _WINDOWS
        TCHAR szFileName[MAX_PATH], szPath[MAX_PATH];
        GetModuleFileName(0, szFileName, MAX_PATH);
        ExtractFilePath(szFileName, szPath);
        return std::string(szPath);//set _MBCS;
#else
        return boost::dll::program_location().parent_path().string();
#endif
    }

    void* bConf::_crypt(void* data, unsigned data_size, void* key, unsigned key_size)
    {
        assert(data && data_size);
        if (!key || !key_size) return data;

        auto* kptr = (uint8_t*)key; // начало ключа
        uint8_t* eptr = kptr + key_size; // конец ключа

        for (auto* dptr = (uint8_t*)data; data_size--; dptr++)
        {
            *dptr ^= *kptr++;
            if (kptr == eptr) kptr = (uint8_t*)key; // переход на начало ключа
        }
        return data;
    }

    std::string bConf::crypt(const std::string &source, const std::string& key) {

        std::string s = source;
        char * text = s.data();
        std::string _key = key;
        char * pass = _key.data();

        _crypt(text, ARR_SIZE(text), pass, ARR_SIZE(pass));

        std::string result(text);

        return result;
    }

}

