#pragma once
#include "stdafx.h"


static constexpr GUID guid_advconfig_branch = { 0x27324fc4, 0x4400, 0x4a9a, { 0xa7, 0x7, 0x7c, 0xba, 0x22, 0xc9, 0x9c, 0x5e } };
static constexpr GUID guid_cfg_dir = { 0x1a4e74fd, 0xf5ef, 0x4fd0, { 0xaf, 0x24, 0x12, 0xdd, 0x11, 0x46, 0x88, 0x0 } };
static constexpr GUID guid_cfg_openafter = { 0xbaf9456c, 0xe2fe, 0x4f0c, { 0x89, 0x8, 0x34, 0x9c, 0xa4, 0xce, 0xba, 0x90 } };


static constexpr char default_cfg_dir[] = "C:\\Users\\dexte\\Sync\\testing2";
static constexpr bool default_cfg_openafter = true;

static advconfig_branch_factory g_advconfigBranch("Dex component", guid_advconfig_branch, advconfig_branch::guid_branch_tools, 0);
static advconfig_string_factory cfg_dir("dir", guid_cfg_dir, guid_advconfig_branch, 0, default_cfg_dir);
static advconfig_checkbox_factory cfg_openafter("open after????????", guid_cfg_openafter, guid_advconfig_branch, 1, default_cfg_openafter);


