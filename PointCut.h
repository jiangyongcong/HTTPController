#pragma once
#include <math.h>


//GPS数据信息
typedef struct _tagGPSInfo
{
	std::string           m_strTime;         //GPS时间
	unsigned int     m_u32Lon;          //经度
	unsigned int     m_u32Lat;          //纬度
	unsigned char    m_u8Speed;         //速度
	unsigned char    m_u8Direction;     //方向
	int m_iDel;

	_tagGPSInfo()
	{
		clean();
	}

	void clean()
	{
		m_strTime.clear();
		m_u32Lon = 0;
		m_u32Lat = 0;
		m_u8Speed = 0;
		m_u8Direction = 0;
		m_iDel = 0;
	}

} GPSInfo;


class CPointCut
{
public:
	CPointCut(void);
	~CPointCut(void);

public:
	//抽稀算法主函数
	int _DoCompress(vector<GPSInfo>& v_vecPoint);
	
private:	
	//步长法
	void _SimplifyStep(vector<GPSInfo>& v_vecPoint,double v_dTol);
	//角度分段法
	void CalculateAnqle(double v_Angle, double v_tolAngle, vector<GPSInfo>& v_vecPoint);
	//道格拉斯-普克法
	void SimplifyDP(double v_tolDP, vector<GPSInfo>& v_vecPoint,int v_start,int v_end);

private:
	//计算两个点之间的距离
	double _SqrNptToNpt(const GPSInfo v_px,  GPSInfo v_py);
	//计算点到直线距离的平方
	double SqrDistPointToLine(const GPSInfo LP0, const GPSInfo LP1, const GPSInfo P);
	
};
