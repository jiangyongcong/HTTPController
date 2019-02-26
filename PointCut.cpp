#include "StdAfx.h"
#include "PointCut.h"


CPointCut::CPointCut(void)
{
}

CPointCut::~CPointCut(void)
{
}

int CPointCut::_DoCompress(vector<GPSInfo>& v_vecPoint)
{
	int iStart = 0;
	int iEnd = v_vecPoint.size() - 1;

	double Lenvtoll = 80.0;
	double llenToll_step = Lenvtoll;//步长法步长阈值，显示数据中为60*60/100（平方值）,这里同样使用长度的平方

	double langleToll_angle = 10.0;//角度分析法角度阈值，显示数据中为20度
	double llenToll_angle = Lenvtoll*Lenvtoll/25;//角度分析法两线段长度阈值，显示数据中为60*60/25(平方值)

	//double ldisToll_angle = Lenvtoll*Lenvtoll*2.13/5;//角度分析法垂距阈值,显示数据中为60*60*2.13/5（平方值）
	double ldistToll_DP = Lenvtoll*Lenvtoll*3/5;//道格拉斯普克法,垂距阈值，显示数据中为60*60*3/5（平方值）

	//采用步长法、角度分析法、以及 Douglas-Peucker算法进行简化
	//使用步长法抽稀 
	_SimplifyStep(v_vecPoint,llenToll_step); 
	
	//使用角度分析法抽稀
	CalculateAnqle(langleToll_angle,llenToll_angle,v_vecPoint);

	//使用道格拉斯普克法抽稀
	SimplifyDP(ldistToll_DP, v_vecPoint,iStart,iEnd);

	return 0;
}

//步长法
void CPointCut::_SimplifyStep(vector<GPSInfo>& v_vecPoint,double v_dTol)
{
	//阈值的平方
	double dTol = v_dTol*v_dTol;

	int iHigh = v_vecPoint.size() -1;
	int iLow = 0;

	//起点、终点必须保留。
	v_vecPoint[iLow].m_iDel = 1;
	v_vecPoint[iHigh].m_iDel = 1;

	//逐一过滤小于阈值的线段，如果两点间线段小于阈值，则将后一节点标识为可删除点
	int iStart = iLow;
	for (int iEnd = iLow +1; iEnd < iHigh; iEnd++)
	{
		if (1 == v_vecPoint[iEnd].m_iDel)
		{//若标记为1，则为保留点，修改下限位置
			iStart = iEnd;
		}
		else if (v_vecPoint[iEnd].m_iDel != -1)
		{//若标记非已删除点，继续判断
			if (_SqrNptToNpt(v_vecPoint[iStart],v_vecPoint[iEnd]) < dTol)
			{//若两线段间线段小于阈值，则将后一节点标记为可删除点
				v_vecPoint[iEnd].m_iDel = -1;
			}
			else
			{
				iStart = iEnd;
			}
		}
	}

	return;
}

