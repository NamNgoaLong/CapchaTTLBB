#include <windows.h>
#include <map>
#include "macro.h"
#include <vector>
#pragma once
#pragma comment( lib, "psapi.lib" )

typedef struct _stDataPackKet
{
	WORD wUmsg;
	DWORD dwIndex;
	DWORD dwRealAddr;
	DWORD dwNewAddr;
	DWORD dwMask;
}stDataPacket;


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
	, GAMEADR_BASE_WORLDMAP							//�����³���ʱ���õ��Ļ���ַ.�����˳����͵�ͼid // Trong kich ban moi, khi duoc su dung trong c�� so dia chi  chua cac canh va ban do id
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
	, GAMEADR_BASE_CLIENTINITFALG		//�ͻ��˳�ʼ����ϵı�� // Client hoan tat khoi co
	, GAMEADR_FUNC_GETMISSIONDATA		//��ȡ�����Ӧ���ڴ��ַ // Nhan nhiem vu dia chi o nho tuong ung
	, GAMEADR_FUNC_INITITEMPACKET
	, GAMEADR_FUNC_GETMISSIONNPCINFO
	, GAMEADR_FUNC_STOPACTION
	, GAMEADR_FUNC_PGETXPARAM
	, GAMEADR_FUNC_SENDPETATTACK
	, GAMEADR_MULTI_CLIENT_HANDLE		//�࿪
	, GAMEADR_MAXINDEX 
} ;
#define ERROR_INDEX 8000
enum GAMEADR_TYPE_ERROR
{
	GAMEADR_ERROR = ERROR_INDEX + 1000
	, GAMEADR_BASE_CINPUTSYSTEM						//input CInputSystem
	, GAMEADR_BASE_CTIMERMANAGER					//timer CTimerMananger
	, GAMEADR_TRADE_DISTANCE
	, GAMEADR_SEND_PACKET_TALK_TO_NPC_TO_YANBAOPIAO_ECX
	, GAMEADR_IS_ATTACK_CALL
	, GAMEADR_AGREE_JOIN_TEAM_APPLY_INIT_CALL 
	, GAMEADR_AGREE_JOIN_TEAM_APPLY_ECX 
	, GAMEADR_AGREE_JOIN_TEAM_APPLY_ENCODE 
	, GAMEADR_AGREE_JOIN_TEAM_ECX  
	, GAMEADR_SEND_FRIEND_SEARCH_ECX 
	, GAMEADR_SEND_PET_ATTACK_ECX  
	, GAMEADR_SEND_HE_CHENG_CAI_LIAO_CALL
	, GAMEADR_SEND_CANCEL_SKILL_ECX
	, GAMEADR_SendPacketPetFeedEcx
	, GAMEADR_PACKET_CANCEL_BUFF_ECX
	, GAMEADR_SEND_ASK_ITEM_INFO_MSG_BAG_OFFSET
	, GAMEADR_SEND_AGREE_JOIN_TEAM_APPLY_INIT_CALL
	, GAMEADR_SEND_AGREE_JOIN_TEM_APLLY_ECX 
	, GAMEADR_SEND_AGREE_JOIN_TEAM_APLLY_ENCODE
	, GAMEADR_EQUIP_ATTR_DESC
	, GAMEADR_EQUIP_ATTR_EXT
	, GAMEADR_SEND_PACKET_BUY_MULTI_ITEM_INIT_CALL
	, GAMEADR_SEND_PACKET_PET_FIGHT_INIT_CALL
	, GAMEADR_SEND_PACKET_PET_FEED_INIT_CALL
	, GAMEADR_QUEST_FRAME_OPTION_CLICKED_ECX
	, GAMEADR_QUEST_FRAME_OPTION_CLICKED_CALL
	, GAMEADR_SEND_PACKET_CANCEL_BUFF_INIT_CALL
	, GAMEADR_SEND_PACKET_CHAT_INIT_CALL
	, GAMEADR_LOCK_OBJECT_STATUS_ECX
	, GAMEADR_LOCK_OBJECT_4_6
	, GAMEADR_SEND_PACKET_TRADE_MONEY_ECX
	, GAMEADR_LUA_FUNCTION_ECX
	, GAMEADR_LUA_FUNCTION_ENV_CALL
	, GAMEADR_LUA_FUNCTION_CALL
	, GAMEADR_SEND_LOCK_ECX
};

