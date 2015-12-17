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

opcode Noise, 0, Kii ;
	kColor, iMute, iOut  xin
	aout rand 0.5, 0.1 ; seed value ?
	;aout tone a1, kColor
	zaw aout, iOut
endop

opcode Mix41A, 0, iiiii   ; MULTIMODE support a/k?
	i1, i2, i3, i4, iOut xin
	a1 zar i1 
	a2 zar i2  
	a3 zar i3 
	a4 zar i4 
	aout = a1 + a2 + a3 + a4
	zaw aout, iOut
endop

instr 1
	 Out2 0.000, 1.000, 0.000, 6, 2
	 OscD 64.000, 0.000, 1.000, 0.000, 1.000, 0, 4
	 OscD 64.000, 0.000, 1.000, 0.000, 1.000, 0, 5
	 Noise 0.000, 1.000, 2
	 Mix41A 4, 5, 0, 2, 6
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
