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
zakinit 7, 3

;******************************
; Opcode Definitions
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

opcode Out2, 0, iiiii
	isource, iMute, iPad, iL, iR xin
	aL zar iL 
	aR zar iR 
	outs aL, aR
	;outs aL*iPad, aR*iPad 
	; iPad = 2 (+6dB) ili iPad = 1
endop 

opcode OscA, 0, KKKKKK
 ; Net knopok
	kfn, kFreq, kFine, kPitch, kModLev xin
	
	kPitch zkr 2 ; CHANGE 
	kMod zkr 1 ; CHANGE 
	; Proverit' amplitudu
	kfine = cent(kFine)
	aout oscilikt 0.5, kFreq*kfine+kPitch+kMod*kModLev, kfn   
	zaw aout, 2  ; CHANGE 
endop

;a2k
opcode A2K, 0, ii
	iIn, iOut xin
	aIn zar i1
	zkw k(aIn), iOut
endop

;k2a
opcode K2A, 0, ii
	iIn, iOut xin
	kIn zkr i1
	zaw a(kIn), iOut
endop

instr 1
	 Constant 0.000, 0.000, 1
	 LevAdd 
	 LevAdd 
	 LevAdd 
	 LevAdd 
	 Out2 0.000, 1.000, 0.000, 8, 4
	 OscA 
	 A2K 6, 2
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
