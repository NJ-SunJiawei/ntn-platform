#include "MyDisActionRequest.h"
#include "DisActionEnums.hpp"
#include "WsfDisInterface.hpp"
#include "WsfSimulation.hpp"
#include "WsfPlatform.hpp"
#include "WsfDisPlatform.hpp"

MyDisActionRequest::MyDisActionRequest(WsfDisInterface* aInterfacePtr)
	: WsfDisActionRequest(aInterfacePtr)
	, mInterfacePtr(aInterfacePtr)
{

}

MyDisActionRequest::MyDisActionRequest(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
	: WsfDisActionRequest(aPdu, aGenI, aInterfacePtr)
	, mInterfacePtr(aInterfacePtr)
{

}

int MyDisActionRequest::Process()
{
	// 与发送端协商的actionId
	if (GetActionId() == DisEnum::Action::Request::OTHER)
	{
		int requestId = GetRequestId();
		auto fixDatas = GetData().GetFixedDatums(); // 与发送到协商的数据内容
		int application, side, entity = 0;
		for (auto fixData : fixDatas)
		{
			if (fixData.GetId() == 1) { application = fixData.GetDataAs<int>(); }
			if (fixData.GetId() == 2) { side = fixData.GetDataAs<int>(); }
			if (fixData.GetId() == 3) { entity = fixData.GetDataAs<int>(); }
		}

		DisEntityId entityId;
		entityId.SetApplication(application);
		entityId.SetSite(side);
		entityId.SetEntity(entity);

		switch (requestId) // 与发送端协商的requestId
		{
		case 1: // 旋转
		{
			// 下面就是对具体数据的处理
			// 我这里的处理是：根据发送到发送的实体Id，旋转实体
			auto disPlatform = mInterfacePtr->FindDisPlatform(entityId);
			if (disPlatform != nullptr) {
				auto platform = disPlatform->GetPlatform();
				if (platform) {
					double heading, pitch, roll = 0;
					platform->GetOrientationNED(heading, pitch, roll);
					heading = heading - 0.1;
					auto mover = platform->GetMover();
					// 如果有Mover，则会按指定路线(route)移动，修改heading会无效，为了测试因此先移除掉
					if (mover) platform->RemoveComponent(mover);
					platform->SetOrientationNED(heading, pitch, roll);
				}
			}
		} break;
		default: break;
		}
	}
	return 1; // 1 表示可删除，0 表示后面再处理
}