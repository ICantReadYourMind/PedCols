# PedVariations
This mod adds varying pedestrian colours and textures to GTA Vice City and GTA III. Extract to your game folder to use. A pedcols.dat must be present in the data folder of the game (an example one is provided in the archive).
 
 
### To use this, the pedestrian models have to be adjusted. Alternatively, you can use converted pedestrian models from Vice City Stories. The materials for which you want colours to change must be like this (also check the mat section of the pedcols.dat file):



First material- R:0 G:0xFF B:0x3C

Second material- R:0xFF G:0 B:0xAF

Third material- R:0x3C G:0 B:0xFF

Fourth material- R:0xFF G:0 B:0x3C


Then, the pedcols.dat should be adjusted accordingly. There is an example pedcols.dat in the source folder and in the release archive.

## Adding variable pedestrian textures
To add varying pedestrian textures, you must set the texture name of the texture (adjust the model as well) you want to vary to set to something like varX_Y, where X is the texture/"thing" to be varied, and Y is the index of the texture that can be used to vary the texture. X and Y can be anything from 0 to infinity. The mod looks for these textures in the txd file of the pedestrian model.

![image](https://github.com/user-attachments/assets/9b1c3808-858c-47a1-bdad-8053efeff9b1)

# Thanks to:
<a href="https://github.com/DK22Pac/plugin-sdk">Plugin-SDK</a>

<a href="https://github.com/gennariarmando/classic-axis">Classic-Axis</a> for premake build script

<a href="https://github.com/CookiePLMonster/ModUtils">Silent's ModUtils</a> for the Module List.

re3 for reversed vehicle colour loading and changing code
