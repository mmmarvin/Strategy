#ifndef ATTACKBASESYSTEM_H
#define ATTACKBASESYSTEM_H

#include "../../entitymanager.h"

namespace strat
{
namespace system
{
  class AttackBaseSystem
  {
  public:
    explicit AttackBaseSystem(EntityManager& entityManager,
                              const Map& map,
                              FogManager& fogs);

    void performAttack(const anax::Entity& entity,
                       const anax::Entity& targetEntity,
                       float attackDamage);
    template<class Func>
    bool trySearchForTarget(const anax::Entity& entity,
                            Func&& func);

  private:
    void trySelfDefence(const anax::Entity& attacker,
                        const anax::Entity& entity);
    void performSelfDefenceFlee(const anax::Entity& attacker,
                                const anax::Entity& entity);
    void performSelfDefenceAttack(const anax::Entity& attacker,
                                  const anax::Entity& entity);

  protected:
    EntityManager*            m_entityManager;
    const Map*                m_map;
    FogManager*               m_fogs;
  };
}
}

#include "attackbasesystem.inl"

#endif // ATTACKBASESYSTEM_H
