#include "entitydatalookup.h"

#include <fstream>
#include "../core/utility/converter_utility.h"
#include "../core/utility/general_utility.h"
#include "../core/utility/stream_utility.h"
#include "../core/utility/string_utility.h"
#include "../core/utility/type_determine_utility.h"

namespace strat
{
  static constexpr auto FILENAME_FACTIONS = "factions.xml";
  static constexpr auto FILENAME_BUILDINGS = "buildings.xml";
  static constexpr auto FILENAME_UNITS = "units.xml";

  static constexpr auto FIELD_FACTIONS = "factions";
  static constexpr auto FIELD_FACTIONS_SECTION_FACTION = "faction";

  static constexpr auto FIELD_BUILDINGS = "buildings";
  static constexpr auto FIELD_BUILDINGS_NAME_ATTRIBUTE = "name";
  static constexpr auto FIELD_BUILDINGS_SECTION_FACTION = "faction";
  static constexpr auto FIELD_BUILDINGS_SECTION_BUILDING = "building";

  static constexpr auto FIELD_UNITS = "units";
  static constexpr auto FIELD_UNITS_NAME_ATTRIBUTE = "name";
  static constexpr auto FIELD_UNITS_SECTION_FACTION = "faction";
  static constexpr auto FIELD_UNITS_SECTION_UNIT = "unit";

  static constexpr auto SUBFIELD_BASE_INFO = "info";
  static constexpr auto SUBFIELD_BASE_COLLIDER = "collider";
  static constexpr auto SUBFIELD_BASE_REQUIREMENTS = "requirements";
  static constexpr auto SUBFIELD_BASE_INFO_NAME_ATTRIBUTE = "name";
  static constexpr auto SUBFIELD_BASE_INFO_TYPE_ATTRIBUTE = "type";
  static constexpr auto SUBFIELD_BASE_INFO_DESCRIPTION_ATTRIBUTE = "description";
  static constexpr auto SUBFIELD_BASE_INFO_ANIMATION_ATTRIBUTE = "animation";

  static constexpr auto SUBFIELD_ATTACK_BASE_ATTACK = "attack";

  static constexpr auto SUBFIELD_UNIT_ATTRIBUTES = "attributes";
  static constexpr auto SUBFIELD_UNIT_MOVE_SPEED = "move_speed";
  static constexpr auto SUBFIELD_UNIT_ARMOR = "armor";

  static constexpr auto SUBFIELD_BUILDING_UNITS_TRAINED = "trained";
  static constexpr auto SUBFIELD_BUILDING_UNITS_TRAINED_TAG = "unit";
  static constexpr auto SUBFIELD_UNIT_BUILDINGS_BUILT = "built";
  static constexpr auto SUBFIELD_UNIT_BUILDINGS_BUILT_TAG = "building";

  static constexpr auto TAG_NAME_UNIT = "UNIT_";
  static constexpr auto TAG_NAME_BUILDING = "BUILDING_";

  bool EntityDataLookup::loadFromFile(const gengine2d::string_t& filename)
  {
    std::ifstream in(filename);
    if(in.is_open()) {
      return loadFromStream(in);
    }

    return false;
  }

  bool EntityDataLookup::loadFromStream(std::istream& stream)
  {
    gengine2d::archive_t dataArchive;
    if(dataArchive.open(stream) == gengine2d::archive_t::EOpenResult::EOR_SUCCESS &&
       dataArchive.fileExists(FILENAME_FACTIONS) &&
       dataArchive.fileExists(FILENAME_BUILDINGS) &&
       dataArchive.fileExists(FILENAME_UNITS)) {
      m_buildingLookup.clear();
      m_unitLookup.clear();

      auto factionsList = loadFactionList(*dataArchive.getFile(FILENAME_FACTIONS)->getInputStream());
      if(factionsList.empty()) {
        return false;
      }

      if(loadBuildings(dataArchive.getFile(FILENAME_BUILDINGS), factionsList) &&
         loadUnits(dataArchive.getFile(FILENAME_UNITS), factionsList)) {
        return true;
      } else {
        m_buildingLookup.clear();
        m_unitLookup.clear();
      }
    }

    return false;
  }

  gengine2d::vector_t<gengine2d::string_t> EntityDataLookup::loadFactionList(const gengine2d::string_t& filename)
  {
    gengine2d::archive_t dataArchive;
    if(dataArchive.open(filename) == gengine2d::archive_t::EOpenResult::EOR_SUCCESS &&
       dataArchive.fileExists(FILENAME_FACTIONS)) {
      return loadFactionList(*dataArchive.getFile(FILENAME_FACTIONS)->getInputStream());
    }

    return {};
  }

