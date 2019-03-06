#include "stdafx.h"

#include <math.h>
#include "DPAlgorithm.h"


namespace nsDP
{
	//计算两点之间距离的平方

	inline double SqrDistPointToPoint(const Vertex_dp& p, const Vertex_dp& q)
	{
		double dx, dy;
		dx = p.x - q.x;
		dy = p.y - q.y;
		return dx * dx + dy * dy;
	}



	//计算点到直线的距离的平方
	// 输入: LP0, LP1直线的两个端点；P待计算距离的点

	inline double SqrDistPointToLine(const Vertex_dp &LP0, const Vertex_dp &LP1, const Vertex_dp &P)
	{
		double A, B, C;
		double tmp;
		double dist;

		if ((LP0.x == LP1.x) && (LP0.y == LP1.y))
		{ 
			A = P.x - LP0.x;
			B = P.y - LP1.y;
			dist = A * A + B * B;		
		}
		else
		{
			A = LP1.y - LP0.y;
			B = LP0.x - LP1.x;
			C = -(LP0.y * B + LP0.x * A); 		

			tmp = (A* P.x + B* P.y + C);
			dist = tmp* tmp / (A* A + B* B);
		}

		return dist;
	}



	/*********************************************************************
	* 函数名 : SimplifyDP
	* 功  能 : 道格拉斯压缩算法
	* 参  数 :
	* v_tol -- 阈值
	* 返回值 : 
	* 作  者 : Cheng.X.R 
	* 日  期 : 2008-07-03 
	*********************************************************************/


	void SimplifyDP(double v_tol, Vertex_dp* v_pPoints, int v_start, int v_end)
	{
		//assert(v_pPoints);
		//assert(v_start < v_end);

		//while ((v_pPoints[v_start].x == v_pPoints[v_end].x) &&
		//	(v_pPoints[v_start].y == v_pPoints[v_end].y))
		//{
		//	if (v_start < v_end - 1)
		//	{
		//		//SimplifyDP(v_tol, v_pPoints, v_start, v_end - 1);
		//		--v_end;			
		//	} 
		//	else
		//	{
		//		return;
		//	}		
		//}

		if (v_end <= v_start + 1) // 两顶点挨在一起，没必要简化
		{
			return;
		}      


		double dTol2 = v_tol * v_tol;

		int iMaxIdx = v_start;  	// 准备记录距离线段的最远顶点的索引

		double dMaxDist = 0.0;  	 // 准备记录最远距离的平方

		bool bFlag; // 首尾相同的标志
		double dDist;
		double a, b, c;	

		if ((v_pPoints[v_start].x == v_pPoints[v_end].x) &&
			(v_pPoints[v_start].y == v_pPoints[v_end].y))
		{
			bFlag = true;
		}
		else
		{
			bFlag = false;
			a = v_pPoints[v_start].y - v_pPoints[v_end].y;
			b = v_pPoints[v_end].x - v_pPoints[v_start].x;
			c = ((double)v_pPoints[v_start].x * (double)v_pPoints[v_end].y) 
				- ((double)v_pPoints[v_start].y * (double)v_pPoints[v_end].x);
		}



		for (int i = v_start + 1; i < v_end; i++)
		{
			if (v_pPoints[i].state == -1)
			{
				continue; //此点已经被删除了
			}

			//dDist = SqrDistPointToLine(v_pPoints[v_start], v_pPoints[v_end], v_pPoints[i]);

			if (bFlag)
			{
				a = v_pPoints[i].x - v_pPoints[v_start].x;
				b = v_pPoints[i].y - v_pPoints[v_start].y;
				dDist = a * a + b * b;	
			}
			else
			{
				dDist = (v_pPoints[i].x * a) + (v_pPoints[i].y * b) + c;
				if (dDist < 0.0)
				{
					dDist = -dDist;
				}
			}		

			if (dDist > dMaxDist)
			{
				dMaxDist = dDist;
				iMaxIdx = i;			
			}
		} 

		if (!bFlag)
		{
			dMaxDist *= dMaxDist / (a * a + b * b);
		}

		//测试tol 
		if (dMaxDist > dTol2)      // 如果最远顶点到线段S的距离大于阈值
		{
			//v_pPoints[iMaxIdx].state = 1;    //记录maxi这个索引， 此顶点将被最后输出

			//递归调用此程序

			SimplifyDP(v_tol, v_pPoints, v_start, iMaxIdx); // 第一条子线段   
			SimplifyDP(v_tol, v_pPoints, iMaxIdx, v_end); // 第二条子线段

			if (v_pPoints[iMaxIdx].state == 1) //表明该点已经预留了
			{
				return;
			}

			//往前找保留的点
			int iLeft = -1;
			int iRight = -1;
			int j;
			for (j = iMaxIdx - 1; j > v_start; j--)
			{
				if (v_pPoints[j].state == 1)
				{
					iLeft = j;
					break;
				}
			}

			if (iLeft == -1)
			{
				v_pPoints[iMaxIdx].state = 1;
				return;
			}

			for (j = iMaxIdx + 1; j < v_end; j++)
			{
				if (v_pPoints[j].state == 1)
				{
					iRight = j;
					break;
				}
			}

			if (iRight == -1)
			{
				v_pPoints[iMaxIdx].state = 1;
				return;
			}

			dDist = SqrDistPointToLine(v_pPoints[iLeft], v_pPoints[iRight],
				v_pPoints[iMaxIdx]);

			v_pPoints[iMaxIdx].state = (dDist < dTol2 ? -1 : 1); 	
			//////////////////////////////////////////
		}
	}



