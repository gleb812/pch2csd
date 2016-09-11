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
zakinit 410, 3

;******************************
; Opcode Definitions
opcode OscD, 0, KKiiiii

	kPitch, kFine, iKBT, iSel, iMute, iPitchMod, iOut xin
	
	;kPitchM zkr iPitchMod 
	; Proverit' amplitudu
	kfine = cent(kFine)
	
	aout oscil 0.5, cpsmidinn(kPitch)*kfine 
	zaw aout, iOut 
endop

opcode Out2, 0, iiiii
	isource, iMute, iPad, iL, iR xin
	aL zar iL 
	aR zar iR 
	outs aL, aR
	;outs aL*iPad, aR*iPad 
	; iPad = 2 (+6dB) ili iPad = 1
endop 


opcode Mix81A, 0, i   ; MULTIMODE support a/k?
	iPad xin
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	a3 zar 1 ; CHANGE 
	a4 zar 1 ; CHANGE
	a5 zar 1 ; CHANGE 
	a6 zar 1 ; CHANGE 
	a7 zar 1 ; CHANGE 
	a8 zar 1 ; CHANGE
	aout = (a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8)*iPad
	zaw aout, 2 ; CHANGE 
endop

opcode Mix11A, 0, ki   ; MULTIMODE support a/k?
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	kLev, iSW xin
	aout = a1 + a2*kLev*iSW
	zaw aout, 2 ; CHANGE 
	; LIN vs DB?
endop

opcode Mix21B, 0, kkii   ; MULTIMODE support a/k?
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	a3 zar 1 ; CHANGE 
	kLev1, kLev2, iSW1, iSW2 xin ; iSW = -1/1
	aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2
	zaw aout, 2 ; CHANGE 
	; LIN vs DB?
endop

opcode FltLP, 0, iikk
; KBT !!!!!
	iKBT, iOrder, kMod, kCF xin ;i  - ON/OFF button?
	ain zar 2 ; CHANGE 
	kmod zkr 1 ; CHANGE 
	aout tonex ain, kCF+kmod*kMOd, iOrder
	zaw aout, 2  ; CHANGE 
endop

opcode Constant, 0, k   ; MULTIMODE support a/k?
	kVal xin
	; BIPOLAR UNIPOLAR ????
	zaw kVal, 2 ; CHANGE 
endop

opcode Mix21A, 0, kkii   ; MULTIMODE support a/k?
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	a3 zar 1 ; CHANGE 
	kLev1, kLev2, iSW1, iSW2 xin ; iSW = 0/1
	aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2
	zaw aout, 2 ; CHANGE 
	; LIN vs DB?
endop

opcode XFade, 0, kk
	kMod, kMix xin
	k1 zkr 1 ;CHANGE
	k2 zkr 2; CHANGE
	kmod zkr 3; CHANGE
	
	kMix += kMod ;+ limiter?
	if (iSW != 0) then
		kout = k1*kMix + k2*(1-kMix)
		goto run 
	endif
	
	kout = k1*log10(kMix) + k2*log10(1-kMix)
run:
	zkw kout, 2 ; CHANGE
endop


opcode LevMult, 0, 0   ; MULTIMODE support a/k?
; Need to check..
	k1 zkr 1 ; CHANGE 
	k2 zkr 1 ; CHANGE 
	zkw k1*k2, 2 ; CHANGE
	; normirovka ??
endop


opcode LevAdd, 0, k   ; MULTIMODE support a/k?
; Need to check..
	kVal xin
	k1 zkr 1 ; CHANGE 
	kVal xin
	zkw k1+kVal, 2 ; CHANGE
endop

opcode sw211, 0, k   ; MULTIMODE support a/k?
; Need to check..
	kSw xin ;kSw = 0 / 1
	a1 zar kSw ; CHANGE 
	zaw a1, 2 ; CHANGE
	; poka bez Ctrl 
endop

opcode ValSw1, 0, ik   ; MULTIMODE support a/k? == sw14 !!!
; Need to check..
	iVal, kSw xin 
	a1 zar 1 ; CHANGE  
	if (kSw != iVal) goto default
	zaw a1, 1 ; CHANGE
	goto halt
