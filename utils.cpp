int charset_convert(const char *from_charset, 
                    const char *to_charset,
                    char *in_buf, 
                    size_t in_left, 
                    char *out_buf, 
                    size_t out_left) {
        iconv_t icd;
        char *pin = in_buf;
        char *pout = out_buf;
        size_t out_len = out_left;
        if ((iconv_t)-1 == (icd = iconv_open(to_charset,from_charset))) {
            return -1; 
        }   
        if ((size_t)-1 == iconv(icd, &pin, &in_left, &pout, &out_left)) {
            iconv_close(icd);
            return -1; 
        }   
        out_buf[out_len - out_left] = 0;
        iconv_close(icd);
        return (int)out_len - out_left;
        return 0;
}

int gbk2utf8(const string&src, string& dst)
{
    size_t out_len = src.size() * 3;
    char *out_buf = new char[out_len];

    if(-1 == charset_convert("GB2312", "UTF-8", (char *)src.c_str(), (size_t)src.size(),
                                                       out_buf, out_len)){
        delete []out_buf;
        return -1; 
    }   

    dst.assign(out_buf);
    delete []out_buf;

    return 0;
    
}

int utf8togbk(const string& src, string& dst)
{
    size_t out_len = src.size() * 3;
    char *out_buf = new char[out_len];

    if (-1 == charset_convert("UTF-8", "GB2312", (char *)src.c_str(), (size_t)src.size(),
                                                        out_buf, out_len)){
        delete []out_buf;
        return -1;
    }

    dst.assign(out_buf);
    delete []out_buf;

    return 0;
}

/* compress gzip data */
int gzcompress(Bytef *data, uLong ndata,
               Bytef *zdata, uLong *nzdata)
{

    if(data && ndata > 0) {
        z_stream c_stream;
        c_stream.zalloc = NULL;
        c_stream.zfree = NULL;
        c_stream.opaque = NULL;
        c_stream.avail_in  = ndata;
        c_stream.next_in  = data;
        c_stream.total_out = 0;
        c_stream.avail_out = 0;
        if(deflateInit2(&c_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                        31, 8, Z_DEFAULT_STRATEGY) == Z_OK)
        {
            while(c_stream.avail_out == 0)
            {
                if(c_stream.total_out >= *nzdata)
                {
                    DEBUG_P(LOG_FATAL, "defination of nzdata is too short for gzip compress! [%s:%d]\n", __FILE__, __LINE__);
                    return -2;
                }
                c_stream.next_out = zdata + c_stream.total_out;
                c_stream.avail_out = *nzdata - c_stream.total_out;
                deflate(&c_stream, Z_FINISH);

            }
            if(deflateEnd(&c_stream) != Z_OK) return -3;
            *nzdata = c_stream.total_out;
            return 0;
        }
    }
    return -3;
}

/* Uncompress gzip data */
int gzdecompress(Bytef *zdata, uLong nzdata,
                 Bytef *data, uLong *ndata)
{
    if(zdata && nzdata > 0){
        z_stream d_stream;
        d_stream.zalloc = NULL;
        d_stream.zfree = NULL;
        d_stream.avail_in = nzdata;
        d_stream.next_in  = zdata;
        d_stream.total_out = 0;
        d_stream.avail_out = 0;

        if(inflateInit2(&d_stream, 47) == Z_OK)
        {
            int status = Z_OK;
            while(status == Z_OK)
            {
                if(d_stream.total_out >= *ndata)
                {
                    DEBUG_P(LOG_FATAL, "defination of ndata is too short for gzip decompress! [%s:%d]\n", __FILE__, __LINE__);
                    return -2;
                }
                d_stream.next_out = data + d_stream.total_out;
                d_stream.avail_out = *ndata - d_stream.total_out;
                status = inflate(&d_stream, Z_SYNC_FLUSH);
            }
            if(inflateEnd(&d_stream) == Z_OK)
            {
                if(status == Z_STREAM_END)
                {
                    *ndata = d_stream.total_out;
                    return 0;
                }
            }
        }
    }
    return -3;
}

string base64_encode(const unsigned char* data,int data_len)
{
    
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    
    string strEncode;
    unsigned char Tmp[4]={0};
    int LineLength=0;
    for(int i=0;i<(int)(data_len / 3);i++)
    {
        Tmp[1] = *data++;
        Tmp[2] = *data++;
        Tmp[3] = *data++;
        strEncode+= EncodeTable[Tmp[1] >> 2];
        strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];
        strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];
        strEncode+= EncodeTable[Tmp[3] & 0x3F];
        if(LineLength+=4,LineLength==76)
        {
            strEncode+="\r\n";      
            LineLength=0;
        }
    }
    
    int Mod=data_len % 3;
    if(Mod==1)
    {
        Tmp[1] = *data++;
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];
        strEncode+= "==";
    }
    else if(Mod==2)
    {
        Tmp[1] = *data++;
        Tmp[2] = *data++;
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];
        strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];
        strEncode+= "=";
    }

    return strEncode;
}

