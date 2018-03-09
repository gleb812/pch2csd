# pch2csd - The Clavia Nord Modular G2 Patch Converter Project

![](https://travis-ci.org/gleb812/pch2csd.svg?branch=master)

The goal of this project is to (re)implement the Clavia Nord Modular G2 sound
engine in Csound, a well-known sound and music computing system. Important wiki
pages:

- The full list of supported Nord modules: [Module implementation
  status](https://github.com/gleb812/pch2csd/wiki/Module-implementation-status).
- A guideline for porting Nord modules: [Making a new
  module](https://github.com/gleb812/pch2csd/wiki/Making-a-new-module).


## Installation

Assuming you have Python 3.5 and
[pip](https://pip.pypa.io/en/stable/installing/) installed:

```
sudo pip3 install pch2csd
```

Before the project has got more stable, we automatically build and upload every
commit made to the repository, so you may want want to update the tool once in a
while.  With newer versions you'll probably get new features and modules
available, as well as bugs fixed. To update the tool just run the following
command:

```
sudo pip3 install -U pch2csd
```

## Usage 

Command line options:

```
$ pch2csd -h
usage: pch2csd [-h] [-p | -c | -v | -e] [arg]

convert Clavia Nord Modular G2 patches to the Csound code

positional arguments:
  arg              a pch2 file path or an UDO numerical ID

optional arguments:
  -h, --help       show this help message and exit
  -p, --print      parse the patch file and print its content
  -c, --check-udo  validate the UDO template file (overrides '-p')
  -v, --version    show program's version number and exit
  -e               show the elephant and exit
```

To generate a new Csound file just pass the `.pch2` file path as the argument.
The new file will be generated in the same directory with the patch file. An
example:

```
pch2csd test_poly_mix2.pch2
```

Option `-p` can be used to print modules and cables present in of the patch.
Module `ID` is a unique number of a module in the patch, module `Type` is a
numerical representation of the module's type (also used to look up for
templates in the `resources/modules`). In the square brackets are the raw MIDI
module parameters. `Area` is the part the module or cable is located; it can
be either `VOICE` or `FX`. In the cable table notation `In2(id=1, out=0) ->
Mix41B(id=2, in=0)` means, that the cable connects the 0th output of the module
`In2` (`ID` 1) with the input 0 of the module `Mix41b` (`ID` 2). Note that the
modules can connect either outputs (`out-in`) to inputs or intputs to inputs
(`in-in`).

```
$ pch2csd -p test_3osc.pch2
Patch file: test_3osc.pch2

Modules
Name        ID    Type  Parameters               Area
--------  ----  ------  -----------------------  ------
OscA         1      97  [64, 51, 1, 0, 2, 1, 0]  VOICE
OscA         2      97  [64, 78, 1, 0, 2, 1, 0]  VOICE
OscA         3      97  [64, 64, 1, 0, 2, 1, 0]  VOICE
Mix41B       4      19  [100, 112, 91, 117, 1]   VOICE
EnvD         6      55  [54, 0]                  VOICE
FltLP        5      87  [75, 34, 0, 1]           VOICE
Out2         7       4  [2, 1, 0]                VOICE
Keyboard     8       1  []                       VOICE
FxIn         1     127  [0, 1, 1]                FX
Clip         2      61  [0, 53, 1, 1]            FX
Out2         3       4  [0, 1, 0]                FX

Cables
From                       To                  Color    Type    Area
---------------------  --  ------------------  -------  ------  ------
OscA(id=1, out=0)      ->  Mix41B(id=4, in=1)  red      out-in  VOICE
OscA(id=2, out=0)      ->  Mix41B(id=4, in=2)  red      out-in  VOICE
OscA(id=3, out=0)      ->  Mix41B(id=4, in=3)  red      out-in  VOICE
Mix41B(id=4, out=0)    ->  EnvD(id=6, in=2)    red      out-in  VOICE
EnvD(id=6, out=1)      ->  FltLP(id=5, in=0)   red      out-in  VOICE
EnvD(id=6, out=0)      ->  FltLP(id=5, in=1)   blue     out-in  VOICE
FltLP(id=5, out=0)     ->  Out2(id=7, in=0)    red      out-in  VOICE
Out2(id=7, in=0)       ->  Out2(id=7, in=1)    red      in-in   VOICE
Keyboard(id=8, out=1)  ->  EnvD(id=6, in=0)    yellow   out-in  VOICE
FxIn(id=1, out=0)      ->  Clip(id=2, in=0)    red      out-in  FX
Clip(id=2, out=0)      ->  Out2(id=3, in=0)    red      out-in  FX
Out2(id=3, in=0)       ->  Out2(id=3, in=1)    red      in-in   FX
```

Option `-c` allows you to check whether the UDO for the specified module has
been implemented or has errors in the implementation:

```
$ pch2csd -c 43
Checking UDO for the type ID 43
Found a module of this type: Constant
The UDO template seems to be correct

$ pch2csd -c 194
Checking UDO for the type ID 194
Found a module of this type: Mix21A
ERROR Unknown table 'CLAEXP'

$ pch2csd -c 100
Checking UDO for the type ID 100
Found a module of this type: Sw21
ERROR UdoTemplate(Sw21, 100.txt): no opcode 'args' annotations were found in the template
```

## History

We started our project during Summer of 2015. The Project's main objective is to
simulate legendary Clavia Nord Modular G2 synthesizer using Csound language. The
Project was first presented at the The Third International Csound Conference
(2-4 October, St. Petersburg, Russia).

## How to contribute

Please, read the
[CONTRIBUTION.md](https://github.com/gleb812/pch2csd/blob/master/CONTRIBUTION.md)
file.

## Why do you need it?

If you are a Nord Modular fan, this software allows you to have your favourite
device ressurected for eternal life in the halls of Csound language. You also
can improve the precision of models and use the whole world of Csound
possibilities together with Clavia.

If you are a Csound person, this is a new branch of our journey. This is great
to have some hardware digital synths running on Csound. Once the conversion
project is done, you are able to use hundreds of interesting Clavia's G2 patches
straigth on Csound.

If you discover the world of modular synthesis and algorithmic composition, the
system provides a good way to describe the graphic patches of Clavia.

If you are a developer of alternative Clavia Nord Modular G2 Editor, you could
merge your graphical editor software with the system to produce the sound.