default:
	zaw a1, 2 ; CHANGE
halt:
endop

opcode LevAmp, 0, k   ; MULTIMODE support a/k?
; Need to check..
	kVal xin
	k1 zkr 1 ; CHANGE 
	kVal xin
	zkw k1*kVal, 2 ; CHANGE
	; LIN vs DB ???
endop

opcode ValSw2, 0, ik   ; MULTIMODE support a/k? == sw14 !!!
; Need to check..
	iVal, kSw xin 
	a1 zar 1 ; CHANGE  
	a2 zar 1 ; CHANGE 
	if (kSw != iVal) goto default
	zaw a1, 2 ; CHANGE
	goto halt
default:
	zaw a2, 2 ; CHANGE
halt:
endop

opcode LevConv, 0, ii
	iIn, iOut xin
	; O4en' Tupo!
	kin zkr 1 ; CHANGE
	if (iIn == 0) then
		if (iOut==0) then
			kout = kin
		elseif (iOut==1) then
			kout = -kin
		elseif (iOut==2) then
			kout = (kin+1)*.5
		elseif (iOut==3) then
			kout = (-kin+1)*.5
		elseif (iOut==4) then	
			kout = (kin+1)*.5
		elseif (iOut==5) then	
			kout = -(kin+1)*.5
		endif
	elseif (iIn == 1) then
		if (iOut==0) then
			kout = 2*kin-1
		elseif (iOut==1) then
			kout = -(2*kin-1)
		elseif (iOut==2) then
			kout = kin
		elseif (iOut==3) then
			kout = 1-kin
		elseif (iOut==4) then	
			kout = -kin
		elseif (iOut==5) then	
			kout = -1+kin
		endif
	elseif (iIn == 2) then
		if (iOut==0) then
			kout = 2*kin+1
		elseif (iOut==1) then
			kout = -(2*kin+1)
		elseif (iOut==2) then
			kout = kin+1
		elseif (iOut==3) then
			kout = -kin
		elseif (iOut==4) then	
			kout = kin
		elseif (iOut==5) then	
			kout = -1+kin
		endif
	
	zkw kout,1 ;CHANGE
endop

opcode Pan, 0, kk
	kMod, kMix xin
	k1 zkr 1 ;CHANGE
	kmod zkr 3; CHANGE
	
	kMix += kMod ;+ limiter?
	if (iSW != 0) then
		kL = k1*kMix
		kR = k2*(1-kMix)
		goto run 
	endif
	
	kL = k1*log10(kMix)
	kR = k2*log10(1-kMix)
run:
	zkw kL, 2 ; CHANGE
	zkw kR, 1 ; CHANGE
endop


opcode Mix41C, 0, kkkkiiiii   ; MULTIMODE support a/k?
kLev1, kLev2, kLev3, kLev4, iSW1, iSW2, iSW3, iSW4, iPad  xin 
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	a3 zar 1 ; CHANGE 
	a4 zar 1 ; CHANGE 
	a5 zar 1 ; CHANGE 
	aout = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2 + a4*kLev3*iSW3 + a5*kLev4*iSW4
	zaw aout*iPad, 2 ; CHANGE 
endop




opcode DelayA, 0, KKKK
	kTime, kFB, kFilter, kDW xin
	
	ain zar 2 ; CHANGE 
	
	abuf delayr 2.7
	atap deltapi kTime
	delayw ain+atap*kFB
	
	aout tone atap, kFilter ; check filter type etc
	zaw aout*kDW+ain*(1-kDW), 1 ; CHANGE 
endop

opcode Mix41S, 0, kkkkiiii   ; MULTIMODE support a/k?
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	a3 zar 1 ; CHANGE 
	a4 zar 1 ; CHANGE 
	a5 zar 1 ; CHANGE 
	a6 zar 1 ; CHANGE 
	a7 zar 1 ; CHANGE 
	a8 zar 1 ; CHANGE
	a9 zar 1 ; CHANGE 
	a10 zar 1 ; CHANGE
	kLev1, kLev2, kLev3, kLev4, iSW1, iSW2, iSW3, iSW4 xin 
	aoutL = a1 + a2*kLev1*iSW1 + a3*kLev2*iSW2 +a4*kLev3*iSW4 + a5*kLev4*iSW5
	aoutR = a6 + a7*kLev1*iSW1+ a8*kLev2*iSW2+ a9*kLev3*iSW4+ a10*kLev4*iSW5
	zaw aoutL, 2 ; CHANGE 
	zaw aoutR, 2 ; CHANGE 
	; LIN vs DB?
