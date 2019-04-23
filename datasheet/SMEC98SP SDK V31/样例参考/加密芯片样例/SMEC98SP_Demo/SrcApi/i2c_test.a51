$include	(.\dw8051.inc)
$include	(.\SMEC98SP_I2C.inc)

	EXTRN	DATA (I2C_count_bytes)
	EXTRN	DATA (I2C_send_bytes)
	EXTRN	DATA (I2C_State)
	EXTRN	XDATA (I2C_Buf)
	
?PR?I2C_TEST SEGMENT CODE

	PUBLIC	i2c_xfer
	RSEG 	?PR?I2C_TEST

i2c_xfer:
   	ORL   	I2CCON2,#0x80      		; software reset
   	ANL   	I2CCON2,#0x00
  	MOV  	I2CCON,#0x44            ; enable i2c, set assert_ack enable
 ; 	MOV  	I2CCON2,#0x08           ; NO DID detection + clock stretching disabled
  	MOV  	I2CCON2,#0x00           ; NO DID detection + clock stretching enabled	
	MOV		I2C_count_bytes,#00H	; Clear I2C count
	MOV		I2C_State,#00H			; Clear I2C status

i2c_xfer_loop:
	JNB		I2CCON.3, $
	MOV		A, I2CSTAT
	XRL		A, #0xA8				; DID Addres received + Read  + Ack
	JZ		readI2C_send_data
	MOV		A, I2CSTAT
	CLR   	I2CCON.3				; clear INT flag
	CJNE	A, #0x60, i2c_xfer		; DID Addres received + Write  + Ack
	
WriteI2C_recv_data:
	MOV		DPL0, #low(I2C_Buf)
	MOV		DPH0, #high(I2C_Buf)
	MOV		I2C_State,#01H			; I2C status=Write
recv_lp:
	JNB		I2CCON.3, $
	MOV		A, I2CBUF	
	MOVX	@DPTR, A
	MOV		A, I2C_count_bytes			;overflow?
	CJNE 	A,#0xFF, recv_inc
	JMP		recv_continue	
recv_inc:
	INC		DPTR
	INC		I2C_count_bytes
recv_continue:
	MOV		A, I2CSTAT					; read before "CLR I2CCON.3"
	MOV		R2, A
	CLR		I2CCON.3					; clear INT flag	 					
	XRL		A, #0x80					; data + ACK
	JZ		recv_lp
	CJNE	R2, #0xA0, i2c_xfer_loop	; STOP ?
recv_stop:	
	ANL   	I2CCON, #0xBF				; disable I2C
	RET

readI2C_send_data:
	MOV		DPL0, #low(I2C_Buf)
	MOV		DPH0, #high(I2C_Buf)
	MOV		I2C_State,#02H			; I2C status=Read
send_lp:
	MOVX	A, @DPTR
	MOV		I2CBUF,A
	MOV  	I2CCON,#0x44            ; enable i2c, set assert_ack enable ; clear IT start transmission

	MOV		A, I2C_send_bytes			;overflow?
	JZ		send_continue
	DJNZ 	I2C_send_bytes, send_inc
	JMP		send_continue
send_inc:
	INC		DPTR
send_continue:
	JNB		I2CCON.3, $
	MOV		A, I2CSTAT
	XRL		A, #0xB8					; master ACK
	JZ		send_lp
	ANL   	I2CCON, #0xBF				; disable I2C
	RET
	

	END
