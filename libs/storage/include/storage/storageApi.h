#pragma once

#if defined(_WIN32)
#if defined(labyrinthStorageIsBuilt)
#define storageApi __declspec(dllexport)
#else
#define storageApi __declspec(dllimport)
#endif
#else
#define storageApi __attribute__((visibility("default")))
#endif
