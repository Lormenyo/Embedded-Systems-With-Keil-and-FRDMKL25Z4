	AREA checkCarDetails CODE, READONLY
	EXPORT checkFuelTank
checkFuelTank  PROC
				LDR R1,=carData
				MOVS R2, #100
				MOVS R3, #4
                LDRH R0, [R1, R3]

                CMP R0, R4
				BLE fuelFinished
				MOVS R0, #0
				B Done
FuelFinished    MOVS R0, #1
				B Done

Done        	NOP
				BX LR
				ENDP

	AREA someData, DATA, READWRITE
	ALIGN
carData     DCW     18,12,2020, 1414, 67 ;day, month, year, car number plate, fuel level

END