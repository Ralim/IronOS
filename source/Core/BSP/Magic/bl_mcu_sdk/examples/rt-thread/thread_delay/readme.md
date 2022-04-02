**注意 rt-thread使用start_gcc.S替代entry.S,所以编译的时候需要将entry.S在cmakelist中屏蔽**

```bash

$ make APP=thread_delay BOARD=bl706_iot

```