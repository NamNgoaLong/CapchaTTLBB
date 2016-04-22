//ver:1.0
#pragma		once
#include	<atlstr.h>
#include	<stdio.h>
#include	<vector>
#include	<windows.h>
//#include	"function.h"

using namespace std;
#pragma optimize("", off)
class CBuff
{
private:
	char *hbuff;	//缓存指针 // con tro cua buffer
	ULONG csize;	//缓存大小 // kich thuoc buffer
	volatile ULONG cpos;		//当前位置 // Vi tri hien tai
public:

	//len 大小， buf,复制原缓存
	//void Print(){
	//	DPrint("===========Print()hbuff=%x", hbuff);
	//}	
	CBuff(ULONG len = 0, char * buf = 0)
	{
		hbuff = 0; csize = 0; cpos = 0;
		if (len) resize(len);
		if (buf && hbuff) memcpy(hbuff, buf, len);
	}

	~CBuff(){clear();}

	//改变大小
	char *resize(ULONG len)
	{
		if (len || hbuff) hbuff = (char *)realloc(hbuff, len);
		csize = len;
		return hbuff;
	};

	//清除
	void clear()
	{
		if (hbuff) free(hbuff);
		hbuff = 0; csize = 0; cpos = 0;
	}

	//添加缓存到最后
	char *add_back(char * buf, ULONG len)
	{
		ULONG addr = csize;
		resize(csize + len);
		if (buf) memcpy(buff(addr), buf, len);
		else memset(buff(addr), 0, len);
		return buff(addr);
	}

	//添加缓冲并不改变大小，返回添加指针
	char *add_buf(int len)
	{
		ULONG addr = csize;
		resize(csize + len);
		csize = addr;
		return buff(addr);
	}

	//插入
	char *insert(ULONG pos, char * buf, ULONG len)
	{
		if (pos > csize)
			pos = csize;
		resize(csize + len);
		memmove(buff(pos + len), buff(pos), csize - len - pos);
		if (buf)
			memcpy(buff(pos), buf, len);
		else
			memset(buff(pos), 0, len);
		return buff(pos);
	}

	//设置缓存
	void set_buff(char * buf, ULONG len)
	{
		resize(len);
		if (buf) memcpy(buff(), buf, len);
		else memset(buff(), 0, len);
	}

	//删除缓存
	void del(ULONG pos, ULONG len)
	{
		ULONG src = pos + len;
		if (src >= csize){resize(pos); return ;}
		memmove(buff(pos), buff(src), csize - src);
		resize(csize - len);
	}

	//获得缓存地址
	char *buff(int addr = 0){return hbuff + addr;}

	//从当前位置移动
	char *seek_cur(int pos = 0){cpos += pos; return hbuff + cpos;}
	//从开始位置移动
	char *seek_set(int pos = 0){cpos = pos; return hbuff + cpos; }
	//从结束位置移动
	char *seek_end(int pos = 0){cpos = csize + pos; return hbuff + cpos;}

	//判断位置是否可用
	bool ispos(int pos = 0){return (cpos + pos) <= csize;}

	//获得当前指针 
	char *tell_buf(int pos = 0){return hbuff + cpos + pos;}

	//获当前得位置
	ULONG tell_pos(){return cpos;}

	//获得大小
	ULONG size(){return csize;}
	//获得位置到结束长度
	ULONG end_len(){return csize - cpos;}


	DWORD64 htonll(DWORD64 value)
	{
		return ( ( (DWORD64)htonl((DWORD32)value) ) << 32 ) | htonl((DWORD32)(value >> 32));
	}

	//网络字节
	template<class T> 
	T bswap(T a)
	{
		if (sizeof(T) == 2) a = (T)htons( (WORD)a );
		else if(sizeof(T) == 4) a = (T)htonl( (DWORD32)a );
		else if(sizeof(T) == 8) a = (T)htonll( (DWORD64)a );
		return a;
	}

//写入数据-----------------------------------------------------------------------------
	template<class T> 
	void write(T a)
	{
		ULONG len = cpos + sizeof(T);
		if (len > csize) resize(len);
		*(T *)(buff(cpos)) = a; 
		seek_cur(sizeof(T));
	}
	template<class T> 
	void writer(T a){write(bswap(a));}

	//写入网络字节
	void writerq(DWORD64 a){ writer(a); }
	void writerd(DWORD a){ writer(a); }
	void writerw(WORD a){ writer(a); }

	//写入
	void writee(double a){ write(a); }
	void writef(float a){ write(a); }
	void writeq(DWORD64 a){ write(a); }
	void writed(DWORD a){ write(a); }
	void writew(WORD a){ write(a); }
	void writeb(BYTE a){ write(a); }

