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

The patch format succefully decoded thanks to Michael Dewberry http://www.dewb.org/g2/pch2format.html



More detailed description will appear soon..
