#include "MyCustomPdu.h"
#include "WsfDisUtil.hpp"
#include "GenI.hpp"
#include "WsfDisInterface.hpp"
#include "WsfSimulation.hpp"

MyCustomPdu::MyCustomPdu(WsfDisInterface* aInterfacePtr)
	: DisPdu()
	, mInterfacePtr(aInterfacePtr)
{


}

MyCustomPdu::MyCustomPdu(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
	: DisPdu(aPdu) // 这里已经把Dis头解析了，只需要解数据
	, mInterfacePtr(aInterfacePtr)
{
	DisPdu::Get(aGenI);
	aGenI >> m_type;
}

int MyCustomPdu::Process()
{
	double simTime = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);
	switch (m_type)
	{
	case 0: // 暂停
		mInterfacePtr->GetSimulation().Pause();
		break;
	case 1: // 恢复
		mInterfacePtr->GetSimulation().Resume();
		break;
	default:
		break;
	}

	return 1;
}

