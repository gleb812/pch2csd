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
zakinit 3, 3

;******************************
; Opcode Definitions
opcode Mix21A, 0, kkii   ; MULTIMODE support a/k?
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	a3 zar 1 ; CHANGE 
	kLev1, kLev2, iSW1, iSW2 xin ; iSW = 0/1
	aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2
	zaw aout, 2 ; CHANGE 
	; LIN vs DB?
endop

instr 1
	 Mix21A 0.305, 1.000, 0.734, 1.000, 1.000, 0, 0, 0, 0
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
