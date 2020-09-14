'''
hex-dump.py
https://qiita.com/taka_baya/items/c22bd5f5e7cb3de90988
'''

import sys
import os.path
import binascii
import string


def is_character_printable(s):
    if s < 126 and s >= 33:
        return True 

def validate_byte_as_printable(byte):
    if is_character_printable(byte):
        return byte
    else:
        return 46

#f = open('neko.wav','rb')
f = open('mic.wav','rb')

chunk = f.read(1024)

memory_address = 0
ascii_string = ""

for byte in chunk:
  ascii_string = ascii_string + chr(validate_byte_as_printable(byte))
  if memory_address%16 == 0:
    print(format(memory_address, '06X'),end = '')
    print(" " + hex(byte)[2:].zfill(2), end='')
  elif memory_address%16 == 15:
    print(" " + hex(byte)[2:].zfill(2),end='')
    print(" " + ascii_string)
    ascii_string = ""
  else:
    print(" " + hex(byte)[2:].zfill(2), end='')
  memory_address = memory_address + 1
   