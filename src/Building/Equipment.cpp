//
// Created by Julia on 2019-12-22.
//

#include "../../include/http-server/Building/Equipment.h"

Equipment::Equipment(std::string _name, enum Type enumType){
    index=++counter;
    name=_name;
    type=enumType;
}

int Equipment::getId() {
    return index;
}