/**
 * Module: LauncherData
 * Author: Riccardo_Morreti
 * 
 * Point Role Play (c) Point-rp.ru 2025
 */


#if defined LauncherData
 	#undef LauncherData
#endif
#define LauncherData. _lchrdata

#if defined this
 	#undef this
#endif
#define this. _lchrdata


#define LAUNCHER_PACKET_ID 					215
#define LAUNCHER_CLIENT_VERSION				"PRP-1"
#define LAUNCHER_IS_PRP_CLIENT  			"IsPRPClient"


private LauncherData.KEY_CODE_TAB 			= 9;
private LauncherData.KEY_CODE_ENTER 		= 13;
private LauncherData.KEY_CODE_SHIFT 		= 16;
private LauncherData.KEY_CODE_CTRL 			= 17;
private LauncherData.KEY_CODE_ALT 			= 18;
private LauncherData.KEY_CODE_CAPS_LOCK 	= 20;
private LauncherData.KEY_CODE_ESC 			= 27;
private LauncherData.KEY_CODE_SPACE 		= 32;
private LauncherData.KEY_CODE_PAGE_UP 		= 33;
private LauncherData.KEY_CODE_PAGE_DOWN 	= 34;
private LauncherData.KEY_CODE_ARROW_LEFT 	= 37;
private LauncherData.KEY_CODE_ARROW_TOP 	= 38;
private LauncherData.KEY_CODE_ARROW_RIGHT 	= 39;
private LauncherData.KEY_CODE_ARROW_BOTTOM 	= 40;
private LauncherData.KEY_CODE_1 			= 49;
private LauncherData.KEY_CODE_2 			= 50;
private LauncherData.KEY_CODE_A 			= 65;
private LauncherData.KEY_CODE_B 			= 66;
private LauncherData.KEY_CODE_C 			= 67;
private LauncherData.KEY_CODE_D 			= 68;
private LauncherData.KEY_CODE_E 			= 69;
private LauncherData.KEY_CODE_F 			= 70;
private LauncherData.KEY_CODE_H 			= 72;
private LauncherData.KEY_CODE_I 			= 73;
private LauncherData.KEY_CODE_J 			= 74;
private LauncherData.KEY_CODE_K 			= 75;
private LauncherData.KEY_CODE_L 			= 76;
private LauncherData.KEY_CODE_M 			= 77;
private LauncherData.KEY_CODE_N 			= 78;
private LauncherData.KEY_CODE_O 			= 79;
private LauncherData.KEY_CODE_P 			= 80;
private LauncherData.KEY_CODE_Q 			= 81;
private LauncherData.KEY_CODE_R 			= 82;
private LauncherData.KEY_CODE_S 			= 83;
private LauncherData.KEY_CODE_T 			= 84;
private LauncherData.KEY_CODE_U 			= 85;
private LauncherData.KEY_CODE_W 			= 87;
private LauncherData.KEY_CODE_X 			= 88;
private LauncherData.KEY_CODE_Y 			= 89;
private LauncherData.KEY_CODE_Z 			= 90;
private LauncherData.KEY_CODE_NUMPAD_1 		= 97;
private LauncherData.KEY_CODE_F1 			= 112;
private LauncherData.KEY_CODE_F2 			= 113;
private LauncherData.KEY_CODE_F5 			= 116;
private LauncherData.KEY_CODE_F6 			= 117;
private LauncherData.KEY_CODE_F7 			= 118;
private LauncherData.KEY_CODE_F8 			= 119;
private LauncherData.KEY_CODE_COMMA 		= 188;
private LauncherData.KEY_CODE_PERIOD 		= 190;


enum LauncherData.CBInfo
{
	LauncherData.cbId,
	LauncherData.cbName[32]
};

