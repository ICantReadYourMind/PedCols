# PedCols
This mod adds varying pedestrian colours and textures to GTA Vice City and GTA III. Extract to your game folder to use. A pedcols.dat must be present in the data folder of the game (an example one is provided in the archive).
 
 
### To use this, the pedestrian models have to be adjusted. Alternatively, you can use converted pedestrian models from Vice City Stories. The materials for which you want colours to change must be like this (also check the mat section of the pedcols.dat file to add more as needed, or custom ones):



First material- R:0 G:0xFF B:0x3C

Second material- R:0xFF G:0 B:0xAF

Third material- R:0x3C G:0 B:0xFF

Fourth material- R:0xFF G:0 B:0x3C


Then, the pedcols.dat should be adjusted accordingly. There is an example pedcols.dat in the source folder and in the release archive.

## Adding variable pedestrian textures
To add varying pedestrian textures, you must set the texture name of the texture (adjust the model as well) you want to vary to set to something like varX_Y, where X is the texture/"thing" to be varied, and Y is the index of the texture that can be used to vary the texture. X and Y can be anything from 0 to infinity. The mod looks for these textures in the txd file of the pedestrian model.
Skin colors/textures can also be done, with the texture format being skinX_Y, where X is the skin colour index and Y is each part. For example, you can have skin0_0 for white hands, skin 0_1 for white face texture, skin1_0 for black hands and skin1_1 for black face texture. The models must be set to use a default skin0_Y for each part.

# Screenshots (thanks to the Liberty City 2001 team)
![image](https://github.com/user-attachments/assets/118ea4f8-c984-4e8e-bf2a-f2d2e7bebba5)
![image](https://github.com/user-attachments/assets/4638d611-9ece-4573-9959-890eec0be895)
![image](https://github.com/user-attachments/assets/d6954e9d-d34c-448c-82a6-d2a2f725760b)
![image](https://github.com/user-attachments/assets/c1130f04-71c7-44b5-acf7-b5ac5aba3de5)



# Thanks to:
<a href="https://github.com/DK22Pac/plugin-sdk">Plugin-SDK</a>

<a href="https://github.com/gennariarmando/classic-axis">Classic-Axis</a> for premake build script

<a href="https://github.com/CookiePLMonster/ModUtils">Silent's ModUtils</a> for the Module List.

re3 for reversed vehicle colour loading and changing code