enum GAMEOFFSET_TYPE
{
	//���� // Network
	  GAMEOFFSET_NET_SOCKET  
	, GAMEOFFSET_NET_CRITICALSECTION
	//CObjectManager
	, GAMEOFFSET_OBJMAN_PPLAYERSELF
	, GAMEOFFSET_OBJMAN_POBJECTTABLE
	//EVENT
	, GAMEOFFSET_EVENT_PFUNCPUSHDEBUGMSG
	//CScriptManager
	, GAMEOFFSET_SCRIPT_PFUNC_GETLUASTATE
	, GAMEOFFSET_SCRIPT_PREVLUAENVSTRING
	//CUISystem
//	, GAMEOFFSET_UI_PDICTIONARYSTRING
	//CDataPool
	, GAMEOFFSET_DATAPOOL_PPETLIST
	, GAMEOFFSET_DATAPOOL_PBUFFTABLE
	, GAMEOFFSET_DATAPOOL_PBAGITEMLIST
	, GAMEOFFSET_DATAPOOL_TARGETINDEX
	, GAMEOFFSET_DATAPOOL_PEQUIPMENTLIST
	, GAMEOFFSET_DATAPOOL_PRIDEBAGLIST 
	, GAMEOFFSET_DATAPOOL_GUILDWAR
	//CWorldManager
	, GAMEOFFSET_WORLDMAN_STATE
	//CUIDataPool
	, GAMEOFFSET_UIDATAPOOL_TALKMSG
	, GAMEOFFSET_UIDATAPOOL_TASKLIST
	, GAMEOFFSET_UIDATAPOOL_TEAMLIST
	, GAMEOFFSET_UIDATAPOOL_TALKDEBUGMSG
	//CDataBaseSystem
	, GAMEOFFSET_DBC_GETINSTANCEPTR
	, GAMEOFFSET_DBC_SPINFANTEXPMIS
	, GAMEOFFSET_DBC_SPMISSIONITEM
	//CInterfaceManager
	, GAMEOFFSET_INTERFACE_LOCKOBJ
	, GAMEOFFSET_INTERFACE_PICKUPOPEN
	, GAMEOFFSET_INTERFACE_SPEEKNPC
	, GAMEOFFSET_INTERFACE_PFUNCCAMPTYPE //����Եй�ϵ 2 - �Ѻ�(NPC) , 0 - �ɹ���(MONSTER)// Co the la Tinh toan moi quan he: tinh ban, co the tan cong
	, GAMEOFFSET_INTERFACE_COMPOSE		 //�ϳɲ��� // Vat lieu tong hop
	, GAMEOFFSET_DATAPOOL_ASKITEMINFO 
	, GAMEOFFSET_INTERFACE_OPENNPC		 //��NPC��Ķ��� // Mo lop doi tuong NPC
	, GAMEOFFSET_INTERFACE_MOVETO		 //��Ѱ·�ı���ͼ�ƶ� // Khong tim thay ban do nay, chuyen dong...
	//CObject
	, GAMEOFFSET_OBJ_INDEX 
	, GAMEOFFSET_OBJ_ID
	, GAMEOFFSET_OBJ_X
	, GAMEOFFSET_OBJ_Z
	, GAMEOFFSET_OBJ_Y
	, GAMEOFFSET_OBJ_PRESOURCE
	, GAMEOFFSET_OBJ_POBJINFO
	, GAMEOFFSET_OBJ_ATTREX
	, GAMEOFFSET_OBJFUNCPTR_CLASS
	, GAMEOFFSET_OBJFUNCPTR_MODELID
	, GAMEOFFSET_OBJ_PACTION
	, GAMEOFFSET_OBJFUNC_USESKILL
	, GAMEOFFSET_OBJFUNC_USESKILL_SPEC2
	, GAMEOFFSET_OBJ_LOGIC				//�ý�ɫ���߼�״̬ move , dead , lead_skill... // trang thai logic cua nhan vat
	//CObjectInfomation
	, GAMEOFFSET_OBJINFO_PARENT
	, GAMEOFFSET_OBJINFO_HPPERCENT
	, GAMEOFFSET_OBJINFO_MPPERCENT
	, GAMEOFFSET_OBJINFO_SID
	, GAMEOFFSET_OBJINFO_MODEL
	, GAMEOFFSET_OBJINFO_FID
	, GAMEOFFSET_OBJINFO_NAME
	, GAMEOFFSET_OBJINFO_SPEED
	, GAMEOFFSET_OBJINFO_LEVEL
	, GAMEOFFSET_OBJINFO_NU
	, GAMEOFFSET_OBJINFO_TITLE
	, GAMEOFFSET_OBJINFO_ATTRI
	, GAMEOFFSET_OBJINFO_COLOR
	, GAMEOFFSET_OBJINFO_MENPAI
	, GAMEOFFSET_OBJINFO_GUILD
	, GAMEOFFSET_OBJINFO_PKLIST
	, GAMEOFFSET_OBJINFO_RELATIVE
	, GAMEOFFSET_OBJINFO_STEALLEVEL
	, GAMEOFFSET_OBJINFO_BIANSHEN
	, GAMEOFFSET_OBJINFO_RIDE
	, GAMEOFFSET_OBJINFO_AITYPE
	, GAMEOFFSET_OBJINFO_PXINFATABLE
	, GAMEOFFSET_OBJINFO_PSKILLTABLE
	//����������� // tai san rieng cua nguoi choi
	, GAMEOFFSET_SELFINFO_TEAMFOLLOW
	, GAMEOFFSET_SELFINFO_HP
	, GAMEOFFSET_SELFINFO_MP
	, GAMEOFFSET_SELFINFO_TEP
	, GAMEOFFSET_SELFINFO_MAXTEP
	, GAMEOFFSET_SELFINFO_EXP
	, GAMEOFFSET_SELFINFO_STRIKEPOINT
	, GAMEOFFSET_SELFINFO_MONEY
	, GAMEOFFSET_SELFINFO_MAXHP
	, GAMEOFFSET_SELFINFO_MAXMP
	, GAMEOFFSET_SELFINFO_VIGOR		//���� // suc song
	, GAMEOFFSET_SELFINFO_MAXVIGOR
	, GAMEOFFSET_SELFINFO_ENERGY	//���� // nang luong
	, GAMEOFFSET_SELFINFO_MAXENERGY 
	, GAMEOFFSET_SELFINFO_PKVALUE
	, GAMEOFFSET_SELFINFO_PET_EXTRANUM 
	, GAMEOFFSET_SELFINFO_MONEY_JZ
	, GAMEOFFSET_SELFINFO_POWER
	, GAMEOFFSET_SELFINFO_KILLMONSTERCOUNT 
	, GAMEOFFSET_SELFINFO_UNIONID
	, GAMEOFFSET_SELFINFO_TODAYEXP 
	, GAMEOFFSET_SELFINFO_FIGHTPET //��ս���� // Tran thu xuat chien
	//����������� // Thuoc tinh bo sung sinh hoc
	//������ϸ��Ϣ // Chi tiet tran thu
	, GAMEOFFSET_PETINFO_SIZE 
	, GAMEOFFSET_PETINFO_ISPRESENT
	, GAMEOFFSET_PETINFO_64ID 
	, GAMEOFFSET_PETINFO_ID
	, GAMEOFFSET_PETINFO_SID
	, GAMEOFFSET_PETINFO_NAME
	, GAMEOFFSET_PETINFO_LOVER64
	, GAMEOFFSET_PETINFO_LEV
	, GAMEOFFSET_PETINFO_EXP
	, GAMEOFFSET_PETINFO_SEX
	, GAMEOFFSET_PETINFO_NHAPPY
	, GAMEOFFSET_PETINFO_HP
	, GAMEOFFSET_PETINFO_MAXHP
	, GAMEOFFSET_PETINFO_LIFE
	, GAMEOFFSET_PETINFO_PSKILLLIST
	//worldmap
	, GAMEOFFSET_WORLDMAP_MAPID
	, GAMEOFFSET_WORLDMAP_SCENEID
	//���� // Khac
	, GAMEOFFSET_XINFAITEM_ID
	, GAMEOFFSET_XINFAITEM_PNAME
	, GAMEOFFSET_SKILLITEM_ID
	, GAMEOFFSET_SKILLITEM_PNAME
	, GAMEOFFSET_MISSION_NAME
	, GAMEOFFSET_MISSION_DESCRIPT
	, GAMEOFFSET_MISSION_STRLIST
	, GAMEOFFSET_MISSION_PARAMINDEX
	, GAMEOFFSET_LOGIN_ACCOUNTNAME
	, GAMEOFFSET_MAXINDEX
} ;

