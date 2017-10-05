set -x	
rm repl
rm -rf output/macos
python ../../Project-Generator/source/generate.py  -p macos -i project.xml -g makefile -n repl
make -f output/macos/Makefile clean
make -f output/macos/Makefile 


