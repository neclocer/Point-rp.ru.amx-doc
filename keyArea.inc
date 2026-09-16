/**
 * Module: KeyArea
 * Author: Donny_Hayes
 *
 * Samp Role Play (c) Samp-rp.ru 2017
 */


#if defined KeyArea
	#undef KeyArea
#endif
#define KeyArea. _kar

#if defined this
	#undef this
#endif
#define this. _kar


#if !defined RELEASED
	#define RELEASED(%0) \
		(((newkeys & (%0)) != (%0)) && ((oldkeys & (%0)) == (%0)))
#endif


#define KAR_KEY_ID   E_STREAMER_CUSTOM(0xFF)
#define KAR_CALLBACK E_STREAMER_CUSTOM(0xAF)
#define KAR_HAVE_KEY E_STREAMER_CUSTOM(0xFA)
#define KAR_ONLY_VEH E_STREAMER_CUSTOM(0xAA)


enum {
	KAR_TYPE_RECT,
	KAR_TYPE_POLYGON,
	KAR_TYPE_CUBE,
	KAR_TYPE_SPHERE
};

enum KeyArea.E_INFO
{
	KeyArea.keyId,
	KeyArea.name[32],
	KeyArea.vehName[32],
	// LAUNCHER
	KeyArea.clientKey[32],
	KeyArea.vehClientKey[32]
}

private KeyArea.arrayKeys[][KeyArea.E_INFO] = {
	{KEY_ACTION, "~k~~PED_ANSWER_PHONE~", "~k~~VEHICLE_FIREWEAPON_ALT~", 		 "TAB", "TAB"},
	{KEY_CROUCH, "~k~~PED_DUCK~", "~k~~VEHICLE_HORN~", 							 "C", "H"},
	{KEY_FIRE, "~k~~PED_FIREWEAPON~", "~k~~VEHICLE_FIREWEAPON~", 				 "CTRL", "CTRL"},
	{KEY_SPRINT, "~k~~PED_SPRINT~", "~k~~VEHICLE_ACCELERATE~", 					 "SHIFT", "SHIFT"},
	{KEY_SECONDARY_ATTACK, "~k~~VEHICLE_ENTER_EXIT~", "~k~~VEHICLE_ENTER_EXIT~", "ENTER", "ENTER"},
	{KEY_JUMP, "~k~~PED_JUMPING~", "~k~~VEHICLE_BRAKE~", 						 "SPACE", "SPACE"},
	{KEY_LOOK_RIGHT, "", "~k~~VEHICLE_LOOKRIGHT~", 								 ""},
	{KEY_HANDBRAKE, "~k~~PED_LOCK_TARGET~", "~k~~VEHICLE_HANDBRAKE~", 			 "RMB", "RMB"},
	{KEY_WALK, "~k~~SNEAK_ABOUT~", "", 											 "ALT", "ALT"}
};

private PlayerText:KeyArea.TDArea[MAX_PLAYERS];

#define KAR_MAX_SLOTS 250
private KeyArea.slots[KAR_MAX_SLOTS] = {-1, ...};


private KeyArea.Init()
{
	Callback.Add(@OnKeyStateChange, #this.OnKeyStateChange);
	Callback.Add(@OnPlayerConnect, #this.OnPlayerConnect);
	Callback.Add(@OnEnterDynamicArea, #this.OnEnterDynamicArea);
	Callback.Add(@OnLeaveDynamicArea, #this.OnLeaveDynamicArea);
}


CALLBACK KeyArea.OnKeyStateChange(playerid, newkeys, oldkeys)
{
	if (!IsValidPVar(playerid, #this.area)) {
		return;
	}

	if (Launcher.IsPlayerUsePRPClient(playerid))
		return;

	new areaid = GetPVarInt(playerid, #this.area);
	if (IsValidDynamicArea(areaid) && IsPlayerInDynamicArea(playerid, areaid))
	{
		new this.key, this.haveKey, this.onlyVehicle;

		this.key = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_KEY_ID);
		this.haveKey = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY);
		this.onlyVehicle = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_ONLY_VEH);

		if (this.onlyVehicle && GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
			return;

		if ((RELEASED(this.key)) && this.haveKey != -1)
			this.OnKeyPressed(playerid);
	}
}

CALLBACK KeyArea.OnEnterDynamicArea(playerid, areaid)
{
	if (IsValidDynamicArea(areaid) && IsPlayerInDynamicArea(playerid, areaid))
	{
		for (new i = 0; i < KAR_MAX_SLOTS; i++) {
			if (this.slots[i] == areaid) {
				if (Streamer_HasIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY)) {
					new this.key, this.haveKey, this.onlyVehicle;

					this.key = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_KEY_ID);
					this.haveKey = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY);
					this.onlyVehicle = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_ONLY_VEH);

					if (this.haveKey != -1)
					{
						new buffer[60];

						new index = this.FindIndexByKey(this.key);
						if (index == -1) {
							return;
						}

						if (Launcher.IsPlayerUsePRPClient(playerid))
						{
							if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER || GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || this.onlyVehicle)
								LauncherSender.SendPacket(playerid, LauncherSender.update, "setKeyAreaVisible(true, '%s')", this.arrayKeys[index][this.vehClientKey]);
							else
								LauncherSender.SendPacket(playerid, LauncherSender.update, "setKeyAreaVisible(true, '%s')", this.arrayKeys[index][this.clientKey]);
						}
						else {
							if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER || GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || this.onlyVehicle)
								format(buffer, sizeof(buffer), "~w~PRESS~b~ %s", this.arrayKeys[index][this.vehName]);
							else
								format(buffer, sizeof(buffer), "~w~PRESS~b~ %s", this.arrayKeys[index][this.name]);
						}
						
						PlayerTextDrawSetString(playerid, this.TDArea[playerid], buffer);
						PlayerTextDrawShow(playerid, this.TDArea[playerid]);

						SetPVarInt(playerid, #this.area, areaid);
						SetPVarInt(playerid, #this.isEnable, 1);
					}
				}
			}
		}
	}
}

