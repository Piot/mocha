set -x
rm repl
rm -rf output/macos
python ../../Project-Generator/source/generate.py  -p macos -i project.xml -g makefile -n repl
make -f output/macos/Makefile clean
make -f output/macos/Makefile

./repl tests/def_test.nim
./repl tests/recur_test.nim
./repl tests/let_test.nim
