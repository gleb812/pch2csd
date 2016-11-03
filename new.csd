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
opcode DelayDual, 0, iiikk
	iTime1, iTime2, imaxdel, kMod1, kMod2 xin
	ain1 zar 2 ; CHANGE 
	adel1 zar 1 ; CHANGE 
	adel2 zar 1 ; CHANGE
	aout1 vdelay ain, iTime1+adel1*kMod1, imaxdel
	aout2 vdelay ain, iTime2+adel2*kMod2, imaxdel
	zaw aout1, 2  ; CHANGE 
	zaw aout2, 2  ; CHANGE 
endop

instr 1
	 DelayDual 3.090, 0.000, 1.550, 0.0940.000, , 0, 0, 0, 0, 0
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
