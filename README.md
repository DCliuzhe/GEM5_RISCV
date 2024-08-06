<a name="hiXQh"></a>
# GEM5服务器部署
本文档将展示如何在服务器上部署GEM5，在部署前需先在服务器下使用conda创建属于自己的python环境，python>=3.9<br />conda使用：[共享Conda使用说明 (yuque.com)](https://www.yuque.com/chunzhen-ksrdg/ahtgif/eg42ihcvos5fo1vl)
<a name="Sb3zd"></a>
## Step 1：安装与环境配置：
```shell
git clone https://github.com/gem5/gem5
cd gem5/
pip install -r requirement.txt
```
<a name="ZpjvR"></a>
## Step 2：编译GEM5
由于编译GEM5时，在默认文件夹下检索不到 **libpython3.9.so.1.0** 文件，因此需要找到并将其额外加入至环境变量，通常处于/data/anaconda3/envs/env_name/lib文件夹下
```cpp
export LD_LIBRARY_PATH=/data/anaconda3/envs/liuzhe/lib:$LD_LIBRARY_PATH$
```
编译过程中服务器上可能会出现gcc版本不匹配的情况，因此需要额外指定gcc版本。如我的服务器上gcc是9.4.0，但所需版本是10.0，因此在编译指令前加上CC=gcc-10 CXX=g++-10
```bash
CC=gcc-10 CXX=g++-10 scons build/{ISA}/gem5.opt -j {thread num} --python=/data/anaconda3/envs/liuzhe/lib
```
<a name="HVTeq"></a>
## Step 3：运行示例程序
编译完成后，会在gem5的build/RISCV/目录下生成一个gem5.opt可执行文件，需要指定一个处理器架构文件和一个编译过后的但运行文件，这里gem5项目提供了运行示例：
```bash
 ./build/RISCV/gem5.opt configs/deprecated/example/se.py -c tests/test-progs/hello/bin/riscv/linux/hello --output=sim.out --errout=sim.err
```
运行后的结果如下：<br />![image.png](https://cdn.nlark.com/yuque/0/2024/png/28537930/1722239418643-e0eb8f86-c685-4f95-b01f-b3e7dcb72c24.png#averageHue=%231e1c1b&clientId=uc89ed86f-49b9-4&from=paste&height=397&id=u77b2f488&originHeight=397&originWidth=1691&originalType=binary&ratio=1&rotation=0&showTitle=false&size=85615&status=done&style=none&taskId=u9d531190-cf43-4708-91d5-4e8833dda15&title=&width=1691)<br />这里指定了输出到**sim.out**文件，运行报错到**sim.err**文件，可以在**gem5/m5out**目录下看到。并且在目录下有**stats.txt**文件记录了程序运行的详细信息。<br />如果需要运行自己编译的程序，则需额外安装riscv toolchain，使用其中的交叉编译器编译程序成二进制文件。<br />如果是自己搭建的架构，不指定是看不到输出结果的，在命令行中增加-stats-file参数指定输出文件路径，即可获得仿真过程中运行的信息。<br />参考：[gem5: Building gem5](https://www.gem5.org/documentation/learning_gem5/part1/building/)
<a name="kdSTm"></a>
# GEM5基础架构搭建
GEM5中所有的对象构建都继承自同一个基类——SimObject 位于项目/src/python/m5文件夹下，m5也是必须要导入的一个库。 SimObject 的大部分实现是用 C++ 编写的，但 gem5 提供了一种机制，可以将这些 C++ 对象导出到 Python 中。这意味着用户可以使用 Python 配置脚本来创建和操作 SimObject，而不需要直接编写 C++ 代码。  
```python
import m5
from m5.object inmport *
```
<a name="pZAZK"></a>
## 创建系统以及系统时钟域
利用System类创建system对象，所有的系统组件的父类均为System，可以用它创建系统的时钟域，下面给出一个创建简单时钟域并指定时钟域的时钟频率和电压域的例子：
```python
system = System()
#设置系统时钟域
system.clk_domain = SrcClockdomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()
```
时钟域还有其他选择，例如DerivedClockDomian()，即派生时钟域，可以从另一个系统时钟域派生过来，二者时钟频率可以不同。另外每一个时钟域内要指定一个电压域，这里使用VoltageDomain初始化，默认值为1V。<br />具体类的代码实现可以在/src/sim文件夹下找到。
<a name="DTOER"></a>
## 设置系统内存
系统内存设置至少需要指定两个参数，系统内存模式和系统的内存地址范围：mem_mode与mem_ranges。<br />系统内存模式分为四类："timing", "invalid", "atomic"以及“atomic_nocaching"<br />`timing`： 精确模拟每个内存操作的时序行为，包括延迟、总线争用等，适用于详细的性能分析。  <br />`invalid`：  这是一个占位符模式，表示当前内存模式无效或未设置。    <br />`atomic`： 每个内存操作在一次调用中完成，不模拟实际的时序延迟，适用于快速功能模拟。  <br />`atomic_nocaching` ： 类似于 `atomic`模式，但不进行缓存，直接与主存通信。  <br />![image.png](https://cdn.nlark.com/yuque/0/2024/png/28537930/1722395973163-58b6158c-9e08-4cb8-8c2d-cee5d65724f3.png#averageHue=%23252220&clientId=ub7dfa964-ab34-4&from=paste&height=66&id=u4f01640d&originHeight=57&originWidth=530&originalType=binary&ratio=1.75&rotation=0&showTitle=false&size=4811&status=done&style=none&taskId=ua59c8c35-a115-4dc5-b7cd-30bb97e10aa&title=&width=613.8571472167969)<br />mem_ranges则是直接指定大小即可（需要带单位）。下面给出设置timing内存模式，地址空间为512MB例子：
```python
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]
```
<a name="FQduA"></a>
## 设置系统CPU
系统cpu设置需要指定system.cpu参数，可选的CPU类型如下：<br />`AtomicSimpleCPU`： 一种原子模式 CPU 模型，不模拟实际的时序延迟，适用于快速功能模拟。  <br />`NonCachingSimpleCPU`： 一种简单的 CPU 模型，它不使用缓存，而是直接与内存进行交互。  <br />`O3CPU`： 一种复杂的乱序执行 CPU 模型，模拟现代处理器的行为，适用于详细的性能分析和架构研究。  <br />`MinorCPU`： 一种简化的乱序执行 CPU 模型，适用于中等复杂度的性能分析。  <br />`TimingSimpleCPU`： 一种简单的时序 CPU 模型，它并不模拟复杂的流水线结构。  <br />![image.png](https://cdn.nlark.com/yuque/0/2024/png/28537930/1722395940949-b97d5cc5-8eec-4fbe-a3c0-d496a44424f3.png#averageHue=%23222120&clientId=ub7dfa964-ab34-4&from=paste&height=496&id=uf3f642b9&originHeight=868&originWidth=859&originalType=binary&ratio=1.75&rotation=0&showTitle=false&size=100456&status=done&style=none&taskId=uc058187d-59bf-4e77-a6fe-9c0732bb4fb&title=&width=490.85714285714283)<br />下面给出设置RISCV，TimingSimpleCPU的示例：
```python
system.cpu = RiscvTimingSimpleCPU()
```
> 设置CPU时，默认是64位

<a name="H8Bnc"></a>
## 设置系统内存总线
系统内存总线有两大类，分别是CoherentXBar和NonconherentXBar，二者均继承自父类BaseXBar，最基本的两个端口是cpu_side_ports和mem_side_ports。<br />![image.png](https://cdn.nlark.com/yuque/0/2024/png/28537930/1722397369713-ea30f582-623e-4d54-a459-a3a6548198bf.png#averageHue=%23242220&clientId=ub7dfa964-ab34-4&from=paste&height=199&id=u0a7ce65c&originHeight=348&originWidth=586&originalType=binary&ratio=1.75&rotation=0&showTitle=false&size=27624&status=done&style=none&taskId=u66c1c87b-b6ae-4574-900a-b12490e0be6&title=&width=334.85714285714283)<br />基于CoherentXBar派生出SystemXBar和L2XBar，基于NoncoherentXBar派生出IOXBar。<br />`SystemXBar`： 是一个通用的交叉开关总线，主要用于连接 CPU、内存控制器、缓存和其他系统级设备。  <br />`L2XBar`： 是一个专用于二级缓存（L2 Cache）的交叉开关总线。它连接多个一级缓存（L1 Cache）和二级缓存，处理缓存层次结构中的数据传输。  <br />`IOXBar`： 一个专用于输入输出设备的交叉开关总线。它处理系统中的外设和 I/O 设备之间的通信。  <br />下面给出使用SystemBar的例子：
```python
system.membus = SystemXBar()
#将总线与cpu端口接在一起
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports
```
<a name="xTbg5"></a>
## 设置系统中断控制器
 创建并配置一个中断控制器（Interrupt Controller）来管理和处理 CPU 的中断请求。示例如下：
```python
system.cpu.createInterruptController()
```
<a name="CfcDq"></a>
## 设置系统内存控制器

1. 先为系统初始化一个MemCtrl
2. 可以指定mem_ctrl连接的DRAM模型，GEM5提供了部分DRAM模型的接口，并定义了DRAM模型参数，接口文件路径为/mem/src/DRAMInterface.py。
3. 之后再将定义后的内存地址空间赋给内存控制器
4. 最后将内存控制器接在总线上
```python
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports
```
至此，我们已经搭建好了如下图所示的系统模型：<br />![image.png](https://cdn.nlark.com/yuque/0/2024/png/28537930/1722399935828-7e669df8-2632-4dad-8a60-3129c9c64ec2.png#averageHue=%23e1e1e1&clientId=ub7dfa964-ab34-4&from=paste&height=610&id=ua81298cf&originHeight=1067&originWidth=1477&originalType=binary&ratio=1.75&rotation=0&showTitle=false&size=87190&status=done&style=none&taskId=u82d41057-98cf-4f12-89ef-0312517c582&title=&width=844)
<a name="nS0Le"></a>
# GEM5中Cache的使用
<a name="Ydas6"></a>
## GEM5中Cache的分类
在GEM5中，Cache分为两大类，一类是Classic Cache，一类是Ruby Cache。因为GEM5是GEMS与m5两个的结合，在GEMS中使用的是Ruby Cache，在m5中则使用的是Ruby Cache。Ruby Cache相比于Classic Cache，其模型更加复杂，并且更细致地考虑了Cache一致性问题，且提供了许多一致性协议供用户选择。<br />因为Cache一致性问题并不是我们所考虑的重点，因此接下来将主要讲述Classic Cache的使用。
<a name="POK7P"></a>
## Cache的参数
所有的Cache类同样继承自SimObject这了父类，基于这个父类，GEM5在/mem/cahce/Cache.py文件中给出了BaseCache类的定义，BaseCache派生出NoncoherentCache和Cache。<br />一些基本的参数如下：<br />`size`：Cache的大小<br />`assoc`：Cache组相联的配置<br />`tag_latency`：Cache中数据的tag比对花费的周期<br />`data_latency`：数据访问延迟周期<br />`response_latency`：Cache未命中时，到下一级内存获取数据并返回所需的周期<br />`write_buffers`：写缓冲区的数量，默认是支持8个写请求<br />`is_read_only`：Cache是否只可读（适用于指令缓存）<br />`replacement_policy`： Cache的替换策略，默认是替换LRU策略使用进行替换。<br />`writeback_clean`：是否采用writeback策略，默认使用<br />更多参数可以参考具体文件中BaseCache的定义。
<a name="UxWEp"></a>
## Cache的使用
有关Cache的使用，可以关注Basic_Test文件目录下的`simple_withCache.py`和`test_Cache.py`两个文件。`test_Cache.py`从Cache类派生出实验所需的L1 Cache和L2 Cache两种类，并进一步从L1 Cache派生出L1ICache和L1DCache用于本次实验。`simple_withCache.py`将test_Cache中的类引入，并搭建了一个具有L1，L2两级Cache的处理器，如下图所示：<br />![image.png](https://cdn.nlark.com/yuque/0/2024/png/28537930/1722927314911-52e37e7a-49de-4764-aca7-ed9919e5bfc1.png#averageHue=%23e2e2e2&clientId=u427b6683-7037-4&from=paste&height=905&id=GWuIv&originHeight=1583&originWidth=1184&originalType=binary&ratio=1.399999976158142&rotation=0&showTitle=false&size=113738&status=done&style=none&taskId=u2f7f92f6-2063-41e1-8d8f-92be6983232&title=&width=676.5714285714286)<br />simple_nocache.txt和simple_cache.txt给出了运行在有Cache和无Cache架构下测试一个16×16GEMM Kernel的stats数据。







