
@{{BLOCK(ball_bn_gfx)

@=======================================================================
@
@	ball_bn_gfx, 24x8@4, 
@	+ palette 16 entries, not compressed
@	+ 3 tiles not compressed
@	Total size: 32 + 96 = 128
@
@	Time-stamp: 2026-04-30, 21:54:28
@	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
@	( http://www.coranac.com/projects/#grit )
@
@=======================================================================

	.section .rodata
	.align	2
	.global ball_bn_gfxTiles		@ 96 unsigned chars
	.hidden ball_bn_gfxTiles
ball_bn_gfxTiles:
	.word 0x00555500,0x058CC850,0x5C8CC8C5,0x58CCCC85,0x58CCCC85,0x5C8CC8C5,0x05C88C50,0x00555500
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x44444444
	.word 0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x66666666

	.section .rodata
	.align	2
	.global ball_bn_gfxPal		@ 32 unsigned chars
	.hidden ball_bn_gfxPal
ball_bn_gfxPal:
	.hword 0x6C1A,0x0427,0x082D,0x0834,0x18C6,0x00EE,0x107D,0x04F6
	.hword 0x11DE,0x5294,0x3ABF,0x6739,0x0FDF,0x3BFF,0x5BFF,0x7FFF

@}}BLOCK(ball_bn_gfx)