CALLBACK KeyArea.OnLeaveDynamicArea(playerid, areaid)
{
	if (IsValidDynamicArea(areaid))
	{
		for (new i = 0; i < KAR_MAX_SLOTS; i++) {
			if (this.slots[i] == areaid) {
				if (Streamer_HasIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY)) {
					new this.haveKey;
					this.haveKey = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY);

					if (this.haveKey != -1) {
						if (Launcher.IsPlayerUsePRPClient(playerid)) LauncherSender.SendPacket(playerid, LauncherSender.update, "setKeyAreaVisible(false)");
						else PlayerTextDrawHide(playerid, this.TDArea[playerid]);

						DeletePVar(playerid, #this.area);
						DeletePVar(playerid, #this.isEnable);
					}
				}
			}
		}
	}
}

CALLBACK KeyArea.OnPlayerConnect(playerid)
{
	this.TDArea[playerid] = CreatePlayerTextDraw(playerid, 320.000000, 351.000000, "~w~PRESS~w~ ");//~k~~VEHICLE_ENTER_EXIT~
	PlayerTextDrawLetterSize(playerid, this.TDArea[playerid], 0.470000, 2.500000);
	PlayerTextDrawTextSize(playerid, this.TDArea[playerid], 1280.000000, 1280.000000);
	PlayerTextDrawAlignment(playerid, this.TDArea[playerid], 2);
	PlayerTextDrawColor(playerid, this.TDArea[playerid], 0xFFFFFFFFFFFFFFFF);
	PlayerTextDrawUseBox(playerid, this.TDArea[playerid], false);
	PlayerTextDrawBoxColor(playerid, this.TDArea[playerid], 0xFFFFFFFF80808080);
	PlayerTextDrawSetShadow(playerid, this.TDArea[playerid], 2);
	PlayerTextDrawSetOutline(playerid, this.TDArea[playerid], 1);
	PlayerTextDrawBackgroundColor(playerid, this.TDArea[playerid], 0x000000FF);
	PlayerTextDrawFont(playerid, this.TDArea[playerid], 1);
	PlayerTextDrawSetProportional(playerid, this.TDArea[playerid], true);
	PlayerTextDrawSetSelectable(playerid, this.TDArea[playerid], false);
}


private KeyArea.FindIndexByKey (keyId) {
	for (new i = 0; i < sizeof(this.arrayKeys); i++) {
		if (this.arrayKeys[i][this.keyId] == keyId) {
			return i;
		}
	}

	return -1;
}


/*
 * API
 */