enum PACKETRECV_MESSAGE_TYPE 
{ 
       GAMEPACKET_RECV_BAGITEMUPDATE_DEL                       //����ɾ��һ�����Ʒ
	,  GAMEPACKET_RECV_BAGITEMUPDATE                           //������ȫ����Ʒ(������ɺ����±���ȫ����Ʒ)
	,  GAMEPACKET_RECV_EQUIPUPDATE_REMOVE                      //�����ϵ�װ��ж��һ��װ��
	,  GAMEPACKET_RECV_BAGITEMUPDATE_SPLIT                     //���������Ʒ
	,  GAMEPACKET_RECV_BAGITEMUPDATE_NEW                       //����������Ʒ
	,  GAMEPACKET_RECV_BAGITEMUPDATE_NEW2                      //����������Ʒ(װ��ʲô��
	,  GAMEPACKET_RECV_BAGITEMUPDATE_WIELD                     //�Ӱ����д���һ��װ��,������ɾ�������Ʒ
	,  GAMEPACKET_RECV_CHANGESCENE                             //��������л���ͼ
	,  GAMEPACKET_RECV_CHARBUFFUPDATE                          //����buffer�仯
	,  GAMEPACKET_RECV_PLAYERSELFINFO                          //(����Լ�)���������Ϣ // (Nguoi choi) Moi nguoi tat ca cac laoi thong tin // Loi_tai_noi_nay
	,  GAMEPACKET_RECV_CHARMODIFYACTION                        //���������ı�
	,  GAMEPACKET_RECV_CHARSKILLLEAD                           //�����༼���ͷ�
	,  GAMEPACKET_RECV_CHARSTOPACTION                          //ֹͣ����
	,  GAMEPACKET_RECV_CHAT                                    //����
	,  GAMEPACKET_RECV_COLLECTBEGIN                            //��ʼ�ɼ�,�ڿ�,�������
	,  GAMEPACKET_RECV_ENETERSCENE						       //(��ͼ)(����Լ��Ľ�ɫ)�������,ID,���ڵ�ͼ������,�̶�26H�ֽ�
	,  GAMEPACKET_RECV_ENTER_NEWWORLD_CONFIRM				   //�����µ�ͼ��ȷ�϶Ի���
	,  GAMEPACKET_RECV_EQUIPUPDATE                             //���ϵ�װ��
	,  GAMEPACKET_RECV_NEWNPCMONSTER_CORPSE                    //����ʬ�����
	,  GAMEPACKET_RECV_NEWNPCMONSTER_MOVEHANDLER               //������� (�ƶ�)
	,  GAMEPACKET_RECV_NEWNPCMONSTER				           //NPC�������� ���� (������)
	,  GAMEPACKET_RECV_PLAYERDISAPPEAR                         //��Ҷ�����ʧ
	,  GAMEPACKET_RECV_OBJECTDISAPPEAR                         //����������ʧ
	,  GAMEPACKET_RECV_OBJECTINFOUPDATE                        //��Ϸ�ڶ���ĸ�����Ϣ,��IDΪ��־ // Thong tin cua tat ca cac loai doi tuong trong tro choi
	,  GAMEPACKET_RECV_PETINFOUPDATE                           //����������Ϣ
	,  GAMEPACKET_RECV_NEWPET_MOVEHANDLER                      //������� ����
	,  GAMEPACKET_RECV_NEWPET_HANDLER                          //������� ����
	,  GAMEPACKET_RECV_NEWPET_DEATHHANDLER                     //����ʬ�����
	,  GAMEPACKET_RECV_PICKUP_OPEN                             //�򿪵��Ϲ��������Ʒ,�õ������������Щ����
	,  GAMEPACKET_RECV_NEWPLAYER_MOVEHANDLER                   //��ҳ���,����
	,  GAMEPACKET_RECV_NEWPLAYER_HANDLER                       //��ҳ���,����
	,  GAMEPACKET_RECV_NEWPLAYER_SKILLCHANNEL                  //��ҳ���,����ڴ�����
	,  GAMEPACKET_RECV_NEWPLAYER_DEATHHANDLER                  //���ʬ�����
	,  GAMEPACKET_RECV_RIDEBAG_INFO                            //���ﱳ����Ϣ
	,  GAMEPACKET_RECV_RIDEBAGMODIFY                           //���ﱳ���仯
	,  GAMEPACKET_RECV_SKILLTABLE                              //�����б�
	,  GAMEPACKET_RECV_SKILL_COLD                              //������ȴ��ʼ�İ�
	,  GAMEPACKET_RECV_SELFBUFFUPDATE                          //�������Ӹ���״̬
	,  GAMEPACKET_RECV_SELFBUFFLISTUPDATE                      //����Լ�Buffer�б����
	,  GAMEPACKET_RECV_CHARBUFFLISTUPDATE                      //���＼��״̬����
	,  GAMEPACKET_RECV_SKILL_SUCCESS                           //���ܳɹ��ͷ�
	,  GAMEPACKET_RECV_SKILLGATHER                             //���ܾ���
	,  GAMEPACKET_RECV_NEWSPECIAL_HANDLER                      //����������
	,  GAMEPACKET_RECV_NEWITEMBOX_HANDLER					   //�����ϳ��ֵı���,����,��ҩ��
	,  GAMEPACKET_RECV_TALKTONPCRET						       //�Ի�NPC�󷵻�
	,  GAMEPACKET_RECV_TALKTONPCRET_ITEMTABLE			       //��NPC�Ի�����ĳһ��ѡ���ڵ���Ʒ�б�
	,  GAMEPACKET_RECV_TARGETLIST_AND_HITFLAGS                 //������ʼ�İ�
	,  GAMEPACKET_RECV_TEAMAPPLY				               //�����������
	,  GAMEPACKET_RECV_TEAMINVITE				               //���յ����˵��������
	,  GAMEPACKET_RECV_TEAMPLAYERJOIN                          //����Ҽ��뵽�����
	,  GAMEPACKET_RECV_TEAMPLAYERPOS                           //��ӳ�Ա����
	,  GAMEPACKET_RECV_TRAINCAPTCHA                            //���ͼƬ��֤
	,  GAMEPACKET_RECV_NEWBUS_MOVEHANDLER                      //�����ǽ�ͨ����ӥ.�ڴ���ͨ������Ĵ��͵ص�,�����յ������ ��������
	,  GAMEPACKET_RECV_NEWBUS_HANDLER                          //�����ǽ�ͨ����ӥ.�ڴ���ͨ������Ĵ��͵ص�,�����յ������ ��������
	,  GAMEPACKET_RECV_NEWBUS_MOVETARGET
	,  GAMEPACKET_RECV_NEWBUS_MOVESTOP
	,  GAMEPACKET_RECV_NEWTELEPORT_HANDLER
	,  GAMEPACKET_RECV_XINFATABLE                              //�ķ��б�
	,  GAMEPACKET_RECV_PICKRESULTHANDLER                       //ʰȡ��Ʒ�ķ��ذ�
	,  GAMEPACKET_RECV_OPERATERESULT                           //���ֲ����Ľ����, -9,��������
	,  GAME_PACKET_MAXINDEX
} ;

