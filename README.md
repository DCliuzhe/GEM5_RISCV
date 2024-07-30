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
由于编译GEM5时，在默认文件夹下检索不到 **libpython3.9.so.1.0** 文件，因此需要找到并将其额外将其加入至环境变量，通常处于/data/anaconda3/envs/env_name/lib文件夹下
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
运行后的结果如下：<br />![image.png](https://cdn.nlark.com/yuque/0/2024/png/28537930/1722239418643-e0eb8f86-c685-4f95-b01f-b3e7dcb72c24.png#averageHue=%231e1c1b&clientId=uc89ed86f-49b9-4&from=paste&height=397&id=u77b2f488&originHeight=397&originWidth=1691&originalType=binary&ratio=1&rotation=0&showTitle=false&size=85615&status=done&style=none&taskId=u9d531190-cf43-4708-91d5-4e8833dda15&title=&width=1691)<br />这里指定了输出到**sim.out**文件，运行报错到**sim.err**文件，可以在**gem5/m5out**目录下看到。并且在目录下有**stats.txt**文件记录了程序运行的详细信息。<br />如果需要运行自己编译的程序，则需额外安装riscv toolchain，使用其中的交叉编译器编译程序成二进制文件。<br />参考：[gem5: Building gem5](https://www.gem5.org/documentation/learning_gem5/part1/building/)
