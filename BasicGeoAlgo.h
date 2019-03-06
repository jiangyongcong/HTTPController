#pragma once

#include "stdint.h"
#include <complex>

typedef struct _tagVertex_L
{
	int32_t x;
	int32_t y;

	_tagVertex_L(int32_t v_x = 0, int32_t v_y = 0): x(v_x), y(v_y)
	{}

	template<typename T>
	_tagVertex_L(T &pt): x(pt.x), y(pt.y)
	{}


	void Reset()
	{
		x = 0;
		y = 0;
	}

	bool operator == (const _tagVertex_L &rhs) const
	{
		return (this->x == rhs.x) && (this->y == rhs.y);
	}

}Vertex_L;

typedef struct _tagRectange_L
{
	int32_t m_i32LX;
	int32_t m_i32BY;
	int32_t m_i32RX;	
	int32_t m_i32TY;


	_tagRectange_L()
	{
		Reset();
	}

	void Reset()
	{
		m_i32LX = INT32_MAX;
		m_i32BY = INT32_MAX;
		m_i32RX = INT32_MIN;//0
		m_i32TY = INT32_MIN;//0
	}

	void Normalize()
	{
		int32_t tmp;

		if (m_i32LX > m_i32RX)
		{
			tmp = m_i32LX;
			m_i32LX = m_i32RX;
			m_i32RX = tmp;
		}

		if (m_i32BY > m_i32TY)
		{
			tmp = m_i32BY;
			m_i32BY = m_i32TY;
			m_i32TY = tmp;
		}
	}

}Rect_L;

class CBasicGeoAlgo
{
public:

	//矩形2必须与矩形1相交或被矩形1包含
	static bool IsRectCross( const Rect_L &v_rect1, const Rect_L &v_rect2, int32_t v_Error = 0)
	{
		if ((v_rect1.m_i32RX + v_Error < v_rect2.m_i32LX) || (v_rect1.m_i32TY + v_Error < v_rect2.m_i32BY)
			|| (v_rect2.m_i32RX + v_Error < v_rect1.m_i32LX) || (v_rect2.m_i32TY + v_Error < v_rect1.m_i32BY)
			)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	template<typename T>
	static bool PtInRect(const T &v_Pt, const Rect_L &v_Rect)
	{
		if (v_Pt.x >= v_Rect.m_i32LX && v_Pt.x <= v_Rect.m_i32RX
			&& v_Pt.y >= v_Rect.m_i32BY && v_Pt.y <= v_Rect.m_i32TY)
		{
			return true;
		}
		else
		{
			return false;
		}

	}

	template<typename T>
	static void Pt2MBR(Rect_L &v_Rect, const T &v_Pt)
	{
		if (v_Pt.x < v_Rect.m_i32LX)
		{
			v_Rect.m_i32LX = v_Pt.x;
		}

		if (v_Pt.x > v_Rect.m_i32RX)
		{
			v_Rect.m_i32RX = v_Pt.x;
		}

		if (v_Pt.y < v_Rect.m_i32BY)
		{
			v_Rect.m_i32BY = v_Pt.y;
		}

		if (v_Pt.y > v_Rect.m_i32TY)
		{
			v_Rect.m_i32TY = v_Pt.y;
		}
	}


	template<typename T>
	static double CosVectorAngle(const T &v_FrPt0, const T &v_ToPt0, const T &v_FrPt1, const T &v_ToPt1 )
	{
		double dx0 = v_ToPt0.x - v_FrPt0.x;
		double dy0 = v_ToPt0.y - v_FrPt0.y;
		double dx1 = v_ToPt1.x - v_FrPt1.x;
		double dy1 = v_ToPt1.y - v_FrPt1.y;

		return (dx0 * dx1 + dy0 * dy1) /
			(sqrt(dx0 * dx0 + dy0 * dy0) * sqrt(dx1 * dx1 + dy1 * dy1));
	}

	
	template<typename T>
	static double D2_P2L(const T &v_FrPt_L, const T &v_ToPt_L, const T &Pt)
	{
		double A, B, C;
		double tmp;
		double dist;

		//同一点
		if ((v_FrPt_L.x == v_ToPt_L.x) && (v_FrPt_L.y == v_ToPt_L.y))
		{ 
			A = Pt.x - v_FrPt_L.x;
			B = Pt.y - v_ToPt_L.y;
			dist = A * A + B * B;		
		}
		else
		{	//点到直线的距离
			A = v_ToPt_L.y - v_FrPt_L.y;
			B = v_FrPt_L.x - v_ToPt_L.x;
			C = -(v_FrPt_L.y * B + v_FrPt_L.x * A); 		

			tmp = (A* Pt.x + B* Pt.y + C);
			dist = tmp* tmp / (A* A + B* B);
		}

		return dist;
	}


	template<typename T>
	static double D2_P2P(const T &v_FrPt, const T &v_ToPt)
	{
		double dx, dy;
		dx = v_FrPt.x - v_ToPt.x;
		dy = v_FrPt.y - v_ToPt.y;
		return dx * dx + dy * dy;
	}


	template<typename T>
	static double D_P2P(const T &v_FrPt, const T &v_ToPt)
	{
		return sqrt(D2_P2P<T>(v_FrPt, v_ToPt));
	}	


	template<class T_PT>
	int IsLeft(T_PT &v_P0, T_PT &v_P1, T_PT &v_P2)
	{
		__int64 i = (((__int64)(v_P1.x - v_P0.x)) * ((__int64)(v_P2.y - v_P0.y))
			- ((__int64)(v_P2.x - v_P0.x)) * ((__int64)(v_P1.y - v_P0.y)));

		if (i == 0)
		{
			return 0;
		}
		else if (i > 0)
		{
			return 1;
		}
		else
		{
			return -1;
		}	
	}

	template<class T>
	int PointInPoly(T *v_pPts, size_t v_iNumPts, T &v_pt)
	{
		int iWind = 0;

		int iLeft;

		for (int i = 0; i < ((int)v_iNumPts) - 1; i++)
		{
			if ((v_pPts[i].x == v_pt.x) && (v_pPts[i].y == v_pt.y))
			{
				return 0;
			}

			if ((v_pPts[i].y == v_pPts[i+1].y) && (v_pt.y == v_pPts[i].y)
				&& (((v_pPts[i].x < v_pt.x) && (v_pPts[i+1].x > v_pt.x)) || 
				((v_pPts[i].x > v_pt.x) && (v_pPts[i+1].x < v_pt.x))))
			{
				return 0;
			}

			if (v_pPts[i].y <= v_pt.y)
			{
				if(v_pPts[i+1].y > v_pt.y)
				{
					iLeft = IsLeft<T>(v_pPts[i], v_pPts[i+1], v_pt);
					if (iLeft > 0)
					{
						++iWind;
					}
					else if (iLeft == 0)
					{
						return 0; //表示点在边界上
					}
				}
			} 
			else
			{
				if(v_pPts[i+1].y <= v_pt.y)
				{
					iLeft = IsLeft<T>(v_pPts[i], v_pPts[i+1], v_pt);
					if (iLeft < 0)
					{
						--iWind;
					}
					else if (iLeft == 0)
					{
						return 0; //表示点在边界上
					}
				}
			}

		}

		if (iWind == 0)
		{
			return -1; //点在多边形外
		}
		else
		{
			return 1; //点在多边形内
		}
	}	
};