  gengine2d::vector_t<gengine2d::string_t> EntityDataLookup::loadFactionList(std::istream& stream)
  {
    gengine2d::vector_t<gengine2d::string_t> ret;
    gengine2d::fast_unordered_set_t<gengine2d::string_t> factions;

    tinyxml2::XMLDocument doc;
    auto factionData = gengine2d::stream_utils::copyStreamToArray(stream);

    if(doc.Parse(reinterpret_cast<const char*>(factionData.data()),
                 factionData.size()) == tinyxml2::XMLError::XML_SUCCESS) {
      auto* factionsSection = doc.FirstChildElement(FIELD_FACTIONS);
      if(factionsSection) {
        for(auto* factionSection = factionsSection->FirstChildElement(FIELD_FACTIONS_SECTION_FACTION);
            factionSection != nullptr;
            factionSection = factionSection->NextSiblingElement(FIELD_FACTIONS_SECTION_FACTION)) {
          auto* factionText = factionSection->GetText();
          if(factionText) {
            if(factions.find(factionText) == factions.end()) {
              factions.emplace(factionText);
              ret.push_back(factionText);
            }
          }
        }
      }
    }

    return ret;
  }

  bool EntityDataLookup::exists(const gengine2d::string_t& name) const
  {
    return m_buildingLookup.exists(name) || m_unitLookup.exists(name);
  }

  const BuildingData& EntityDataLookup::getBuildingData(const gengine2d::string_t& name) const
  {
    return m_buildingLookup.getResource(name);
  }

  const UnitData& EntityDataLookup::getUnitData(const gengine2d::string_t& name) const
  {
    return m_unitLookup.getResource(name);
  }

  bool EntityDataLookup::loadBuildings(gengine2d::archive_file_t* buildingDataFile,
                                 const gengine2d::vector_t<gengine2d::string_t>& factionList)
  {
    tinyxml2::XMLDocument doc;
    auto buildingBinData = gengine2d::stream_utils::copyStreamToArray(*buildingDataFile->getInputStream());

    if(doc.Parse(reinterpret_cast<const char*>(buildingBinData.data()),
                 buildingBinData.size()) == tinyxml2::XMLError::XML_SUCCESS) {
      auto* buildingsSection = doc.FirstChildElement(FIELD_BUILDINGS);
      if(buildingsSection) {
        for(auto* factionSection = buildingsSection->FirstChildElement(FIELD_BUILDINGS_SECTION_FACTION);
            factionSection != nullptr;
            factionSection = factionSection->NextSiblingElement(FIELD_BUILDINGS_SECTION_FACTION)) {
          auto* factionName = factionSection->Attribute(FIELD_BUILDINGS_NAME_ATTRIBUTE);
          if(!factionName) {
            m_buildingLookup.clear();
            return false;
          }

          if(std::find(factionList.begin(), factionList.end(), factionName) == factionList.end()) {
            m_buildingLookup.clear();
            return false;
          }

          for(auto* buildingSection = factionSection->FirstChildElement(FIELD_BUILDINGS_SECTION_BUILDING);
              buildingSection != nullptr;
              buildingSection = buildingSection->NextSiblingElement(FIELD_BUILDINGS_SECTION_BUILDING)) {
            auto buildingData = loadBuilding(factionName, buildingSection);
            if(buildingData) {
              m_buildingLookup.addResource(std::move(buildingData.value().first),
                                           std::move(buildingData.value().second));
            } else {
              m_buildingLookup.clear();
              return false;
            }
          }
        }

        return true;
      }
    }

    m_buildingLookup.clear();
    return false;
  }

