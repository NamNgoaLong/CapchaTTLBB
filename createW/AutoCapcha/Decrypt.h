#include <windows.h>
#include <map>
#include "macro.h"

typedef struct _stDataPackKet{
	WORD wUmsg;
	DWORD dwIndex;
	DWORD dwRealAddr;
	DWORD dwNewAddr;
	DWORD dwMask;
}stDataPacket;

class Decrypt{
public:
	static std::map<DWORD, stDataPacket> m_stPacketData;
	static DWORD m_dwDatatAddr[MAX_ADDRESS];
	static DWORD m_dwOffsetData[MAX_OFFSET];
public:
	BOOL initPacketMsgProcess(void) ;
	BOOL initClientAdrProcess(void) ;
	BOOL initClientOffsetProcess(void);
	
	BOOL matchPacketCode( HMODULE hModule  , DWORD dwFuncAdr , DWORD& uIndex , char* lpOutNameStr , int nNameStrLen , char* lpOutNoteStr , int nNoteStrLen) ;
	BOOL matchClientCode( DWORD uIndex , DWORD& dwData , HMODULE dwGameMainBase, HMODULE dwGameUI, HMODULE dwLuaPlus) ;
	BOOL matchOffsetCode( DWORD uIndex , DWORD& dwData , HMODULE dwGameMainBase, HMODULE dwGameUI, HMODULE dwLuaPlus) ;



};


enum GAMEADR_TYPE {
	GAMEADR_BASE_TTIMESYSTEM = 0						//time tTimeSystem
	, GAMEADR_BASE_CNETMANAGER						//netman CNetManager
	, GAMEADR_BASE_CNEWSOUNDSYSTEMFMOD				//snd CNewSoundSystemFMod
	, GAMEADR_BASE_CRENDERSYSTEM					//gfx CRenderSystem
	, GAMEADR_BASE_COBJECTMANAGER					//objman CObjectManager
	, GAMEADR_BASE_CWORLDMANAGER					//worldman CWorldManager
	, GAMEADR_BASE_CDATABASESYSTEM					//dbc CDataBaseSystem
	, GAMEADR_BASE_CUISYSTEM						//bin CUISystem
	, GAMEADR_BASE_CGAMEINTERFACE					//interface CGameInterface
	, GAMEADR_BASE_CUIDATAPOOL						//datapool CUIDataPool
	, GAMEADR_BASE_CDATAPOOL						//datapool CDataPool
	, GAMEADR_BASE_CCURSORMNG						//cursor CCursorMng
	, GAMEADR_BASE_CSCRIPTSYSTEM					//script CScriptSystem
	, GAMEADR_BASE_CEVENTSYSTEM						//event CEventSystem
	, GAMEADR_BASE_CACTIONSYSTEM					//action CActionSystem
	, GAMEADR_BASE_CFAKEOBJSYSTEM					//fake CFakeObjSystem
	, GAMEADR_BASE_CHELPERSYSTEM					//helper CHelperSystem
	, GAMEADR_BASE_CRESOURCEPROVIDER				//resprovider CResourceProvider
	, GAMEADR_BASE_CTRANSFERITEMSYSTEM				//transfer CTransferItemSystem
	, GAMEADR_BASE_CITEMSYSTEM						//item CItemSystem
	, GAMEADR_BASE_CMINICLIENTSYSTEM				//miniclient CMiniClientSystem
	, GAMEADR_BASE_WORLDMAP							//进入新场景时候用到的基地址.包含了场景和地图id // Trong kich ban moi, khi duoc su dung trong có so dia chi  chua cac canh va ban do id
	, GAMEADR_LUAPLUSCALL_DOSTRING
	, GAMEADR_LUAPLUSCALL_GETGLOBALS
	, GAMEADR_LUAPLUSCALL_CREATETABLE
	, GAMEADR_LUAPLUSCALL_REMOVEFROMUSEDLIST
	, GAMEADR_LUAPLUSCALL_SETNUMBER
	, GAMEADR_LUAPLUSCALL_DOWBUFFER
	, GAMEADR_LUAPLUSCALL_RAWGETI
	, GAMEADR_LUAPLUSCALL_GETTOP
	, GAMEADR_LUAPLUSCALL_TYPE
	, GAMEADR_LUAPLUSCALL_TONUMBER
	, GAMEADR_LUAPLUSCALL_REMOVE
	, GAMEADR_PACKET_DECODE_13F_CALL
	, GAMEADR_IS_IN_GAME_FLAG
	, GAMEADR_PACKET_DECODE_311_CALL
	, GAMEADR_FUNC_LOCKNPCCOUNT
	, GAMEADR_SEND_PACKET_ECX
	, GAMEADR_SEND_PACKET_CALL
    , GAMEADR_DECODE_CAPTCHA_DATA_CALL
	, GAMEADR_LUA_STATE_ECX
	, GAMEADR_DECODE_CAPTCHA_PURE_LUA_CODE_LEN
	, GAMEADR_DECODE_CAPTCHA_PURE_LUA_CODE_PTR
	, GAMEADR_DECODE_CAPTCHA_FIX_NUMBER_CALL
	, GAMEADR_FUNC_PETUSESKILL
	, GAMEADR_BASE_CLIENTINITFALG		//客户端初始化完毕的标记 // Client hoan tat khoi co
	, GAMEADR_FUNC_GETMISSIONDATA		//获取任务对应的内存地址 // Nhan nhiem vu dia chi o nho tuong ung
	, GAMEADR_FUNC_INITITEMPACKET
	, GAMEADR_FUNC_GETMISSIONNPCINFO
	, GAMEADR_FUNC_STOPACTION
	, GAMEADR_FUNC_PGETXPARAM
	, GAMEADR_FUNC_SENDPETATTACK
	, GAMEADR_MULTI_CLIENT_HANDLE		//多开
	, GAMEADR_MAXINDEX 
} ;

