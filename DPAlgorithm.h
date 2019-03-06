#ifndef __dp_algorithm_h_
#define __dp_algorithm_h_


namespace nsDP
{
	typedef struct _tagVertex_dp
	{
		long x;
		long y;		
		int state; // 点的状态：
		           // 0 -- 自由态，该点在抽稀的过程中根据情况保留或删除
		           // -1 -- 删除态，该点在抽稀的过程中必然被删除
		           // 1 -- 保留态，该点在抽稀的过程中必然保留

		_tagVertex_dp(long v_x = 0, long v_y = 0, int v_state = 0)
			: x(v_x), y(v_y), state(v_state)
		{}
		
	}Vertex_dp;

	extern void DataCompress_Poly(double v_tol, Vertex_dp* v_lpPt, int v_iPtNum);

}

#endif
