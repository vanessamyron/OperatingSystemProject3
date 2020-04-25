COP4610 Project 3 Readme
Group 28
Cameron Heffelfinger
Jacob Hobson
Vanessa Myron

Division of Labor:
Implimentation for Instruction intake: Jacob
exit: Jacob
info: Jacob
size: Jacob
ls: Vanessa
cd: Vanessa
create:Jacob  
mkdir: Jacob
mv: Jacob
open: Vanessa
close: Vanessa
read: Not Implemented
write: Not Implemented
rm: Cameron
cp: Cameron
readme: Cameron
makefile: Vanessa Cameron

Extra Credit:
rmdir: Not yet attempted
cp -r: Not yet attempted

Contents:
proj3.c: Entire program
makefile: The project makefile
readme.txt: The project readme, containing all relevant information

Compilation:
This project was tested using Linprog, to compile have the proj3.c, makefile, and the .img file all in the same directory. Then type make, then run the project3 executable

Known Bugs/Unfinished Portions of the project:
-couldnt get a working implementation of cp, so it just prints out the command and arguments when used
-Couldnt get read or write completed
-Couldnt get open working
-Adding DirEntries to a directory will not allocated a new cluster, so if one is added in the 16th position a directory is full and will not be able to add anymore without creating corrupt data.
