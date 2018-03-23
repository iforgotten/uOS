#include "defs.h"
#include "x86.h"
#include "elf.h"

const unsigned int SECTORSIZE = 512;

// 大于1MB的空间，即0x100000开始，是真正的扩展空间
// 0x100000 是入口点
// bootloader的边界 - 0x000A0000是空闲空间
const struct elfhdr* ELFHDR = (struct elfhdr*)(0x090000);

static void
waitdisk() {
    // 0x1f7 状态和命令寄存器
    // 操作时先给命令，再读取内容
    // 如果不是忙状态就从0x1f0端口读数据
    // AT hard disk, 7:Busy, 6: Drive ready
    // 所以当其标志寄存器位数为：0100 0000 = 0x40h时
    // 磁盘就绪

    while((inb(0x1F7) & 0xC0) != 0x40);
}

/*
 *  CHS(cylinders-heads-sectors，磁柱-磁头-扇区)寻址模式
 *  LBA 设备
    https://baike.baidu.com/item/LBA/2025827
    0x1F2: AT hard disk controller
           Sector count
           Read / Write count of sectors for operation.

    0x1F3: AT hard disk controller
           sector number
           Read / Write current/starting logical sector number.
           LBA(Logical Block Address) 0 - 7

    0x1F4: AT hard disk controller
           Cylinder high (bits 0-1 are bits 8-9 of 10-bit cylinder number)
           LBA 模式的8-15

    0x1F5: AT hard disk controller
           Cylinder low (bits 0-7 of 10-bit cylinder number)
           LBA 模式的16-23位

    0x1F6: AT hard disk controller:
           Drive & Head.
           Read/Write: bits indicate head, drive for operation
           第0~3位：如果是LBA模式就是24-27位
           第4位：为0主盘；为1从盘
           第6位：为1=LBA模式；0 = CHS模式
           第7位和第5位必须为1

    0x1F7: AT hard disk
           command register:
           1?H = Restore to cylinder 0
           7?H = Seek to cylinder
           2?H = Read sector
           3xH = Write sector
           50H = Format track
           4xH = verify read
           90H = diagnose
           91H = set parameters for drive
**/

// 从第@secno扇区，读取数据到dst
static void
readSector(void *dst, uint32_t secno) {
    // 等待磁盘是否可用
    waitdisk();

    // 告诉磁盘控制器，读写的扇区数为1
    outb(0x1F2, 1);
    // 0bit - 7bit
    outb(0x1F3, (secno & 0xFF));
    // 8bit - 15bit
    outb(0x1F4, ((secno >> 8) & 0xFF));
    // 16bit - 23bit
    outb(0x1F5, ((secno >> 16) & 0xFF));

    // 通过0x1F6设置为LBA模式访问磁盘
    // 1110 0000 == 0xE0
    // 低4位为 24bit - 27bit
    outb(0x1F6, ((secno >> 24) & 0xF) | 0xE0);

    // 0x2x == 读取扇区
    outb(0x1F7, 0x20);

    // 等待磁盘准备好
    waitdisk();

    // 拷贝数据到内存中
    insl(0x1F0, dst, SECTORSIZE / 4);

} // 采用CPU查询模式来访问IO


// 从内核中，从@offset中读取@count个字节的数据到@va处
static void
readSegment(uintptr_t va,uint32_t count, uint32_t offset) {
    // 计算结束地址
    uintptr_t end_va = va + count;

    va -= (offset % SECTORSIZE);

    // 计算数据所在的扇区
    uint32_t secno = (offset / SECTORSIZE) + 1;

    // 如果没有到结束地址，循环读数据，每次读一个扇区
    // 一个扇区一个扇区的读数据
    // 不足512字节的，算作512字节; 即：按512字节对齐
    while(va < end_va) {
        // 从磁盘上读取数据
        readSector((void*)va, secno);
        va += SECTORSIZE;
        ++secno;
    }
}

// 将ELF可执行文件从链接地址映射到内存
void
bootmain(void)
{
    // 读取1号扇区
    // 根据intel白皮书，页最小位4KB
    // 可能读多了
    readSegment((uint32_t)ELFHDR, (SECTORSIZE*8), 0);
    // 判断是否是合法的ELF文件
    if(ELFHDR->e_magic != ELF_MAGIC) {
        // 不是合法的ELF文件
        goto bad;
    }

    // 读取OS二进制文件的入口地址
    //  ELF Header ： Executable and Linkable Format
    /*
     *  e_entry : 程序的入口地址
     *  e_phoff : program header table 在文件中的偏移量（以字节计数）
     *  e_ehsize : Elf header size
     *  e_phnum : 有几个段
     * */

    // Program Header : 描述的是程序运行时所需要加载的一个段
    /*
     * p_offset 段的第一个字节在文件中的偏移
     * p_vaddr  段的第一个字节在内存中的虚拟地址
     * p_filesz 段在文件中的长度
     * p_memsz  段在内存中的长度
     * */
    struct proghdr *ph;
    ph = (struct proghdr *)((uintptr_t)ELFHDR + ELFHDR->e_phoff);

    // 加载所有的节，即：段内容进内存
    int i = 0;
    while(++i < ELFHDR->e_phnum) {
        readSegment(ph->p_va, ph->p_memsz,ph->p_offset);
        ph++;
    }

    // 并跳转过去，将控制权交给操作系统
    ((void (*)(void))(ELFHDR->e_entry))();

bad:

    // do nothing
    while(1);
}
