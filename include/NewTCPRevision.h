 #ifndef __NewTCP_REVISION_H__
#define __NewTCP_REVISION_H__

#include <ExeInfo.h>

class NewTCPRevision : public RevInfo 
{
	public:
		
		const char* GetRevision();
		const char* GetBuildDate();
		const char* GetBuildTime();

	private:
		NewTCPRevision();
		static NewTCPRevision s_NewTCPRev;
};
#endif
