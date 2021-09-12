# 进程环境

## main函数

* 当内核执行C程序时（使用exec），调用main前调用一个特殊的启动例程，将此启动例程指定为起始地址。
* 启动例程从内核取得命令行参数和环境变量值

* 若将启动例程以C形式表示，应该是这样的：（实际上常用汇编来写）

```c
exit(main(argc, argv))
```



## 进程终止

有8种方式terminate一个进程，其中5种为正常终止

1. 从main返回
2. 调用exit（执行标准I/O库的清理fclose后进入内核）
3. 调用\_exit和\_Exit（立刻进入内核）
4. 最后一个线程从其启动例程返回
5. 最后一个线程调用pthread_exit

3种为异常终止

	1. 调用abort
	2. 接到信号
	3. 最后一个线程对取消请求做出响应

* main中返回一个值和用该值调用exit是等价的，所以exit(0)和return(0)是等价的
* **函数atexit**：按照ISO C规定，一个进程可以登记至多32个*终止处理函数*（使用atexit来登记）

```c
int atexit(void (*func)(void));
```

* 调用顺序和登记顺序相反，exit会先调用各种终止处理函数后再调用fclose
* 若调用了exec，则将清除所有已安装的中止处理程序



## 环境表

* 每个程序都会收到一张字符指针数组的**环境表**，其中每个指针包含一个以null结束的C字符串的地址（字符串如HOME=/home/sar\0, PTH=:/bin:/usr/bin\0）
* 全局变量environ包含了该指针数组的地址



## C程序的储存空间布局

* 正文段。由CPU执行的机器指令部分。通常是可共享的、只读的
* 初始化数据段（数据段）。包含了程序中需明确地赋予初值的变量，比如在C中任何函数之外的声明

```c
int maxcount = 99;
```



* 未初始化数据段（bss）。程序开始执行之前，内核将此段中的数据初始化为0或空指针

```c
long sum[1000];
```

* 栈。自动变量以及每次函数调用所需保存的信息都保存在此。每次函数调用的返回地址，调用者的环境信息（寄存器值）都放在这此，最近被调用的函数在栈上为其自动和临时变量分配存储空间，**每调用一个函数就产生一个新的栈帧**
* 堆。动态存储分配



## 存储空间分配

```c
void *malloc(size_t size);
void *calloc(size_t nobj, size_t size);
void *realloc(void *ptr, size_t newsize);

void free(void *ptr);
```

* malloc分配存储区的初始值不定
* calloc为指定数量指定长度的对象分配存储空间，分配的每一bit初始化为0
* realloc增加或减少以前的长度，若高地址空间不够可能会移动区域，初始化值不定
* **这些alloc函数大多都是通过sbrk系统调用实现的**
* free后的区域可能不会还给内核，使他们保持在malloc池中



## 函数setjmp和longjmp

* 在c中，goto语句无法跨越函数，而setjmp和longjmp则提供了这个功能

* longjmp非局部goto的原理是在栈上跳过若干调用帧，返回到当前函数调用路径上的某个函数中
* 执行原理：
  * 调用setjmp，将所需信息记入变量jmp_buf并返回0
  * longjmp（可正向可反向）绕道setjmp的位置
  * 如果非第一次到setjmp位置则返回非0
* **longjmp之后仅仅跳过了栈帧，不会更改register中的的值**
* 全局变量、易失变量（volatile）、静态变量（static）的值不受longjmp的影响（值是最近所呈现的值）
* **进行优化之后，局部变量和标记了register的变量存储在寄存器中，故longjmp会损失改变后的值**

### 自动变量（局部变量）的潜在问题

* 观察下面这段代码

```c
#include <stdio.h>

FILE* open_data(void) {
    FILE *fp;
    char databuf[BUFSIZE];
    
    if((fp = fopen("datafile", "r")) == NULL)
        	return NULL;
    if(setvbuf(fp, databuf, _IOLBF, BUFSIZ) != 0) // 定义如何缓冲
        	return NULL;
    return fp;
}
```

* 当open_data返回时，栈上使用的空间由下一个被调用的栈帧使用
* **然而标准I/O库仍将使用这部分存储空间作为流的缓冲区**，应在全局存储空间（static或extern）或动态地分配databuf分配空间



## 函数getrlimit和setrlimit

* 用于获取或设置进程的一组资源限制

```c
int getrlimit(int resource, struct rlimit *rlptr);
int setrlimit(int resource, const struct rlimit *rlptr);

struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
};
```

* resource取的是以*RLIMIT_*开头的一系列宏之一
* **resource的值可以调整（包括但不限于）以下的资源**：
  * RLIMIT_AS：进程总的可用存储空间的最大长度，影响到sbrk和mmap函数
  * RLIMIT_NICE：调整进程的调度优先级
  * RLIMIT_DATA：数据段的最大字节长度
  * ...



## 习题

1. 实际的gcc编译器将其自动优化为了0（gcc允许ISO C扩展的编译选项，则返回值总是0）

2. exit函数结束后，调用fflush将输出写入到stdout

3. *没有其他方式*

4. **C程序解引用空指针出错时，执行该程序的进程会终止，可以利用这种方法来终止进程**

5. 

   ```c
   typedef void Exitfunc(void); 
   ```

6. long型的数字为0，但指针不为null

7. 堆栈大小

8. **只有通过exec执行一个程序时，才会分配堆栈**

9. 链接后会将库的代码也链接入程序

10. 不正确

