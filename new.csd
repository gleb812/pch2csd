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
opcode DelayB, 0, KKKKKKKK
	kTime, kFB, kLP, kHP,  kDW, kFBmod, kDWmod xin
	
	ain zar 2 ; CHANGE 
	kDW += kDWmod
	kFB += kFBmod
	
	abuf delayr 2.7
	atap deltapi kTime
	delayw ain+atap*kFB
	
	aout tone atap, kLP ; check filter type etc
	aout atone aout, kHP ; check filter type etc
	zaw aout*kDW+ain*(1-kDW), 1 ; CHANGE 
endop

instr 1
	 DelayB 2.680, 0.500, 1.000, 0.500, 0.000, 0.000, 0.000, 0.008, 0.000, 0.0000.000, 
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
