# NCD

This repository contains two implementations of NCD, the Bash implementation is a 
simple script created to compare only two files. You can run the command as follows:
```
sh NCD.sh file1 file2
```
The output of the script is the NCD distance between the two files.

The second is a C implementation which aims to create a pairwise matrix from a folder
calculating the NCD distance among all the .bin/.exe elements of the folder. This
implementation uses threads. The output is triangular matrix whit two columns: the 
first column corresponds to the NCD values and the second to the order. This 
implementation is under development.
 
