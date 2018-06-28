#pragma once

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)

#ifdef ELK_TOOLS_EXPORT_DLL
#define ELK_TOOLS_EXPORT __declspec(dllexport)
#else
#define ELK_TOOLS_EXPORT __declspec(dllimport)
#endif // ELK_TOOLS_EXPORT_DLL