//角度分段法
void CPointCut::CalculateAnqle(double v_Angle, double v_tolAngle, vector<GPSInfo>& v_vecPoint)
{
	if (v_vecPoint.size() < 2) // 至少要三个点来判断两线段的夹角
	{
		return;
	}

	double dDist;
	double a, b, c;
	int k;
	int firstPoint,middlePoint,secondPoint;
	double a1, b1, a2, b2;
	double k1, k2, L1, L2;
	double angle;

	firstPoint = 0;
	for (int i = 1; i < (int)v_vecPoint.size(); i++)//从第个点开始判断
	{
		angle = 0;

		if (v_vecPoint[i].m_iDel == 1)//该点为保留点
		{
			firstPoint = i;
			continue; 
		}
		else if (v_vecPoint[i].m_iDel == -1)
		{
			continue; //此点已经被删除了
		}
		else
		{
			middlePoint = i;
		}

		for (k = i + 1; k <= (int)v_vecPoint.size()-1; k++)
		{
			if (v_vecPoint[k].m_iDel == -1)
			{
				k++; //此点已经被删除了
			}
			else
			{
				secondPoint = k;
				break;
			}
		}

		if (k >(int)v_vecPoint.size()-1)
		{
			break;
		}

		a1 = (double)v_vecPoint[firstPoint].m_u32Lat - (double)v_vecPoint[middlePoint].m_u32Lat;
		b1 = (double)v_vecPoint[middlePoint].m_u32Lon - (double)v_vecPoint[firstPoint].m_u32Lon;		

		if ((double)v_vecPoint[middlePoint].m_u32Lon == (double)v_vecPoint[firstPoint].m_u32Lon)
		{
			k1 = -a1;//垂直方向斜率：将横轴坐标值偏移
		}
		else
		{
			k1 = -a1 / b1;//斜率
		}

		a2 = (double)v_vecPoint[middlePoint].m_u32Lat - (double)v_vecPoint[secondPoint].m_u32Lat;
		b2 = (double)v_vecPoint[secondPoint].m_u32Lon - (double)v_vecPoint[middlePoint].m_u32Lon;

		if ((double)v_vecPoint[secondPoint].m_u32Lon == (double)v_vecPoint[middlePoint].m_u32Lon)
		{
			k2 = -a2;//垂直方向斜率：将横轴坐标值偏移
		}
		else
		{
			k2 = -a2 / b2;//斜率
		}

		double tangent = (k2 - k1) / (1 + k1 * k2);//夹角公式
		double radian = atan(abs(tangent));//反正切求出弧度值
		angle = radian * 180.0 / Pi;//弧度转换成角度

		L1 = sqrt(a1 * a1 + b1 *b1);//线段长度的开平方//sqrt(a1 * a1 + b1 *b1);
		L2 = sqrt(a2 * a2 + b2 * b2); //线段长度的开平方//sqrt(a2 * a2 + b2 * b2);

		//可以添加：如果k1无斜率(也就是该线段横轴值为)，但角度、线段长度都小于阈值，判断纵轴值
		//v_pPoints[middlePoint].y < v_pPoints[secondPoint].y,则该点不删除。同理k2

		if ((angle < v_Angle))//两线段间的夹角
		{
			if (L1 < v_tolAngle || L2 < v_tolAngle)//两线段之一与长度阈值相比
			{
				v_vecPoint[middlePoint].m_iDel = -1;
				continue;
			}
			else
			{
				//垂距限值法
				a = (double)v_vecPoint[firstPoint].m_u32Lat - (double)v_vecPoint[secondPoint].m_u32Lat;//首尾两点纵向长度:a=ys-ye
				b = (double)v_vecPoint[secondPoint].m_u32Lon - (double)v_vecPoint[firstPoint].m_u32Lon;//首尾两点横向长度:b=xe-xs		 

				c = ((double)v_vecPoint[firstPoint].m_u32Lon * (double)v_vecPoint[secondPoint].m_u32Lat) //直线方程y=kx+c1=(-a/b)*x+c1
					- ((double)v_vecPoint[firstPoint].m_u32Lat * (double)v_vecPoint[secondPoint].m_u32Lon);//c1=(xs*ye-ys*xe)/(xs-xe)=c/(-b)

				//点(x0,y0)到线段Ax+By+c1=0的距离公式：d=(|A*x0+B*y0+c1|)/[sqrt(A*A+B*B)] 
				dDist = ((double)v_vecPoint[middlePoint].m_u32Lon * a) + ((double)v_vecPoint[middlePoint].m_u32Lat * b) + c;//点到线段距离公式的分子部分
				if (dDist < 0.0)
				{
					dDist = -dDist;
				}

				double RdDist = dDist / sqrt(a * a + b * b);//真正的点到线段距离的平方
				if (RdDist < v_tolAngle)
				{
					v_vecPoint[middlePoint].m_iDel = -1;
				}
			}
		}
		else
		{
			firstPoint = i;
		}	
	}	

}

