#include "sfmlrenderer.h"

#include <fstream>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../../styles/shape2dstyle.h"
#include "../../styles/text2dstyle.h"
#include "../../utility/general_utility.h"
#include "../../utility/stream_utility.h"
#include "../../utility/vector_utility.h"
#include "../../console.h"
#include "../../logger.h"

namespace gengine2d
{
namespace render
{
namespace
{
  using anon_position_modifier_func_type = std::function<Vector2<position_point_2d_t>(const Vector2<position_point_2d_t>&)>;

  void generateRectangle(const Rect<position_point_2d_t>& rect,
                         array_t<Vertex2, 6>& outVertices,
                         const anon_position_modifier_func_type& positionModifierFunc)
  {
    auto l = rect.left,
         t = rect.top,
         w = rect.width,
         h = rect.height;

    outVertices[0].position = Vector2f(l, t);
    outVertices[1].position = outVertices[0].position + Vector2f(w, 0.f);
    outVertices[2].position = outVertices[0].position + Vector2f(0.f, h);
    outVertices[3].position = outVertices[2].position;
    outVertices[4].position = outVertices[0].position + Vector2f(w, h);
    outVertices[5].position = outVertices[1].position;

    outVertices[0].position = positionModifierFunc(outVertices[0].position);
    outVertices[1].position = positionModifierFunc(outVertices[1].position);
    outVertices[2].position = positionModifierFunc(outVertices[2].position);
    outVertices[3].position = positionModifierFunc(outVertices[3].position);
    outVertices[4].position = positionModifierFunc(outVertices[4].position);
    outVertices[5].position = positionModifierFunc(outVertices[5].position);
  }

  array_t<Vertex2, 6> generateRectangle(const Rect<position_point_2d_t>& rect,
                                        const anon_position_modifier_func_type& positionModifierFunc)
  {
    array_t<Vertex2, 6> v;
    generateRectangle(rect, v, positionModifierFunc);

    return v;
  }
}
  /// Font
  SFMLRenderer::SFMLFont::SFMLFont(const sf::Font& font,
                     list_t<vector_t<unsigned char>>::iterator fontDataIt) :
    m_font(font),
    m_fontDataIt(fontDataIt)
  {
  }

  Vector2u SFMLRenderer::SFMLFont::getDimension(unsigned int characterSize) const
  {
    return m_font.getTexture(characterSize).getSize();
  }

  /// Texture
  SFMLRenderer::SFMLTexture::SFMLTexture(sf::Texture texture) :
    m_texture(texture)
  {
  }

  const sf::Texture& SFMLRenderer::SFMLTexture::getTexture() const
  {
    return m_texture;
  }

  /// RenderTexture
  bool SFMLRenderer::SFMLRenderTexture::create(unsigned int width, unsigned int height)
  {
    return m_texture.create(width, height);
  }

  void SFMLRenderer::SFMLRenderTexture::clear(const Color& color)
  {
    m_texture.clear(color);
  }

  void SFMLRenderer::SFMLRenderTexture::display()
  {
    m_texture.display();
  }

  const sf::Texture& SFMLRenderer::SFMLRenderTexture::getTexture() const
  {
    return m_texture.getTexture();
  }

  /// Camera2D
  SFMLRenderer::SFMLCamera2D::SFMLCamera2D()
  {
  }

  SFMLRenderer::SFMLCamera2D::SFMLCamera2D(const Vector2<position_point_2d_t>& center,
                                           const Vector2<position_point_2d_t>& size) :
    m_view(center, size)
  {
  }

  void SFMLRenderer::SFMLCamera2D::setCenter(const Vector2<position_point_2d_t>& center)
  {
    m_view.setCenter(center);
  }

  void SFMLRenderer::SFMLCamera2D::setSize(const Vector2<position_point_2d_t>& size)
  {
    m_view.setSize(size);
  }

  void SFMLRenderer::SFMLCamera2D::zoom(float factor)
  {
    m_view.zoom(factor);
  }

  Vector2<position_point_2d_t> SFMLRenderer::SFMLCamera2D::getCenter() const
  {
    return m_view.getCenter();
  }

  Vector2<position_point_2d_t> SFMLRenderer::SFMLCamera2D::getSize() const
  {
    return m_view.getSize();
  }

