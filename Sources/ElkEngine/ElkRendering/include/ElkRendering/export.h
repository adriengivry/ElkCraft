#pragma once

#ifdef ELK_RENDERING_EXPORT_DLL
#define ELK_RENDERING_EXPORT __declspec(dllexport)
#else
#define ELK_RENDERING_EXPORT __declspec(dllimport)
#endif // ELK_RENDERING_EXPORT_DLL