# -*- coding:utf-8 -*-
import serial
import serial.tools.list_ports
import struct
import time
import sys
import random
import threading
import datetime


###############################################################################
#文件设置
###############################################################################
writesize = 50*1024
# readsize = 0*1024
filesize = 5*1024*1024
###############################################################################
#COM设置
###############################################################################
loopbackcom = "COM21"
baudrate = 2000000
###############################################################################
#单串口回环任务
###############################################################################
def loopback_tx_thread(func):
    print("loopback thread start")
    txalllen = 0
    txdatalen = 0
    txflag = 0
    txendtime = datetime.datetime.now()
    rxalllen = 0
    rxdatalen = 0
    rxflag = 0
    rxendtime = datetime.datetime.now()
    begin = datetime.datetime.now()
    while True:
        if txflag == 0:
            if txalllen < filesize:
                loopback_send_data = loopback_send_obj.read(writesize)
                txdatalen = loopback_serial.write(loopback_send_data)
                txalllen += txdatalen
            else:
                txflag = 1
                print("loopback send end")
                break
    end = datetime.datetime.now()
    k = end - begin
    print("loopback tx time cost:", k)
    time.sleep(0.2)
    print("loopback tx thread end")

###############################################################################
#单串口回环任务
###############################################################################
def loopback_rx_thread(func):
    print("loopback thread start")
    txalllen = 0
    txdatalen = 0
    txflag = 0
    txendtime = datetime.datetime.now()
    rxalllen = 0
    rxdatalen = 0
    rxflag = 0
    rxendtime = datetime.datetime.now()
    begin = datetime.datetime.now()
    while True:
        if rxalllen < filesize:
            loopback_recv_data = loopback_serial.read(writesize)
            rxdatalen = loopback_recv_obj.write(loopback_recv_data)
            rxalllen += rxdatalen
        else:
            rxflag = 1
            print("loopback recv end")
            break
    end = datetime.datetime.now()
    k = end - begin
    print("loopback rx time cost:", k)
    time.sleep(0.2)
    print("loopback rx thread end")

###############################################################################
#创建原始文件
###############################################################################
#create usb send file
begin = datetime.datetime.now()
loopback_send_obj = open("loopback_send_file.txt", "w+")
for i in range(filesize):
    loopback_send_obj.write(str(random.randint(0, 9)))
loopback_send_obj.close()
end = datetime.datetime.now()
k = end - begin
print("file time cost:", k)

#create uart receive file
loopback_recv_obj = open("loopback_recv_file.txt", "w+")
loopback_recv_obj.close()

print("files created")

###############################################################################
#打开原始文件
###############################################################################
#open source file for read
loopback_send_obj = open("loopback_send_file.txt", "rb+")

#open target file for write
loopback_recv_obj = open("loopback_recv_file.txt", "wb+")

print("files opened")

###############################################################################
#打开USB和UART串口
#PC端serial的波特率不准确，比606稍快，在发送内容较短时可通过减小PC端波特率解决
#uart_serial = serial.Serial("COM15", 2500000 - 20000, timeout = 2)
#uart_serial = serial.Serial("COM15", 5000000 - 100000, timeout = 2)
###############################################################################
#open usb serial for read & write
loopback_serial = serial.Serial(loopbackcom, baudrate, timeout = 0)
loopback_serial.set_buffer_size(rx_size = 1024 * 1024, tx_size = 1024 * 1024)

print("serial opened")

###############################################################################
#将各任务加入线程列表
###############################################################################
#threads list
threads = []
loopback_tx_th = threading.Thread(target=loopback_tx_thread, args=("1",))
loopback_rx_th = threading.Thread(target=loopback_rx_thread, args=("2",))
threads.append(loopback_tx_th)
threads.append(loopback_rx_th)
###############################################################################
#####################################MAIN######################################
###############################################################################
if __name__ == '__main__':
    time.sleep(1)
    loopback_tx_th.setDaemon(True)
    loopback_rx_th.setDaemon(True)
    loopback_rx_th.start()
    loopback_tx_th.start()
    loopback_rx_th.join()
    loopback_tx_th.join()
    loopback_send_obj.close()
    loopback_recv_obj.close()
    loopback_serial.close()
    print("all end")
    print("")
###############################################################################