endop

opcode Mix11S, 0, ki   ; MULTIMODE support a/k?
	a1 zar 1 ; CHANGE 
	a2 zar 1 ; CHANGE 
	a3 zar 1 ; CHANGE 
	a4 zar 1 ; CHANGE 
	kLev, iSW xin
	aoutL = a1 + a3*kLev*iSW
	aoutR = a2 + a4*kLev*iSW
	zaw aoutL, 2 ; CHANGE 
	zaw aoutR, 2 ; CHANGE 
	; LIN vs DB?
endop

opcode Eq3Band, 0, KKKKKKK
; rbjeq ???
	kFreqL, kFreqH, kGainL, kGainH, kFreqM, kGainM, kLev xin 
	ain zar 2 ; CHANGE 
	ai1 pareq ain, kFreqL, kGainL, sqrt(.5), 2
	ai2 pareq ai1, kFreqH, kGainH, sqrt(.5), 1
	aout pareq ai2, kFreqM, kGainM, sqrt(.5), 0 ; BW???
	zaw aout*kLev, 2  ; CHANGE 
endop

opcode EqPeak, 0, KKKK
	kFreq, kGain, kBW, kLev xin ;i  - ON/OFF button?
	ain zar 2 ; CHANGE 
	aout pareq ain, kFreq, kGain, kBW
	zaw aout*kLev, 2  ; CHANGE 
endop

instr 1
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 Out2 0.000, 1.000, 0.000

	 Mix81A 
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 OscD 64.000, 0.000, 0.000, 0.000, 1.000
	 Mix11A 
	 Mix21B 
	 FltLP 
	 Constant 
	 Mix21A 
	 FltLP 
	 Mix21A 
	 XFade 
	 Mix21A 
	 Constant 
	 Mix21A 
	 Constant 
	 Mix21A 
	 Constant 
	 Mix21A 
	 XFade 
	 Mix21A 
	 XFade 
	 Mix21A 
	 Constant 
	 Mix11A 
	 Constant 
	 XFade 
	 XFade 
	 Constant 
	 Constant 
	 XFade 
	 XFade 
	 Constant 
	 XFade 

	 FltLP 
	 Constant 
	 Constant 
	 Constant 
	 Constant 
	 Constant 
	 Constant 
	 Constant 
	 FltLP 
	 Mix21B 
	 LevMult 

	 LevAdd 
	 LevAdd 
	 sw211 


	 Mix21B 
	 FltLP 
	 FltLP 
	 FltLP 
	 FltLP 
	 Mix21B 
	 LevMult 

	 LevAdd 

	 sw211 
	 sw211 
	 Mix21B 
	 Mix21B 
	 ValSw1 
	 Mix21B 
	 LevAmp 
	 ValSw2 

	 LevConv 
	 sw211 
	 sw211 
	 XFade 
	 Pan 
	 XFade 
	 Mix11A 
	 LevAdd 
	 Mix21B 
	 LevAdd 
	 LevAmp 

	 ValSw2 
	 ValSw1 
	 sw211 
	 LevAmp 
	 LevAdd 
	 LevAdd 
	 Mix11A 

	 LevAdd 
	 Mix41C 


	 LevAmp 

	 XFade 
	 XFade 
	 LevAdd 

	 LevAdd 

	 Mix41C 
	 LevAmp 
	 LevAmp 




endin
instr 2

	 Out2 0.000, 1.000, 0.000
	 DelayA 
	 Mix41S 
	 Mix11S 
	 Eq3Band 
	 DelayA 
	 LevAmp 
	 EqPeak 
endin

;******************************
</CsInstruments>
<CsScore>
i1 0 [60*60*24*7]
i2 0 [60*60*24*7]
</CsScore>
</CsoundSynthesizer>
