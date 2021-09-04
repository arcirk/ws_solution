//
// Created by arcady on 27.07.2021.
//

#ifndef WS_SOLUTION_PUBLIC_CHANNELS_H
#define WS_SOLUTION_PUBLIC_CHANNELS_H

#include <arc_json.h>

#include <vector>

typedef boost::uuids::uuid> uuid_;

class public_channel{

public:
     std::string name;
     void join(uuid_ sub){

     }

private:
    std::vector<uuid_> subscribers_;

};

#endif //WS_SOLUTION_PUBLIC_CHANNELS_H
