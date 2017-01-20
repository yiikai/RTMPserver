/******************************************************
# Author       : Yii-Kai Mao
# Last modified: 2017-01-20 11:27
# Email        : yiikai1987910@gmail.com
# Filename     : gopcache.h
# Description  : 
******************************************************/
#include "streamsource.h"
#include <vector>
using namespace std;

class gopcache
{
	public:
		gopcache();
		~gopcache();
		void add_gop_cache(const sharedMessage& msg);
		vector<sharedMessage>& getCache()
		{
			return m_cache;
		}
	private:
		bool is_Ifream(const sharedMessage& msg);
	private:
		vector<sharedMessage> m_cache;	//对应流的每一次跟新的gop缓存
};