  Vector2<position_point_2d_t> SFMLRenderer::SFMLCamera2D::getPositionInTermsOf(const Vector2<position_point_2d_t>& position) const
  {
    auto cameraSize = m_view.getSize();
    auto cameraCenter = cameraSize / 2.f;
    auto distanceFromCenter = position - cameraCenter;

    return m_view.getCenter() + distanceFromCenter;
  }

  SFMLRenderer::SFMLCamera2D::SFMLCamera2D(const sf::View& view) :
    m_view(view)
  {
  }

  /// Animation2D
  SFMLRenderer::SFMLAnimation2D::SFMLAnimation2D(std::initializer_list<std::pair<Rectf, float>> frames,
                                                 const SFMLTexture* texture) :
    m_frames(frames),
    m_texture(texture)
  {
  }

  const Rectf& SFMLRenderer::SFMLAnimation2D::getFrame(std::size_t index) const
  {
    return getFrameImpl(index).first;
  }

  const float& SFMLRenderer::SFMLAnimation2D::getFrameDuration(std::size_t index) const
  {
    return getFrameImpl(index).second;
  }

  const SFMLRenderer::SFMLTexture* SFMLRenderer::SFMLAnimation2D::getTexture() const
  {
    return m_texture;
  }

  const std::pair<Rectf, float>& SFMLRenderer::SFMLAnimation2D::getFrameImpl(std::size_t index) const
  {
    return m_frames[index];
  }

  /// Renderer
  SFMLRenderer::SFMLRenderer() :
    m_renderTarget(&m_renderWindow)
  {
#ifdef NDEBUG
    // disable logging of sfml
    sf::err().rdbuf(nullptr);
#endif // NDEBUG
  }

  std::pair<bool, SFMLRenderer::Font> SFMLRenderer::loadFont(const string_t& name, const string_t& filename)
  {
    std::ifstream in(filename);
    if(in.is_open()) {
      return loadFont(name, in);
    }

    return std::make_pair(false, Font(sf::Font(), m_fontData.end()));
  }

  std::pair<bool, SFMLRenderer::Font> SFMLRenderer::loadFont(const string_t&, vector_t<unsigned char> data)
  {
    auto insertIt = m_fontData.insert(m_fontData.end(), std::move(data));

    sf::Font font;
    if(!font.loadFromMemory(m_fontData.back().data(), m_fontData.back().size())) {
      m_fontData.erase(insertIt);
      return std::make_pair(false, Font(sf::Font(), m_fontData.end()));
    }

    return std::make_pair(true, Font(std::move(font), insertIt));
  }

  std::pair<bool, SFMLRenderer::Font> SFMLRenderer::loadFont(const string_t& name, std::istream& stream)
  {
    auto fontStreamData = stream_utils::copyStreamToArray(stream);
    if(!fontStreamData.empty()) {
      return loadFont(name, std::move(fontStreamData));
    }

    return std::make_pair(false, Font(sf::Font(), m_fontData.end()));
  }

  void SFMLRenderer::removeFont(const Font& font)
  {
    m_fontData.erase(font.m_fontDataIt);
  }

  std::pair<bool, SFMLRenderer::Texture> SFMLRenderer::loadTexture(const string_t& name, const string_t& filename)
  {
    std::ifstream in(filename);
    if(in.is_open()) {
      return loadTexture(name, in);
    }

    return std::make_pair(false, Texture(sf::Texture()));
  }

  std::pair<bool, SFMLRenderer::Texture> SFMLRenderer::loadTexture(const string_t&,
                                                                   vector_t<unsigned char> data)
  {
    sf::Texture texture;
    if(texture.loadFromMemory(data.data(), data.size())) {
      return std::make_pair(true, Texture(texture));

    }

    return std::make_pair(false, Texture(sf::Texture()));
  }

  std::pair<bool, SFMLRenderer::Texture> SFMLRenderer::loadTexture(const string_t& name, std::istream& stream)
  {
    auto textureStreamData = stream_utils::copyStreamToArray(stream);
    if(!textureStreamData.empty()) {
      return loadTexture(name, std::move(textureStreamData));
    }

    return std::make_pair(false, Texture(sf::Texture()));
  }

  void SFMLRenderer::removeTexture(const Texture&)
  {
  }

