#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <iosfwd>
#include "lookup/texturelookup.h"
#include "controlmap.h"
#include "inputevent.h"
#include "inputstate.h"
#include "game_internal_types.h"
#include "type.h"

namespace gengine2d
{
  class GameState
  {
  public:
    GameState(std::initializer_list<game_internal::Renderer::Camera2D*> cameras,
              std::initializer_list<std::tuple<string_t, bool, unsigned int, std::function<void(GameState*)>>> initialKeyMapped);
    virtual ~GameState();

    virtual bool apply(vector_t<string_t>) { return false; }
    // NOTE: init is called after the state has been added to the game engine's state list
    virtual bool init() { return true; }

    virtual string_t getDebugName() const = 0;

    virtual void draw(float alpha) = 0;
    virtual void update(float dt) = 0;
    virtual void interpolate(float dt, float accumulator) = 0;
    virtual void handleInput(const InputEvent& event) = 0;

    virtual vector_t<string_t> getCommandList() const { return {}; }

    void setMessage(string_t msg);
    bool hasMessage() const;
    void popMessage();
    const string_t& getMessage() const;

  protected:
    bool addFont(const string_t& name, const string_t& filename);
    bool addFont(const string_t& name, std::istream& stream);

    bool addTexture(const string_t& name, const string_t& filename);
    bool addTexture(const string_t& name, std::istream& stream);
    [[deprecated]] bool addTextureFromPack(const string_t& textureFilename,
                                           const string_t& texturePackFilename);

    bool addTexturesToLookup(const string_t& filename);
    bool addTexturesToLookup(const string_t& filename, std::istream& stream);
    bool addTextureFromPackToLookup(const string_t& textureFilename,
                                    const string_t& texturePackFilename);

//    bool addSound(const string_t& name, const string_t& filename);
//    bool addSound(const string_t& name, std::istream& stream);
//    bool loadSoundFromPack(const string_t& soundName, const string_t& soundPackFilename);

//    bool addMusic(const string_t& name, const string_t& filename);
//    bool addMusic(const string_t& name, std::istream& stream);
//    bool loadMusicFromPack(const string_t& musicName, const string_t& musicPackFilename);

    const ControlMap& getControlMap() const { return m_controlMap; }
    const InputState& getInputState() const { return m_inputState; }

    TextureLookup& getTextureLookup() { return m_textureLookup; }
    const TextureLookup& getTextureLookup() const { return m_textureLookup; }

  private:
    ControlMap                                    m_controlMap;
    InputState                                    m_inputState;
    vector_t<game_internal::Renderer::Camera2D*>  m_mousePositionOnviews;

    TextureLookup                                 m_textureLookup;
    fast_unordered_set_t<string_t>                m_addedFonts;
    fast_unordered_set_t<string_t>                m_addedTextures;

    vector_t<string_t>                            m_messages;
//    fixed_unordered_set_t<string_t> m_addedSounds;
//    fixed_unordered_set_t<string_t> m_addedMusics;
  };
}

#endif // GAMESTATE_H
