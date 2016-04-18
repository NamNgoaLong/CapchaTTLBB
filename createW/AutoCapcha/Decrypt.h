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

enum PACKETRECV_MESSAGE_TYPE { 
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
