0000000000000000 <main>:
   0:   55                      push   %rbp
   1:   48 89 e5                mov    %rsp,%rbp
   4:   c7 45 fc 04 00 00 00    movl   $0x4,-0x4(%rbp)
   b:   83 7d fc 00             cmpl   $0x0,-0x4(%rbp)
   f:   74 06                   je     17 <main+0x17>
  11:   83 45 fc 04             addl   $0x4,-0x4(%rbp)
  15:   eb 04                   jmp    1b <main+0x1b>
  17:   c1 65 fc 02             shll   $0x2,-0x4(%rbp)
  1b:   b8 00 00 00 00          mov    $0x0,%eax
  20:   5d                      pop    %rbp
  21:   c3                      retq  