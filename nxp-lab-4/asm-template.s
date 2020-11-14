//mcuxpresso/gnu/armclang

		.section myFxn2, "ax"
		.global Fxn2
		.type Fxn2, "function"
Fxn2:
	// only returns a value 6 placed in r0
MOVS R0, #0x6
BX LR
.end



//keil

			AREA myFxn2, CODE, READONLY
			EXPORT Fxn2
Fxn2		PROC				
; only returns a value 6 placed in r0
			MOVS R0, #0x6
			BX LR
			ENDP
			END
