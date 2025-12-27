#ifndef CustomWsfPlugin_HPP
#define CustomWsfPlugin_HPP

#include "WsfSimulationExtension.hpp"
#include "WsfDisInterface.hpp"
#include "UtCallbackHolder.hpp"

class CustomWsfPlugin : public WsfSimulationExtension
{
public:
    CustomWsfPlugin();
    ~CustomWsfPlugin() noexcept override;

    bool Initialize() override;

private:
    void AdvanceTime(double aSimTime);                  // 推进时间

private:
	UtCallbackHolder    mCallbacks;
	double              mPreSimTime = 0.0;

    WsfDisInterface* mDisInterface = nullptr;
};

#endif // CustomWsfPlugin_HPP
