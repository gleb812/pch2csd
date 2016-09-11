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
opcode FltLP, 0, iikk
; KBT !!!!!
	iKBT, iOrder, kMod, kCF xin ;i  - ON/OFF button?
	ain zar 2 ; CHANGE 
	kmod zkr 1 ; CHANGE 
	aout tonex ain, kCF+kmod*kMOd, iOrder
	zaw aout, 2  ; CHANGE 
endop

opcode FltHP, 0, iikk
; KBT !!!!!
	iKBT, iOrder, kMod, kCF xin ;i  - ON/OFF button?
	ain zar 2 ; CHANGE 
	kmod zkr 1 ; CHANGE 
	aout atonex ain, kCF+kmod*kMOd, iOrder
	zaw aout, 2  ; CHANGE 
endop

instr 1
	 FltLP 
	 FltHP 
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