	//写入数据
	void writem(char * a, int len)
	{
		resize(csize + len);
		if (a) memcpy(tell_buf(), a, len);
		else memset(tell_buf(), 0, len);
		seek_cur(len);
		return;
	}

	//写入带结束符的文本
	int writes(char * a, int len = 0)
	{
		if (!len){
			len = strlen(a) + 1;
			resize(csize + len);
		}else{
			resize(csize + len);
			memset(tell_buf(), 0, len);
		}
		if (a) strcpy_s(tell_buf(), len, a);
		seek_cur(len);
		return len;
	}
	//写入带结束符Unicode文本,未指定长度写入带结束符
	template<class D>
	int writews(D &str, int wlen = 0)
	{
		int len = 0;
		CStringW wstr = str;
		len = wstr.GetLength() * 2;
		wlen *= 2;
		writem((char *)wstr.GetBuffer(), len);
		if (!wlen){
			write<WORD>(0);
		}else{
			if (wlen > len) writem(0, wlen - len);
		}
		return len;
	}

	//写入带长度头的文本
	template<class T>
	int writest(char * a, int len = 0)
	{
		if (!len) len = strlen(a);
		write<T>(len);
		writem(a, len);
		return len;
	}

	//写入带网络字节长度头的文本
	template<class T>
	int writesr(char * a, int len = 0)
	{
		if (!len) len = strlen(a);
		writer<T>(len);
		writem(a, len);
		return len;
	}



	//写入带长度头的数据
	template<class T>
	void writemt(char * a, int len)
	{
		write<T>(len);
		writem(a, len);
		return;
	}

	//写入带网络字节长度头的数据
	template<class T>
	void writemr(char * a, int len)
	{
		writer<T>(len);
		writem(a, len);
		return;
	}

	//写入带长度的Unicode文本
	template<class T, class D>
	int writewt(D &str)
	{
		CStringW wstr = str;
		int len = wstr.GetLength() * 2;
		writemt<T>((char *)wstr.GetBuffer(), len);
		return len;
	}


	//写入带头的UTF8文本
	template<class T, class D>
	int writet_utf8(D &str)
	{
		CStringW wstr = str;
		char *cbuf;
		int len;
		len = WideCharToMultiByte(CP_UTF8, 0, wstr.GetString(), wstr.GetLength(), 0, 0, 0, 0);
		cbuf = (char *)malloc(len);
		len = WideCharToMultiByte(CP_UTF8, 0, wstr.GetString(), wstr.GetLength(), cbuf, len, 0, 0);

		writemt<T>((char *)cbuf, len);
		free(cbuf);
		return len;
	}
//读取数据-----------------------------------------------------------------------------

	template<class T>
	T read(){if (!ispos(sizeof(T))) return 0; T a = *(T *)(tell_buf()); seek_cur(sizeof(T)); return a;}

	template<class T>
	T readr(){return bswap(read<T>());}

	//读取网络字节
	DWORD64 readrq(){ return readr<DWORD64>();}
	DWORD readrd(){ return readr<DWORD>(); }
	WORD readrw(){ return readr<WORD>(); }

	//读取
	double reade(){ return read<double>();}
	float readf(){ return read<float>(); }
	DWORD64 readq(){ return read<DWORD64>(); }
	DWORD readd(){ return read<DWORD>(); }
	WORD readw(){ return read<WORD>(); }
	BYTE readb(){ return read<BYTE>(); }

	//读取文本
	int reads(char * a, int len = 0)
	{
		if (!len){
			len = strnlen(tell_buf(), csize - cpos);
		}
		len = readm(a, len);
		if (a){a[len] = 0;}
		return len;
	}

	//读取带结束符Unicode文本,指定长度不读取结束符，未指定长度读取结束符
	template<class D>
	int readws(D &str, int wlen = 0)
	{
		int len;
		if (!wlen){
			wlen = wcsnlen((const wchar_t *)tell_buf(), (csize - cpos) / 2);// Tra ve kich thuoc cua chuoi
			len = (wlen + 1) * 2;
		}else{
			len = wlen * 2;
		}
		CStringW wstr((const wchar_t *)tell_buf(), wlen);
		seek_cur(len);
		str = wstr;
		return len;
	}

	//读取带长度头的文本
	template<class T>
	int readst(char * a)
	{
		int len = read<T>();
		if (!ispos(len)) return 0;
		if (a){memcpy(a, tell_buf(), len); a[len] = 0;}
		seek_cur(len);
		return len;
	}

	//读取带长度头的文本
	template<class T>
	int readst(CString &cstr)
	{
		int len = read<T>();
		if (!ispos(len)) return 0;
		cstr.SetString(tell_buf(), len);
		seek_cur(len);
		return len;
	}

	//读取带网络字节长度头的文本
	template<class T>
	int readsr(char * a)
	{
		int len = readr<T>();
		if (!ispos(len)) return 0;
		if (a){memcpy(a, tell_buf(), len); a[len] = 0;}
		seek_cur(len);
		return len;
	}

