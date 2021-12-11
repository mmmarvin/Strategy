#include "mat4.h"

namespace gengine2d
{
  template<class T>
  Mat4<T>::Mat4()
  {
  }

  template<class T>
  Mat4<T>::Mat4(T v00, T v01, T v02, T v03,
                T v10, T v11, T v12, T v13,
                T v20, T v21, T v22, T v23,
                T v30, T v31, T v32, T v33)
  {
    row(*this, 0)(0) = v00;
  }

  template<class T>
  Mat4<T>& Mat4<T>::translate(T x, T y, T z)
  {
//    Mat4
  }
}
