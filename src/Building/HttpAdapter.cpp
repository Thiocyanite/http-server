//
// Created by Julia on 2019-12-30.
//


#include <http-server/Building/HttpAdapter.h>
#include <list>

using namespace boost::xpressive;


HttpResponser::HttpResponser(std::shared_ptr<BuildingSystem> building){
    buildingSystem=building;
    get_regex = sregex::compile(
            "^GET /buildings"
            "("
            /**/"/(?P<building>[[:digit:]]+)"
            /**/"("
            /*  */"/floors/(?P<floor>[[:digit:]]+)"
            /*  */"(/rooms/(?P<room>[[:digit:]]+))?"
            /**/")?"
            ")?"
            "/(?P<what>equipment|structure)"
    );
    put_regex = sregex::compile(
            "^PUT /buildings"
            "("
            /**/"/(?P<building>[[:digit:]]+)/floors"
            /**/"("
            /*  */"/(?P<floor>[[:digit:]]+)/rooms"
            /*      */"("
            /*      */"/(?P<room>[[:digit:]]+)/equipment"
            /*  */")?"
            /**/")?"
            ")?$"
    );

    post_regex = sregex::compile(
            "^POST /buildings(?P<building>[[:digit:]]+)"
            "/floors/(?P<floor>[[:digit:]]+)"
            "/rooms/(?P<room>[[:digit:]]+)"
            "/equipment/(?P<equipment>[[:digit:]]+)"
            "$"
    );
    destination_regex = sregex::compile(
            "^/buildings(?P<building>[[:digit:]]+)"
            "/floors/(?P<floor>[[:digit:]]+)"
            "/rooms/(?P<room>[[:digit:]]+)"
            "$"
    );
    delete_regex = sregex::compile(
            "^DELETE /buildings/(?P<building>[[:digit:]]+)"
            "("
            /**/"/floors/(?P<floor>[[:digit:]]+)"
            /**/"("
            /*  */"/rooms/(?P<room>[[:digit:]]+)"
            /*  */"("
            /*      */"/equipment/(?P<equipment>[[:digit:]]+)"
            /*  */")?"
            /**/")?"
            ")?$"
    );
}

