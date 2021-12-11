#ifndef MAT4_H
#define MAT4_H

#include "type.h"

namespace gengine2d
{
  template<class T>
  class Mat4
  {
  public:
    Mat4();
    Mat4(T v00, T v01, T v02, T v03,
         T v10, T v11, T v12, T v13,
         T v20, T v21, T v22, T v23,
         T v30, T v31, T v32, T v33);

    Mat4<T>& combine(const Mat4<T>& other);

    Mat4<T>& translate(T x, T y, T z);
    Mat4<T>& rotate(float angle);
    Mat4<T>& scale(float x, float y, float z);

  private:
    array_t<T, 16> m_mat;
  };
}

#endif // MAT4_H
