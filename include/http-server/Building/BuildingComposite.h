//
// Created by Julia on 2019-12-20.
//

#ifndef HTTP_SERVER_BUILDINGCOMPOSITE_H
#define HTTP_SERVER_BUILDINGCOMPOSITE_H

#include<vector>
#include "BuildingComponent.h"
#include <memory>
#include "Equipment.h"

class BuildingComposite : BuildingComponent{
private:
    std::vector<std::shared_ptr<BuildingComponent>> buildingComponents;
    std::string street;
public:
    BuildingComposite(int idx, std::string name);
    BuildingComposite(int idx, std::string name, std::string _street);
    std::shared_ptr<BuildingComponent> getComponentById(int id);
    int GetChildCount();
    std::shared_ptr<BuildingComponent> GetLastChild();
    void AddChild(std::shared_ptr<BuildingComponent> buildingComponent);
    void addEquipment(std::shared_ptr<Equipment> eq, int roomId, int floorId);
    void deleteEquipment(int eqId, int roomId, int floorId);
    std::shared_ptr<Equipment> getEquipment(int idx, int roomId, int floorId);
     void addFloor(std::shared_ptr<BuildingComponent> floor);
     void addRoom(int floorId, std::shared_ptr<BuildingComponent> room);
     void deleteFloor(int floorId);
    void deleteRoom(int floorId, int roomId);
    std::string showMyInfo();
    std::string showMyEq();
    std::string showFloorInfo(int floorId);
    std::string showFloorEq(int floorId);
    std::string showRoomInfo(int floorId, int roomId);
    std::string showRoomEq(int floorId, int roomId);
};




#endif //HTTP_SERVER_BUILDINGCOMPOSITE_H
