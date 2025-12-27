#ifndef WSFPLUGIN_EXPORT_H
#define WSFPLUGIN_EXPORT_H

#ifdef WSFPLUGIN_STATIC_DEFINE
#  define WSFPLUGIN_EXPORT
#  define WSFPLUGIN_NO_EXPORT
#else
#  ifndef WSFPLUGIN_EXPORT
#    ifdef WSFPLUGIN_EXPORTS
        /* We are building this library */
#      define WSFPLUGIN_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define WSFPLUGIN_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef WSFPLUGIN_NO_EXPORT
#    define WSFPLUGIN_NO_EXPORT 
#  endif
#endif

#ifndef WSFPLUGIN_DEPRECATED
#  define WSFPLUGIN_DEPRECATED __declspec(deprecated)
#endif

#ifndef WSFPLUGIN_DEPRECATED_EXPORT
#  define WSFPLUGIN_DEPRECATED_EXPORT WSFPLUGIN_EXPORT WSFPLUGIN_DEPRECATED
#endif

#ifndef WSFPLUGIN_DEPRECATED_NO_EXPORT
#  define WSFPLUGIN_DEPRECATED_NO_EXPORT WSFPLUGIN_NO_EXPORT WSFPLUGIN_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef WSFPLUGIN_NO_DEPRECATED
#    define WSFPLUGIN_NO_DEPRECATED
#  endif
#endif

#pragma warning(disable: 4251)
#pragma warning(disable: 4275)
#pragma warning(disable: 4819)

#endif /* WSFPLUGIN_EXPORT_H */
