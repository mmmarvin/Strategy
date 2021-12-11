#ifndef SFMLRENDERER_H
#define SFMLRENDERER_H

#include "../../color.h"
#include "../../rect.h"
#include "../../vertex2.h"
#include "../../type.h"
#include "sfml_types.h"

namespace gengine2d
{
  class Console;
  class Game;
  struct Shape2DStyle;
  struct Text2DStyle;
namespace render
{
  class SFMLRenderer
  {
    struct SFMLEmptyType {};

    class SFMLFont
    {
      explicit SFMLFont(const sf::Font& font,
                        list_t<vector_t<unsigned char>>::iterator fontDataIt);

      friend class SFMLRenderer;

    public:
      Vector2u getDimension(unsigned int characterSize) const;

    private:
      sf::Font                                  m_font;
      list_t<vector_t<unsigned char>>::iterator m_fontDataIt;
    };

    class SFMLTexture
    {
    public:
      SFMLTexture() {}

    private:
      SFMLTexture(sf::Texture texture);


      friend class SFMLRenderer;

      const sf::Texture& getTexture() const;

      sf::Texture m_texture;
    };

    class SFMLRenderTexture
    {
    public:
      SFMLRenderTexture() {}

      bool create(unsigned int width, unsigned int height);
      void clear(const Color& color = Color(0, 0, 0, 255));
      void display();

    private:
      friend class SFMLRenderer;

      const sf::Texture& getTexture() const;

      sf::RenderTexture m_texture;
    };

    class SFMLCamera2D
    {
    public:
      SFMLCamera2D();
      SFMLCamera2D(const Vector2<position_point_2d_t>& center,
                   const Vector2<position_point_2d_t>& size);

      void setCenter(const Vector2<position_point_2d_t>& center);
      void setSize(const Vector2<position_point_2d_t>& size);
      void zoom(float factor);

      Vector2<position_point_2d_t> getCenter() const;
      Vector2<position_point_2d_t> getSize() const;
      Vector2<position_point_2d_t> getPositionInTermsOf(const Vector2<position_point_2d_t>& position) const;

    private:
      friend class SFMLRenderer;

      SFMLCamera2D(const sf::View& view);

      sf::View m_view;
    };

    class SFMLAnimation2D
    {
    public:
      explicit SFMLAnimation2D(std::initializer_list<std::pair<Rectf, float>> frames,
                             const SFMLTexture* texture);

      const Rectf& getFrame(std::size_t index) const;
      const float& getFrameDuration(std::size_t index) const;
      const SFMLTexture* getTexture() const;

    private:
      const std::pair<Rectf, float>& getFrameImpl(std::size_t index) const;

      vector_t<std::pair<Rectf, float>> m_frames;
      const SFMLTexture*                m_texture;
    };

    using position_modifier_function_type = std::function<Vector2<position_point_2d_t>(const Vector2<position_point_2d_t>&)>;

  public:
    using Font = SFMLFont;
    using Texture = SFMLTexture;
    using RenderableTexture = SFMLRenderTexture;

    using Camera2D = SFMLCamera2D;
    using Animation2D = SFMLAnimation2D;

    using SceneNode = SFMLEmptyType;

    SFMLRenderer();

    std::pair<bool, Font> loadFont(const string_t& name, const string_t& filename);
    std::pair<bool, Font> loadFont(const string_t& name, vector_t<unsigned char> data);
    std::pair<bool, Font> loadFont(const string_t& name, std::istream& stream);
    void removeFont(const Font& font);

    std::pair<bool, Texture> loadTexture(const string_t& name, const string_t& filename);
    std::pair<bool, Texture> loadTexture(const string_t& name, vector_t<unsigned char> data);
    std::pair<bool, Texture> loadTexture(const string_t& name, std::istream& stream);
    void removeTexture(const Texture& texture);

    Vector2<position_point_2d_t> calculate2DTextBounds(const string_t& text,
                                                       const Text2DStyle& textStyle);
    Vector2<position_point_2d_t> draw2DText(const Vector2<position_point_2d_t>& position,
                                            string_t text,
                                            const Text2DStyle& textStyle);
    void draw2DLine(const Vector2<position_point_2d_t>& start,
                    const Vector2<position_point_2d_t>& end,
                    const Color& color);
    void draw2DLines(const Vertex2* vertices,
                     std::size_t verticesCount);
    void draw2DRect(const Rect<position_point_2d_t>& rect,
                    const Shape2DStyle& style);
    void draw2DRect(const Rect<position_point_2d_t>& rect,
                    const Shape2DStyle& style,
                    const position_modifier_function_type& positionModifierFunc);
    void draw2DCircle(const Vector2<position_point_2d_t>& position,
                      float radius,
                      const Shape2DStyle& style);
    void draw2DCircle(const Vector2<position_point_2d_t>& position,
                      float radius,
                      const Shape2DStyle& style,
                      const position_modifier_function_type& positionModifierFunc);
    void draw2DShape(const Vertex2* vertices,
                     std::size_t verticesCount,
                     const Texture* texture);

    static void ImGuiImage(const RenderableTexture& texture,
                           const Rect<position_point_2d_t>& rect);
    static void ImGuiImage(const Texture& texture,
                           const Vector2<position_point_2d_t>& textureSize);
    static void ImGuiImage(const Texture& texture,
                           const Vector2<position_point_2d_t>& textureSize,
                           const Rect<position_point_2d_t>& rect);
    static bool ImGuiImageButton(const Texture& texture);
    static bool ImGuiImageButton(const Texture& texture, const Vector2<position_point_2d_t>& buttonSize);

    void ImGuiHandleInit();
    void ImGuiHandleUpdate(float timeStep);
    void ImGuiHandleRender();

    void set2DCamera(const Camera2D& camera);
    Camera2D get2DCamera() const;
    Camera2D get2DGUICamera() const;
    void reset2DCamera();

    void setRenderTargetToTexture(RenderableTexture& texture);
    void resetRenderTarget();

    Vector2u getScreenSize() const;

  private:
    friend class ::gengine2d::Game;

    sf::Text generateTextFromStyles(const Text2DStyle& style);

    bool init(Console& console,
              string_t title,
              int width,
              int height,
              int bits,
              bool fullscreen,
              bool vsync,
              bool stencilBuffer,
              void* eventReceiver);
    void reset();

    void clear();
    void display();
    void drawScene() {}

    bool run();

    list_t<vector_t<unsigned char>>   m_fontData;
    sf::RenderWindow                  m_renderWindow;
    sf::RenderTarget*                 m_renderTarget;
  };
}
}

#endif // SFMLRENDERER_H