//---------------------------------------------------------------------
namespace memdata
{
	//62B8C2AC  9C 06 D3 01 03 00 00 00 06 00 00 00 07 00 00 00 
	//			sid
	//62B8C2BC  04 00 00 00 6A 47 C8 62 75 47 C8 62 80 47 C8 62  
	//						Ӣ�Ĵ���    ��������    ����
	//62B8C2CC  01 00 00 00 04 00 00 00 01 00 00 00 23 00 00 00 
	//62B8C2DC  01 00 00 00 31 B6 0A 00 56 00 00 00 00 00 00 00 
	//62B8C2EC  FF FF FF FF FF FF FF FF FF FF FF FF 05 00 00 00 
	//62B8C2FC  61 47 C8 62 0A 00 00 00 00 00 00 00 D5 07 00 00  
	//62B8C30C  FF FF FF FF 00 00 00 00 00 00 00 00  
	typedef 
	struct _itemdata
	{
		DWORD sid ;
		DWORD u1 ;
		DWORD u2 ;
		DWORD u3 ;
		DWORD u4 ;
		char* lpCodeName ;
		char* lpName ;
		char* lpDescript ;
	} itemdata , *itemdataptr ;

	typedef
	struct _growdata 
	{
		DWORD sid ;
		char* lpName ;
	} growdata , *growdataptr ;

