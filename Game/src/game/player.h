#ifndef PLAYER_H
#define PLAYER_H

namespace strat
{
  class Player
  {
  public:
    enum EPlayerNumber : unsigned int
    {
      PN_PLAYER1 = 0,
      PN_PLAYER2,
      PN_PLAYER3,
      PN_PLAYER4,
      PN_PLAYER5,
      PN_PLAYER6,
      PN_PLAYER7,
      PN_PLAYER8,
      PN_PLAYER9,
      PN_PLAYER10,
      PN_NEUTRAL,
      PN_NATURE,
      PN_MAX_PLAYER_NUMBER
    };
  };
}

#endif // PLAYER_H