//参考维基百科：
//完整的base64定义可见RFC 1421和RFC 2045。编码后的数据比原始数据略长，
//根据RFC 822规定，每76个字符，还需要加上一个回车换行。
//可以估算编码后数据长度大约为原长的135.1%。
string base64_decode(const char* data,int data_len)
{
    
    const char DecodeTable[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        62, // '+'
        0, 0, 0,
        63, // '/'
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, // '0'-'9'
        0, 0, 0, 0, 0, 0, 0,
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
        13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, // 'A'-'Z'
        0, 0, 0, 0, 0, 0,
        26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
        39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, // 'a'-'z'
    };
    
    string strDecode;
    int nValue;
    int i= 0;
    while (i < data_len)
    {
        if (*data != '\r' && *data!='\n')
        {
            nValue = DecodeTable[*data++] << 18;
            nValue += DecodeTable[*data++] << 12;
            strDecode+=(nValue & 0x00FF0000) >> 16;
            if (*data != '=')
            {
                nValue += DecodeTable[*data++] << 6;
                strDecode+=(nValue & 0x0000FF00) >> 8;
                if (*data != '=')
                {
                    nValue += DecodeTable[*data++];
                    strDecode+=nValue & 0x000000FF;
                }
            }
            i += 4;
        }
        else// 回车换行,跳过: 有些输入会按76字节加回车换行，这里解码跳过
        {
            data++;
            i++;
        }
     }
    return strDecode;
}

vector<string> spilit_str(const string& str, const char* delim)
{
    int len = str.size();
    char *buf= new char[len + 1];
    memcpy(buf, str.c_str(), len + 1);

    char *inner_ptr=NULL;

    vector<string> strs;
    char *p = NULL;
    while((p=strtok_r(buf, delim, &inner_ptr))!=NULL)
    {    
        strs.push_back(p);
        buf=NULL;
    }    

    delete []buf;
    return strs;
}

int ID15to18(const char *str,char *idcard)
{
    bzero(idcard,19);
    int wt[] = {7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
    char ve[] = {'1','0','X','9','8','7','6','5','4','3','2'};
    if (strlen(str) != 15)
    {    
       return -1;
    }    

    int j = 0; 
    int sum = 0; 
    int mode = 0; 
    for(int i = 0; i< 17; i++) 
    {    
       if (i == 6)
       {    
           idcard[i] = '1'; 
       }    
       else if(i == 7)
       {    
           idcard[i] = '9'; 
       }    
       else 
       {    
           idcard[i] = str[j];
           j++; 
       }    
       sum += (idcard[i] - '0') * wt[i];
    }    
    mode = sum % 11;
    idcard[17] = ve[mode];
    idcard[18] = '\0';
    return 0;

}

int checkID(const char * idcard)
{
    char tmp_id[19] = {0};
    if (strlen(idcard) == 15 )
    {
        ID15to18(idcard,tmp_id);
    }
    else
    {
        snprintf(tmp_id,19,"%s",idcard);
    }

    const char * upper_id = toUpper((char *)tmp_id);

    if (preg_match("^[0-9]{17}[0-9xX]{1}$",upper_id) != 0)
    {
        return -1;
    }

    int wt[] = {7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
    char ve[] = {'1','0','X','9','8','7','6','5','4','3','2'};
    int sum = 0;
    int mode = 0;
    for(int i = 0; i < 17; i++)
    {
        sum += (upper_id[i] - '0') * wt[i];
    }
    mode = sum % 11;
    if (ve[mode] == upper_id[17])
    {
        return 0;
    }
    return -1;
}

int parseResult(const string& rsp, map<string, string>& resMap)
{
    int len = rsp.size();
    char *buf= new char[len + 1]; 
    memcpy(buf, rsp.c_str(), len + 1); 
    
    char *outer_ptr=NULL;
    char *inner_ptr=NULL;
    
    char *p = NULL;
    while((p=strtok_r(buf, "&", &outer_ptr))!=NULL)
    {   
        buf = p;
        p = strtok_r(p, "=", &inner_ptr);
        resMap[p] = inner_ptr;
        buf = NULL;
    }   
    
    delete []buf;
    return 0;
}
