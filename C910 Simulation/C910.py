import m5
from m5.objects import *
from C910_Cache import *
from lpddr4 import *

system = System()

system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1.85GHz'
system.clk_domain.voltage_domain = VoltageDomain()

system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('16GB')]

system.cpu = RiscvO3CPU()
system.cpu.isa = RiscvISA(vlen = 128)

system.membus = SystemXBar()

system.cpu.icache = L1ICache()
system.cpu.dcache = L1DCache()
system.cpu.icache.connectCPU(system.cpu)
system.cpu.dcache.connectCPU(system.cpu)

system.l2bus = L2XBar()
system.cpu.icache.connectBus(system.l2bus)
system.cpu.dcache.connectBus(system.l2bus)

system.membus = SystemXBar()
system.l2cache = L2Cache()
system.l2cache.connectCPUSideBus(system.l2bus)
system.l2cache.connectMemSideBus(system.membus)

system.cpu.createInterruptController()
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = LPDDR4X_3733_4x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

binary = './kernel/gemm_v2'

system.workload = SEWorkload.init_compatible(binary)

process = Process()
process.cmd = [binary]
system.cpu.workload = process
system.cpu.createThreads()

root = Root(full_system = False, system = system)
m5.instantiate()
print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick {} because {}'
      .format(m5.curTick(), exit_event.getCause()))