;Question 1
	AREA question1, CODE, READONLY
	EXPORT Fxn1
		
Fxn1    PROC		
		MOVS R0, #1 ;a
		MOVS R1, #2 ;b
		MOVS R2, #3 ;c
		MOVS R3, #4 ;d


			CMP R0, R1
			BEQ IfEqualTo	
			BLE IfLessThan
			SUBS R3, R2, R0
IfEqualTo  	MOVS R3, #10
IfLessThan 	ADD R3, R3, R2


			MOVS R4, #100
			
Loop                  
			ADD R3, R3, R4
			LSLS R5, R2, #2
			SUBS R2, R5, R4
			SUBS R4, R4, #1
			CMP R4, R0
			BGE Loop
			BLE Done
Done
			NOP
		
		
			
Loop2                  
			SUBS R2, R2, R1
			SUBS R3, R3, #1
			CMP R3, R0
			BGE Loop2
			BLE Done2
Done2		NOP
		
			BX LR
			ENDP
		
;Question 2
	AREA myFxn, CODE, READONLY
	EXPORT Fxn

Fxn			PROC
				CMP R0, R1
				BLE LessThan1
				B   ContinueCMP
LessThan1 		
				MOVS R0, R1
				B ContinueCMP

ContinueCMP		CMP R0, R2
				BLE LessThan2
				B DoneCMP
LessThan2   	
				MOVS R0, R2
				B DoneCMP
DoneCMP
				NOP
				BX LR
				ENDP
					
;Question 3					
	AREA special, CODE, READONLY
	EXPORT specialFxn
			; r0 = input variable n
			; r0 = output variable
specialFxn     PROC
				MOVS r1,#0

a_loop
				ADD r1,r1,r0
				SUBS r0, r0, #1
				BNE a_loop
				MOV r0, r1
				MOV PC, LR
				ENDP
					


;Question 4
	AREA myFxn4, CODE, READONLY
	EXPORT Fxn4
		
Fxn4        PROC
				MOVS R0, #0
	            MOVS R1, #1
				MOVS R2, #0
				MOVS R3, #1
				MOV R7, SP

storeIntegers      
				STR R3, [R7, R2]
				ADDS R3, R3, #1
                ADDS R2, R2, #4
				CMP R2, #32
                BEQ storeIntegerDone
                BNE storeIntegers  
storeIntegerDone  MOVS R2, #0
storeSquares      
                  LDR R3, [R7, R2]
                  MULS R3, R3, R3
                  STR R3, [R7, R2]
                  ADDS R2, R2, #4
				  CMP R2, #32
                  BEQ storeSquaresDone
                  BNE storeSquares
storeSquaresDone  MOVS R2, #0
addIntegers      
                 LDR R3, [R7, R2]
                 ADDS R3, R3, R1
                 STR R3, [R7, R2]
                 ADDS R2, R2, #4
                 ADDS R1, R1, #1
				 CMP R2, #32
                 BEQ addIntegersDone
                 BNE addIntegers 
addIntegersDone  MOVS R2, #0
sumIntegers      
                 LDR R3, [R7, R2]
                 ADDS R0, R0, R3
                 ADDS R2, R2, #4
				 CMP R2, #32
                 BEQ sumDone
                 BNE sumIntegers      
sumDone 
				 NOP
				 BX LR
				 ENDP

					
;Question 5
	AREA myFxn5, CODE, READONLY
	EXPORT Fxn5
Fxn5         PROC
				LDR R1,=myData
				MOVS R3, #0
                LDRH R0, [R1, R3]
Fxn5Loop
				LDRH R4, [R1, R3]
				ADDS R3, R3, #2
                CMP R0, R4
				BLE Less1
Less1 	        MOVS R0, R4
				CMP R3, #16
				BEQ Fxn5Done
				BNE Fxn5Loop
Fxn5Done        NOP
				BX LR
				ENDP

	AREA someData, DATA, READWRITE
	ALIGN
myData     DCW     15,4,22,3,55,100, 255, 1829, 17, 10

END


			
;Question 6
	AREA myFxn2, CODE, READONLY
	EXPORT Fxn2
		
Fxn2        PROC
				MOVS R0, #3
				;calling specialFxn
				PUSH {LR}
				BL specialFxn
				pop {PC}
				ENDP

		END

