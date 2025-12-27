#include "CustomWsfPlugin.hpp"

#include "RegisterWsfPlugin.hpp"

// WSF
#include "WsfApplication.hpp"
#include "observer/WsfPlatformObserver.hpp"
#include "observer/WsfTrackObserver.hpp"
#include "observer/WsfSimulationObserver.hpp"
#include "WsfPlatform.hpp"
#include "sensor/WsfSensor.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"

#include "WsfDisInterface.hpp"

#include "WsfDisActionRequest.hpp"
#include "DisActionEnums.hpp"

CustomWsfPlugin::CustomWsfPlugin()
{
}

CustomWsfPlugin::~CustomWsfPlugin() noexcept
{
}

bool CustomWsfPlugin::Initialize()
{
	mDisInterface = WsfDisInterface::Find(GetSimulation());

	mCallbacks.Add(WsfObserver::AdvanceTime(&GetSimulation()).Connect(&CustomWsfPlugin::AdvanceTime, this));

	return true;
}

// 定时发送自定义pdu,  MyCustomPdu
void CustomWsfPlugin::AdvanceTime(double aSimTime)
{
	double deltaTime = aSimTime - mPreSimTime;
	if (deltaTime < 0.0000001) return;

	//std::cout << "AdvanceTime: " << aSimTime << std::endl;

	WsfDisActionRequest advanceTime(mDisInterface);
	advanceTime.SetActionId(DisEnum::Action::Request::TIME_ADVANCE);

	DisDatumSpec data;
	DisFixedDatum fixedDatum;
	fixedDatum.SetId(0);
	fixedDatum.SetDataAs(int(aSimTime * 1000));
	data.AddFixedDatum(fixedDatum);
	advanceTime.SetDatumSpec(data);
	mDisInterface->PutPdu(aSimTime, std::make_unique<WsfDisActionRequest>(advanceTime));

	mPreSimTime = aSimTime;
}
