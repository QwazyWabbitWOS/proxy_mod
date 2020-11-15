// ProxyMod v1.03 - basic proxy game module for Quake II
// Copyright (c) 1998, by Avi Rozen (Zung!)
// e-mail: avrozen@ibm.net

// proxymod.h -- shared definitions for generic proxy module

#ifdef _WIN32
#define GAME_MODULE			"gamex86.dll"
#define WIN32_LEAN_AND_MEAN	//non-MFC
#if _MSC_VER > 1500
#pragma warning(disable : 4996)	// disable warnings from Visual Studio about deprecated CRT functions (_CRT_SECURE_NO_WARNINGS).
#endif /*_MSC_VER */
#endif

#define	GAME_API_VERSION	3

#define PROXYMODVERNUM   "1.03"
#define PROXYMODVERSTATUS "RELEASE"
#define PROXYMODPLATFORM "x86"
#define PROXYMODVERSION  PROXYMODVERNUM " " PROXYMODPLATFORM " " __DATE__ " " PROXYMODVERSTATUS

#include <windows.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

extern HMODULE hGameDLL;

#define	MAX_OSPATH			128
#define	MAX_CLIENTS			256

typedef unsigned char 		byte;
typedef enum { false, true }	qboolean;

#ifndef NULL
#define NULL ((void *)0)
#endif

// cvars:

#define	CVAR_ARCHIVE	1	
#define	CVAR_USERINFO	2	
#define	CVAR_SERVERINFO	4	
#define	CVAR_NOSET		8
#define	CVAR_LATCH		16

typedef struct cvar_s
{
	char*		name;
	char*		string;
	char*		latched_string;
	int			flags;
	qboolean	modified;
	float		value;
	struct cvar_s* next;
} cvar_t;


// edicts & clients:

typedef struct edict_s edict_t;
typedef struct gclient_s gclient_t;

typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec5_t[5];

typedef struct cplane_s
{
	vec3_t	normal;
	float	dist;
	byte	type;
	byte	signbits;
	byte	pad[2];
} cplane_t;

typedef struct csurface_s
{
	char		name[16];
	int			flags;
	int			value;
} csurface_t;

typedef struct
{
	qboolean	allsolid;
	qboolean	startsolid;
	float		fraction;
	vec3_t		endpos;
	cplane_t	plane;
	csurface_t* surface;
	int			contents;
	struct edict_s* ent;
} trace_t;


typedef enum
{
	PM_NORMAL,
	PM_SPECTATOR,
	PM_DEAD,
	PM_GIB,
	PM_FREEZE
} pmtype_t;


#define	PMF_DUCKED		1
#define	PMF_JUMP_HELD	2


typedef struct
{
	pmtype_t	pm_type;
	short		origin[3];
	short		velocity[3];
	byte		pm_flags;
	byte		pm_time;
	short		gravity;
	short		delta_angles[3];
} pmove_state_t;

#define	MAX_STATS				32

typedef struct
{
	pmove_state_t	pmove;
	vec3_t		viewangles;
	vec3_t		viewoffset;
	vec3_t		kick_angles;
	vec3_t		gunangles;
	vec3_t		gunoffset;
	int			gunindex;
	int			gunframe;
	float		blend[4];
	float		fov;
	int			rdflags;
	short		stats[MAX_STATS];
} player_state_t;

struct gclient_s
{
	player_state_t	ps;
	int	ping;
};


typedef struct entity_state_s
{
	int		number;
	vec3_t	origin;
	vec3_t	angles;
	vec3_t	old_origin;
	int		modelindex;
	int		modelindex2, modelindex3, modelindex4;
	int		frame;
	int		skinnum;
	int		effects;
	int		renderfx;
	int		solid;
	int		sound;
	int		event;
} entity_state_t;


typedef struct link_s
{
	struct link_s* prev, * next;
} link_t;

typedef enum
{
	SOLID_NOT,
	SOLID_TRIGGER,
	SOLID_BBOX,
	SOLID_BSP
} solid_t;

#define	MAX_ENT_CLUSTERS	16

struct edict_s
{
	entity_state_t	s;
	struct gclient_s* client;
	qboolean	inuse;
	int			linkcount;
	link_t		area;
	int			num_clusters;
	int			clusternums[MAX_ENT_CLUSTERS];
	int			headnode;
	int			areanum, areanum2;
	int			svflags;
	vec3_t		mins, maxs;
	vec3_t		absmin, absmax, size;
	solid_t		solid;
	int			clipmask;
	edict_t* owner;
};

#define	BUTTON_ATTACK		1
#define	BUTTON_USE			2
#define	BUTTON_ANY			128

typedef struct usercmd_s
{
	byte	msec;
	byte	buttons;
	short	angles[3];
	short	forwardmove, sidemove, upmove;
	byte	impulse;
	byte	lightlevel;
} usercmd_t;


#define	MAXTOUCH	32

