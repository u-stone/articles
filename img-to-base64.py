#!/usr/bin/python
# -*- coding: utf-8 -*-

import base64
# import clipboard
import sys

# print('Argument List:', str(sys.argv[1:]))

# 二进制方式打开图文件
f=open(str(sys.argv[1]),'rb')
ls_f=base64.b64encode(f.read()) #读取文件内容，转换为base64编码
f.close()

# clipboard.set(ls_f.decode("utf-8"))
# pyperclip.copy(ls_f.decode("utf-8"))
# spam = pyperclip.paste()

fw=open('./base64.txt', 'w')
fw.write(ls_f.decode("utf-8"))
fw.close()
