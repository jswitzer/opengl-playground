lua: 
Small embeddable language For writing game scripts.
Downloaded from https://www.lua.org/download.html

glew:
Gl extension wrangler - for wrapping opengl functions.
downloaded from http://glew.sourceforge.net/
github: https://github.com/nigels-com/glew

glfw:
Cross platform toolkit to handle input, windowing, output, etc.
https://github.com/glfw/glfw
downloaded from https://www.glfw.org/download.html

TCC:
For compiling c code.
downloaded from http://download.savannah.gnu.org/releases/tinycc/
PRE-CHECKIN MODIFICATIONS:
  tar -xjf tcc-0.9.27.tar.bz2 
  tar -czf tcc-0.9.27.tar.gz tcc-0.9.27
  rm -rf tcc-0.9.27 tcc-0.9.27.tar.bz2 

Readline: 
Adds history and line editing for the command line (on linux/osx). 
downloaded from https://ftp.gnu.org/gnu/readline/
PRE-CHECKIN MODIFICATIONS:
  tar -xzf readline-8.1.tar.gz 
  rm -rf readline-8.1/doc
  rm -f readline-8.1.tar.gz 
  tar -czf readline-8.1.tar.gz readline-8.1
  rm -rf readline-8.1

Wineditline:
A readline alternative for windows.
downloaded from https://sourceforge.net/projects/mingweditline/
PRE-CHECKIN MODIFICATIONS:
  tar -xjf wineditline-2.206.tar.bz2 
  rm -rf wineditline-2.206/bin* wineditline-2.206/lib*
  rm wineditline-2.206.tar.bz2
  tar -czf wineditline-2.206.tar.gz wineditline-2.206
  rm -rf wineditline-2.206

soil: 
For loading textures into opengl objects.
https://github.com/littlstar/soil
downloaded from https://github.com/littlstar/soil/releases

