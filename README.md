# 用到的工具
1. eclipse-cdt  
2. zylin Embedded CDT  
3. gdb  
4. gcc, objcopy, objdump, dd  
5. IDA pro 6.4 linux
6. qemu-system-i386
7. make  

# 备注
1. Ubuntu修复依赖项命令  
sudo apt-get install -f  
2. Ubuntu安装Eclipse-CDT  
sudo apt-get install eclipse-cdt  
3. Ubuntu安装Zylin Embeded CDT，要求EclipseCDT的版本低于4.5.即，JVM8版本的eclipse  
(1) eclipse -> Help -> Install New Software -> 输入以下地址，进行安装  
Zylin Embedded CDT - http://opensource.zylin.com/zylincdt  
4. Ubuntu安装qemu-system-i386
sudo apt-get install qemu  
sudo ln -s /usr/bin/qemu-system-i386 /usr/bin/qemu  
5. Ubuntu安装gdb  
sudo apt-get install gdb  

# 参考资料：  
1. 使用Qemu + gdb调试内核  
http://blog.csdn.net/iamljj/article/details/5655169  
2. Inline assembly for x86 in linux  
https://www.ibm.com/developerworks/library/l-ia/index.html  
3. 8259A中断控制器  
http://218.5.241.24:8018/C35/Course/ZCYL-HB/WLKJ/jy/Chap08/8.3.4.htm  
4. 1987_80386_Hardware_Reference_Manual.pdf  
http://www.s100computers.com/My%20System%20Pages/80386%20Board/1987_80386_Hardware_Reference_Manual.pdf  
5. 8255A datasheet  
http://www.ndr-nkc.de/download/datenbl/i8255.pdf  
6. X86 Assembly/Programmable Interrupt Controller  
https://en.wikibooks.org/wiki/X86_Assembly/Programmable_Interrupt_Controller  
7. uCore OS Tsinghua  
https://legacy.gitbook.com/book/chyyuu/simple_os_book/details  
8. ucore_os_lab chyyuu  
https://github.com/chyyuu/ucore_os_lab  
9. TSS: Task state segment  
https://en.wikipedia.org/wiki/Task_state_segment  
10. 可编程时钟控制器8253  
https://blog.csdn.net/u013007900/article/details/50408903  
11. Intel 64 and IA32 Architectures Software Developer Instruction Set Reference Manual  
https://software.intel.com/sites/default/files/managed/39/c5/325462-sdm-vol-1-2abcd-3abcd.pdf 