  bool EntityDataLookup::loadUnits(gengine2d::archive_file_t* unitDataFile,
                             const gengine2d::vector_t<gengine2d::string_t>& factionList)
  {
    tinyxml2::XMLDocument doc;
    auto unitBinData = gengine2d::stream_utils::copyStreamToArray(*unitDataFile->getInputStream());

    if(doc.Parse(reinterpret_cast<const char*>(unitBinData.data()),
                 unitBinData.size()) == tinyxml2::XMLError::XML_SUCCESS) {
      auto* unitsSection = doc.FirstChildElement(FIELD_UNITS);
      if(unitsSection) {
        for(auto* factionSection = unitsSection->FirstChildElement(FIELD_UNITS_SECTION_FACTION);
            factionSection != nullptr;
            factionSection = factionSection->NextSiblingElement(FIELD_UNITS_SECTION_FACTION)) {
          auto* factionName = factionSection->Attribute(FIELD_UNITS_NAME_ATTRIBUTE) ;
          if(!factionName) {
            m_unitLookup.clear();
            return false;
          }

          if(std::find(factionList.begin(), factionList.end(), factionName) == factionList.end()) {
            m_unitLookup.clear();
            return false;
          }

          for(auto* unitSection = factionSection->FirstChildElement(FIELD_UNITS_SECTION_UNIT);
              unitSection != nullptr;
              unitSection = unitSection->NextSiblingElement(FIELD_UNITS_SECTION_UNIT)) {
            auto unitData = loadUnit(factionName, unitSection);
            if(unitData) {
              m_unitLookup.addResource(std::move(unitData.value().first),
                                       std::move(unitData.value().second));
            } else {
              m_unitLookup.clear();
              return false;
            }
          }
        }

        return true;
      }
    }

    m_unitLookup.clear();
    return false;
  }

  std::optional<std::pair<gengine2d::string_t, BuildingData>> EntityDataLookup::loadBuilding(const gengine2d::string_t& faction,
                                                                                       tinyxml2::XMLElement* buildingElement)
  {
    auto buildingTypeLoader = [](auto* type) {
      auto buildingType = gengine2d::string_t(type);
      if(buildingType == "castle") {
        return BuildingData::EBuildingType::BT_TYPE_CASTLE;
      } else if(buildingType == "trainer") {
        return BuildingData::EBuildingType::BT_TYPE_TRAINER;
      } else if(buildingType == "tower") {
        return BuildingData::EBuildingType::BT_TYPE_TOWER;
      } else if(buildingType == "wall") {
        return BuildingData::EBuildingType::BT_TYPE_WALL;
      }

      return BuildingData::EBuildingType::BT_TYPE_OTHER;
    };

    // FIXME: Do bounds checking for loading of datas
    auto* infoSection = buildingElement->FirstChildElement(SUBFIELD_BASE_INFO);
    auto* colliderSection = buildingElement->FirstChildElement(SUBFIELD_BASE_COLLIDER);
    auto* requirementsSection = buildingElement->FirstChildElement(SUBFIELD_BASE_REQUIREMENTS);
    auto* attackSection = buildingElement->FirstChildElement(SUBFIELD_ATTACK_BASE_ATTACK);
    auto* trainedSection = buildingElement->FirstChildElement(SUBFIELD_BUILDING_UNITS_TRAINED);
    if(infoSection && colliderSection && requirementsSection && attackSection && trainedSection) {
      BuildingData data;

      auto* name = infoSection->Attribute(SUBFIELD_BASE_INFO_NAME_ATTRIBUTE);
      auto* type = infoSection->Attribute(SUBFIELD_BASE_INFO_TYPE_ATTRIBUTE);
      auto* description = infoSection->Attribute(SUBFIELD_BASE_INFO_DESCRIPTION_ATTRIBUTE);
      auto* animation = infoSection->Attribute(SUBFIELD_BASE_INFO_ANIMATION_ATTRIBUTE);

      if(!name || !type || !description || !animation) {
        return std::nullopt;
      }

      auto dataName = gengine2d::string_t(TAG_NAME_BUILDING) +
                      gengine2d::toLower(faction) +
                      "_" +
                      gengine2d::toLower(name);
      data.name = name;
      data.description = description;
      data.type = buildingTypeLoader(type);

      if(!loadColliderData(data, colliderSection)) {
        return std::nullopt;
      }

      if(!loadRequirementData(data, requirementsSection)) {
        return std::nullopt;
      }

      if(!loadAttackData(data, attackSection)) {
        return std::nullopt;
      }

      gengine2d::vector_t<gengine2d::string_t> unitsTrained;
      for(auto* unitTrainedSection = trainedSection->FirstChildElement(SUBFIELD_BUILDING_UNITS_TRAINED_TAG);
          unitTrainedSection != nullptr;
          unitTrainedSection = unitTrainedSection->NextSiblingElement(SUBFIELD_BUILDING_UNITS_TRAINED_TAG)) {
        auto* unitId = unitTrainedSection->GetText();
        if(unitId) {
          unitsTrained.push_back(unitId);
        }
      }

      data.unitsTrained = std::move(unitsTrained);

      return std::make_pair(dataName, data);
    }

    return std::nullopt;
  }

