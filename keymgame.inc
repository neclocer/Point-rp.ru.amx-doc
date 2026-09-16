/**
 * Module: KeyMGame
 * Author: Riccardo_Morreti
 *
 * Point Role Play (c) Point-rp.ru 2025
 */


#if defined KeyMGame
	#undef KeyMGame
#endif
#define KeyMGame. _keymgame

#if defined this
	#undef this
#endif
#define this. _keymgame

enum
{
	KEYMGAME_STATE_FAIL,
	KEYMGAME_STATE_FINISH,
	KEYMGAME_STATE_UPDATE
}

#define KEYMGAME_MAX_PROGRESS   20


private PlayerText:KeyMGame.textDraw[MAX_PLAYERS][6];


KeyMGame.Init ()
{
	Callback.Add(@OnPlayerConnect, #this.OnPlayerConnect);
	Callback.Add(@OnPlayerDisconnect, #this.OnPlayerDisconnect);
	Callback.Add(@OnClientSideKey, #this.OnClientSideKey);
	Callback.Add(@OnPlayerSpawn, #this.OnPlayerSpawn);
	Callback.Add(@OnKeyStateChange, #this.OnKeyStateChange);

	SetTimer(#this.OnTimer1sec, 1000, true);
}

CALLBACK KeyMGame.OnPlayerConnect (playerid)
{
	this.CreateTextDraws(playerid);
	LauncherSender.SendPacket(playerid, LauncherSender.update, "setMiniGameVisible(false)");
}

CALLBACK KeyMGame.OnPlayerDisconnect (playerid, reason)
{
	this.Fail(playerid);
}

CALLBACK KeyMGame.OnPlayerSpawn (playerid, skip)
{
	this.Fail(playerid);
	return skip;
}

CALLBACK KeyMGame.OnKeyStateChange (playerid, newkeys, oldkeys)
{
	if (newkeys & KEY_SECONDARY_ATTACK)
		KeyMGame.Fail(playerid);
}

CALLBACK KeyMGame.OnTimer1sec ()
{
	for (new i = GetPlayerPoolSize(); i >= 0; --i)
	{
		if (!IsPlayerLogin(i))
			continue;

		if (Launcher.IsPlayerUsePRPClient(i))
			continue;

		if (IsValidPVar(i, #this.miniGame))
		{
			SetPVarInt(i, #this.miniGameProgress, GetPVarInt(i, #this.miniGameProgress)-1);

			if (GetPVarInt(i, #this.miniGameProgress) <= 0) this.Fail(i);

			this.SetProgressTDValue(i, float(GetPVarInt(i, #this.miniGameProgress))*(100.0/KEYMGAME_MAX_PROGRESS));
		}
	}
}

CALLBACK KeyMGame.OnClientSideKey (playerid, keys)
{
	if (!IsValidPVar(playerid, #this.miniGame))
		return;

	if (Launcher.IsPlayerUsePRPClient(playerid))
		return;

	new key = GetPVarInt(playerid, #this.miniGameKey);

	this.UpdateKey(playerid);

	if (key != keys)
	{
		SetPVarInt(playerid, #this.miniGameProgress, GetPVarInt(playerid, #this.miniGameProgress)-2);
		return;
	}

	SetPVarInt(playerid, #this.miniGameProgress, GetPVarInt(playerid, #this.miniGameProgress)+1);

	if (GetPVarInt(playerid, #this.miniGameProgress) >= KEYMGAME_MAX_PROGRESS)
	{
		Callback.CallPublic(GetPVarInt(playerid, #this.miniGameCallback), playerid, KEYMGAME_STATE_FINISH);
		this.End(playerid);
	}
	else this.SetProgressTDValue(playerid, float(GetPVarInt(playerid, #this.miniGameProgress))*(100.0/KEYMGAME_MAX_PROGRESS));
}

CALLBACK KeyMGame.OnGameKeyFinish (playerid)
{
	SetPVarInt(playerid, #this.miniGameProgress, GetPVarInt(playerid, #this.miniGameProgress)+1);

	if (GetPVarInt(playerid, #this.miniGameProgress) >= GetPVarInt(playerid, #this.miniGameNeed)) {
		Callback.CallPublic(GetPVarInt(playerid, #this.miniGameCallback), playerid, KEYMGAME_STATE_FINISH);
		this.End(playerid);
	}
	else this.UpdateKey(playerid);
}


private Float:KeyMGame.GetProgressTDPercent (Float:value)
{
	return (259.4785+(value/100*(386.4162-259.4785)));
}

private KeyMGame.ShowTextDraws (playerid)
{
	if (!IsValidPVar(playerid, #this.miniGame))
		return;

	for (new i = 0; i < 5; i++)
		PlayerTextDrawShow(playerid, this.textDraw[playerid][i]);
}

private KeyMGame.HideTextDraws (playerid)
{
	if (!IsValidPVar(playerid, #this.miniGame))
		return;

	for (new i = 0; i < 5; i++)
		PlayerTextDrawHide(playerid, this.textDraw[playerid][i]);
}

private KeyMGame.SetProgressTDValue (playerid, Float:value)
{
	if (!IsValidPVar(playerid, #this.miniGame))
		return;

	value = (value < 0.0) ? (0.0) : (value > 100.0) ? (100.0) : (value);

	PlayerTextDrawUseBox(playerid, this.textDraw[playerid][2], value > 0.0);
	PlayerTextDrawTextSize(playerid, this.textDraw[playerid][2],
		this.GetProgressTDPercent(value),
		0.0);

	this.ShowTextDraws(playerid);
}

private KeyMGame.CreateTextDraws (playerid)
{
	this.textDraw[playerid][0] = CreatePlayerTextDraw(playerid, 261.0000, 360.4884, "Box"); // пусто
	PlayerTextDrawLetterSize(playerid, this.textDraw[playerid][0], 0.0000, 1.5750);
	PlayerTextDrawTextSize(playerid, this.textDraw[playerid][0], 387.0000, 0.0000);
	PlayerTextDrawAlignment(playerid, this.textDraw[playerid][0], 1);
	PlayerTextDrawColor(playerid, this.textDraw[playerid][0], -1);
	PlayerTextDrawUseBox(playerid, this.textDraw[playerid][0], 1);
	PlayerTextDrawBoxColor(playerid, this.textDraw[playerid][0], 488450047);
	PlayerTextDrawBackgroundColor(playerid, this.textDraw[playerid][0], 255);
	PlayerTextDrawFont(playerid, this.textDraw[playerid][0], 1);
	PlayerTextDrawSetProportional(playerid, this.textDraw[playerid][0], 1);
	PlayerTextDrawSetShadow(playerid, this.textDraw[playerid][0], 0);

	this.textDraw[playerid][1] = CreatePlayerTextDraw(playerid, 324.7500, 353.0220, "_"); // пусто
	PlayerTextDrawLetterSize(playerid, this.textDraw[playerid][1], 0.1413, 0.7505);
	PlayerTextDrawAlignment(playerid, this.textDraw[playerid][1], 2);
	PlayerTextDrawColor(playerid, this.textDraw[playerid][1], -1);
	PlayerTextDrawBackgroundColor(playerid, this.textDraw[playerid][1], 255);
	PlayerTextDrawFont(playerid, this.textDraw[playerid][1], 2);
	PlayerTextDrawSetProportional(playerid, this.textDraw[playerid][1], 1);
	PlayerTextDrawSetShadow(playerid, this.textDraw[playerid][1], 0);

	this.textDraw[playerid][2] = CreatePlayerTextDraw(playerid, 261.4500, 360.9888, "Box"); // пусто
	PlayerTextDrawLetterSize(playerid, this.textDraw[playerid][2], 0.0000, 1.4608);
	PlayerTextDrawTextSize(playerid, this.textDraw[playerid][2], 386.5385, 0.0000);
	PlayerTextDrawAlignment(playerid, this.textDraw[playerid][2], 1);
	PlayerTextDrawColor(playerid, this.textDraw[playerid][2], -1);
	PlayerTextDrawUseBox(playerid, this.textDraw[playerid][2], 1);
	PlayerTextDrawBoxColor(playerid, this.textDraw[playerid][2], -646512385);
	PlayerTextDrawBackgroundColor(playerid, this.textDraw[playerid][2], 255);
	PlayerTextDrawFont(playerid, this.textDraw[playerid][2], 1);
	PlayerTextDrawSetProportional(playerid, this.textDraw[playerid][2], 1);
	PlayerTextDrawSetShadow(playerid, this.textDraw[playerid][2], 0);

	this.textDraw[playerid][3] = CreatePlayerTextDraw(playerid, 314.5502, 376.6665, "Box"); // пусто
	PlayerTextDrawLetterSize(playerid, this.textDraw[playerid][3], 0.0000, 1.4999);
	PlayerTextDrawTextSize(playerid, this.textDraw[playerid][3], 332.8035, 0.0000);
	PlayerTextDrawAlignment(playerid, this.textDraw[playerid][3], 1);
	PlayerTextDrawColor(playerid, this.textDraw[playerid][3], -1);
	PlayerTextDrawUseBox(playerid, this.textDraw[playerid][3], 1);
	PlayerTextDrawBoxColor(playerid, this.textDraw[playerid][3], 488450047);
	PlayerTextDrawBackgroundColor(playerid, this.textDraw[playerid][3], 255);
	PlayerTextDrawFont(playerid, this.textDraw[playerid][3], 1);
	PlayerTextDrawSetProportional(playerid, this.textDraw[playerid][3], 1);
	PlayerTextDrawSetShadow(playerid, this.textDraw[playerid][3], 0);

	this.textDraw[playerid][4] = CreatePlayerTextDraw(playerid, 324.0000, 380.0887, "F"); // пусто
	PlayerTextDrawLetterSize(playerid, this.textDraw[playerid][4], 0.1692, 0.8159);
	PlayerTextDrawAlignment(playerid, this.textDraw[playerid][4], 2);
	PlayerTextDrawColor(playerid, this.textDraw[playerid][4], -1);
	PlayerTextDrawBackgroundColor(playerid, this.textDraw[playerid][4], 255);
	PlayerTextDrawFont(playerid, this.textDraw[playerid][4], 1);
	PlayerTextDrawSetProportional(playerid, this.textDraw[playerid][4], 1);
	PlayerTextDrawSetShadow(playerid, this.textDraw[playerid][4], 0);

	this.textDraw[playerid][5] = CreatePlayerTextDraw(playerid, 314.5502, 392.2674, "Box"); // пусто
	PlayerTextDrawLetterSize(playerid, this.textDraw[playerid][5], 0.0000, -1.9490);
	PlayerTextDrawTextSize(playerid, this.textDraw[playerid][5], 333.0000, 0.0000);
	PlayerTextDrawAlignment(playerid, this.textDraw[playerid][5], 1);
	PlayerTextDrawColor(playerid, this.textDraw[playerid][5], -1);
	PlayerTextDrawUseBox(playerid, this.textDraw[playerid][5], 1);
	PlayerTextDrawBoxColor(playerid, this.textDraw[playerid][5], -646512385);
	PlayerTextDrawBackgroundColor(playerid, this.textDraw[playerid][5], 255);
	PlayerTextDrawFont(playerid, this.textDraw[playerid][5], 1);
	PlayerTextDrawSetProportional(playerid, this.textDraw[playerid][5], 1);
	PlayerTextDrawSetShadow(playerid, this.textDraw[playerid][5], 0);
}


/*
 * API
 */
stock KeyMGame.Start (playerid, const name[], const callback[], needCount = 10)
{
	if (IsValidPVar(playerid, #this.miniGame))
		return false;

	SetPVarInt(playerid, #this.miniGame, 1);
	SetPVarInt(playerid, #this.miniGameCallback, Callback.GetAddress(callback));
	SetPVarInt(playerid, #this.miniGameNeed, needCount);
	SetPVarInt(playerid, #this.miniGameProgress, 0);

	if (Launcher.IsPlayerUsePRPClient(playerid)) this.UpdateKey(playerid);
	else
	{
		SetPVarInt(playerid, #this.miniGameProgress, 10);

		this.SetProgressTDValue(playerid, float(GetPVarInt(playerid, #this.miniGameProgress))*(100.0/KEYMGAME_MAX_PROGRESS));
		this.UpdateKey(playerid);

		PlayerTextDrawSetString(playerid, this.textDraw[playerid][1], name);

		for (new i = 0; i < 5; i++)
			PlayerTextDrawShow(playerid, this.textDraw[playerid][i]);
	}

	KeyArea.Disable(playerid);
	TogglePlayerControllable(playerid, false);

	return true;
}

stock KeyMGame.UpdateKey (playerid)
{
	if (!IsValidPVar(playerid, #this.miniGame))
		return;

	if (Launcher.IsPlayerUsePRPClient(playerid))
	{
		new key[][] = {"Q", "W", "E", "A", "S", "D", "C", "SHIFT", "CTRL", "SPACE"};
		new rand = random(sizeof(key));
		new antiWhile;

		while (GetPVarInt(playerid, #this.lastKey) == rand)
		{
			if (antiWhile >= 50)
				assert(printf("[KeyMGame] Loop detected, lastkey = %d, rand = %d", GetPVarInt(playerid, #this.lastKey), rand));

			rand = random(sizeof(key));
			antiWhile++;
		}

		LauncherSender.SendPacket(playerid, LauncherSender.update, "setMiniGameVisible(true, '%s')", key[rand]);
		SetPVarInt(playerid, #this.lastKey, rand);
	}
	else
	{
		new key[][] = {"E", "SHIFT", "O", "L"};
		new rand = random(sizeof(key));

		switch (rand) {
			case 0: SetPVarInt(playerid, #this.miniGameKey, 69);
			case 1: SetPVarInt(playerid, #this.miniGameKey, 16);
			case 2: SetPVarInt(playerid, #this.miniGameKey, 79);
			case 3: SetPVarInt(playerid, #this.miniGameKey, 76);
		}

		PlayerTextDrawSetString(playerid, this.textDraw[playerid][4], key[rand]);
	}

	Callback.CallPublic(GetPVarInt(playerid, #this.miniGameCallback), playerid, KEYMGAME_STATE_UPDATE);
}

stock KeyMGame.End (playerid)
{
	if (!IsValidPVar(playerid, #this.miniGame))
		return;

	this.HideTextDraws(playerid);

	DeletePVar(playerid, #this.miniGame);
	DeletePVar(playerid, #this.miniGameKey);
	DeletePVar(playerid, #this.miniGameProgress);

	TogglePlayerControllable(playerid, true);

	LauncherSender.SendPacket(playerid, LauncherSender.update, "setMiniGameVisible(false)");
	KeyArea.Enable(playerid);
}

stock KeyMGame.Fail (playerid)
{
	if (!IsValidPVar(playerid, #this.miniGame))
		return;

	Callback.CallPublic(GetPVarInt(playerid, #this.miniGameCallback), playerid, KEYMGAME_STATE_FAIL);
	this.End(playerid);
}

stock KeyMGame.IsEnable (playerid)
{
	return IsValidPVar(playerid, #this.miniGame);
}


CALLBACK OnModeInit ()
{
    this.Init();
    #if defined KeyMGame@OnModeInit
    KeyMGame@OnModeInit();
    #endif
}

#if defined KeyMGame@OnModeInit
    forward KeyMGame@OnModeInit();
#endif
#if defined _ALS_OnModeInit
    #undef OnModeInit
#else
    #define _ALS_OnModeInit
#endif
#define OnModeInit KeyMGame@OnModeInit