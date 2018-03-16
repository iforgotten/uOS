# uOS

# Bootloader

> dd if=xxxx.bin of=xxx.img bs=512 count=1 conv=notrunc  
> qemu -fda xxx.img  

# 备注
暂时不会用qemu进行无symbol的反汇编调试，用bochs的反汇编功能代替  

# 参考资料：  
使用Qemu + gdb调试内核  
http://blog.csdn.net/iamljj/article/details/5655169  
Inline assembly for x86 in linux  
https://www.ibm.com/developerworks/library/l-ia/index.html  
修复依赖项命令  
sudo apt-get install -f  