  std::optional<std::pair<gengine2d::string_t, UnitData>> EntityDataLookup::loadUnit(const gengine2d::string_t& faction,
                                                                               tinyxml2::XMLElement* unitElement)
  {
    auto unitTypeLoader = [](auto* type) {
      auto unitType = gengine2d::string_t(type);
      if(unitType == "soldier" ) {
        return UnitData::EUnitType::UT_SOLDIER;
      } else if(unitType == "worker|soldier" ||
                unitType == "soldier|worker" ||
                unitType == "worker | soldier" ||
                unitType == "soldier | worker") {
        return static_cast<UnitData::EUnitType>(UnitData::EUnitType::UT_SOLDIER | UnitData::EUnitType::UT_WORKER);
      }

      return UnitData::EUnitType::UT_WORKER;
    };

    // FIXME: Do bounds checking for loading of datas
    auto* infoSection = unitElement->FirstChildElement(SUBFIELD_BASE_INFO);
    auto* colliderSection = unitElement->FirstChildElement(SUBFIELD_BASE_COLLIDER);
    auto* requirementsSection = unitElement->FirstChildElement(SUBFIELD_BASE_REQUIREMENTS);
    auto* attributesSection = unitElement->FirstChildElement(SUBFIELD_UNIT_ATTRIBUTES);
    if(infoSection && colliderSection && requirementsSection) {
      UnitData data;

      auto* name = infoSection->Attribute(SUBFIELD_BASE_INFO_NAME_ATTRIBUTE);
      auto* type = infoSection->Attribute(SUBFIELD_BASE_INFO_TYPE_ATTRIBUTE);
      auto* description = infoSection->Attribute(SUBFIELD_BASE_INFO_DESCRIPTION_ATTRIBUTE);
      auto* animation = infoSection->Attribute(SUBFIELD_BASE_INFO_ANIMATION_ATTRIBUTE);
      if(!name || !type || !description || !animation) {
        return std::nullopt;
      }

      auto dataName = gengine2d::string_t(TAG_NAME_UNIT) +
                      gengine2d::toLower(faction) +
                      "_" +
                      gengine2d::toLower(name);
      data.name = name;
      data.description = description;
      data.unitType = unitTypeLoader(type);

      if(!loadColliderData(data, colliderSection)) {
        return std::nullopt;
      }

      if(!loadRequirementData(data, requirementsSection)) {
        return std::nullopt;
      }

      auto* moveSpeed = attributesSection->Attribute(SUBFIELD_UNIT_MOVE_SPEED);
      auto* armor = attributesSection->Attribute(SUBFIELD_UNIT_ARMOR);
      if(!moveSpeed || !armor ||
         !gengine2d::type_determine_utils::isFloatingPoint(moveSpeed) ||
         !gengine2d::type_determine_utils::isFloatingPoint(armor)) {
        return std::nullopt;
      }
      data.moveSpeed = gengine2d::converter_utils::stringTo<float>(moveSpeed);
      data.armor = gengine2d::converter_utils::stringTo<float>(armor);

      if(data.unitType & UnitData::EUnitType::UT_SOLDIER) {
        auto* attackSection = unitElement->FirstChildElement(SUBFIELD_ATTACK_BASE_ATTACK);
        if(!loadAttackData(data, attackSection)) {
          return std::nullopt;
        }
      }

      if(data.unitType & UnitData::EUnitType::UT_WORKER) {
        auto* builtSection = unitElement->FirstChildElement(SUBFIELD_UNIT_BUILDINGS_BUILT);

        gengine2d::vector_t<gengine2d::string_t> buildingsBuilt;
        for(auto* buildingsBuiltSection = builtSection->FirstChildElement(SUBFIELD_UNIT_BUILDINGS_BUILT_TAG);
            buildingsBuiltSection != nullptr;
            buildingsBuiltSection = buildingsBuiltSection->NextSiblingElement(SUBFIELD_UNIT_BUILDINGS_BUILT_TAG)) {
          auto* buildingId = buildingsBuiltSection->GetText();
          if(buildingId) {
            buildingsBuilt.push_back(buildingId);
          }
        }

        data.buildingsBuilt = std::move(buildingsBuilt);
      }

      return std::make_pair(dataName, data);
    }

    return std::nullopt;
  }

