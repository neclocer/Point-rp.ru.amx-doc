/**
 * Module: ActorSay
 * Author: Riccardo_Morreti
 * 
 * Point Role Play (c) Point-rp.ru 2026
 */


#if defined ActorSay
 	#undef ActorSay
#endif
#define ActorSay. _actorSay

#if defined this
 	#undef this
#endif
#define this. _actorSay


#define ASAY_MAX_TALKS	100


enum ActorSay.EDataInfo
{
	bool:ActorSay.isUsed,
	ActorSay.actorId,
	ActorSay.actorName[MAX_PLAYER_NAME],
	ActorSay.textColor,
	ActorSay.text[144],
	ActorSay.callbackAddress
}
private ActorSay.data[MAX_PLAYERS][ASAY_MAX_TALKS][ActorSay.EDataInfo];
private ActorSay.timer[MAX_PLAYERS] = {-1, ...};



stock ActorSay.Add (playerid, actorid, const actorName[], color, const message[], event_address = 0)
{
	for (new i = 0; i < ASAY_MAX_TALKS; i++)
	{
		if (this.data[playerid][i][this.isUsed])
			continue;

		this.data[playerid][i][this.isUsed] = true;
		this.data[playerid][i][this.actorId] = actorid;
		this.data[playerid][i][this.textColor] = color;
		this.data[playerid][i][this.callbackAddress] = event_address;

		strmid(this.data[playerid][i][this.actorName], actorName, 0, strlen(actorName), MAX_PLAYER_NAME);
		strmid(this.data[playerid][i][this.text], message, 0, strlen(message), 144);
		break;
	}
}

stock ActorSay.Start (playerid, actorid = INVALID_STREAMER_ID)
{
	for (new i = 0; i < ASAY_MAX_TALKS; i++)
	{
		if (!this.data[playerid][i][this.isUsed])
			continue;

		if (actorid != INVALID_STREAMER_ID && this.data[playerid][i][this.actorId] != actorid)
			continue;

		SendFormatMsg(playerid, this.data[playerid][i][this.textColor], "- %s: %s", this.data[playerid][i][this.actorName], this.data[playerid][i][this.text]);

		new address = this.data[playerid][i][this.callbackAddress];
		if (address)
			Callback.CallPublic(address, playerid, i);

		if (this.timer[playerid] != -1)
			KillTimer(this.timer[playerid]);

		this.timer[playerid] = SetTimerEx(#this.OnTimerEnd, (strlen(this.data[playerid][i][this.text])/40)*1000, false, "ii", playerid, actorid);

		this.data[playerid][i][this.isUsed] = false;
		this.data[playerid][i][this.actorId] = INVALID_STREAMER_ID;
		this.data[playerid][i][this.textColor] = -1;

		this.data[playerid][i][this.actorName][0] = EOS;
		this.data[playerid][i][this.text][0] = EOS;

		this.data[playerid][i][this.callbackAddress] = 0;

		Anims.ActorPlay(actorid, "PED", "IDLE_chat", 4.100, true, true, true, true, 1);

		return;
	}

	if (this.timer[playerid] != -1)
		KillTimer(this.timer[playerid]);

	this.timer[playerid] = -1;
}

CALLBACK ActorSay.OnTimerEnd (playerid, actorid)
{
	Anims.ActorPlay(actorid, "PED", "IDLE_CHAT", 4.1, false, true, true, false, 1);

	this.Start(playerid, actorid);
}


public OnPlayerDisconnect(playerid, reason)
{
	if (this.timer[playerid] != -1)
		KillTimer(this.timer[playerid]);

	this.timer[playerid] = -1;

	for (new i = 0; i < ASAY_MAX_TALKS; i++)
	{
		this.data[playerid][i][this.isUsed] = false;
		this.data[playerid][i][this.actorId] = INVALID_STREAMER_ID;
		this.data[playerid][i][this.textColor] = -1;

		this.data[playerid][i][this.actorName][0] = EOS;
		this.data[playerid][i][this.text][0] = EOS;

		this.data[playerid][i][this.callbackAddress] = 0;
	}
	
    #if defined ActorSay@OnPlayerDisconnect
   	ActorSay@OnPlayerDisconnect(playerid, reason);
    #endif
    return 1;
}
#if defined _ALS_OnPlayerDisconnect
    #undef OnPlayerDisconnect
#else
    #define _ALS_OnPlayerDisconnect
#endif
#define OnPlayerDisconnect ActorSay@OnPlayerDisconnect
#if defined ActorSay@OnPlayerDisconnect
forward ActorSay@OnPlayerDisconnect(playerid, reason);
#endif