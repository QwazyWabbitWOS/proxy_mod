// ProxyMod v1.03 - basic proxy game module for Quake II
// Copyright (c) 1998, by Avi Rozen (Zung!)
// e-mail: avrozen@ibm.net

#include "proxymod.h"

HMODULE hGameDLL;

game_import_t	pmi; // original import functions (used by proxy)
game_export_t*	pme;// export functions returned to server
game_import_t	gi;  // import functions to be used by game/next proxy module
game_export_t	ge;  // export functions returned from game/next proxy module

cvar_t* pm_version;
cvar_t* basedir;
cvar_t* game;
cvar_t* proxy;
cvar_t* nextproxy;

void ShutdownProxyMod(void)
{
	ge.Shutdown(); // shutdown game DLL
	FreeLibrary(hGameDLL);
	pmi.dprintf("=== Shutdown (ProxyMod) ===\n");
}


void InitProxyMod(void)
{
	pmi.dprintf("==== Init (ProxyMod) ====\n");
	ge.Init();
}


void LoadGameModule(char* game_basedir, char* game_dir)
{
	char GameLibPath[MAX_OSPATH];

	if (game_dir == NULL || strlen(game_dir) == 0 || strchr(game_dir, '\\') != NULL || strchr(game_dir, '/') != NULL || strstr(game_dir, "..") != NULL) {
		pmi.cvar_forceset("nextproxy", "");
		sprintf(GameLibPath, "%s/baseq2/" GAME_MODULE, basedir->string);
		pmi.dprintf("...loading default game module \"%s\": ", GameLibPath);
	}
	else
		if (strlen(game_basedir) != 0) {
			sprintf(GameLibPath, "%s/%s/%s/" GAME_MODULE, basedir->string, game_basedir, game_dir);
			pmi.dprintf("...loading proxy module \"%s\": ", GameLibPath);
		}
		else {
			sprintf(GameLibPath, "%s/%s/" GAME_MODULE, basedir->string, game_dir);
			pmi.dprintf("...loading game module \"%s\": ", GameLibPath);
		}
	hGameDLL = LoadLibrary(GameLibPath);
	if (hGameDLL != NULL)
		pmi.dprintf("ok\n");
	else {
		pmi.dprintf("failed\n");
		if (strlen(game_basedir) != 0) { // proxy chain aborted - load game
			pmi.cvar_forceset("nextproxy", "");
			LoadGameModule("", game->string);
			return;
		}
		if (strlen(game_dir) == 0)    // failed to load default game module
			return;
		LoadGameModule("", ""); // attempt to load default module
	}
	return;
}



typedef game_export_t* (*GetGameAPI_t) (game_import_t*);

game_export_t* GetGameAPI(game_import_t* gimport)
{
	char CurrentProxy[MAX_OSPATH];
	char NextProxy[MAX_OSPATH];
	char* LoopProxy;
	char* colon;
	int nextcolon;
	GetGameAPI_t GetGameAPI_f;

	memcpy(&pmi, gimport, sizeof(game_import_t));

	pmi.dprintf("\nProxyMod v" PROXYMODVERNUM " " PROXYMODVERSTATUS "\nCopyright (c) 1998, by Avi Rozen (Zung!)\ne-mail: avrozen@ibm.net\n\n");

	pm_version = pmi.cvar("pm_version", PROXYMODVERSION, CVAR_NOSET);
	basedir = pmi.cvar("basedir", ".", CVAR_NOSET);
	game = pmi.cvar("game", "", CVAR_SERVERINFO | CVAR_LATCH);
	proxy = pmi.cvar("proxy", "", CVAR_SERVERINFO | CVAR_LATCH);
	nextproxy = pmi.cvar("nextproxy", "", CVAR_NOSET);

	pmi.cvar_forceset("pm_version", PROXYMODVERSION);

	CurrentProxy[0] = '\0';
	NextProxy[0] = '\0';

	if ((proxy->string[0] != '\0') && (nextproxy->string[0] == '\0'))
		pmi.cvar_forceset("nextproxy", proxy->string);
	if (nextproxy->string[0] != '\0' && strcmp(nextproxy->string, ":") != 0) {
		if ((colon = strchr(nextproxy->string, ':')) == NULL)
			strcpy(CurrentProxy, nextproxy->string);
		else {
			strncpy(CurrentProxy, nextproxy->string, colon - nextproxy->string);
			CurrentProxy[min(MAX_OSPATH - 1, colon - nextproxy->string)] = '\0';
			// prevent infinite loops
			if ((LoopProxy = strstr(colon + 1, CurrentProxy)) != NULL) {
				// weird stuff: win95 considers "folder  " and "folder" to be the same
				// while "  folder" and "folder" are different (should be different in
				// both cases!)
				nextcolon = strlen(CurrentProxy);
				while (LoopProxy[nextcolon] && isspace(LoopProxy[nextcolon]))
					nextcolon++;
				if ((LoopProxy[nextcolon] == ':' ||
					LoopProxy[nextcolon] == '\0') &&
					(NextProxy[LoopProxy - NextProxy - 1] == ':')) {
					pmi.dprintf("Warning: found a loop in proxy chain!\n");
					colon--; // hack: this will cause next proxy load to fail
				}
			}
			strcpy(NextProxy, colon + 1);
		}
		if (NextProxy[0] != '\0')
			pmi.cvar_forceset("nextproxy", NextProxy);
		else
			pmi.cvar_forceset("nextproxy", ":");
	}
	if ((CurrentProxy[0] != '\0') && strcmp(CurrentProxy, ".."))
		LoadGameModule("proxy", CurrentProxy);
	else {
		pmi.cvar_forceset("nextproxy", "");
		LoadGameModule("", game->string);
	}


	if (hGameDLL == NULL) {
		pmi.error("game module not found");
		return NULL;
	}

	GetGameAPI_f = (GetGameAPI_t)GetProcAddress(hGameDLL, "GetGameAPI");

	if (GetGameAPI_f == NULL) {
		pmi.error("can't get game API");
		return NULL;
	}

	memcpy(&gi, gimport, sizeof(game_import_t));
	// to capture API calls from game module change gi to point
	// to local functions (i.e. gi.dprintf = proxy_dprintf;)

	pme = GetGameAPI_f(&gi);

	if (strcmp(nextproxy->string, ":") == 0)
		pmi.cvar_forceset("nextproxy", "");

	memcpy(&ge, pme, sizeof(game_export_t));
	// to capture API calls from server change pme to point
	// to local functions (i.e. pme->ClientConnect = proxy_ClientConnect;)

	pme->Init = InitProxyMod;
	pme->Shutdown = ShutdownProxyMod;

	return pme;
}
