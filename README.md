# pch2csd
The Clavia Nord Modular G2 Patch Convertor Project

by Gleb G. Rogozinsky and Michael Chesnokov 2015
gleb.rogozinsky@gmail.com

We started our project during Summer of 2015. The Project's main objective is to simulate legendary Clavia Nord Modular G2 synthesizer using CSound language. The Project was first presented at the The Third International CSound Conference (2-4 October, St. Petersburg, Russia).

The main idea behind implementation is to model all the modules of Clavia with CSound UDOs. See /Modules for modules description based on CSound language. The module numbers correspond to original Clavia module IDs.

The output of our converter is csd file with all used UDO's and two separate instruments (VA and FX).

If some module exists in the patch, we have to insert the corresponding Csound UDO to the output csd file.
Please notice, that all UDOs have only inputs. They are patched to each other not directly, but through the zak-space.
It allows us to maintain the orbitrary order of modules, where CSound typicaly reads the code from top to bottom.

Another remarkable issue is cabling, which is completely different to CSound. 

Check /Tables to see mapping for module parameter values. Being MIDI compatible, Clavia stores all parameters as an 7 bit integers. So it is not possible to know the real value of Hz or dB. For this purpose we manually created the mapping tables looking at values at software editor.

/IO directory contains module input/output tables. The order of inputs/outputs is strictly fixed. 

Check the Status List to find which modules are implemented and what are the next to-dos by group. 
The top priorities are oscillators, filters and envelope generators. Any help is welcome.

*************************************************************
UDO EXAMPLES (to be moved to separate file later)

This is an example of UDO model for Out2 module. Check CSound manual for details on CSound programming


opcode Out2, 0, iiiii	; a new UDO definition. 0 - no outputs, iiiii - five inputs/parameters of i-type

	isource, iMute, iPad, iL, iR xin    ; getting parameter values from the code.
	
	                                    ; isource - source selector, iMute - mute toggle
	                                    
	                                    ; iPad - padding button, iL,iR - numbers of patch cables for the module
	                                    
	aL zar iL                           ; Patching iL cable to audio part of zak space
	
	aR zar iR                           ; Patching iR cable to audio part of zak space
	
	outs aL, aR                         ; output sound
	
endop                                 ; end of UDO definition 


***************************************************************
The patch format succefully decoded thanks to Michael Dewberry http://www.dewb.org/g2/pch2format.html

HOW TO START DEVELOPING:

1. Read Clavia Nord G2 manual to get familiar with Clavia's modules

2. Grab your Clavia Nord Modular G2 or Clavia Nord Modular G2 Engine and wipe off the dust from it
(or install a free Clavia Nord Modular G2 Demo (should fully work on XP and partly on Win7,
there is also OS X version available)

3. Open a patch you would like to convert, or simple make a new one

4. Use pch2csd utility

5. Run your csd with CSound


If some modules were not converted into CSound UDO's, it means you need to add them to /Modules directory.
You also need to create input-output routing table for those modules and place it in /IO
In case there is some parameter error, you need to add a new mapping table and place it in /Maps

Refer to Status List.pdf to discovered which modules are already implemented.


More detailed description will appear soon..