  bool EntityDataLookup::loadColliderData(BaseData& data, const tinyxml2::XMLElement* colliderSection)
  {
    auto* colliderX = colliderSection->Attribute("x");
    auto* colliderY = colliderSection->Attribute("y");
    auto* colliderRadius = colliderSection->Attribute("radius");
    if(!colliderX || !colliderY || !colliderRadius ||
       !gengine2d::type_determine_utils::isFloatingPoint(colliderX) ||
       !gengine2d::type_determine_utils::isFloatingPoint(colliderY) ||
       !gengine2d::type_determine_utils::isFloatingPoint(colliderRadius)){
      return false;
    }

    data.colliderOffsetX = gengine2d::converter_utils::stringTo<float>(colliderX);
    data.colliderOffsetY = gengine2d::converter_utils::stringTo<float>(colliderY);
    data.colliderRadius = gengine2d::converter_utils::stringTo<float>(colliderRadius);

    return true;
  }

  bool EntityDataLookup::loadRequirementData(BaseData& data, const tinyxml2::XMLElement* requirementsSection)
  {
    auto* requirementFood = requirementsSection->Attribute("food");
    auto* requirementGold = requirementsSection->Attribute("gold");
    auto* requirementStone = requirementsSection->Attribute("stone");
    auto* requirementWood = requirementsSection->Attribute("wood");
    if(!requirementFood || !requirementGold || !requirementStone || !requirementWood ||
       !gengine2d::type_determine_utils::isInteger(requirementFood) ||
       !gengine2d::type_determine_utils::isInteger(requirementGold) ||
       !gengine2d::type_determine_utils::isInteger(requirementStone) ||
       !gengine2d::type_determine_utils::isInteger(requirementWood)) {
      return false;
    }

    data.creationRequirements.requirements[GlobalResource::GR_RES_FOOD] = gengine2d::general_utils::clamp(gengine2d::converter_utils::stringTo<int>(requirementFood), 0, 9999);
    data.creationRequirements.requirements[GlobalResource::GR_RES_GOLD] = gengine2d::general_utils::clamp(gengine2d::converter_utils::stringTo<int>(requirementGold), 0, 9999);
    data.creationRequirements.requirements[GlobalResource::GR_RES_STONE] = gengine2d::general_utils::clamp(gengine2d::converter_utils::stringTo<int>(requirementStone), 0, 9999);
    data.creationRequirements.requirements[GlobalResource::GR_RES_WOOD] = gengine2d::general_utils::clamp(gengine2d::converter_utils::stringTo<int>(requirementWood), 0, 9999);

    return true;
  }

  bool EntityDataLookup::loadAttackData(AttackData& data, const tinyxml2::XMLElement* attackSection)
  {
    auto* attackDamage = attackSection->Attribute("damage");
    auto* attackDamageDropOff = attackSection->Attribute("damage_dropoff");
    auto* attackDamageMin = attackSection->Attribute("damage_min");
    auto* attackDamageRadius = attackSection->Attribute("damage_radius");
    auto* attackRange = attackSection->Attribute("range");
    auto* attackSpeed = attackSection->Attribute("speed");
    if(!attackDamage || !attackDamageDropOff || !attackDamageMin || !attackDamageRadius ||
       !attackRange || !attackSpeed ||
       !gengine2d::type_determine_utils::isFloatingPoint(attackDamage) ||
       !gengine2d::type_determine_utils::isFloatingPoint(attackDamageDropOff) ||
       !gengine2d::type_determine_utils::isFloatingPoint(attackDamageMin) ||
       !gengine2d::type_determine_utils::isFloatingPoint(attackDamageRadius) ||
       !gengine2d::type_determine_utils::isFloatingPoint(attackRange) ||
       !gengine2d::type_determine_utils::isFloatingPoint(attackSpeed)) {
      return false;
    }

    data.attackDamage = gengine2d::converter_utils::stringTo<float>(attackDamage);
    data.attackDamageDropoff = gengine2d::converter_utils::stringTo<float>(attackDamageDropOff);
    data.attackDamageMin = gengine2d::converter_utils::stringTo<float>(attackDamageMin);
    data.attackDamageRadius = gengine2d::converter_utils::stringTo<float>(attackDamageRadius);
    data.attackRange = gengine2d::converter_utils::stringTo<float>(attackRange);
    data.attackSpeed = gengine2d::converter_utils::stringTo<float>(attackSpeed);

    return true;
  }
}
