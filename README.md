# pch2csd

The Clavia Nord Modular G2 Patch Converter Project

## Project status

`TODO`

## Installation

The converter can be installed as follows (Python 3.6 is required):

```
git clone https://github.com/gleb812/pch2csd.git
cd pch2csd
sudo pip3 install setuptools
sudo python3 setup.py install
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

Option `-p` can be used to print the modules and the cables of a patch file.
`id` is a unique number of a module in the patch, `type_id` is a numerical
representation of the module's type (also used to look up for templates in the
`resources/modules`). In the square brackets are the raw MIDI mo~ule parameters
are in the square brackets.

```
$ pch2csd -p test_in2in.pch2
Patch file: test_in2in.pch2

Modules:
  (VOICE) In2(id=1)	[0, 1, 1] (type_id=170)
  (VOICE) Mix41B(id=2)	[100, 100, 100, 100, 0] (type_id=19)
  (VOICE) Mix41B(id=3)	[100, 100, 100, 100, 0] (type_id=19)

Cables:
  (VOICE) (out-in)	In2(id=1, out=0) -> Mix41B(id=2, in=0)
  (VOICE) (in-in)	Mix41B(id=2, in=0) -> Mix41B(id=2, in=1)
  (VOICE) (in-in)	Mix41B(id=2, in=1) -> Mix41B(id=2, in=2)
  (VOICE) (in-in)	Mix41B(id=2, in=2) -> Mix41B(id=3, in=0)
  (VOICE) (in-in)	Mix41B(id=3, in=0) -> Mix41B(id=3, in=1)
  (VOICE) (in-in)	Mix41B(id=3, in=1) -> Mix41B(id=3, in=2)
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
