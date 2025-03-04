# patches/

This directory contains patches with code placed out of the main source tree for various reasons.


## Content

 - `swap-buttons-fully.diff`: swap two buttons on supported hardware **completely** (see [here](https://github.com/Ralim/IronOS/pull/2059) for more details).


## Instruction

 - pick any patch and apply it like this from the root source tree: `$ patch -p1 < ./patches/name-of-the-patch.diff`;
 - make a build as usual.

