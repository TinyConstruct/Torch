/*
General purpose games-math library.
Vectors: I didn't want to have class-like structs, and I'm not personally a fan of numerical indexes into vectors. 
Fourth component of v4s (w) is 0 by default
Vector functions working by value is somewhat intentional, since they are small, so the tradeoff of passing by reference is hard to assess. 
Matrices: row-major order.
Points: are vectors where the fourth component (w) is 1 by default
*/

#define min(a,b)   ((a) < (b) ? (a) : (b))
#define max(a,b)   ((a) > (b) ? (a) : (b))
#define M_PI  3.14159265358979323846f
#define degToRad(a)  ((a)*(M_PI/180))
#define radToDeg(a)  ((a)*(180/M_PI))



////////////// V2 ////////////////////////////////
struct v2 {
  float x,y;
};

inline v2 V2(float X, float Y) {
  v2 v;
  v.x = X; 
  v.y = Y;
  return v;
}

inline v2 operator+(v2 a, v2 b) {
  v2 result; 
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

inline v2 operator-(v2 a, v2 b) {
  v2 result; 
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return result;
}


inline v2 operator*(float a, v2 v) {
  v2 result; 
  result.x = v.x*a;
  result.y = v.y*a;
  return result;
}

inline v2 operator*=(v2 &v, float a) {
  v = a*v;
  return v;
}

inline v2 operator/(v2 v, float a) {
  v2 result; 
  float s = 1.0f/a;
  result.x = v.x*s;
  result.y = v.y*s;
}

inline v2 operator/=(v2 &v, float a) {
  v = v/a;
  return v;
}

inline float magnitude(const v2& v) {
  return sqrt((v.x*v.x + v.y*v.y));
}

inline v2 normalize(const v2& v) {
  return (v/magnitude(v));
}


////////////// V3 ////////////////////////////////
struct v3 {
  float x,y,z;
};

inline v3 V3(float X, float Y, float Z) {
  v3 v;
  v.x = X; 
  v.y = Y;
  v.z = Z;
  return v;
}

inline v3 operator+(v3 a, v3 b) {
  v3 result; 
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  return result;
}

inline v3 operator-(v3 a, v3 b) {
  v3 result; 
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}

inline v3 operator*(float a, v3 v) {
  v3 result; 
  result.x = v.x*a;
  result.y = v.y*a;
  result.z = v.z*a;
  return result;
}

inline v3 operator*=(v3 &v, float a) {
  v = a*v;
  return v;
}

inline v3 operator/(v3 v, float a) {
  v3 result; 
  float s = 1.0f/a;
  result.x = v.x*s;
  result.y = v.y*s;
  result.z = v.z*s;
}

inline v3 operator/=(v3 &v, float a) {
  v = v/a;
  return v;
}

inline v3 V3(v2 XY, float Z) {
  v3 v;
  v.x = XY.x; 
  v.y = XY.y;
  v.z = Z;
  return v;
}

inline float magnitude(const v3& v) {
  return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

inline v3 normalize(const v3& v) {
  return (v/magnitude(v));
}

////////////// V4 ////////////////////////////////
struct v4 {
  float x,y,z,w;
};

inline v4 V4(float X, float Y, float Z, float W) {
  v4 v;
  v.x = X; 
  v.y = Y;
  v.z = Z;
  v.w = W;
  return v;
}

inline v4 operator+(v4 a, v4 b) {
  v4 result; 
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  result.z = a.z + b.z;
  result.w = a.w + b.w;
  return result;
}

inline v4 operator-(v4 a, v4 b) {
  v4 result; 
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  result.w = a.w - b.w;
  return result;
}

inline v4 operator*(float a, v4 v) {
  v4 result; 
  result.x = v.x*a;
  result.y = v.y*a;
  result.z = v.z*a;
  result.w = v.w*a;
  return result;
}

inline v4 operator*=(v4 &v, float a) {
  v = a*v;
  return v;
}

inline v4 operator/(v4 v, float a) {
  v4 result; 
  float s = 1.0f/a;
  result.x = v.x*s;
  result.y = v.y*s;
  result.z = v.z*s;
  result.w = v.w*s;
}

inline v4 operator/=(v4 &v, float a) {
  v = v/a;
  return v;
}

inline v4 V4(v3 XYZ, float W) {
  v4 v;
  v.x = XYZ.x; 
  v.y = XYZ.y;
  v.z = XYZ.z;
  v.w = W;
  return v;
}

inline v4 V4(v3 XYZ) {
  v4 v;
  v.x = XYZ.x; 
  v.y = XYZ.y;
  v.z = XYZ.z;
  v.w = 1;
  return v;
}

//Points have implied w=1 components
////////////// P3 ////////////////////////////////
struct p3 {
  float x,y,z;
};

inline p3 P3(float X, float Y, float Z) {
  p3 p;
  p.x = X; 
  p.y = Y;
  p.z = Z;
  return p;
}

inline v3 operator-(p3 a, p3 b) {
  v3 result; 
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  result.z = a.z - b.z;
  return result;
}


////////////// Matrix 4x4 ////////////////////////////////
//TODO: think about reference passing cost and SIMD 
struct m4x4 {
    //Row major!
    float n[4][4];
};

m4x4 M4x4(float n00,float n01,float n02,float n03,
          float n10, float n11,float n12,float n13,
          float n20, float n21,float n22,float n23,
          float n30, float n31,float n32,float n33) {
  m4x4 r;
  r.n[0][0]=n00;r.n[0][1]=n01;r.n[0][2]=n02;r.n[0][3]=n03;
  r.n[1][0]=n10;r.n[1][1]=n11;r.n[1][2]=n12;r.n[1][3]=n13;
  r.n[2][0]=n20;r.n[2][1]=n21;r.n[2][2]=n22;r.n[2][3]=n23;
  r.n[3][0]=n30;r.n[3][1]=n31;r.n[3][2]=n32;r.n[3][3]=n33;
  return r;
}

m4x4 rows4x4(v3 a, v3 b, v3 c) {
  m4x4 result = 
  {
    {{a.x, a.y, a.z, 0},
    {b.x, b.y, b.z, 0},
    {c.x, c.y, c.z, 0},
    {0, 0, 0, 1}},
  };
  return(result);
}

m4x4 cols4x4(v3 a, v3 b, v3 c) {
  m4x4 result = 
  {
    {{a.x, b.x, c.x, 0},
    {a.y, b.y, c.y, 0},
    {a.z, b.z, c.z, 0},
    {0, 0, 0, 1}},
  };
  return(result);
}

inline m4x4
identity(void) {
    m4x4 result = 
    {
      {{1, 0, 0, 0},
      {0, 1, 0, 0},
      {0, 0, 1, 0},
      {0, 0, 0, 1}},
    };
    return(result);
}

inline v3
col(m4x4& a, uint32 c) {
    v3 result = {a.n[0][c], a.n[1][c], a.n[2][c]};
    return(result);
}

inline v3
row(m4x4& a, uint32 r) {
    v3 result = {a.n[r][0], a.n[r][1], a.n[r][2]};
    return(result);
}

inline v4 operator*(m4x4& m, v4 v) {
  v4 result = { m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z + m.n[0][3] * v.w,
    m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z + m.n[1][3] * v.w,
    m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z + m.n[2][3] * v.w,
    m.n[3][0] * v.x + m.n[3][1] * v.y + m.n[3][2] * v.z + m.n[3][3] * v.w };
  return result;
}

//for m4x4 * v3, treat the v3 as if it were a a v4 where w = 0
inline v4 operator*(m4x4& m, v3 v) {
  v4 result = { m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z,
    m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z,
    m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z,
    m.n[3][0] * v.x + m.n[3][1] * v.y + m.n[3][2] * v.z};
}

//for m4x4 * p3, treat the p3 as if it were a a v4 where w = 1
inline v4 operator*(m4x4& m, p3 v) {
  v4 result = { m.n[0][0] * v.x + m.n[0][1] * v.y + m.n[0][2] * v.z + m.n[0][3],
    m.n[1][0] * v.x + m.n[1][1] * v.y + m.n[1][2] * v.z + m.n[1][3],
    m.n[2][0] * v.x + m.n[2][1] * v.y + m.n[2][2] * v.z + m.n[2][3],
    m.n[3][0] * v.x + m.n[3][1] * v.y + m.n[3][2] * v.z + m.n[3][3] };
}

inline m4x4 operator*(m4x4& a, m4x4& b) {
  //TODO: this is slow
  m4x4 result = M4x4(
    a.n[0][0]*b.n[0][0] + a.n[0][1]*b.n[1][0] + a.n[0][2]*b.n[2][0] + a.n[0][3]*b.n[3][0],
    a.n[1][0]*b.n[0][0] + a.n[1][1]*b.n[1][0] + a.n[1][2]*b.n[2][0] + a.n[1][3]*b.n[3][0],
    a.n[2][0]*b.n[0][0] + a.n[2][1]*b.n[1][0] + a.n[2][2]*b.n[2][0] + a.n[2][3]*b.n[3][0],
    a.n[3][0]*b.n[0][0] + a.n[3][1]*b.n[1][0] + a.n[3][2]*b.n[2][0] + a.n[3][3]*b.n[3][0],

    a.n[0][0]*b.n[0][1] + a.n[0][1]*b.n[1][1] + a.n[0][2]*b.n[2][1] + a.n[0][3]*b.n[3][1],
    a.n[1][0]*b.n[0][1] + a.n[1][1]*b.n[1][1] + a.n[1][2]*b.n[2][1] + a.n[1][3]*b.n[3][1],
    a.n[2][0]*b.n[0][1] + a.n[2][1]*b.n[1][1] + a.n[2][2]*b.n[2][1] + a.n[2][3]*b.n[3][1],
    a.n[3][0]*b.n[0][1] + a.n[3][1]*b.n[1][1] + a.n[3][2]*b.n[2][1] + a.n[3][3]*b.n[3][1],

    a.n[0][0]*b.n[0][2] + a.n[0][1]*b.n[1][2] + a.n[0][2]*b.n[2][2] + a.n[0][3]*b.n[3][2],
    a.n[1][0]*b.n[0][2] + a.n[1][1]*b.n[1][2] + a.n[1][2]*b.n[2][2] + a.n[1][3]*b.n[3][2],
    a.n[2][0]*b.n[0][2] + a.n[2][1]*b.n[1][2] + a.n[2][2]*b.n[2][2] + a.n[2][3]*b.n[3][2],
    a.n[3][0]*b.n[0][2] + a.n[3][1]*b.n[1][2] + a.n[3][2]*b.n[2][2] + a.n[3][3]*b.n[3][2],

    a.n[0][0]*b.n[0][3] + a.n[0][1]*b.n[1][3] + a.n[0][2]*b.n[2][3] + a.n[0][3]*b.n[3][3],
    a.n[1][0]*b.n[0][3] + a.n[1][1]*b.n[1][3] + a.n[1][2]*b.n[2][3] + a.n[1][3]*b.n[3][3],
    a.n[2][0]*b.n[0][3] + a.n[2][1]*b.n[1][3] + a.n[2][2]*b.n[2][3] + a.n[2][3]*b.n[3][3],
    a.n[3][0]*b.n[0][3] + a.n[3][1]*b.n[1][3] + a.n[3][2]*b.n[2][3] + a.n[3][3]*b.n[3][3]);
  return result;
}

//Vector operations//////////////////////////////////////
inline float dot(v2 a, v2 b) {
  return (a.x*b.x + a.y*b.y);
}

inline float dot(v3 a, v3 b) {
  return (a.x*b.x + a.y*b.y + a.z*b.z);
}

inline float dot(v4 a, v4 b) {
  return (a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w);
}

inline v3 cross(v3 a, v3 b) {
  return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
inline v3 project(v3 a, v3 b) {
  return ( (dot(a,b) / dot(b,b)) * b);
}

//Transformation matrix functions///////////////////////////
inline m4x4 makeRotX(float t) {
  float c = cos(t);
  float s = sin(t);
  return (M4x4(1.0f, 0.0f, 0.0f, 0.0f,
               0.0f, c,    -s,   0.0f,
               0.0f, s,    c,    0.0f,
               0.0f, 0.0f, 0.0f, 1.0f));
}

inline m4x4 makeRotY(float t) {
  float c = cos(t);
  float s = sin(t);
  return (M4x4(c,    0.0f, s,    0.0f,
               0.0f, 1.0f, 0.0f, 0.0f,
               -s,   0.0f, c,    0.0f, 
               0.0f, 0.0f, 0.0f, 1.0f));
}

inline m4x4 makeRotZ(float t) {
  float c = cos(t);
  float s = sin(t);
  return (M4x4(c,    -s,   0.0f, 0.0f,
               s,    c,    0.0f, 0.0f, 
               0.0f, 0.0f, 1.0f, 0.0f,
               0.0f, 0.0f, 0.0f, 1.0f));
}

inline m4x4 translate(v3 v) {
  return (M4x4(1.0f,  0.0f, 0.0f, v.x,
               0.0f,  1.0f, 0.0f, v.y,
               0.0f,  0.0f, 1.0f, v.z, 
               0.0f,  0.0f, 0.0f, 1.0f));

}

inline m4x4 translate(m4x4& m, v3 v) {
  return (M4x4(m.n[0][0], m.n[0][1], m.n[0][2], m.n[0][3] + v.x, 
               m.n[1][0], m.n[1][1], m.n[1][2], m.n[1][3] + v.y, 
               m.n[2][0], m.n[2][1], m.n[2][2], m.n[2][3] + v.z, 
               m.n[3][0], m.n[3][1], m.n[3][2], m.n[3][3]));
}

void getOrthoProjMatrix( 
    const float &b, const float &t, const float &l, const float &r, 
    const float &n, const float &f, m4x4& M) 
{ 

    // set OpenGL orthographic projection matrix
    M.n[0][0] = 2 / (r - l); 
    M.n[0][1] = 0; 
    M.n[0][2] = 0; 
    M.n[0][3] = 0; 
 
    M.n[1][0] = 0; 
    M.n[1][1] = 2 / (t - b); 
    M.n[1][2] = 0; 
    M.n[1][3] = 0; 
 
    M.n[2][0] = 0; 
    M.n[2][1] = 0; 
    M.n[2][2] = -2 / (f - n); 
    M.n[2][3] = 0; 
 
    M.n[3][0] = -(r + l) / (r - l); 
    M.n[3][1] = -(t + b) / (t - b); 
    M.n[3][2] = -(f + n) / (f - n); 
    M.n[3][3] = 1; 
} 