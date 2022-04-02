## 1.romfs 介绍

romfs  是一种简单的只读文件系统，通常用来当做初始文件系统来使用的， 在嵌入式 linux 通常使用它作为引导系统的文件系统，作为过渡。

这里使用 romfs 来构建一个小型文件系统，方便将文件烧录到 flash 里使用，如音频、图片等，无需转化为 hex 数组加入程序中、编译下载，并且能形成一个文件层次结构,，可用使用 open、read 等文件读操作函数。

## 2 romfs 文件系统的制作

romfs 除了需要相关 代码提供的 api 来读取操作外，还需要使用工具制作一个 romfs 文件系统的镜像，这里使用genromfs 工具来实现镜像制作：

#### 2.1 编译 genromfs 工具

SDK 库这里 提供了 genromfs  源码与 makefile 文件，需要在 linux 环境下编译出可执行文件，以当前路径在bl_mcu_sdk 为例：

```
# 进入文件夹路径
$ cd components/romfs/genromfs
# 编译
$ make
```

之后在 components/romfs/genromfs 下面会出现一个 genromfs 可执行文件，查看使用帮助：

```
$ ./genromfs -h
```



#### 2.2. 放入需要的文件

建立一个文件夹，将需要的文件与文件夹放入到其中，注意所有文件与文件夹名只能支持：小写字符 “a~z”、大写字符 “A-Z”、数字 “0~9”、下划线 “_”、横线 “-”、点 “.” 。其他字符不支持，并且长度不能超过 14 字节。



#### 2.3 使用 genromfs 制作出镜像

使用以下命令 从选中的文件夹生成镜像文件：

```
$ ./genromfs -d <source_>  -f  <image.bin>
```

其中 <source_> 是需要打包的文件夹路径，<image.bin> 是要生成的 romfs 文件系统镜像，例如在就在当前目录 genromfs 下有一个文件夹 picture，那么执行以下命令就能生成它的 romfs 文件系统镜像文件 picture.bin :

```
$ ./genromfs -d picture -f picture.bin
```

此时就会生成一个文件 picture.bin ，这就是我们生成的 romfs 文件系统镜像，里面包含了 picture 文件夹的所有文件与层次结构



## 3. 将 romfs 烧录到 flash

​	生成的 romfs 文件系统镜像还需要烧录到 MCU 的 flash 中，用 BouffaloLabDevCube 工具烧录：

1. 打开 BouffaloLabDevCube 工具，选择 MCU 型号，进入主页面
2. 在左上角 View 菜单下选择 IOT 模式 
3. 勾选 Single Download Config 下方的 Enable 框，点击 Browse 选择之前生成的 镜像文件
4. 在 Enable 框 右边填入 要烧录的 flash 地址，注意位置要大于用户程序的大小，避免覆盖了用户代码，如烧录在512K位置：0x80000
5. 选择正确的串口号，点击 Create&Download 按键下载。



## 4. romfs 使用

经过以上步骤，你需要的文件已经通过 romfs 镜像文件系统烧录进入 flash 中了，即可通过 romfs 代码使用了，但在使用前你需要指定 flash 中 romfs 文件系统的位置。

打开 components/romfs/genromfs/bl_romfs.h 文件，找到宏：

```c
/* romfs address,plase Change the address for your romfs-image programming*/
#define ROMFS_ROOT_ADDRESS (0x23000000 - 0x2000 + 0x80000)
```

其中最后的 0x80000 就是我们在上面烧录步骤中设置的烧录位置，根据自己实际修改。关于 0x23000000 - 0x2000 是与实际地址映射设置的，需要了解请参考其他文档

最后，调用 romfs_mount() 函数，即可使用 romfs文件系统了，如：

```c
romfs_file_t img_file;
char buff[512];

romfs_mount();
romfs_open(&img_file, "/romfs/img_1.jpg", 0);
romfs_read(&img_file, buff, 512);
romfs_close(&img_file);
```