	typedef
	struct _equipdata
	{
		DWORD sid ;
		DWORD un[9] ;
		char* lpName ;
	} equipdata , *equipdataptr ;

	typedef
	struct _bagitem_o
	{
		DWORD64 id ;
		DWORD sid ;
		BYTE u[0x24] ;
		BYTE uFlag ; //��λ�� 0a �Ѷ�λ ,������δ��λ״̬ , ��Ϊ��Ʊʱ, u2 �� max_usetimes ��ϳ�dword��ʾ��� , �ر�ͼ 0x1 �Ѷ�λ ,�����Դ˸��� word - mapid , word - x , word - y
		BYTE u1;
		BYTE max_usetimes ;//����������
		BYTE cur_usetimes ;//��ǰ���ô���
		WORD mapid ;	//��λ����λ�ĵ�ͼ
		WORD map_x ;	//��λ����x
		WORD map_y ;	//��λ����y
		WORD u3 ;
		BYTE num ;		//��Ϊһ����Ʒʱ,��������,װ��ʱ���;�����
		BYTE use_lev ;	//ʹ�õȼ�
		BYTE u5 ;	
		BYTE cur_dur ;	//һ����ƷΪ 0xff ,װ��ʱ�ǵ�ǰ�;�
	} bagitem_o , *bagitem_o_ptr ;

