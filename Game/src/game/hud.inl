#include "hud.h"

namespace strat
{
  template<class Func>
  bool HUD::forEachButton(Func&& func)
  {
    for(const auto& [btnAction, btnTexture, btnEnabled, btnTooltip, btnTooltipCallBack, btnCallBack] : m_selectedEntityButtonActions) {
      if(std::forward<Func>(func)(btnAction,
                                  btnTexture,
                                  btnEnabled,
                                  btnTooltip,
                                  btnTooltipCallBack,
                                  btnCallBack)) {
        return true;
      }
    }

    return false;
  }
}
