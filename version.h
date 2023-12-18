﻿#pragma once
#define COMPONENT_NAME_H "Vital bookmarks"
#define COMPONENT_NAME_HC "Vital Bookmarks"
#define COMPONENT_DESC "Provides playback bookmark functionality."
#define COMPONENT_NAME "foo_vbookmark"
#define COMPONENT_NAME_DLL COMPONENT_NAME ".dll"
#define COMPONENT_YEAR "2023"

#define COMPONENT_VERSION_MAJOR 1
#define COMPONENT_VERSION_MINOR 3
#define COMPONENT_VERSION_PATCH 0
#define COMPONENT_VERSION_SUB_PATCH 0

#define MAKE_STRING(text) #text
#define MAKE_COMPONENT_VERSION(major,minor,patch) MAKE_STRING(major) "." MAKE_STRING(minor) "." MAKE_STRING(patch)
#define MAKE_DLL_VERSION(major,minor,patch,subpatch) MAKE_STRING(major) "." MAKE_STRING(minor) "." MAKE_STRING(patch) "." MAKE_STRING(subpatch)
#define MAKE_API_SDK_VERSION(sdk_ver, sdk_target) MAKE_STRING(sdk_ver) " " MAKE_STRING(sdk_target)

#define FOO_COMPONENT_VERSION MAKE_COMPONENT_VERSION(COMPONENT_VERSION_MAJOR,COMPONENT_VERSION_MINOR,COMPONENT_VERSION_PATCH)

#define DLL_VERSION_NUMERIC COMPONENT_VERSION_MAJOR, COMPONENT_VERSION_MINOR, COMPONENT_VERSION_PATCH, COMPONENT_VERSION_SUB_PATCH
#define DLL_VERSION_STRING MAKE_DLL_VERSION(COMPONENT_VERSION_MAJOR,COMPONENT_VERSION_MINOR,COMPONENT_VERSION_PATCH,COMPONENT_VERSION_SUB_PATCH)

//fb2k ver
#define PLUGIN_FB2K_SDK MAKE_API_SDK_VERSION(FOOBAR2000_SDK_VERSION, FOOBAR2000_TARGET_VERSION)
