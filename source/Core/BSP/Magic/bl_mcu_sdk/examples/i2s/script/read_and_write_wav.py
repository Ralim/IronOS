# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""
from __future__ import print_function
from builtins import range
import os
import wave
import struct
import time
import re
import subprocess
import binascii

# bh6bao 2021.03.05

from pydub import AudioSegment
filePath = './'

# 操作函数
def get_wav_make(dataDir):
    sound= AudioSegment.from_wav(dataDir)
    duration = sound.duration_seconds * 1000  # 音频时长（ms）
    begin = 0
    end = 10000
    cut_wav = sound[begin:end]   #以毫秒为单位截取[begin, end]区间的音频
    cut_wav.export(filePath+ 'fhm_onechannel_8k_10.wav', format='wav')   #存储新的wav文件

# wav文件转PCM数组
def get_wave_array_str(filename, target_bits):
    wave_read = wave.open(filename, "r")
    array_str = "// start\n\t"
    nchannels, sampwidth, framerate, nframes, comptype, compname = wave_read.getparams()
    sampwidth *= 8
    for i in range(wave_read.getnframes()):
        val, = struct.unpack("<H", wave_read.readframes(1))
        scale_val = (1 << target_bits) - 1
        cur_lim   = (1 << sampwidth) - 1
        # scale current data to 8-bit data
        val       = val * scale_val / cur_lim
        val       = int(val + ((scale_val + 1) // 2)) & scale_val
        array_str += "0x%02x, " % (val)
        if (i + 1) % 16 == 0:
            array_str += "\n\t"
    return array_str


# PCM 数组合并
def gen_wave_table(wav_file_list, target_file_name, scale_bits=8):
    with open(target_file_name, "w") as audio_table:
        print('#include <stdio.h>', file=audio_table)
        print("// time:{}".format(time.strftime('%Y-%m-%d %H:%M:%S',time.localtime(time.time()))), file=audio_table)
        print('const unsigned char audio_table[] = {', file=audio_table)
        for wav in wav_file_list:
            print("processing: {}".format(wav))
            print('//name:{}\n'.format(wav), file=audio_table)
            print(get_wave_array_str(filename=wav, target_bits=scale_bits), file=audio_table)
        print('};\n', file=audio_table)
    print("Done...")


# 获取wav文件信息
def get_wave_info(filename):
    wave_read = wave.open(filename, "r")
    nchannels, sampwidth, framerate, nframes, comptype, compname = wave_read.getparams()

    print('nchannels:{}'.format(nchannels))
    print('sampwidth:{}'.format(sampwidth))
    print('framerate:{}Hz'.format(framerate))
    print('nframes:{}'.format(nframes))
    print('comptype:{}'.format(comptype))
    print('compname:{}'.format(compname))
    return framerate


# 二进制文件转数组
def file2array(filename,h_filename=None):
    path, name = os.path.split(filename)
    if h_filename is None:
        h_filename = os.path.join(path, name.split('.')[0] +'.h')

    array_str = '\n//total len:{} bytes\n'.format(os.path.getsize(filename))
    array_str += 'const unsigned char '+name.split('.')[0]+'[]={\n\t'
    # print(array_str)

    h_file = open(h_filename, "w")
    file = open(filename, 'rb')
    try:
        while True:
            chunk = file.read(16*32)
            if not chunk:
                break

            for i in range(len(chunk)):
                array_str += "0x%02x, " % (chunk[i])
                if (i + 1) % 16 == 0:
                    array_str += "\n\t"
            # print('get:{}'.format(binascii.b2a_hex(chunk)))
    except:
        print('read ERR')
    finally:
        print('read file done!')
        array_str += '\n};\r\n'
        # print('array='+array_str)
        print(array_str, file=h_file)

    h_file.close()
    file.close()
    return 0



# ffmpeg路径
ffpeg_path = 'D:/BouffaloLabWS/script_py/ffmpeg_4.3.2_full_build/bin'

# amr文件转MP3
def amr2mp3(amr_path,mp3_path=None):
    path, name = os.path.split(amr_path)
    if name.split('.')[-1]!='amr':
        print('not a amr file')
        return 0
    if mp3_path is None or mp3_path.split('.')[-1]!='mp3':
        mp3_path = os.path.join(path, name.split('.')[0] +'.mp3')
    error = subprocess.run(['./ffmpeg.exe','-i',amr_path,mp3_path])
    print('ERR:'+error)
    if error:
        return 0
    print ('success')
    return mp3_path

# wav文件转amr,wav必须为8000Hz
def wave2amr(wave_path,amr_path=None):
    path, name = os.path.split(wave_path)
    if name.split('.')[-1]!='wav':
        print ('not a wave file')
        return 0
    if amr_path is None or amr_path.split('.')[-1]!='amr':
        amr_path = os.path.join(path, name.split('.')[0] +'.amr')
    error = subprocess.run(['ffmpeg','-i',wave_path,'-y',amr_path])
    print('ERR:{}'.format(error))
    if error:
        return 0
    print ('amr success')
    return amr_path

# wav 格式化为8000Hz
def wave2Hz8000(wave_path,out_path=None):
    path, name = os.path.split(wave_path)
    if name.split('.')[-1]!='wav':
        print ('not a wave file')
        return 0
    if out_path is None or wave_path.split('.')[-1]!='wav':
        out_path = os.path.join(path, name.split('.')[0] +'_8k.wav')
    error = subprocess.run(['ffmpeg.exe','-i',wave_path,'-ar','8000','-y',out_path])
    #error = subprocess.run(ffpeg_path+'ffmpeg.exe'+' -i '+wave_path+' -ar 8000 -y '+out_path)
    print('ERR:{}'.format(error))
    if error:
        return 0
    print ('wav success')
    print (out_path)
    return out_path

# 将目录下所有wav文件转为amr，并转为对应的数组文件
if __name__ == '__main__':
    print("Generating audio array...")
    # wav_list = []
    # for filename in os.listdir("./"):
    #     # print(os.getcwd())
    #     if filename.endswith(".wav"):
    #         print('src_name:',filename)
    #         framerate = get_wave_info(filename)
    #         if framerate != 8000:
    #             print('to 8000Hz')
    #             filename = wave2Hz8000(filename)
    #             print(filename)

    #         amr_file = wave2amr('fhm_onechannel_8k.wav')
    #         if amr_file!=0:

    get_wav_make('fhm_onechannel_8k.wav')
    wave2amr('fhm_onechannel_8k_10.wav')
    file2array('fhm_onechannel_8k_10.wav')

            # dst_name = "0_"+filename
            # print('dst_name:',filename)
            # os.rename(filename, dst_name)
            # wav_list.append(filename)
    # gen_wave_table(wav_file_list=wav_list, target_file_name="audio_example_file.h")