	typedef
	struct _bagitem
	{
		DWORD funclist_ptr ;
		DWORD u1 ;
		DWORD sid ;
		DWORD u2 ;
		DWORD u3 ;
		bagitem_o_ptr ptr_data_o ;
		BYTE bCheck ;	//����Ʒ�Ƿ�Ҫ��ѯ��ϸ��Ϣ(1-��ʾ��Ҫ��ѯ,������δ��ѯ��)
		BYTE bLock ;	//��Ʒ�Ƿ�����(������ʱ,�ͻ����� 1 ,��Ϊ����״̬)
		BYTE u5;
		BYTE u6 ;
		DWORD u4[3] ;
		void* ptr_info ;//װ�� equipdataptr , ��Ʒ itemdataptr
	} bagitem , *bagitemptr ;

	typedef
	struct _taskitem 
	{
		int		m_idMission;
		int		m_idScript;
		// 0x0000|PetChanged�¼�|ItemChanged�¼�|EnterZone�¼�|KillObject�¼�|
		BYTE	m_yFlags;
		DWORD	dwParam[8] ;	//Ŀǰ��/hien dang xem, index = 0 ��������ɱ��/ co nhiem vu hoan thanh
	} taskitem , *taskitemptr ;

	typedef
	struct _objectattrex
	{
		int	id;
		char* pName;
		int	nCannotBeAttack;
		int nLevel;
		DWORD un1[2];
		DWORD nIdleInterval;
		DWORD un2[6];
		int	nCanHandle;				//�Ƿ�ɽ���default = -1
		int	nMonsterBossFlag ;
		int un3[29] ;
		DWORD nModelID;
		DWORD un4[21] ;
		int hp_show ;
		DWORD un5[5] ;
	} objectattrex , *objectattrexptr;

	typedef
	struct _missionnpcinfo_o
	{
		int id ;
		char* pName ;
		char* pPreName ;
		int mapid ;
		int x ;
		int y ;
		char* pLink ;
	} mission_npcinfo_o , *mission_npcinfo_o_ptr ;

	typedef 
	struct _dbc_infantexpmission
	{
		int id ;
		DWORD un1[3] ;
		char* un2 ;
		char* pItemName ;
		DWORD un3[2] ;
		int nMissionItemID ;
	} dbc_InfantExpMission , *dbc_InfantExpMission_ptr ;

	typedef
	struct _dbc_missionitem
	{
		int id ;
		char* pName ;
		char* pBuyLink ;
	} dbc_MissionItem , *dbc_MissionItem_ptr ;
}


class Decrypt
{
public:

//	RESULT_INIT_ADDR

	static std::map<DWORD, stDataPacket> m_stPacketData;
	static DWORD m_dwDatatAddr[GAMEADR_MAXINDEX];
	static DWORD m_dwOffsetData[GAMEOFFSET_MAXINDEX];
public:
	BOOL initPacketMsgProcess(void) ;
	BOOL initClientAddrProcess(void) ;
	BOOL initClientOffsetProcess(void);
	
	BOOL matchPacketCode( HMODULE hModule  , DWORD dwFuncAdr , DWORD& uIndex , char* lpOutNameStr , int nNameStrLen, char* lpOutNoteStr , int nNoteStrLen ) ;
	BOOL matchClientCode( DWORD uIndex , DWORD& dwData , HMODULE dwGameMainBase, HMODULE dwGameUI, HMODULE dwLuaPlus ) ;
	BOOL matchOffsetCode( DWORD uIndex , DWORD& dwData , HMODULE dwGameMainBase, HMODULE dwGameUI, HMODULE dwLuaPlus ) ;

	BOOL getAccountInfo( char* pBuffer , int len );	
	bool initData( void ) ;
//	bool initPacketData( void ) ;
	static unsigned int _stdcall initAddrData( LPVOID lParam ) ;
//	static unsigned int _stdcall initPacketData( LPVOID lParam ) ;
	
	void startInitAddrProcess( void ) ;
	bool hookGameRecv( void );
	int _fastcall onInlineRecv( DWORD pPacketObj , DWORD pPacket , DWORD pNetman );
	void onRecvEx( char* lpBuffer , int uIndex , int uMsg , DWORD& dwOutExeFlag );
	bool unHookGameRecv( void ) ;
};

//---------------------------------------------

inline char* _tl_string_c_str( void* lpstring ) ;
inline int _tl_string_length( void* lpstring ) ;

class CObject
{
protected:
	DWORD	m_dwObject;

	template<class T, class _T> T GetVal(DWORD dwOffset, T def = 0)
	{
		T result = def;
		if ( m_dwObject ) 
			result = (T)(*(_T*)(m_dwObject + dwOffset));
		return result;
	} ;

