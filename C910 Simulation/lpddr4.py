# Copyright (c) 2012-2021 Arm Limited
# All rights reserved.
#
# The license below extends only to copyright in the software and shall
# not be construed as granting a license to any other intellectual
# property including but not limited to intellectual property relating
# to a hardware implementation of the functionality of the software
# licensed hereunder.  You may use the software subject to the license
# terms below provided that you ensure that this notice is replicated
# unmodified and in its entirety in all distributions of the software,
# modified or unmodified, in source code or in binary form.
#
# Copyright (c) 2013 Amin Farmahini-Farahani
# Copyright (c) 2015 University of Kaiserslautern
# Copyright (c) 2015 The University of Bologna
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

"""Interfaces for LPDDR4 memory devices

These memory "interfaces" contain the timing,energy,etc parameters for each
memory type and are usually based on datasheets for the memory devices.

You can use these interfaces in the MemCtrl object as the ``dram`` timing
interface.
"""

from m5.objects import DRAMInterface


class LPDDR4X_3733_4x8(DRAMInterface):
    # LPDDR4X的设备大小，调整为4x8配置
    device_size = "512MiB"  # 每个设备大小减少到512MiB

    # 4x8配置，4个设备，每个设备有8位接口
    device_bus_width = 8

    # LPDDR4X的突发长度保持为16
    burst_length = 16

    # 每个设备的行缓冲区大小
    device_rowbuffer_size = "1KiB"  # 行缓冲区大小调整为1KiB

    # 每通道4个设备
    devices_per_rank = 4

    # LPDDR4X通常为单通道，设置为1
    ranks_per_channel = 1

    # LPDDR4X通常具有8个银行
    banks_per_rank = 8

    # 1866 MHz时钟频率，对应的tCK
    tCK = "0.536ns"  # 因为3733 MT/s的实际时钟频率是1866 MHz

    # 16次突发（BL16），对应8个时钟周期
    tBURST = "4.29ns"  # 根据新的tCK计算得到的突发时间

    # LPDDR4X-3733时序参数
    tRCD = "18.5ns"
    tCL = "18.5ns"
    tRP = "18.5ns"
    tRAS = "42ns"
    tRRD = "3.3ns"
    tXAW = "10ns"
    activation_limit = 4
    tRFC = "160ns"

    tWR = "15ns"

    # WTR和RTP参数
    tWTR = "7.5ns"
    tRTP = "7.5ns"

    # 同级读到写转换时间，@1866 MHz = 2个时钟周期
    tRTW = "1.07ns"

    # 不同级的总线延迟时间，@1866 MHz = 2个时钟周期
    tCS = "1.07ns"

    # 刷新间隔，<=85C，为7.8us
    tREFI = "7.8us"

    # 活动降电和预充电降电退出时间
    tXP = "5ns"

    # 自刷新退出时间
    tXS = "200ns"

    # 电流值，通常基于实际芯片数据手册的电流消耗
    IDD0 = "45mA"
    IDD2N = "20mA"
    IDD3N = "25mA"
    IDD4W = "100mA"
    IDD4R = "110mA"
    IDD5 = "150mA"
    IDD3P1 = "25mA"
    IDD2P1 = "20mA"
    IDD6 = "2mA"
    VDD = "1.1V"

