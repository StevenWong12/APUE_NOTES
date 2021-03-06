# 文件I/O

1. 本章使用的函数被称为不带缓冲的I/O，不带缓冲指的是每个read和write都调用内核中的一个系统调用

## open和openat函数

```c
int open(const char *path, int oflag, ...);
int openat(int fd, const char *path, int oflag, ...);
```

ISO C 将...表示余下的参数的数量和类型是可变的



## creat函数

```c
int creat(const char *path, mode_t mode);

// open(path, O_WRONLY|O_CREAT|o_TRUNC, mode)
```



## lseek函数

​	每个打开文件都有一个与其相关联的*当前文件偏移量（current file offset）*，读写都是从当前文件偏移量处开始，打开文件时，除非指定O_APPEND，否则偏移量被设置为0

​	可以调用lseek显示地为一个打开文件设置偏移量，若成功则返回新的文件偏移量

```c
off_t lseek(int fd, off_t offset, int whence);
```

| whence   | 说明                                         |
| -------- | -------------------------------------------- |
| SEEK_SET | 将文件偏移量设置为距文件开始处offset个字节   |
| SEEK_CUR | 设置问距当前值加offset，可正可负             |
| SEEK_END | 将文件偏移量设置为文件长度加offset，可正可负 |

**文件偏移量可以大于文件当前长度，对该文件下一次写将加长该文件，并在文件中构成一个空洞**



## read函数

read成功则返回读到的字节数，如已到达文件的尾端，则返回0

```c
ssize_t read(int fd, void *buf, size_t nbytes);
```

* 假如到文件尾端只有30个字节，而要求读100个字节，则read返回30，下一次再调用则返回0
* 从终端设备读时最多读一行
* 从网络读时，缓冲机制可能导致返回值小于要求读的字节数
* 从管道或FIFO读时，若管道包含字节数较少时返回实际可用的字节数