	/*********************************************************************
	* 函数名 : DataCompress_Poly
	* 功  能 : 矢量图形压缩
	* 参  数 :
	* v_tol -- 阈值
	* v_pPoly -- 存放折线上个点的数组， 如果是存放多边形，则必须保证最后一个点不能与第一点相同
	* v_len -- 折线上的点的个数
	* 返回值 : 
	* v_lpPt中的byDel如果被标记为1，表示保留改点；其他值包括0或-1都是可删除的点
	* 作  者 : Cheng.X.R 
	* 日  期 : 2008-07-03 
	*********************************************************************/
	void DataCompress_Poly(double v_tol, Vertex_dp* v_pPoly, int v_iNumPts)
	{
		//assert(v_lpPt);
		//assert(v_iNumPts >= 2);


		int iLow = v_iNumPts - 1;
		int iHigh = 0;
		double dTol2 = v_tol * v_tol;	 //阈值的平方



		//第一步：通过顶点之间的距离判断，是否删除某些顶点，被删除的点delFlag标记为-1
		for (int i = 0; i < v_iNumPts; i++)
		{
			if (v_pPoly[i].state != -1)
			{
				iLow = i;
				break;
			}
		}	

		for (int i = v_iNumPts - 1; i >= 0; i--)
		{
			if (v_pPoly[i].state != -1)
			{
				iHigh= i;
				break;
			}
		}	

		/////  
		int s = iLow;
		for (int e = iLow + 1; e < iHigh; e++) //对输入的每个顶点循环处理
		{
			if (v_pPoly[e].state == 1)
			{
				s = e;
			}
			else if (v_pPoly[e].state != -1)
			{
				if (SqrDistPointToPoint(v_pPoly[s], v_pPoly[e]) < dTol2)
				{
					v_pPoly[e].state = -1;
				}
				else
				{
					s = e;
				}
			}
		}
		///// 


		//第二步：采用 Douglas-Peucker算法进行简化  
		v_pPoly[iLow].state = v_pPoly[iHigh].state = 1;	 // 给第一个和最后一个顶点标记为1，保留

		SimplifyDP(v_tol, v_pPoly, iLow, iHigh);
	}

}

