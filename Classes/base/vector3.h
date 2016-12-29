/*

Copyright (c) 2016 David Morton

*/
#ifndef BASE_VECTOR3_H_
#define BASE_VECTOR3_H_

namespace base{

class Vector3{
public:
	int m_iX;
	int m_iY;
	int m_iZ;
	int m_iId;

	Vector3();
	~Vector3();
	Vector3(int x, int y, int z);
	Vector3(int id, int x, int y, int z);
	int Compare(const Vector3 *other);
};

} // namespace base

#endif