	template<class T> VOID SetVal(DWORD dwOffset, T Val)
	{
		if ( m_dwObject ) 
			*(T*)(m_dwObject + dwOffset) = (T)(Val);
	} ;

public:
	CObject(void){} ;
	virtual BOOL Init( DWORD dwObject ) = 0 ;
	BOOL Init( DWORD dwObject , DWORD nSize ) 
	{ 
		m_dwObject = 0 ;
		if ( 0 == dwObject )
			return FALSE ;
		if ( ::IsBadReadPtr( (VOID*)dwObject , nSize ) )
			return FALSE ;

		m_dwObject = dwObject ;
		return TRUE ;
	} ;

	BOOL operator == (const DWORD &dwObject){return m_dwObject == dwObject;} ;
	BOOL operator != (const DWORD &dwObject){return m_dwObject != dwObject;} ;

	DWORD GetBase(){return m_dwObject;} ;
};

class CScriptSystem : public CObject
{
public:
	virtual BOOL Init( DWORD dwObject = Decrypt::m_dwDatatAddr[GAMEADR_BASE_CSCRIPTSYSTEM] )
	{
		if ( ::IsBadReadPtr((void*) dwObject , 4) )
			return FALSE ;
		return CObject::Init( *(DWORD*)dwObject , 0x100 ) ;
	} ;

	DWORD GetLuaState() 
	{
		DWORD pFuncList = GetVal<DWORD,DWORD>(0) ;
		DWORD dwOffset = Decrypt::m_dwOffsetData[GAMEOFFSET_SCRIPT_PFUNC_GETLUASTATE] ;// 3c
		DWORD dwResult = 0 ;
//00 44 69 76
		if ( pFuncList )
		{
			__asm
			{
				pushfd
				mov ecx , m_dwObject
				mov ebx , dwOffset
				mov edx , pFuncList
				call dword ptr[edx+ebx]
				mov dwResult , eax
				popfd
			}
		}
		return dwResult ;
	} ;

	static BOOL IsInit() 
	{
		return *(DWORD*)(Decrypt::m_dwDatatAddr[GAMEADR_BASE_CLIENTINITFALG]) != 0 ? TRUE : FALSE ;
	} ;

	void* GetPrevEnvString() 
	{
		return (void*)( m_dwObject + Decrypt::m_dwOffsetData[GAMEOFFSET_SCRIPT_PREVLUAENVSTRING] ) ;
	} ;
};

class CObjectManager : public CObject 
{
public:
	virtual BOOL Init( DWORD dwObject = Decrypt::m_dwDatatAddr[GAMEADR_BASE_COBJECTMANAGER] )
	{
		if ( ::IsBadReadPtr((void*) dwObject , 4) )
			return FALSE ;
		return CObject::Init( *(DWORD*)dwObject , 0x100 ) ;
	} ;

	DWORD GetObjectTable() { return GetVal<DWORD,DWORD>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJMAN_POBJECTTABLE]) ;};//68
	DWORD GetPlayerSelfAdr() { return GetVal<DWORD,DWORD>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJMAN_PPLAYERSELF]) ;};//+74
};


class CCommonObjectTable : public CObject
{
private:
	DWORD node_empty ;
	DWORD node_root ;

	typedef struct _node
	{
		_node* left ;
		_node* parent ;
		_node* right ;
		int key ;		//�ڵ�� key �� int ,�и���
		DWORD data ;
		BYTE un ;
		BYTE is_empty ;
	} _stnode;
public:
	virtual BOOL Init( DWORD dwObject )
	{
		node_empty = 0 ;
		node_root = 0 ;

		if ( ::IsBadReadPtr((void*)dwObject , 8) )
			return FALSE ;

		return CObject::Init( dwObject , 4 ) ;
	}

	void GetAllNode( std::vector<DWORD>&rOut )
	{
		node_empty = m_dwObject ;
		node_root = *(DWORD*)(node_empty + 4) ;
		IterateNodes( (_node*)node_root , rOut ) ;
	} ;

	DWORD FindObjectByID( DWORD dwID )
	{
		node_empty = m_dwObject ;
		node_root = *(DWORD*)(node_empty + 4) ;
		return FindObjectByID( (_node*)node_root ,dwID ) ;
	} ;

private:
	void IterateNodes( _node* ptr_node , std::vector<DWORD>&rOut )
	{
		if ( !ptr_node )
			return ;
		if ( ptr_node->is_empty )
			return ;
		if ( ptr_node->data )
			rOut.push_back( ptr_node->data ) ;
		IterateNodes( ptr_node->left , rOut ) ;
		IterateNodes( ptr_node->right , rOut ) ;
	}

