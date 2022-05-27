//#include <arcirk.h>
#include "../arcirk/include/arcirk.h"

class ServeResponse
{
public:
    ServeResponse(const std::string& resp);

    std::string message;
    std::string command;
    std::string result;
    bool isParse;
    std::string recipient;
    std::string token;
    std::string uuid;
    std::string uuid_session;
    std::string contentType;
    std::string recipientName;
    std::string app_name;
    std::string uuid_form;
    std::string ip_address;

    static arcirk::bJson parseResp(const std::string& resp);

    static std::string base64_decode(const std::string& resp);

    [[nodiscard]] std::string to_string() const;

private:
    void parse(const std::string& resp);


};