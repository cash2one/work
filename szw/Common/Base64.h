 
#ifndef _BASE64_INCLUDE__H__
#define _BASE64_INCLUDE__H__
#include <string.h>
#include <stdio.h>


//*************************************************************************************
// 定义Base64编码命名空间：
//*************************************************************************************
namespace stringcoding
{
    static const char
    _B64_[64]={ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
                'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
                'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
                'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
                'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
                'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
                'w', 'x', 'y', 'z', '0', '1', '2', '3',
                '4', '5', '6', '7', '8', '9', '+', '/'
                };
    //--------------------------------------------------
    //Encodeing:
    //--------------------------------------------------
    inline void encodetribyte(unsigned char * in,unsigned char * out,int len)
    {
        if(len==0) return;
        int i;
        unsigned char inbuf[3];
        memset(inbuf,0,sizeof(unsigned char)*3);
        for(i=0;i<len;i++)
        {
            inbuf[i]=in[i];
        }
        out[0]=_B64_[ inbuf[0]>>2 ];
        out[1]=_B64_[ ((inbuf[0]&0x03)<<4)|((inbuf[1]&0xf0)>>4) ];
        //if(len>1) means len=={2,3}
        //else means len==1, out[2]='=';
        out[2]=(len>1?_B64_[ ((inbuf[1]&0x0f)<<2)|((inbuf[2]&0xc0)>>6) ]:'=');
        //if(len>2) menas len==3
        //else means len=={1,2} out[3]='=';
        out[3]=(len>2?_B64_[ inbuf[2]&0x3f ]:'=');
    }
    //--------------------------------------------------
    //Decoding:
    //--------------------------------------------------
    inline int decodetribyte(unsigned char * in, unsigned char * out)
    {   
        int i,j,len;
        char dec[4];
        memset(dec,0,sizeof(char)*4);
        len=3;
        //Get effective original text char count:
        if(in[3]=='=') len--;
        if(in[2]=='=') len--;
        //Find code according to input charactors:
        for(i=0;i<64;i++)
        {
            for(j=0;j<4;j++)
            {
                if(in[j]==_B64_[i]) dec[j]=i;
            }
        }
        //Re-compose original text code:
        out[0]=(dec[0]<<2|dec[1]>>4);
        if(len==1) return 1;
        out[1]=(dec[1]<<4|dec[2]>>2);
        if(len==2) return 2;
        out[2]=(((dec[2]<<6)&0xc0)|dec[3]);
        return 3;
    }

