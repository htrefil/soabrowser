#ifndef soamath_h
#define soamath_h

#include <limits>
#include <cmath>
#include <cstdlib>



struct SoaPosition
{
	float x, y;

	SoaPosition() : x(0.0f), y(0.0f) {}
	SoaPosition(float _x, float _y) : x(_x), y(_y) {}

	void Set(float _x, float _y) {
		x = _x;
		y = _y;
	}
};



struct SoaPoint
{
	float x, y, z;

	SoaPoint() : x(0.0f), y(0.0f), z(0.0f) {}
	SoaPoint(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	SoaPoint(float *p) : x(p[0]), y(p[1]), z(p[2]) {}
	SoaPoint(const SoaPoint &p) : x(p.x), y(p.y), z(p.z) {}

	SoaPoint &operator=(const SoaPoint &p) { x = p.x; y = p.y; z = p.z; return *this; }
	SoaPoint &operator=(const float *p) { x = p[0]; y = p[1]; z = p[2]; return *this; }
	SoaPoint &operator/=(const float s) { x /= s; y /= s; z /= s; return *this; }
	SoaPoint &operator+=(const SoaPoint &p) { x += p.x; y += p.y; z += p.z; return *this; }
	SoaPoint operator-(const SoaPoint &p) { return SoaPoint(x - p.x, y - p.y, z - p.z); }
	SoaPoint operator+(const SoaPoint &p) { return SoaPoint(x + p.x, y + p.y, z + p.z); }

	bool IsNull() { return x == 0.0f && y == 0.0f && z == 0.0f; }
	float Length() { return sqrt(x * x + y * y + z * z); }
	SoaPoint &Normalize() { float l = sqrt(x * x + y * y + z * z); x /= l; y /= l; z /= l; return *this; }
	SoaPoint &NormalizeTo(float s) { float l = sqrt(x * x + y * y + z * z); x *= s/l; y *= s/l; z *= s/l; return *this; }
	SoaPoint &Randomize() { x = (float)rand() / RAND_MAX; y = (float)rand() / RAND_MAX; z = (float)rand() / RAND_MAX; return *this; }
	void ToArray(float *a) { a[0] = x; a[1] = y; a[3] = z; }
};



struct SoaMinmax
{
	float min, max;

	SoaMinmax() : min(std::numeric_limits<float>::max()), max(-std::numeric_limits<float>::max()) {}
	SoaMinmax(float _min, float _max) : min(_min), max(_max) {}

	void Reset() {
		min = std::numeric_limits<float>::max();
		max = -std::numeric_limits<float>::max();
	}

	void Absorb(float v) {
		if (v < min)
			min = v;
		if (v > max)
			max = v;
	}

	float Mid() const {
		return (max + min) * 0.5f;
	}

	float Range() const {
		return max - min;
	}
};



struct SoaBounds
{
	SoaMinmax x, y, z;

	SoaBounds() {}
	SoaBounds(const SoaMinmax &_x, const SoaMinmax &_y, const SoaMinmax &_z) : x(_x), y(_y), z(_z) {}

	void Reset() {
		x.Reset();
		y.Reset();
		z.Reset();
	}

	void Absorb(float *v) {
		x.Absorb(v[0]);
		y.Absorb(v[1]);
		z.Absorb(v[2]);
	}

	SoaPoint Center() {
		return SoaPoint(x.Mid(), y.Mid(), z.Mid());
	}

	float Deviation() {
		float xr = x.Range(), yr = y.Range(), zr = z.Range();
		float d = sqrt(xr * xr + yr * yr + zr * zr);
		return (d != 0.0f) ? 100.0f / d : 1.0f;
	}
};



struct SoaRect
{
	SoaPoint o;
	float hW, hH;

	SoaRect() : hW(0.0f), hH(0.0f) {}
	SoaRect(float x, float y, float z, float _hW, float _hH) : o(x, y, z), hW(_hW), hH(_hH) {}
};

#endif
