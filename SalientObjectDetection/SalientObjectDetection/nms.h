//////////////////////////////////////////////////////////////////////////
//	simple implementation of non-maximum suppression
//	fengjie@MSRA.VC
//	2010-7-27
//////////////////////////////////////////////////////////////////////////


#pragma once

#include "SalientObjectDetector.h"
#include <vector>
#include <algorithm>
using namespace std;


bool comprect(ScoreRect a, ScoreRect b)
{
	if(a.score>b.score)
		return true;
	else
		return false;
}


vector<ScoreRect> nms(vector<ScoreRect>& input, float overlapRate)
{
	vector<ScoreRect> res;

	//sort windows by scores (high->low)
	sort(input.begin(), input.end(), comprect);
	while(input.size()>0)
	{
		res.push_back(input[0]);
		if(res.size()>=10)
			break;
		for(size_t i=1; i<input.size(); i++)
		{
			int xx1 = max(input[0].box.x, input[i].box.x);
			int yy1 = max(input[0].box.y, input[i].box.y);
			int xx2 = min(input[0].box.x+input[0].box.width-1, input[i].box.x+input[i].box.width-1);
			int yy2 = min(input[0].box.y+input[0].box.height-1, input[i].box.y+input[i].box.height-1);
			float width = xx2-xx1+1;
			float height = yy2-yy1+1;
			if(width>0 && height>0)
			{
				float overlap = (width*height)/(input[0].box.width*input[0].box.height);
				if(overlap > overlapRate)
				{
					//remove it and step back
					input.erase(input.begin()+i);
					i--;
				}
			}
		}
		input.erase(input.begin());
	}

	return res;

}