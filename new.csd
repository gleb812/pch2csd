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
zakinit 8, 3

;******************************
; Opcode Definitions
opcode Constant, 0, k   ; MULTIMODE support a/k?
	kVal xin
	; BIPOLAR UNIPOLAR ????
	zaw kVal, 2 ; CHANGE 
endop

opcode LevAdd, 0, k   ; MULTIMODE support a/k?
; Need to check..
	kVal xin
	k1 zkr 1 ; CHANGE 
	kVal xin
	zkw k1+kVal, 2 ; CHANGE
endop

opcode Out2, 0, iiiii
	isource, iMute, iPad, iL, iR xin
	aL zar iL 
	aR zar iR 
	outs aL, aR
	;outs aL*iPad, aR*iPad 
	; iPad = 2 (+6dB) ili iPad = 1
endop 

opcode OscD, 0, KKiiiii

	kPitch, kFine, iKBT, iSel, iMute, iPitchMod, iOut xin
	
	;kPitchM zkr iPitchMod 
	; Proverit' amplitudu
	kfine = cent(kFine)
	
	aout oscil 0.5, cpsmidinn(kPitch)*kfine 
	zaw aout, iOut 
endop

opcode LevAdd, 0, k   
; Need to check..
	kVal xin
	aVal = a(kVal)
	a1 zar 1 ; CHANGE 
	zaw a1+aVal, 2 ; CHANGE
endop

;k2a
opcode K2A, 0, ii
	iIn, iOut xin
	kIn zkr i1
	zaw a(kIn), iOut
endop

;a2k
opcode A2K, 0, ii
	iIn, iOut xin
	aIn zar i1
	zkw k(aIn), iOut
endop

instr 1
	 Constant 0.000, 0.000, 1
	 LevAdd 
	 LevAdd 
	 LevAdd 
	 LevAdd 
	 Out2 0.000, 1.000, 0.000, 8, 6
	 OscD 64.000, 64.000, 1.000, 0.000, 1.000, 0.000, 0, 7
	 A2K 2, 2
	 K2A 2, 8
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
