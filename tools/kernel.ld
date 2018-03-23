OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kern_init)

SECTIONS {
	. = 0x100000;

	.text : {
		*(.text .stub .text.* .gnu.linkonce.t.*)
	}

	PROVIDE(etext = .);

	. = ALIGN(0x1000);

	.data : {
		*(.data)
	}

	PROVIDE(edata = .);

	.bss : {
		*(.bss)
	}
	
	PROVIDE(end = .);

	/DISCARD/ : {
		*(.en_frame .note.GNU-stack)
	}
}