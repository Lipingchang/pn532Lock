import base64
from Crypto.Cipher import AES
import json
import datetime

class AESCipher:

    def __init__(self, key,iv):
        self.key = key[0:16] #只截取16位
        self.iv = iv # key #"\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" # 16位字符，用来填充缺失内容，可固定值也可随机字符串，具体选择看需求。

    def __pad(self, text):
        """填充方式，加密内容必须为16字节的倍数，若不足则使用self.iv进行填充"""
        text_length = len(text)
        amount_to_pad = AES.block_size - (text_length % AES.block_size)
        if amount_to_pad == 0:
            amount_to_pad = AES.block_size
        pad = chr(amount_to_pad)
        return text + pad * amount_to_pad

    def __unpad(self, text):
        pad = ord(text[-1])
        return text[:-pad]

    def encrypt(self, raw):
        """加密"""
        raw = self.__pad(raw)
        cipher = AES.new(self.key, AES.MODE_CBC, self.iv)
        return base64.b64encode(cipher.encrypt(raw)) 

    def decrypt(self, enc):
        """解密"""
        enc = base64.b64decode(enc)
        cipher = AES.new(self.key, AES.MODE_CBC, self.iv )
        return self.__unpad(cipher.decrypt(enc).decode("utf-8"))


passin = False
inputfilename = "enAES.txt"
outputfilename = "deAES.txt"
with open(inputfilename,'r') as inputf:
    with open(outputfilename,'w') as outputf:
        lines = inputf.readlines();
        #第一行　是加密数据　第二行是密码
        enc_str = lines[0].split()[0]
        key = lines[1].split()[0]

        if( len(key) < 16 ):
            key = key + '0'*(16- len(key) )
        if( len(key) > 16 ):
            key = key[0:16]

        print("enc_str<"+enc_str+">key<"+key+">")

        iv = "0000000000000000" #key[0:16]
        e = AESCipher(key,iv)
        try:
            dec_str = e.decrypt(enc_str)
            print('dec str: ' + dec_str) 
            j = json.loads(dec_str)
            print(j)
            start = j["start"]
            end =j["end"]
            if start < int(datetime.datetime.now().timestamp()) < end :
                outputf.write(j["guestName"])
                print("success in "+j["guestName"])
                passin = True #(1);
            else:
                print("time not right")

        except BaseException as e :
            #outputf.write("")
            print('decrypt fail {0}'.format(e))
if passin:
    exit(1)
else:
    exit(0)





    # e.encrypt(secret_data)

    # print('enc_str: ' + enc_str.decode())
    
    