  Vector2<position_point_2d_t> SFMLRenderer::calculate2DTextBounds(const string_t& text,
                                                                   const Text2DStyle& textStyle)
  {
    auto t = generateTextFromStyles(textStyle);
    t.setString(text);

    const auto& textBounds = t.getLocalBounds();
    return Vector2<position_point_2d_t>(textBounds.width, textBounds.height);
  }

  Vector2<position_point_2d_t> SFMLRenderer::draw2DText(const Vector2<position_point_2d_t>& position,
                                                        string_t text,
                                                        const Text2DStyle& textStyle)
  {
    auto t = generateTextFromStyles(textStyle);
    t.setPosition(position);
    t.setString(text);

    std::uint32_t style = 0;
    if(textStyle.style & gengine2d::Text2DStyle::TS_REGULAR) {
      style |= sf::Text::Regular;
    }
    if(textStyle.style & gengine2d::Text2DStyle::TS_BOLD) {
      style |= sf::Text::Bold;
    }
    if(textStyle.style & gengine2d::Text2DStyle::TS_ITALIC) {
      style |= sf::Text::Italic;
    }
    t.setStyle(style);

    m_renderTarget->draw(t);

    const auto& textBounds = t.getLocalBounds();
    return Vector2<position_point_2d_t>(textBounds.width, textBounds.height);
  }

  void SFMLRenderer::draw2DLine(const Vector2<position_point_2d_t>& start,
                                const Vector2<position_point_2d_t>& end,
                                const Color& color)
  {
    array_t<sf::Vertex, 2> v;
    v[0].position = vector_utils::convertTo<float>(start);
    v[0].color = color;
    v[1].position = vector_utils::convertTo<float>(end);
    v[1].color = color;

    m_renderTarget->draw(v.data(), v.size(), sf::PrimitiveType::Lines);
  }

  void SFMLRenderer::draw2DLines(const Vertex2* vertices,
                                  std::size_t verticesCount)
  {
    // As of SFML v.2.5.1, gengine2d::Vertex2 and sf::Vertex are data compatible
    // for casting
    m_renderTarget->draw(general_utils::static_cast_to_ptr_type<const sf::Vertex>(vertices),
                         verticesCount,
                         sf::PrimitiveType::Lines);
  }

  void SFMLRenderer::draw2DRect(const Rect<position_point_2d_t>& rect,
                                const Shape2DStyle& style)
  {
    draw2DRect(rect, style, [](const Vector2<position_point_2d_t>& v) { return v; });
  }

  void SFMLRenderer::draw2DRect(const Rect<position_point_2d_t>& rect,
                                const Shape2DStyle& style,
                                const position_modifier_function_type& positionModifierFunc)
  {
    array_t<Vertex2, 6> v = generateRectangle(rect, positionModifierFunc);

    v[0].color = style.fillColor;
    v[1].color = style.fillColor;
    v[2].color = style.fillColor;
    v[3].color = style.fillColor;
    v[4].color = style.fillColor;
    v[5].color = style.fillColor;

    auto tl = style.textureRect.left,
         tt = style.textureRect.top,
         tw = style.textureRect.width,
         th = style.textureRect.height;

    v[0].textureCoordinate = sf::Vector2f(tl, tt);
    v[1].textureCoordinate = v[0].textureCoordinate + sf::Vector2f(tw, 0.f);
    v[2].textureCoordinate = v[0].textureCoordinate + sf::Vector2f(0.f, th);
    v[3].textureCoordinate = v[2].textureCoordinate;
    v[4].textureCoordinate = v[0].textureCoordinate + sf::Vector2f(tw, th);
    v[5].textureCoordinate = v[1].textureCoordinate;

    draw2DShape(v.data(), v.size(), style.texture);

    // draw the outline
    if(style.outlineThickness > 0) {
      v[0].color = style.outlineColor;
      v[1].color = style.outlineColor;
      v[2].color = style.outlineColor;
      v[3].color = style.outlineColor;
      v[4].color = style.outlineColor;
      v[5].color = style.outlineColor;

      // draw left outline
      generateRectangle(Rect<position_point_2d_t>(rect.left - style.outlineThickness,
                                                  rect.top,
                                                  style.outlineThickness,
                                                  rect.height), v, positionModifierFunc);
      draw2DShape(v.data(), v.size(), nullptr);

      // draw top outline
      generateRectangle(Rect<position_point_2d_t>(rect.left - style.outlineThickness,
                                                  rect.top - style.outlineThickness,
                                                  rect.width + (style.outlineThickness * 2),
                                                  style.outlineThickness), v, positionModifierFunc);
      draw2DShape(v.data(), v.size(), nullptr);

      // draw right outline
      generateRectangle(Rect<position_point_2d_t>(rect.left + rect.width,
                                                  rect.top,
                                                  style.outlineThickness,
                                                  rect.height), v, positionModifierFunc);
      draw2DShape(v.data(), v.size(), nullptr);

      // draw bottom outline
      generateRectangle(Rect<position_point_2d_t>(rect.left - style.outlineThickness,
                                                  rect.top + rect.height,
                                                  rect.width + (style.outlineThickness * 2),
                                                  style.outlineThickness), v, positionModifierFunc);
      draw2DShape(v.data(), v.size(), nullptr);
    }
  }

