#pragma once
#ifdef BUILDING_NODE_PLUGIN
#  define NODE_PLUGIN_API __declspec(dllexport)
#else
#  define NODE_PLUGIN_API __declspec(dllimport)
#endif