#ifndef RECTANGLE_H
#define RECTANGLE_H

namespace gengine2d
{
  template<class T>
  struct Rectangle
  {
    Rectangle() : left(0), top(0), width(0), height(0) {}
    Rectangle(T left_, T top_, T width_, T height_) : left(left_), top(top_), width(width_), height(height_) {}

    T left;
    T top;
    T width;
    T height;
  };

  using Rectanglef = Rectangle<float>;
  using Rectanglei = Rectangle<int>;
  using Rectangleu = Rectangle<unsigned int>;
}

#endif // RECTANGLE_H