  void SFMLRenderer::draw2DCircle(const Vector2<position_point_2d_t>& position,
                                  float radius,
                                  const Shape2DStyle& style)
  {
    draw2DCircle(position, radius, style, [](const Vector2f& v) { return v; });
  }

  void SFMLRenderer::draw2DCircle(const Vector2<position_point_2d_t>& position,
                                  float radius,
                                  const Shape2DStyle& style,
                                  const position_modifier_function_type& positionModifierFunc)
  {
    class ModifiableCircle : public sf::CircleShape
    {
    public:
      explicit ModifiableCircle(const position_modifier_function_type& positionModifierFunc,
                                float radius = 0.f,
                                unsigned int pointCount = 30) :
        sf::CircleShape(radius, pointCount),
        m_modifierFunc(positionModifierFunc)
      {
      }

      sf::Vector2f getPoint(std::size_t index) const override final
      {
        return m_modifierFunc(sf::CircleShape::getPoint(index));
      }

    private:
      position_modifier_function_type m_modifierFunc;
    };

    ModifiableCircle circle(positionModifierFunc);
    circle.setPosition(position);
    circle.setRadius(radius);
    circle.setOutlineThickness(style.outlineThickness);
    circle.setOutlineColor(style.outlineColor);
    circle.setFillColor(style.fillColor);
    if(style.texture) {
      circle.setTexture(&style.texture->getTexture());
    }
    circle.setTextureRect(sf::IntRect(style.textureRect.left,
                                      style.textureRect.top,
                                      style.textureRect.width,
                                      style.textureRect.height));

    m_renderTarget->draw(circle);
  }

  void SFMLRenderer::draw2DShape(const Vertex2* vertices,
                                 std::size_t verticesCount,
                                 const Texture* texture)
  {
    // As of SFML v.2.5.1, gengine2d::Vertex2 and sf::Vertex are data compatible
    // for casting
    if(texture) {
      m_renderTarget->draw(general_utils::static_cast_to_ptr_type<const sf::Vertex>(vertices),
                           verticesCount,
                           sf::PrimitiveType::Triangles,
                           sf::RenderStates(&texture->getTexture()));
    } else {
      m_renderTarget->draw(general_utils::static_cast_to_ptr_type<const sf::Vertex>(vertices),
                           verticesCount,
                           sf::PrimitiveType::Triangles);
    }
  }

  void SFMLRenderer::ImGuiImage(const RenderableTexture& texture,
                                const Rect<position_point_2d_t>& rect)
  {
    // TODO: Change this as gengine2d::Vector2f and gengine2d::Rectf are created
    ImGui::Image(texture.m_texture,
                 sf::Vector2f(rect.width, rect.height),
                 sf::FloatRect(rect.left, rect.top, rect.width, rect.height));
  }

  void SFMLRenderer::ImGuiImage(const Texture& texture,
                                const Vector2<position_point_2d_t>& textureSize)
  {
    ImGui::Image(texture.m_texture,
                 sf::Vector2f(textureSize.x, textureSize.y));
  }

  void SFMLRenderer::ImGuiImage(const Texture& texture,
                                const Vector2<position_point_2d_t>& textureSize,
                                const Rect<position_point_2d_t>& rect)
  {
    ImGui::Image(texture.m_texture, textureSize, rect);
  }

  bool SFMLRenderer::ImGuiImageButton(const Texture& texture)
  {
    return ImGui::ImageButton(texture.m_texture);
  }

