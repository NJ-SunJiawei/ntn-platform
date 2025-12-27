#include "wsfplugin_export.h"
#include "RegisterWsfPlugin.hpp"

#include "WsfPlugin.hpp"

#include "WsfApplication.hpp"
#include "WsfApplicationExtension.hpp"
#include "UtMemory.hpp"

extern "C"
{
   WSFPLUGIN_EXPORT void WsfPluginVersion(UtPluginVersion& aVersion)
   {
	   aVersion = UtPluginVersion(
		   WSF_PLUGIN_API_MAJOR_VERSION,
		   WSF_PLUGIN_API_MINOR_VERSION,
		   WSF_PLUGIN_API_COMPILER_STRING
		   );
   }
   WSFPLUGIN_EXPORT void WsfPluginSetup(WsfApplication& aApplication)
   {
	   // 注册本插件工程
	   // 因为RegisterWsfPlugin是Scenario扩展类，因此此处使用默认Application扩展
	   aApplication.RegisterExtension("register_demo-wsfplugin", ut::make_unique<WsfDefaultApplicationExtension<RegisterWsfPlugin>>());
   }
}
