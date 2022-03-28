﻿// Copyright (c) 2022 Alexey Ivanchukov (lewa_j)
#include "bsp-converter.h"
#include "map.h"
#include "gltf_export.h"

#if _MSC_VER
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#define strcasestr StrStrIA
#endif

int main(int argc, const char *argv[])
{
	printf(HLBSP_CONVERTER_NAME "\n");
	if (argc < 2 || !strcmp(argv[1], "-h"))
	{
		printf("Usage: bsp-converter map.bsp [-lm <lightmap atlas size>] [-lstyles <light style index>|all] [-skip_sky] [-uint16] [-tex]\n");
		return -1;
	}

	std::string mapName = argv[1];
	std::string rootPath;
	{
		int p = mapName.find_last_of("/\\");
		if (p != std::string::npos) {
			rootPath = mapName.substr(0, p + 1);
			mapName = mapName.substr(p + 1);
		}
		auto l = mapName.find_last_of('.');
		if (l)
			mapName = mapName.substr(0, l);
	}

	Map::LoadConfig config;

	for (int i = 2; i < argc; i++)
	{
		if (!strcmp(argv[i], "-game"))
		{
			if (argc > i + 1)
			{
				i++;
				config.gamePath = argv[i];
			}
			else
			{
				printf("Warning: '-game' parameter requires a path\n");
			}
		}
		else if (!strcmp(argv[i], "-lm"))
		{
			if (argc > i + 1)
			{
				i++;
				config.lightmapSize = atoi(argv[i]);
			}
			else
			{
				printf("Warning: '-lm' parameter requires a number - lightmap atlas resolution\n");
			}
		}
		else if (!strcmp(argv[i], "-lstyle"))
		{
			if (argc > i + 1)
			{
				i++;
				if (!strcmp(argv[i], "all"))
					config.lstylesAll = true;
				else
					config.lstyle = atoi(argv[i]);
			}
			else
			{
				printf("Warning: '-lstyle' parameter requires a number - light style index, or a word 'all'\n");
			}
		}
		else if (!strcmp(argv[i], "-skip_sky"))
		{
			config.skipSky = true;
			printf("Sky polygons will be excluded from export\n");
		}
		else if (!strcmp(argv[i], "-uint16"))
		{
			config.uint16Inds = true;
			printf("Set indices type to uint16\n");
		}
		else if (!strcmp(argv[i], "-tex"))
		{
			config.allTextures = true;
			printf("All textures will be exported\n");
		}
		else
		{
			printf("Warning: unknown parameter \"%s\"\n", argv[i]);
		}
	}
	if (!config.lightmapSize)
		config.lightmapSize = 1024;

	if (config.gamePath.size() && config.gamePath.back() != '/' && config.gamePath.back() != '\\')
	{
		config.gamePath += '/';
	}

	std::string mapPath;
	if (strcasestr(argv[1], ".bsp") != nullptr)
	{
		mapPath = argv[1];
	}
	else
	{
		if (config.gamePath.size())
			mapPath = config.gamePath;

		mapPath += "maps/";
		mapPath += argv[1];
		mapPath += ".bsp";
	}

	Map map;
	if (!map.load(mapPath.c_str(), mapName.c_str(), &config))
	{
		fprintf(stderr, "Can't load map\n");
		return -1;
	}

	if (!gltf::ExportMap(mapName, map))
	{
		fprintf(stderr, "Export failed\n");
		return -1;
	}

	printf("Success\n");
	return 0;
}
