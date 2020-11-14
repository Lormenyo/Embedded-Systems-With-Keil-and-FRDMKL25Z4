	AREA myFxn, CODE, READONLY
			EXPORT Fxn

Fxn			PROC
			CMP R0, R1
			BLE LessThan1
LessThan1 	MOVS R0, R1
			CMP R0, R2
			BLE LessThan2          
LessThan2   MOVS R0, R2
			BX LR
			ENDP

		END