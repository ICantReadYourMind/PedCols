# PedColsVC
This mod adds varying pedestrian colours to GTA Vice City.
 
 
### To use this, the pedestrian models have to be adjusted. The materials for which you want colours to change must be like this:



First material- R:0 G:0xFF B:0x3C

Second material- R:0xFF G:0 B:0xAF

Third material- R:0x3C G:0 B:0xFF

Fourth material- R:0xFF G:0 B:0x3C


Then, the pedcols.dat should be adjusted accordingly. The format for the pedcols.dat is similar to that of the carcols.dat, where the col section lists all the colour variations used, and the ped section has the first element of the first line be the model name, the only difference is that each ped now has 4 adjustable colours instead of 2, and that the first index of the colours will always be white. Moreover, adding a minus sign (-) before a non-zero ped colour will let that material have a 50% chance of getting hidden, which is useful for hiding ped props. An example pedcols.dat is provided in the archive as a reference.

![image](https://github.com/user-attachments/assets/9b1c3808-858c-47a1-bdad-8053efeff9b1)

# Thanks to:
<a href="https://github.com/DK22Pac/plugin-sdk">Plugin-SDK</a>

re3