enum PACKETRECV_MESSAGE_TYPE { 
       GAMEPACKET_RECV_BAGITEMUPDATE_DEL                       //包裹删除一格的物品
	,  GAMEPACKET_RECV_BAGITEMUPDATE                           //包裹内全部物品(交易完成后会更新背包全部物品)
	,  GAMEPACKET_RECV_EQUIPUPDATE_REMOVE                      //从身上的装备卸下一件装备
	,  GAMEPACKET_RECV_BAGITEMUPDATE_SPLIT                     //包裹拆分物品
	,  GAMEPACKET_RECV_BAGITEMUPDATE_NEW                       //包裹增加物品
	,  GAMEPACKET_RECV_BAGITEMUPDATE_NEW2                      //包裹增加物品(装备什么的
	,  GAMEPACKET_RECV_BAGITEMUPDATE_WIELD                     //从包裹中穿上一件装备,包裹中删除这个物品
	,  GAMEPACKET_RECV_CHANGESCENE                             //玩家自身切换地图
	,  GAMEPACKET_RECV_CHARBUFFUPDATE                          //对象buffer变化
	,  GAMEPACKET_RECV_PLAYERSELFINFO                          //(玩家自己)人物各种信息 // (Nguoi choi) Moi nguoi tat ca cac laoi thong tin // Loi_tai_noi_nay
	,  GAMEPACKET_RECV_CHARMODIFYACTION                        //人物运气改变
	,  GAMEPACKET_RECV_CHARSKILLLEAD                           //发功类技能释放
	,  GAMEPACKET_RECV_CHARSTOPACTION                          //停止动作
	,  GAMEPACKET_RECV_CHAT                                    //聊天
	,  GAMEPACKET_RECV_COLLECTBEGIN                            //开始采集,挖矿,开宝箱等
	,  GAMEPACKET_RECV_ENETERSCENE						       //(换图)(玩家自己的角色)人物出现,ID,所在地图和坐标,固定26H字节
	,  GAMEPACKET_RECV_ENTER_NEWWORLD_CONFIRM				   //进入新地图的确认对话框
	,  GAMEPACKET_RECV_EQUIPUPDATE                             //身上的装备
	,  GAMEPACKET_RECV_NEWNPCMONSTER_CORPSE                    //怪物尸体出现
	,  GAMEPACKET_RECV_NEWNPCMONSTER_MOVEHANDLER               //怪物出现 (移动)
	,  GAMEPACKET_RECV_NEWNPCMONSTER				           //NPC或怪物出现 被动 (加密了)
	,  GAMEPACKET_RECV_PLAYERDISAPPEAR                         //玩家对象消失
	,  GAMEPACKET_RECV_OBJECTDISAPPEAR                         //其他对象消失
	,  GAMEPACKET_RECV_OBJECTINFOUPDATE                        //游戏内对象的各种信息,以ID为标志 // Thong tin cua tat ca cac loai doi tuong trong tro choi
	,  GAMEPACKET_RECV_PETINFOUPDATE                           //宠物具体的信息
	,  GAMEPACKET_RECV_NEWPET_MOVEHANDLER                      //宠物出现 主动
	,  GAMEPACKET_RECV_NEWPET_HANDLER                          //宠物出现 被动
	,  GAMEPACKET_RECV_NEWPET_DEATHHANDLER                     //宠物尸体出现
	,  GAMEPACKET_RECV_PICKUP_OPEN                             //打开地上怪物掉落物品,得到里面具体是哪些东西
	,  GAMEPACKET_RECV_NEWPLAYER_MOVEHANDLER                   //玩家出现,主动
	,  GAMEPACKET_RECV_NEWPLAYER_HANDLER                       //玩家出现,被动
	,  GAMEPACKET_RECV_NEWPLAYER_SKILLCHANNEL                  //玩家出现,玩家在打坐中
	,  GAMEPACKET_RECV_NEWPLAYER_DEATHHANDLER                  //玩家尸体出现
	,  GAMEPACKET_RECV_RIDEBAG_INFO                            //坐骑背包信息
	,  GAMEPACKET_RECV_RIDEBAGMODIFY                           //坐骑背包变化
	,  GAMEPACKET_RECV_SKILLTABLE                              //技能列表
	,  GAMEPACKET_RECV_SKILL_COLD                              //技能冷却开始的包
	,  GAMEPACKET_RECV_SELFBUFFUPDATE                          //身上增加辅助状态
	,  GAMEPACKET_RECV_SELFBUFFLISTUPDATE                      //玩家自己Buffer列表更新
	,  GAMEPACKET_RECV_CHARBUFFLISTUPDATE                      //人物技能状态更新
	,  GAMEPACKET_RECV_SKILL_SUCCESS                           //技能成功释放
	,  GAMEPACKET_RECV_SKILLGATHER                             //技能聚气
	,  GAMEPACKET_RECV_NEWSPECIAL_HANDLER                      //特殊对象出现
	,  GAMEPACKET_RECV_NEWITEMBOX_HANDLER					   //地面上出现的宝箱,矿物,草药等
	,  GAMEPACKET_RECV_TALKTONPCRET						       //对话NPC后返回
	,  GAMEPACKET_RECV_TALKTONPCRET_ITEMTABLE			       //与NPC对话返回某一个选项内的物品列表
	,  GAMEPACKET_RECV_TARGETLIST_AND_HITFLAGS                 //攻击开始的包
	,  GAMEPACKET_RECV_TEAMAPPLY				               //别人申请入队
	,  GAMEPACKET_RECV_TEAMINVITE				               //接收到别人的组队邀请
	,  GAMEPACKET_RECV_TEAMPLAYERJOIN                          //有玩家加入到组队中
	,  GAMEPACKET_RECV_TEAMPLAYERPOS                           //组队成员坐标
	,  GAMEPACKET_RECV_TRAINCAPTCHA                            //打怪图片验证
	,  GAMEPACKET_RECV_NEWBUS_MOVEHANDLER                      //好像是交通工具鹰.在大理通往剑阁的传送地点,可以收到此类包 主动出现
	,  GAMEPACKET_RECV_NEWBUS_HANDLER                          //好像是交通工具鹰.在大理通往剑阁的传送地点,可以收到此类包 被动出现
	,  GAMEPACKET_RECV_NEWBUS_MOVETARGET
	,  GAMEPACKET_RECV_NEWBUS_MOVESTOP
	,  GAMEPACKET_RECV_NEWTELEPORT_HANDLER
	,  GAMEPACKET_RECV_XINFATABLE                              //心法列表
	,  GAMEPACKET_RECV_PICKRESULTHANDLER                       //拾取物品的返回包
	,  GAMEPACKET_RECV_OPERATERESULT                           //各种操作的结果包, -9,超出距离
	,  GAME_PACKET_MAXINDEX
} ;