typedef struct
{
	pmove_state_t	s;
	usercmd_t		cmd;
	qboolean		snapinitial;
	int			numtouch;
	struct edict_s* touchents[MAXTOUCH];
	vec3_t		viewangles;
	float		viewheight;
	vec3_t		mins, maxs;
	struct edict_s* groundentity;
	int			watertype;
	int			waterlevel;
	trace_t(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end);
	int			(*pointcontents) (vec3_t point);
} pmove_t;


// export/import:

#define	PRINT_LOW			0
#define	PRINT_MEDIUM		1
#define	PRINT_HIGH			2
#define	PRINT_CHAT			3

#define	ERR_FATAL			0
#define	ERR_DROP			1
#define	ERR_DISCONNECT		2

#define	PRINT_ALL			0
#define PRINT_DEVELOPER		1
#define PRINT_ALERT			2		

typedef enum
{
	MULTICAST_ALL,
	MULTICAST_PHS,
	MULTICAST_PVS,
	MULTICAST_ALL_R,
	MULTICAST_PHS_R,
	MULTICAST_PVS_R
} multicast_t;


typedef struct
{
	void	(*bprintf) (int printlevel, char* fmt, ...);
	void	(*dprintf) (char* fmt, ...);
	void	(*cprintf) (edict_t* ent, int printlevel, char* fmt, ...);
	void	(*centerprintf) (edict_t* ent, char* fmt, ...);
	void	(*sound) (edict_t* ent, int channel, int soundindex, float volume, float attenuation, float timeofs);
	void	(*positioned_sound) (vec3_t origin, edict_t* ent, int channel, int soundinedex, float volume, float attenuation, float timeofs);
	void	(*configstring) (int num, char* string);
	void	(*error) (char* fmt, ...);
	int		(*modelindex) (char* name);
	int		(*soundindex) (char* name);
	int		(*imageindex) (char* name);
	void	(*setmodel) (edict_t* ent, char* name);
	trace_t(*trace) (vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, edict_t* passent, int contentmask);
	int		(*pointcontents) (vec3_t point);
	qboolean(*inPVS) (vec3_t p1, vec3_t p2);
	qboolean(*inPHS) (vec3_t p1, vec3_t p2);
	void		(*SetAreaPortalState) (int portalnum, qboolean open);
	qboolean(*AreasConnected) (int area1, int area2);
	void	(*linkentity) (edict_t* ent);
	void	(*unlinkentity) (edict_t* ent);
	int		(*BoxEdicts) (vec3_t mins, vec3_t maxs, edict_t** list, int maxcount, int areatype);
	void	(*Pmove) (pmove_t* pmove);
	void	(*multicast) (vec3_t origin, multicast_t to);
	void	(*unicast) (edict_t* ent, qboolean reliable);
	void	(*WriteChar) (int c);
	void	(*WriteByte) (int c);
	void	(*WriteShort) (int c);
	void	(*WriteLong) (int c);
	void	(*WriteFloat) (float f);
	void	(*WriteString) (char* s);
	void	(*WritePosition) (vec3_t pos);
	void	(*WriteDir) (vec3_t pos);
	void	(*WriteAngle) (float f);
	void* (*TagMalloc) (int size, int tag);
	void	(*TagFree) (void* block);
	void	(*FreeTags) (int tag);
	cvar_t* (*cvar) (char* var_name, char* value, int flags);
	cvar_t* (*cvar_set) (char* var_name, char* value);
	cvar_t* (*cvar_forceset) (char* var_name, char* value);
	int		(*argc) (void);
	char* (*argv) (int n);
	char* (*args) (void);
	void	(*AddCommandString) (char* text);
	void	(*DebugGraph) (float value, int color);
} game_import_t;


typedef struct
{
	int			apiversion;
	void		(*Init) (void);
	void		(*Shutdown) (void);
	void		(*SpawnEntities) (char* mapname, char* entstring, char* spawnpoint);
	void		(*WriteGame) (char* filename, qboolean autosave);
	void		(*ReadGame) (char* filename);
	void		(*WriteLevel) (char* filename);
	void		(*ReadLevel) (char* filename);
	qboolean(*ClientConnect) (edict_t* ent, char* userinfo);
	void		(*ClientBegin) (edict_t* ent);
	void		(*ClientUserinfoChanged) (edict_t* ent, char* userinfo);
	void		(*ClientDisconnect) (edict_t* ent);
	void		(*ClientCommand) (edict_t* ent);
	void		(*ClientThink) (edict_t* ent, usercmd_t* cmd);
	void		(*RunFrame) (void);
	void		(*ServerCommand) (void);
	struct edict_s* edicts;
	int			edict_size;
	int			num_edicts;
	int			max_edicts;
} game_export_t;


// pm_main.c

extern game_import_t gi;
extern game_export_t ge;
extern game_import_t pmi;
extern game_export_t* pme;

extern cvar_t* pm_version;
extern cvar_t* basedir;
extern cvar_t* game;
extern cvar_t* proxy;
