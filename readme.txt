COP4610 Project 3 Readme
Group 28
Cameron Heffelfinger
Jacob Hobson
Vanessa Myron

Division of Labor:
exit: Jacob
info: Jacob
size: Jacob
ls: Vanessa
cd: Vanessa
create: 
mkdir: 
mv: 
open: Vanessa
close: Vanessa
read: Not Implemented
write: Not Implemented
rm: Cameron
cp: Cameron
readme: Cameron

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
rm deletes the file selected and all files in the directory that come after it (if you ls all files to the right are deleted as well)
couldnt get a working implementation of cp, so it just prints out the command and arguments when used
Couldnt get read or write completed
Couldnt get open working
