.global tss_flush
tss_flush:
    mov 4(%esp), %eax     # the selector, 0x28
    ltr %ax               # load task register
    ret
