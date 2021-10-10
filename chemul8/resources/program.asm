; Draw a number at the left top corner
;

start:
	LD V0, 0	; x position
	LD V1, 0	; y position
	LD V2, 0	; start by displaying the number 0
loop:
	LD F, V2	; set the I register to point at the sprite for the number of register 3
	DRW V0, V1, 5 	; Display 5-byte sprite starting at memory location I at (V0, V1), set VF = collision
	LD V2, K 	; Wait for a key press, store the value of the key in Vx. Stops execution
	CLS 		; clear screen
	JP loop		;