enum
{
	LAUNCHER_CALLBACK_AUTH,
	LAUNCHER_CALLBACK_TAKE_BONUS,
	LAUNCHER_CALLBACK_REG,
	LAUNCHER_CALLBACK_GENDER,
	LAUNCHER_CALLBACK_REG_SKIN,
	LAUNCHER_CALLBACK_AUTH_CODE,
	LAUNCHER_CALLBACK_FLIP_RECON,
	LAUNCHER_CALLBACK_FIXVEH_RECON,
	LAUNCHER_CALLBACK_STATS_RECON,
	LAUNCHER_CALLBACK_GETIP_RECON,
	LAUNCHER_CALLBACK_KICK_RECON,
	LAUNCHER_CALLBACK_WARN_RECON,
	LAUNCHER_CALLBACK_BAN_RECON,
	LAUNCHER_CALLBACK_EXIT_RECON,
	LAUNCHER_CALLBACK_KMG_FINISH,
	LAUNCHER_CALLBACK_KAR_PRESSED,
	LAUNCHER_CALLBACK_RACC,
	LAUNCHER_CALLBACK_RACC_SEND,
	LAUNCHER_CALLBACK_RACC_CHECK
};

private LauncherData.callback[][LauncherData.CBInfo] =
{
	{LAUNCHER_CALLBACK_AUTH, 			"OnAuthButtonClick"},
	{LAUNCHER_CALLBACK_TAKE_BONUS, 		"OnBonusButtonClick"},
	{LAUNCHER_CALLBACK_REG, 			"OnRegButtonClick"},
	{LAUNCHER_CALLBACK_GENDER, 			"OnGenderSelect"},
	{LAUNCHER_CALLBACK_REG_SKIN, 		"OnSkinSelect"},
	{LAUNCHER_CALLBACK_AUTH_CODE, 		"OnSecurityButtonClick"},
	{LAUNCHER_CALLBACK_FLIP_RECON, 		"OnReconFlipButtonClick"},
	{LAUNCHER_CALLBACK_FIXVEH_RECON, 	"OnReconFixVehButtonClick"},
	{LAUNCHER_CALLBACK_STATS_RECON, 	"OnReconStatsButtonClick"},
	{LAUNCHER_CALLBACK_GETIP_RECON, 	"OnReconGetIpButtonClick"},
	{LAUNCHER_CALLBACK_KICK_RECON, 		"OnReconKickButtonClick"},
	{LAUNCHER_CALLBACK_WARN_RECON, 		"OnReconWarnButtonClick"},
	{LAUNCHER_CALLBACK_BAN_RECON, 		"OnReconBanButtonClick"},
	{LAUNCHER_CALLBACK_EXIT_RECON, 		"OnReconExitButtonClick"},
	{LAUNCHER_CALLBACK_KMG_FINISH, 		"OnKeyMiniGameFinish"},
	{LAUNCHER_CALLBACK_KAR_PRESSED, 	"OnKeyAreaHandle"},
	{LAUNCHER_CALLBACK_RACC, 			"OnRecoveryButtonClick"},
	{LAUNCHER_CALLBACK_RACC_SEND, 		"OnRestoreEmailSend"},
	{LAUNCHER_CALLBACK_RACC_CHECK, 		"OnRestoreEmailCodeSend"}
};
const LAUNCHER_MAX_CALLBACKS = sizeof(LauncherData.callback);

private LauncherData.allowedSkins[] = {14, 20, 21, 22, 25, 89, 15345, 15352, 88, 39, 38, 75, 15522};
const LAUNCHER_MAX_ALLOWED_SKINS = sizeof(LauncherData.allowedSkins);


stock BitStream:LauncherData.bs;


LauncherData.Init ()
{
	this.bs = BS_New();
}

stock BitStream:LauncherData.GetBitStream ()
{
	return this.bs;
}

stock LauncherData.FindCallback (const callback[])
{
	for (new i = 0; i < LAUNCHER_MAX_CALLBACKS; i++) {
		if (StringCompare(callback, this.callback[i][this.cbName]))
			return this.callback[i][this.cbId];
	}
	return -1;
}

stock LauncherData.FindAllowSkinId (skinId)
{
	for (new i = 0; i < LAUNCHER_MAX_ALLOWED_SKINS; i++) {
		if (this.allowedSkins[i] == skinId)
			return true;
	}
	return false;
}