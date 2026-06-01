ENTRY(_start)

SECTIONS
{
	. = 1M;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.multiboot)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
		*(.rodata.*)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}
  
  .heap BLOCK(4K) : ALIGN(4K)
    {
        heap_start = .;
        . += 1M;        /* 1MB static heap */
        heap_end = .;
    }
}