HttpResponse HttpResponser::createResponse(std::shared_ptr<HttpRequest> request){
    responseBuilder.init();

    std::string str = request->getRequest();
    smatch match_path;
    if (regex_search(str, match_path, get_regex))
    {
        int building = std::atoi(match_path["building"].str().c_str());
        int floor = std::atoi(match_path["floor"].str().c_str());
        int room = std::atoi(match_path["room"].str().c_str());
        std::string what = match_path["what"].str();
        /*ZAWSZE PODANE JEST WHAT*/

        try {
            if (what=="eqipment")
                responseBuilder.setBody(buildingSystem->find({building,floor,room}).first->showMyEq());
            else
            responseBuilder.setBody(buildingSystem->find({building,floor,room}).first->showMyInfo());
        }
        catch (...){
            responseBuilder.setBody("Something wrong\n");
            responseBuilder.setStatusCode(StatusCode::Bad_Request);
            responseBuilder.setHeaderInfo({
                                                  {"Server",       {"Test"}},
                                                  {"Connection",   {"close"}},
                                                  {"Content-Type", {"text/xml"}}
                                          });
        }
    }
    else if (regex_search(str, match_path, post_regex))
    {
        auto values = request->getFieldValue("destination");
        if (values.size() == 1 && values[1] != HttpRequest::NO_SUCH_KEY)
        {
            smatch match_destination;
            if (regex_search(str, match_destination, destination_regex))
            {
                int source_building = std::atoi(match_path["building"].str().c_str());
                int source_floor = std::atoi(match_path["floor"].str().c_str());
                int source_room = std::atoi(match_path["room"].str().c_str());
                int source_equipment = std::atoi(match_path["equipment"].str().c_str());

                int destination_building = std::atoi(match_destination["building"].str().c_str());
                int destination_floor = std::atoi(match_destination["floor"].str().c_str());
                int destination_room = std::atoi(match_destination["room"].str().c_str());
                /*W OBU PRZYPADKACH ZAWSZE WSZYSTKO PODANE*/
                try{
                    buildingSystem->move(source_equipment, {source_building,source_floor,source_room},
                            {destination_building,destination_floor,destination_room});
                    responseBuilder.setStatusCode(StatusCode::OK);
                    responseBuilder.setHeaderInfo({
                                           {"Server",       {"Test"}},
                                           {"Connection",   {"close"}},
                                           {"Content-Type", {"text/xml"}}
                                   });
                }
                catch (...){
                    responseBuilder.setBody("Couldn't move this element.\n");
                    responseBuilder.setStatusCode(StatusCode::Not_Modified);
                    responseBuilder.setHeaderInfo({
                                           {"Server",       {"Test"}},
                                           {"Connection",   {"close"}},
                                           {"Content-Type", {"text/xml"}}
                                   });
                }
            }
            else
            {
                responseBuilder.setBody("Destination is inproper.\n");
                responseBuilder.setStatusCode(StatusCode::Not_Found);
                responseBuilder.setHeaderInfo({
                                                      {"Server",       {"Test"}},
                                                      {"Connection",   {"close"}},
                                                      {"Content-Type", {"text/xml"}}
                                              });
                /*DESTINATION IS GARBAGE*/
            }
        }
        else
        {
            responseBuilder.setBody("Destination wasn't given.\n");
            responseBuilder.setStatusCode(StatusCode::Precondition_Failed);
            responseBuilder.setHeaderInfo({
                                                  {"Server",       {"Test"}},
                                                  {"Connection",   {"close"}},
                                                  {"Content-Type", {"text/xml"}}
                                          });
            /*DESTINATION NOT GIVEN*/
        }
    }
    else if (regex_search(str, match_path, delete_regex))
    {
        int source_building = std::atoi(match_path["building"].str().c_str());
        int source_floor = std::atoi(match_path["floor"].str().c_str());
        int source_room = std::atoi(match_path["room"].str().c_str());
        int source_equipment = std::atoi(match_path["equipment"].str().c_str());
        try {
            buildingSystem->remove({source_building,source_floor,source_room,source_equipment});
            responseBuilder.setBody("Item deleted.\n");
            responseBuilder.setStatusCode(StatusCode::OK);
            responseBuilder.setHeaderInfo({
                                                  {"Server",       {"Test"}},
                                                  {"Connection",   {"close"}},
                                                  {"Content-Type", {"text/xml"}}
                                          });
        }
        catch (...){
            responseBuilder.setBody("Something wrong\n");
            responseBuilder.setStatusCode(StatusCode::Bad_Request);
            responseBuilder.setHeaderInfo({
                                                  {"Server",       {"Test"}},
                                                  {"Connection",   {"close"}},
                                                  {"Content-Type", {"text/xml"}}
                                          });
        }
        /*CO NAJMNIEJ BUILDING PODANE*/
    }
    else if (regex_search(str, match_path, put_regex))
    {
        int source_building = std::atoi(match_path["building"].str().c_str());
        int source_floor = std::atoi(match_path["floor"].str().c_str());
        int source_room = std::atoi(match_path["room"].str().c_str());
        int source_equipment = std::atoi(match_path["equipment"].str().c_str());
        /*JEŚLI NIC NIE PODANE DODAJEMY BUDYNEK
         *JEŚLI PODANY JEST BUDYNEK TO DODAJEMY PIĘTRO
         *JEŚLI PODANY JEST BUDYNEK I PIĘTRO DODAJEMY POKÓJ*/
       try{
            if (source_equipment!=0){
                //std::shared_ptr<Equipment> eq=buildingSystem->equipmentFromJson(request);
                //buildingSystem->add({source_building,source_floor,source_room,source_equipment}, eq);
            }
            else{
                if (source_room!=0){
                    //::shared_ptr<BuildingComponent> room= buildingSystem->roomFromJson(request);
                    //buildingSystem->add({source_building,source_floor,source_room}, room);
                }
                //else
                    //std::shared_ptr<BuildingComponent> composite = buildingSystem->compositeFromJson(request);
                    //buildingSystem->add({source_building,source_floor},composite);
            }
           responseBuilder.setBody("Item created\n");
           responseBuilder.setStatusCode(StatusCode::OK);
           responseBuilder.setHeaderInfo({
                                                 {"Server",       {"Test"}},
                                                 {"Connection",   {"close"}},
                                                 {"Content-Type", {"text/xml"}}
                                         });
       }
       catch(...){
           responseBuilder.setBody("Something wrong\n");
           responseBuilder.setStatusCode(StatusCode::Bad_Request);
           responseBuilder.setHeaderInfo({
                                                 {"Server",       {"Test"}},
                                                 {"Connection",   {"close"}},
                                                 {"Content-Type", {"text/xml"}}
                                         });
       }
    }
    return responseBuilder.getResponse();
}
