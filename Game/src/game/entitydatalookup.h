#ifndef ENTITYDATALOOKER_H
#define ENTITYDATALOOKER_H

#include <optional>
#include "../core/3rdparty/tinyxml/tinyxml2.h"
#include "../core/archive_type.h"
#include "../core/resourcelookup.h"
#include "data/building_data.h"
#include "data/unit_data.h"

namespace strat
{
  class EntityDataLookup
  {
  public:
    EntityDataLookup() {}

    bool loadFromFile(const gengine2d::string_t& filename);
    bool loadFromStream(std::istream& stream);

    gengine2d::vector_t<gengine2d::string_t> loadFactionList(const gengine2d::string_t& filename);
    gengine2d::vector_t<gengine2d::string_t> loadFactionList(std::istream& stream);

    bool exists(const gengine2d::string_t& name) const;
    const BuildingData& getBuildingData(const gengine2d::string_t& name) const;
    const UnitData& getUnitData(const gengine2d::string_t& name) const;

  private:
    bool loadBuildings(gengine2d::archive_file_t* buildingDataFile,
                       const gengine2d::vector_t<gengine2d::string_t>& factionList);
    bool loadUnits(gengine2d::archive_file_t* unitDataFile,
                   const gengine2d::vector_t<gengine2d::string_t>& factionList);

    std::optional<std::pair<gengine2d::string_t, BuildingData>> loadBuilding(const gengine2d::string_t& faction,
                                                                             tinyxml2::XMLElement* buildingElement);
    std::optional<std::pair<gengine2d::string_t, UnitData>> loadUnit(const gengine2d::string_t& faction,
                                                                     tinyxml2::XMLElement* buildingElement);

    bool loadColliderData(BaseData& data, const tinyxml2::XMLElement* colliderSection);
    bool loadRequirementData(BaseData& data, const tinyxml2::XMLElement* requirementsSection);
    bool loadAttackData(AttackData& data, const tinyxml2::XMLElement* attackSection);

    gengine2d::ResourceLookup<BuildingData> m_buildingLookup;
    gengine2d::ResourceLookup<UnitData>     m_unitLookup;
  };
}

#endif // ENTITYDATALOOKER_H
