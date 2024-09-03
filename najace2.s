.section sekcija

.weak klopa

ld $0xFFFFFEFE, %sp

ld $0x315234, %r1
ld $0, %r4

add %r1,%r4,shr 1
add %r4,%r3, shl 1
klopa:

halt
.end