#ifndef GAME_DEF_H
#define GAME_DEF_H

#include <cinttypes>

namespace strat
{
  static constexpr float TILE_SIZE = 64.f;
  static constexpr float UNIT_SIZE = TILE_SIZE / 8.f;

  static constexpr float CAMERA_SIZE_X = 1024;
  static constexpr float CAMERA_SIZE_Y = 768;
  static constexpr float CAMERA_SIZE_X_MIN = CAMERA_SIZE_X - 200.f;
  static constexpr float CAMERA_SIZE_X_MAX = CAMERA_SIZE_X + 200.f;

  static constexpr float MINIMAP_UNIT_SIZE = UNIT_SIZE / 1.5f;
  static constexpr float MINIMAP_BUILDING_SIZE = UNIT_SIZE;
  static constexpr float MINIMAP_RESOURCE_SIZE = UNIT_SIZE;
  static constexpr float MINIMAP_SIZE = 150.f;

  static constexpr long int RESOURCE_MAX_VALUE = 9999999;

  static constexpr float HUD_UPPER_HEIGHT = 10.f;
  static constexpr float HUD_LOWER_HEIGHT = 137.f;
  static constexpr float HUD_LOWER_WIDTH = 565.f;
  static constexpr float HUD_LOWER_BUTTON_AREA_WIDTH = 200.f;
  static constexpr unsigned int HUD_MAX_NUMBER_OF_BUTTONS = 12;
  static constexpr float HUD_RESOURCE_ICON_SIZE = 16.f;
  static constexpr float HUD_RESOURCE_PORTION_SIZE = 50.f;
  static constexpr float HUD_MENU_BUTTON_SIZE = 50.f;
  static constexpr float HUD_ENTITY_IMAGE_SIZE = 64.f;
  static constexpr float HUD_ENTITY_MULTIPLE_IMAGE_SIZE = 32.f;
  static constexpr std::size_t HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN = ((HUD_LOWER_WIDTH - HUD_LOWER_BUTTON_AREA_WIDTH) / (HUD_ENTITY_MULTIPLE_IMAGE_SIZE + 15.f)) - 1;
  static constexpr std::size_t HUD_MAX_ENTITY_MULTIPLE_IMAGES = HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN * 3;
  static constexpr float HUD_ENTITY_TRAIN_IMAGE_SIZE = 32.f;
  static constexpr float HUD_ENTITY_TRAIN_PROGRESS_WIDTH = 32.f;
  static constexpr std::size_t HUD_MAX_TRAIN_ICON_NUMBER = 6;
  static constexpr float HUD_BUTTON_SIZE = 32.f;
  static constexpr std::size_t HUD_BUTTON_MAX_BUTTONS_PER_COLUMN = 4;
  static constexpr std::size_t HUD_BUTTON_MAX_BUTTONS = 12;

  static constexpr float HUD_CHAT_WIDTH = 300;
  static constexpr float HUD_CHAT_HEIGHT = 150;
  static constexpr float HUD_CHAT_INPUT_HEIGHT = 30;
}

#endif // GAME_DEF_H
