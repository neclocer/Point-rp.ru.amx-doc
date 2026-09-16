/**
 * Module: LauncherSender
 * Author: Riccardo_Morreti
 * 
 * Point Role Play (c) Point-rp.ru 2025
 */


#if defined LauncherSender
 	#undef LauncherSender
#endif
#define LauncherSender. _lchrsender

#if defined this
 	#undef this
#endif
#define this. _lchrsender


enum LauncherSender.ETypes
{
	LauncherSender.open = 4,
	LauncherSender.close = 5,
	LauncherSender.update = 2
};

stock LauncherSender.SendPacket (playerid, LauncherSender.ETypes:type, const text[], {Float, _}:...)
{
	static args, buffer[2048];

	if ((args = numargs()) == 3)
	{
		this.ExecutePacket(playerid, type, text);
		return true;
	}

	while (--args >= 3)
	{
		#emit LCTRL 5
		#emit LOAD.alt args
		#emit SHL.C.alt 2
		#emit ADD.C 12
		#emit ADD
		#emit LOAD.I
		#emit PUSH.pri
	}
	#emit PUSH.S text
	#emit PUSH.C 2048
	#emit PUSH.C buffer
	#emit PUSH.S 8
	#emit SYSREQ.C format
	#emit LCTRL 5
	#emit SCTRL 4

	this.ExecutePacket(playerid, type, buffer);

	#emit RETN
	return true;
}

stock LauncherSender.ExecutePacket (playerid, LauncherSender.ETypes:type, const buffer[], const buffer2[] = "")
{
	new bool:sPacket;

	if (type == this.ETypes:this.open || type == this.ETypes:this.close)
		sPacket = true;

	BS_Reset(LauncherData.bs);
    BS_WriteValue(LauncherData.bs,
		PR_UINT8, LAUNCHER_PACKET_ID,
		PR_UINT16, 2,
		PR_UINT32, this.ETypes:type,
		PR_UINT8, (!sPacket ? 1 : 2),
		PR_STRING32, buffer);

	if (sPacket != false)
		BS_WriteValue(LauncherData.bs, PR_STRING32, buffer2);

    PR_SendPacket(LauncherData.bs, playerid);
}