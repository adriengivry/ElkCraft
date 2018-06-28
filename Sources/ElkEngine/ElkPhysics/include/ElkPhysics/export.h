#pragma once

#ifdef ELK_PHYSICS_EXPORT_DLL
#define ELK_PHYSICS_EXPORT __declspec(dllexport)
#else
#define ELK_PHYSICS_EXPORT __declspec(dllimport)
#endif // ELK_PHYSICS_EXPORT_DLL