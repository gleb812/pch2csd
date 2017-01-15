<CsoundSynthesizer>
<CsOptions>
</CsOptions>
<CsInstruments>
sr = 44100 ;96000?
kr = 22050 ;24000?
nchnls = 2
0dbfs = 1

;******************************
; Initialize the ZAK space
zakinit 10, 6

;******************************
; Opcode Definitions


opcode DelayA, 0, KKKK
	kTime, kFB, kFilter, kDW xin
	
	ain zar 2 ; CHANGE 
	
	abuf delayr 2.7
	atap deltapi kTime
	delayw ain+atap*kFB
	
	aout tone atap, kFilter ; check filter type etc
	zaw aout*kDW+ain*(1-kDW), 1 ; CHANGE 
endop


opcode Out2, 0, iiiii
	isource, iMute, iPad, iL, iR xin
	aL zar iL 
	aR zar iR 
	outs aL, aR
	;outs aL*iPad, aR*iPad 
	; iPad = 2 (+6dB) ili iPad = 1
endop 

opcode LevAdd, 0, k   ; MULTIMODE support a/k?
; Need to check..
	kVal xin
	k1 zkr 1 ; CHANGE 
	kVal xin
	zkw k1+kVal, 2 ; CHANGE
endop

opcode Constant, 0, k   ; MULTIMODE support a/k?
	kVal xin
	; BIPOLAR UNIPOLAR ????
	zaw kVal, 2 ; CHANGE 
endop

opcode LevAdd, 0, k   
; Need to check..
	kVal xin
	aVal = a(kVal)
	a1 zar 1 ; CHANGE 
	zaw a1+aVal, 2 ; CHANGE
endop

instr 1


	 DelayA 2.680, 0.500, 0.500, 0.500, 1.000, 0.000, 0.0000.000, , 2, 0


	 Out2 2.680, , 4, 6
	 Out2 2.680, , 0, 0
	 LevAdd 0.000, 1.000, 2, 0
	 LevAdd 
	 Constant 0.000, 0.000, 0
endin
instr 2
endin

;******************************
</CsInstruments>
<CsScore>
i1 0 [60*60*24*7]
i2 0 [60*60*24*7]
</CsScore>
</CsoundSynthesizer>