	//读取带网络字节长度头的文本
	template<class T>
	int readsr(CString &cstr)
	{
		int len = readr<T>();
		if (!ispos(len)) return 0;
		cstr.SetString(tell_buf(), len);
		seek_cur(len);
		return len;
	}

	//读取数据
	int readm(char * a, int len)
	{
		if (!ispos(len)) return 0;
		if (a) memcpy(a, tell_buf(), len);
		seek_cur(len);
		return len;
	}

	//读取带长度头的数据
	template<class T>
	int readmt(char * a)
	{
		int len = read<T>();
		return readm(a, len);
	}

	//读取带长度头的数据
	template<class T>
	int readmt(vector<char> &out, int slen = 0)
	{
		int len = read<T>();
		out.resize(len + slen);
		if (slen) memset(&out[len], 0, slen);
		return readm(&out[0], len);
	}

	//读取带网络字节长度头的数据
	template<class T>
	int readmr(char * a)
	{
		int len = readr<T>();
		return readm(a, len);
	}

	//读取带长度头的Unicode文本
	template<class T>
	int readwt(CStringA &cstr, char *out = 0)
	{
		CStringW wstr;
		int len = read<T>();
		if (!ispos(len)) return 0;
		wstr.SetString((LPWSTR)(tell_buf()), len / 2);
		cstr = wstr;
		if (out) strcpy(out, cstr.GetString());
		seek_cur(len);
		return len;
	}

	//读取带长度头的UTF8文本
	template<class T, class D>
	int readt_utf8(D &str)
	{
		CStringW wstr;
		wchar_t *wbuf;
		int len = read<T>();
		if (!ispos(len)) return 0;
		int ulen;
		ulen = MultiByteToWideChar(CP_UTF8, 0, tell_buf(), len, 0, 0);
		wbuf = wstr.GetBufferSetLength(ulen);
		ulen = MultiByteToWideChar(CP_UTF8, 0, tell_buf(), len, wbuf, ulen);
		wstr.ReleaseBuffer();
		str = wstr;
		seek_cur(len);
		return len;
	}

//设置数据-----------------------------------------------------------------------------
	template<class T>
	void set(T a, ULONG pos = 0)
	{
		ULONG len = tell_pos() + pos + sizeof(T);
		if (len > csize) resize(len);
		*(T *)(tell_buf(pos)) = a;
	}

	template<class T>
	void setr(T a, ULONG pos = 0){set(bswap(a), pos);}

	//设置网路字节
	void setrq(DWORD64 a, ULONG pos = 0){ setr(a, pos); }
	void setrd(DWORD a, ULONG pos = 0){ setr(a, pos); }
	void setrw(WORD a, ULONG pos = 0){ setr(a, pos); }

	//设置
	void sete(double a, ULONG pos = 0){ set(a, pos); }
	void setf(float a, ULONG pos = 0){ set(a, pos); }
	void setq(DWORD64 a, ULONG pos = 0){ set(a, pos); }
	void setd(DWORD a, ULONG pos = 0){ set(a, pos); }
	void setw(WORD a, ULONG pos = 0){ set(a, pos); }
	void setb(BYTE a, ULONG pos = 0){ set(a, pos); }
//获得数据-----------------------------------------------------------------------------
	//获得当期位置数据
	template<class T>
	T get(ULONG pos = 0)
	{
		if (!ispos(pos + sizeof(T))) return 0;
		return (*(T *)(tell_buf(pos)));
	}

	template<class T, class D>
	bool get(ULONG pos, D &value)
	{
		if (!ispos(pos + sizeof(T))) return false;
		value = *(T *)(tell_buf(pos));
		return true;
	}

	template<class T, class D>
	bool getr(ULONG pos, D &value)
	{
		if (!ispos(pos + sizeof(T))) return false;
		value = bswap<T>(*(T *)(tell_buf(pos)));
		return true;
	}

	//获取网络字节
	DWORD64 getrq(ULONG pos = 0){ return htonll(get<DWORD64>(pos)); }
	DWORD getrd(ULONG pos = 0){ return htonl(get<DWORD>(pos)); }
	WORD getrw(ULONG pos = 0){ return htons(get<WORD>(pos)); }

	//获取
	double gete(ULONG pos = 0){ return get<double>(pos); }
	float getf(ULONG pos = 0){ return get<float>(pos); }
	DWORD64 getq(ULONG pos = 0){ return get<DWORD64>(pos); }
	DWORD getd(ULONG pos = 0){ return get<DWORD>(pos); }
	WORD getw(ULONG pos = 0){ return get<WORD>(pos); }
	BYTE getb(ULONG pos = 0){ return get<BYTE>(pos); }
};
#pragma optimize("", on)