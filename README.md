# pch2csd

The Clavia Nord Modular G2 Patch Converter Project

## Project status

`TODO`

## Installation

Assuming you have Python 3.5 and
[pip](https://pip.pypa.io/en/stable/installing/) installed:

```
sudo pip3 install pch2csd
```

You may want to update the tool to a newer version after you have installed it, for example, when there are important bug fixes or new features available. Just run the following command to update the tool:

```
sudo pip3 install -U pch2csd
```

## Usage 

Command line options:

```
$ pch2csd -h
usage: pch2csd [-h] [-p] [--version] file

convert Clavia Nord Modular G2 patches to the Csound code

positional arguments:
  file         a patch or an UDO template file

optional arguments:
  -h, --help   show this help message and exit
  -p, --parse  parse the patch file and print its content
  --version    show program's version number and exit
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
module parameters. `Area` is the place the module or cable is located in; it can
be either `VOICE` or `FX`. In the cable table notation `In2(id=1, out=0) ->
Mix41B(id=2, in=0)` means, that the cable connects the 0th output of the module
`In2` (`ID` 1) with the input 0 of the module `Mix41b` (`ID` 2). Note that the
modules can connect either outputs (`out-in`) to inputs or intputs to inputs
(`in-in`).

```
$ pch2csd -p test_in2in.pch2
Patch file: test_in2in.pch2

Modules
Name      ID    Type  Parameters               Area
------  ----  ------  -----------------------  ------
In2        1     170  [0, 1, 1]                VOICE
Mix41B     2      19  [100, 100, 100, 100, 0]  VOICE
Mix41B     3      19  [100, 100, 100, 100, 0]  VOICE

Cables
From                    To                  Color    Type    Area
------------------  --  ------------------  -------  ------  ------
In2(id=1, out=0)    ->  Mix41B(id=2, in=0)  red      out-in  VOICE
Mix41B(id=2, in=0)  ->  Mix41B(id=2, in=1)  red      in-in   VOICE
Mix41B(id=2, in=1)  ->  Mix41B(id=2, in=2)  red      in-in   VOICE
Mix41B(id=2, in=2)  ->  Mix41B(id=3, in=0)  red      in-in   VOICE
Mix41B(id=3, in=0)  ->  Mix41B(id=3, in=1)  red      in-in   VOICE
Mix41B(id=3, in=1)  ->  Mix41B(id=3, in=2)  red      in-in   VOICE
```

## History

We started our project during Summer of 2015. The Project's main objective is to
simulate legendary Clavia Nord Modular G2 synthesizer using Csound language. The
Project was first presented at the The Third International Csound Conference
(2-4 October, St. Petersburg, Russia).

## Why do you need it?

If you are a Nord Modular fan, this software allows you to have your favourite
device ressurected for internal life in the halls of Csound language. You also
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