  bool SFMLRenderer::ImGuiImageButton(const Texture& texture,
                                      const Vector2<position_point_2d_t>& buttonSize)
  {
    return ImGui::ImageButton(texture.m_texture, sf::Vector2f(buttonSize.x, buttonSize.y));
  }

  void SFMLRenderer::ImGuiHandleInit()
  {
    ImGui::SFML::Init(m_renderWindow);
  }

  void SFMLRenderer::ImGuiHandleUpdate(float timeStep)
  {
    ImGui::SFML::Update(m_renderWindow, sf::seconds(timeStep));
  }

  void SFMLRenderer::ImGuiHandleRender()
  {
    ImGui::SFML::Render(m_renderWindow);
  }

  void SFMLRenderer::set2DCamera(const Camera2D& camera)
  {
    m_renderWindow.setView(camera.m_view);
  }

  SFMLRenderer::Camera2D SFMLRenderer::get2DCamera() const
  {
    return Camera2D(m_renderWindow.getView());
  }

  SFMLRenderer::Camera2D SFMLRenderer::get2DGUICamera() const
  {
    return Camera2D(m_renderWindow.getDefaultView());
  }

  void SFMLRenderer::reset2DCamera()
  {
    m_renderWindow.setView(m_renderWindow.getDefaultView());
  }

  void SFMLRenderer::setRenderTargetToTexture(RenderableTexture& texture)
  {
    m_renderTarget = &texture.m_texture;
  }

  void SFMLRenderer::resetRenderTarget()
  {
    m_renderTarget = &m_renderWindow;
  }

  Vector2u SFMLRenderer::getScreenSize() const
  {
    return m_renderWindow.getSize();
  }

  sf::Text SFMLRenderer::generateTextFromStyles(const Text2DStyle& style)
  {
    sf::Text t;
    t.setCharacterSize(style.characterSize);
    t.setOutlineThickness(style.outlineThickness);
    t.setOutlineColor(style.outlineColor);
    t.setFillColor(style.fillColor);
    if(style.font) {
      t.setFont(style.font->m_font);
    }

    return t;
  }

  bool SFMLRenderer::init(Console& console,
                          string_t title,
                          int width,
                          int height,
                          int bits,
                          bool fullscreen,
                          bool vsync,
                          bool,
                          void* eventReceiver)
  {
    if(bits < 0) {
      GameLogger::getInstance().writeError("[CRITICAL] Invalid negative color bits.");
      return false;
    }

    if(sf::VideoMode::getFullscreenModes().empty()) {
      GameLogger::getInstance().writeError("[CRITICAL] No valid resolution available on your system.");
      return false;
    }

    sf::VideoMode videoMode;
    if(width < 0 || height < 0) {
      videoMode = sf::VideoMode::getFullscreenModes().front();
      console.printWarning("Invalid resolution " +
                           stl_utils::to_string(width) +
                           "x" +
                           stl_utils::to_string(height) +
                           "!");
      console.printWarning("Using " +
                           stl_utils::to_string(videoMode.width) +
                           "x" +
                           stl_utils::to_string(videoMode.height) +
                           " instead.");
    } else {
      videoMode = sf::VideoMode(width, height, bits);
      if(!videoMode.isValid()) {
        videoMode = sf::VideoMode::getFullscreenModes().front();
        console.printWarning("Video mode " +
                             stl_utils::to_string(width) +
                             "x" +
                             stl_utils::to_string(height) +
                             " is not supported by your system.");
        console.printWarning("Using " +
                             stl_utils::to_string(videoMode.width) +
                             "x" +
                             stl_utils::to_string(videoMode.height) +
                             " instead.");
      }
    }

    m_renderWindow.create(videoMode, title, fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    m_renderWindow.setVerticalSyncEnabled(vsync);

    if(eventReceiver) {
      static_cast<game_internal::InputHandler*>(eventReceiver)->m_renderWindow = &m_renderWindow;
    }

    return true;
  }

  void SFMLRenderer::reset()
  {
    m_renderWindow.close();
  }

  void SFMLRenderer::clear()
  {
    m_renderWindow.clear();
  }

  void SFMLRenderer::display()
  {
    m_renderWindow.display();
  }

  bool SFMLRenderer::run()
  {
    return m_renderWindow.isOpen();
  }
}
}