stock KeyArea.Create(const callback[], const Float:points[], type = KAR_TYPE_SPHERE, key, worldid = 0, intid = 0, Float:minz = -FLOAT_INFINITY, Float:maxz = FLOAT_INFINITY, size = sizeof(points), bool:onlyVehicle = false)
{
	new areaid = INVALID_STREAMER_ID;
	new slot = -1;

	for (new i = 0; i < KAR_MAX_SLOTS; i++) {
		if (this.slots[i] == -1) {
			slot = i;
			break;
		}
	}

	if (slot == -1)
		assert(print("[KeyArea.Create] slot == INVALID_STREAMER_ID"));

	switch (type) {
		case KAR_TYPE_RECT: areaid = CreateDynamicRectangle(points[0], points[1], points[2], points[3], worldid, intid);
		case KAR_TYPE_POLYGON: areaid = CreateDynamicPolygon(points, minz, maxz, size, worldid, intid);
		case KAR_TYPE_CUBE: areaid = CreateDynamicCuboid(points[0], points[1], minz, points[2], points[3], maxz, worldid, intid);
		case KAR_TYPE_SPHERE: areaid = CreateDynamicSphere(points[0], points[1], points[2], points[3], worldid, intid);
	}

	if (areaid == INVALID_STREAMER_ID) {
		return INVALID_STREAMER_ID;
	}

	if (!IsValidDynamicArea(areaid)) {
		return INVALID_STREAMER_ID;
	}
	
	Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, KAR_KEY_ID, key);
	Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY, 1);
	Streamer_SetIntData(STREAMER_TYPE_AREA, areaid, KAR_ONLY_VEH, onlyVehicle ? 1 : 0);
	Streamer_SetArrayData(STREAMER_TYPE_AREA, areaid, KAR_CALLBACK, callback, 32);

	this.slots[slot] = areaid;
	return areaid;
}

stock KeyArea.Enable (playerid) {
	if (!IsValidPVar(playerid, #this.area))
		return;

	new areaid = GetPVarInt(playerid, #this.area);
	if (IsValidDynamicArea(areaid) && IsPlayerInDynamicArea(playerid, areaid))
	{
		for (new i = 0; i < KAR_MAX_SLOTS; i++) {
			if (this.slots[i] == areaid) {
				if (Streamer_HasIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY)) {
					new this.key, this.haveKey, this.onlyVehicle;

					this.key = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_KEY_ID);
					this.haveKey = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_HAVE_KEY);
					this.onlyVehicle = Streamer_GetIntData(STREAMER_TYPE_AREA, areaid, KAR_ONLY_VEH);

					if (this.haveKey != -1)
					{
						new buffer[60];

						new index = this.FindIndexByKey(this.key);
						if (index == -1) {
							continue;
						}

						if (Launcher.IsPlayerUsePRPClient(playerid)) {
							if (this.onlyVehicle && (GetPlayerState(playerid) != PLAYER_STATE_DRIVER || GetPlayerState(playerid) != PLAYER_STATE_PASSENGER))
								continue;

							LauncherSender.SendPacket(playerid, LauncherSender.update, "setKeyAreaVisible(true, '%s')", this.arrayKeys[index][this.clientKey]);
						}
						else {
							if (GetPlayerState(playerid) == PLAYER_STATE_DRIVER || GetPlayerState(playerid) == PLAYER_STATE_PASSENGER || this.onlyVehicle)
								format(buffer, sizeof(buffer), "~w~PRESS~b~ %s", this.arrayKeys[index][this.vehName]);
							else
								format(buffer, sizeof(buffer), "~w~PRESS~b~ %s", this.arrayKeys[index][this.name]);

							PlayerTextDrawSetString(playerid, this.TDArea[playerid], buffer);
							PlayerTextDrawShow(playerid, this.TDArea[playerid]);
						}

						SetPVarInt(playerid, #this.area, areaid);
						break;
					}
				}
			}
		}
	}

	SetPVarInt(playerid, #this.isEnable, 1);
}

stock KeyArea.Disable (playerid) {
	if (!IsValidPVar(playerid, #this.area))
		return;

	if (Launcher.IsPlayerUsePRPClient(playerid)) LauncherSender.SendPacket(playerid, LauncherSender.update, "setKeyAreaVisible(false)");
	else PlayerTextDrawHide(playerid, this.TDArea[playerid]);

	DeletePVar(playerid, #this.isEnable);
}

stock KeyArea.IsEnable (playerid)
{
	return IsValidPVar(playerid, #this.isEnable);
}

stock KeyArea.OnKeyPressed (playerid) {
	if (!IsValidPVar(playerid, #this.area))
		return;

	if (player_DialogID[playerid] != -1)
		return;

	if (!this.IsEnable(playerid))
		return;

	new areaid = GetPVarInt(playerid, #this.area);
	new callback[32];

	Streamer_GetArrayData(STREAMER_TYPE_AREA, areaid, KAR_CALLBACK, callback);

	if (strlen(callback))
		CallLocalFunction(callback, "dd", playerid, areaid);
}


CALLBACK OnModeInit ()
{
	this.Init();
	#if defined KeyArea@OnModeInit
	KeyArea@OnModeInit();
	#endif
}
#if defined KeyArea@OnModeInit
	forward KeyArea@OnModeInit();
#endif
#if defined _ALS_OnModeInit
	#undef OnModeInit
#else
	#define _ALS_OnModeInit
#endif
#define OnModeInit KeyArea@OnModeInit