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

instr 1
	 Out2 0.000, 0.000, 0.000, 0, 0
	 OscD 
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
