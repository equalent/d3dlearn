// Copyright (c) 2019 Andrey Tsurkan. All rights reserved
// This code is distributed under MIT License

#pragma once


#if defined(LEARNENGINE_EXPORTS)
#define LE_API __declspec(dllexport)
#else
#define LE_API __declspec(dllimport)
#endif