//道格拉斯-普克法
void CPointCut::SimplifyDP(double v_tolDP, vector<GPSInfo>& v_vecPoint,int v_start,int v_end)
{
	if (v_end <= v_start + 1) // 两顶点挨在一起，没必要简化
	{
		return;
	} 

	int iMaxIdx = v_start;  	// 准备记录距离线段最远顶点的索引

	double dMaxDist = 0.0;  	 // 准备记录最远距离的平方

	bool bFlag; // 首尾相同的标志
	double dDist;
	double a, b, c;		

	if (((double)v_vecPoint[v_start].m_u32Lon == (double)v_vecPoint[v_end].m_u32Lon) &&
		((double)v_vecPoint[v_start].m_u32Lat == (double)v_vecPoint[v_end].m_u32Lat))
	{
		bFlag = true;	//第一个点与最后一个点重叠
	}
	else
	{
		bFlag = false; //为不同的两个点
		a = (double)v_vecPoint[v_start].m_u32Lat - (double)v_vecPoint[v_end].m_u32Lat;//首尾两点纵向长度:a=ys-ye
		b = (double)v_vecPoint[v_end].m_u32Lon - (double)v_vecPoint[v_start].m_u32Lon;//首尾两点横向长度:b=xe-xs
		c = ((double)v_vecPoint[v_start].m_u32Lon * (double)v_vecPoint[v_end].m_u32Lat) //直线方程y=kx+c1=(-a/b)*x+c1
			- ((double)v_vecPoint[v_start].m_u32Lat * (double)v_vecPoint[v_end].m_u32Lon);//c1=(xs*ye-ys*xe)/(xs-xe)=c/(-b)
	}

	for (int i = v_start + 1; i < v_end; i++)//从第个点开始判断
	{
		if (v_vecPoint[i].m_iDel == -1)
		{
			continue; //此点已经被删除了
		}

		//dDist = SqrDistPointToLine(v_pPoints[v_start], v_pPoints[v_end], v_pPoints[i]);

		if (bFlag)
		{
			////第一个点与最后一个点重叠情况
			a = (double)v_vecPoint[i].m_u32Lon - (double)v_vecPoint[v_start].m_u32Lon;
			b = (double)v_vecPoint[i].m_u32Lat - (double)v_vecPoint[v_start].m_u32Lat;
			dDist = a * a + b * b;	//两点之间的距离
		}
		else
		{
			//点(x0,y0)到线段Ax+By+c1=0的距离公式：d=(|A*x0+B*y0+c1|)/[sqrt(A*A+B*B)] 
			dDist = ((double)v_vecPoint[i].m_u32Lon * a) + ((double)v_vecPoint[i].m_u32Lat * b) + c;//点到线段距离公式的分子部分
			if (dDist < 0.0)
			{
				dDist = -dDist;
			}
		}		

		if (dDist > dMaxDist)//dMaxDist: 记录最远距离的平方
		{
			dMaxDist = dDist;
			iMaxIdx = i;			
		}
	} 

	if (!bFlag)
	{
		dMaxDist *= dMaxDist / (a * a + b * b);//真正的点到线段距离的平方
	}

	//测试tol 
	if (dMaxDist > v_tolDP )      // 如果最远顶点到线段S的距离大于阈值
	{
		//v_pPoints[iMaxIdx].iDel = 1;    //记录maxi这个索引，此顶点将被最后输出

		//递归调用此程序

		SimplifyDP(v_tolDP, v_vecPoint, v_start, iMaxIdx); // 第一条子线段  
		SimplifyDP(v_tolDP, v_vecPoint, iMaxIdx, v_end); // 第二条子线段

		if (v_vecPoint[iMaxIdx].m_iDel == 1) //表明该点已经预留了
		{
			return;
		}

		//往前找保留的点
		int iLeft = -1;
		int iRight = -1;
		int j;
		for (j = iMaxIdx - 1; j > v_start; j--)
		{
			if (v_vecPoint[j].m_iDel == 1)
			{
				iLeft = j;
				break;
			}
		}

		if (iLeft == -1)
		{
			v_vecPoint[iMaxIdx].m_iDel = 1;
			return;
		}

		for (j = iMaxIdx + 1; j < v_end; j++)
		{
			if (v_vecPoint[j].m_iDel == 1)
			{
				iRight = j;
				break;
			}
		}

		if (iRight == -1)
		{
			v_vecPoint[iMaxIdx].m_iDel = 1;
			return;
		}

		dDist = SqrDistPointToLine(v_vecPoint[iLeft], v_vecPoint[iRight], v_vecPoint[iMaxIdx]);

		v_vecPoint[iMaxIdx].m_iDel = (dDist < v_tolDP ? -1 : 1);
	}

}

//计算两个点之间距离的平方
double CPointCut::_SqrNptToNpt(const GPSInfo v_p, const GPSInfo v_q)
{
	double dx, dy;
	dx = (double)v_p.m_u32Lon - (double)v_q.m_u32Lon;
	dy = (double)v_p.m_u32Lat - (double)v_q.m_u32Lat;
	return dx * dx + dy * dy;
}

//计算点到直线距离的平方
double CPointCut::SqrDistPointToLine(const GPSInfo LP0, const GPSInfo LP1, const GPSInfo P)
{
	double A, B, C;
	double tmp;
	double dist;

	if ((LP0.m_u32Lon == LP1.m_u32Lon) && (LP0.m_u32Lat == LP1.m_u32Lat))
	{ 
		A = (double)P.m_u32Lon - (double)LP0.m_u32Lon;
		B = (double)P.m_u32Lat - (double)LP1.m_u32Lat;
		dist = A * A + B * B;		
	}
	else
	{
		A = (double)LP1.m_u32Lat - (double)LP0.m_u32Lat;
		B = (double)LP0.m_u32Lon - (double)LP1.m_u32Lon;
		C = -((double)LP0.m_u32Lat * B + (double)LP0.m_u32Lon * A); 		

		tmp = (A* (double)P.m_u32Lon + B* (double)P.m_u32Lat + C);
		dist = tmp* tmp / (A* A + B* B);
	}

	return dist;
}