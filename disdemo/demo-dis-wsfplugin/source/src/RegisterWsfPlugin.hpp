#ifndef RegisterWsfPlugin_HPP
#define RegisterWsfPlugin_HPP

#include "CustomWsfPlugin.hpp"

#include "WsfScenarioExtension.hpp"
#include "WsfSimulation.hpp"
#include "UtMemory.hpp"

#include "WsfDisInterface.hpp"
#include "MyDisPduFactory.h"

class RegisterWsfPlugin : public WsfScenarioExtension
{
public:
    ~RegisterWsfPlugin() noexcept override = default;

    void SimulationCreated(WsfSimulation& aSimulation) override
    {
        // Simulation对象创建完成后，注册Simulation扩展
        aSimulation.RegisterExtension("demo-wsfplugin", ut::make_unique<CustomWsfPlugin>());

        // 绑定自定义的PduFactory到dis接口类
        auto disInterface = WsfDisInterface::Find(aSimulation);
        if (disInterface) {
            disInterface->SetPduFactory(new MyDisPduFactory(disInterface));
        }
    }
};

#endif
