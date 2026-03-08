 #include <NewTCPRevision.h>
#include <string>
	#ifndef NewTCPMajorRev
	#define NewTCPMajorRev  1
	#endif
	#ifndef NewTCPMinorRev
	#define NewTCPMinorRev 0
	#endif
	#ifndef NewTCPPatchRev
	#define NewTCPPatchRev  0
	#endif

	static const char* NewTCP_Date = __DATE__;
	static const char* NewTCP_Time = __TIME__;
	static const int NewTCP_MajorRev = NewTCPMajorRev; 
	static const int NewTCP_MinorRev = NewTCPMinorRev; 
	static const int NewTCP_PatchRev = NewTCPPatchRev; 

	static const std::string NewTCP_RevStr(std::to_string(NewTCP_MajorRev) + "." + 
				std::to_string(NewTCP_MinorRev) + "." + 
				std::to_string(NewTCP_PatchRev));

 	NewTCPRevision NewTCPRevision::s_NewTCPRev;

	NewTCPRevision::NewTCPRevision() :RevInfo("NewTCP")
	{
		ExeRevInfos::instance().AddLibInfo(this);
	}


	const char* NewTCPRevision::GetRevision()
	{ 
		return NewTCP_RevStr.c_str(); 
	}
	const char* NewTCPRevision::GetBuildDate()
	{ 
		return NewTCP_Date; 
	}

	const char* NewTCPRevision::GetBuildTime()
	{ 
		return NewTCP_Time; 
	}