	DWORD FindObjectByID( _node* ptr_node , int dwID )
	{
		if ( !ptr_node )
			return 0 ;
		if ( ptr_node->is_empty )
			return 0 ;
		if ( ptr_node->key == dwID )
			return ptr_node->data ;

		_node* pNext = ptr_node->right ;
		if ( ptr_node->key > dwID )
			pNext = ptr_node->left ;
		return FindObjectByID( pNext , dwID ) ;
	}
};

class CCommonObject : public CObject // Thuoc tinh nguoi choi
{
public:
	virtual BOOL Init( DWORD dwObject )  
	{
		return CObject::Init(dwObject , 0x400 ) ;
	} ;

	BOOL InitByID( DWORD dwID )
	{
		CObjectManager tObjMan ;
		if ( !tObjMan.Init() )// init(GAMEADR_BASE_COBJECTMANAGER)
			return FALSE ;
		CCommonObjectTable tTableMan ;
		if ( !tTableMan.Init( tObjMan.GetObjectTable()) ) // m_obj =  (return (0x68))
			return FALSE ;
		return Init( tTableMan.FindObjectByID( dwID) ) ;
	} ;

	BOOL InitBySelf()//ok
	{
		CObjectManager tObjman ;
		if ( !tObjman.Init())//m_obj = *GAMEADR_BASE_COBJECTMANAGER
		{
			return FALSE ;
		}
		return Init( tObjman.GetPlayerSelfAdr() ) ;//tObjman.GetPlayerSelfAdr() = m_obj= return *(+74))
	}

	DWORD GetFuncPtrTable() { return GetVal<DWORD,DWORD>(0) ;};
	char* GetClassName()
	{
		DWORD ptr_func = GetFuncPtrTable() ;
		if ( !ptr_func )
			return NULL ;		

		ptr_func = *(DWORD*)(ptr_func + Decrypt::m_dwOffsetData[GAMEOFFSET_OBJFUNCPTR_CLASS]) ;
		if ( IsBadCodePtr((FARPROC) ptr_func ) )
			return NULL ;
		
		DWORD ptr_class_name = 0 ;
		__asm
		{
			call ptr_func ;
			mov ptr_class_name , eax 
		}

		if ( IsBadReadPtr( (void*)ptr_class_name , 4 ) )
			return NULL ;
		ptr_class_name = *(DWORD*)ptr_class_name ;

		if ( IsBadStringPtrA( (LPCSTR)ptr_class_name , 0x50 ) )
			return NULL ;
		return (char*)ptr_class_name ;
	} ;

	int GetIndex() { return GetVal<int,int>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_INDEX]) ;} ;
	DWORD GetID() { return GetVal<DWORD,DWORD>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_ID]) ;} ;
	float GetX() { return GetVal<float,float>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_X]) ;} ;
	float GetY() { return GetVal<float,float>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_Y]) ;} ;
	float GetZ() { return GetVal<float,float>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_Z]) ;} ;
	int GetLogic() { return GetVal<int,int>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_LOGIC]);} ;

	DWORD GetResourcePtr() { return GetVal<DWORD,DWORD>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_PRESOURCE]) ;} ;
	DWORD GetInfoPtr() { return GetVal<DWORD,DWORD>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_POBJINFO]) ; } ;//0x1ec
	DWORD GetActionPtr() { return GetVal<DWORD,DWORD>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_PACTION]) ; } ;
	memdata::objectattrex* GetAttrExPtr() { return GetVal<memdata::objectattrex*,DWORD>(Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_ATTREX]) ;} ;

	void SetLogic(int val) { SetVal<int>( Decrypt::m_dwOffsetData[GAMEOFFSET_OBJ_LOGIC] , val); } ;
};

class CAccountInfo : public CObject
{
public:
	virtual BOOL Init( DWORD dwObject = (Decrypt::m_dwDatatAddr[GAMEADR_BASE_WORLDMAP] - 0x10) )
	{
		if ( ::IsBadReadPtr((void*) dwObject , 4) )
			return FALSE ;
		return CObject::Init( *(DWORD*)dwObject , 0x100 ) ;
	} ;

	inline void GetAccountName(char *pBuffer , int buffer_len)
	{
		int offset_des = Decrypt::m_dwOffsetData[GAMEOFFSET_LOGIN_ACCOUNTNAME] ;
		DWORD adr_des = m_dwObject + offset_des ;
		int len = _tl_string_length( (void*)adr_des ) ;
		strncpy_s( pBuffer , buffer_len , _tl_string_c_str((void*)adr_des ) , len < buffer_len ? len : buffer_len - 1 ) ;// Username = pBuffer
	}
};