    //Encode input byte stream, please ensure lenghth of out
    //buffer big enough to hold all codes.
    //The b64 code array size is 4*(tri-byte blocks in original text).
    inline int Base64Encode(unsigned char * b64,const unsigned char * input,ULONG stringlen=0)
    {
        if(!b64||!input||stringlen<0) return 0;
        ULONG slen,imax;
        register unsigned int i,idin,idout;
        int rd,re,len;
        slen=(stringlen)?stringlen:strlen((char *)input);
        if(slen==0) return 0;
        rd=slen%3;
        rd=(rd==0)?3:rd;
        //Maximun tri-byte blocks:
        imax=(slen+(3-rd))/3-1;    
        for(i=0;i<=imax;i++)
        {
            idin =i*3;
            idout=i*4;
            len=(i==imax)?rd:3;
            encodetribyte((unsigned char *)&input[idin],&b64[idout],len);
        }
        re=(imax+1)*4;
        b64[re]='\0';
        return re;
    }
    //Decode input byte stream, please ensure lenghth of out
    //buffer big enough to hold all codes.
    //The b64 code array size is about 3*(quad-byte blocks in b64 code).
    inline int Base64Decode(unsigned char * output,const unsigned char * b64,ULONG codelen=0)
    {
        if(!output||!b64||codelen<0) return 0;
        ULONG slen,imax;
        register unsigned int i,idin,idout;
        int rd,re,len;
        slen=(codelen)?codelen:strlen((char *)b64);
        if(slen<4) return 0;
        rd=slen%4;
        if(rd!=0) return 0;    //Code error!.
        imax=slen/4-1;
        for(i=0;i<=imax;i++)
        {
            idin =i*4;
            idout=i*3;
            len=decodetribyte((unsigned char *)&b64[idin],&output[idout]);
        }
        re=(imax*3)+len;
        output[re]='\0';
        return re;
    }
//-----------------------------------------------------------------------------------------
    //--------------------------------------------------
    // Convert CString to char * string
    // Return string length:
    //--------------------------------------------------
    inline unsigned char * CString2Char(const CString & str,int & converted)
    {
        UINT len;
        DWORD error;
        unsigned char * buf=0;
        if(str==L"") return 0;
        len=str.GetLength()*sizeof(TCHAR)/sizeof(unsigned char)+1;
        buf=new unsigned char[len];
        memset(buf,0,sizeof(unsigned char)*len);
        converted=WideCharToMultiByte(CP_ACP,0,str,str.GetLength(),(char *)buf,len,0,0);
        if(converted==0)
        {
            error=GetLastError();
        }
        return buf;
    }
    //----------------------------------------
    // 对CString字符串进行base64编码，
    // 编码后字符串变长大约1/3：
    //----------------------------------------
    inline CString StringBase64Encode(CString & text)
    {
        CString dest;
        unsigned char * bufchar=0,* buf64=0;
        UINT sz,bytes;
        //............................................
        // Convert text (Unicode) to UTF-8:
        //............................................
        //A. Get buffer size when converted to UTF-8.
        //   Remember to add one byte to hold '\0':
        bytes=WideCharToMultiByte(CP_UTF8,0,text.GetBuffer(),text.GetLength(),NULL,0,0,0);
        //B. Convert string to UTF-8 format:
        sz=bytes+1;
        bufchar=new unsigned char[sz];
        memset(bufchar,0,sizeof(unsigned char)*sz);
        WideCharToMultiByte(CP_UTF8,0,text.GetBuffer(),text.GetLength(),(char *)bufchar,bytes,0,0);
        //............................................
        // Convert UTF-8 to base64 code:
        //............................................
        sz=(bytes+(3-bytes%3))/3*4+1;
        buf64=new unsigned char[sz];    //Multiply 2 to avoid memory overflow.
        memset(buf64,0,sizeof(unsigned char)*sz);
        Base64Encode(buf64,bufchar);//Convert to Base64 code.
        dest=buf64;                    //Get base64 code
        //............................................
        // Delete pointers and return:
        //............................................
        if(bufchar)
            delete [] bufchar;
        if(buf64)
            delete [] buf64;
        return dest;
    }
    //----------------------------------------
    // 对CString字符串进行base64解码：
    // 解码后字符串变短大约1/4：
    //----------------------------------------
    inline CString StringBase64Decode(CString & base64code)
    {
        CString dest;
        unsigned char * buf64=0, * bufu8=0;
        wchar_t * bufchar=0;
        UINT sz;
        int u8len;
        //............................................
        //Convert base64 code from Unicode to char:
        //............................................
        buf64=CString2Char(base64code,(int&)u8len);
        if(buf64==0)
        {
            delete [] buf64;
            return L"";
        }
        //............................................
        //Decode base64 code, we obtain string in
        // UTF-8 format:
        //............................................
        bufu8=new unsigned char[u8len*2];    //When decoding, string size will decrease,
                                //therefore size of bufu8 is the same as
                                //that of buf64.
        memset(bufu8,0,sizeof(unsigned char)*u8len*2);
        Base64Decode(bufu8,buf64,u8len);
        //............................................
        // Convert UTF-8 to original text (Unicode):
        //............................................
        //A. Get buffer size when converted to text:
        sz=MultiByteToWideChar(CP_UTF8,0,(char *)bufu8,u8len,NULL,0);
        //B. Decode string to original text:
        bufchar=new wchar_t[sz];
        memset(bufchar,0,sizeof(wchar_t)*sz);
        MultiByteToWideChar(CP_UTF8,0,(char *)bufu8,u8len,bufchar,sz);
        dest=bufchar;//Get original text
        if(buf64)
            delete [] buf64;
        if(bufu8)
            delete [] bufu8;
        if(bufchar)
            delete [] bufchar;
        return dest;
    }
//*************************************************************************************
// Base64编码命名空间结束
//*************************************************************************************
};
#endif // #ifndef _BASE64_INCLUDE__H_