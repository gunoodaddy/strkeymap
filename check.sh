#!/bin/bash

rm -rf result.valgrind*
#valgrind -v --tool=memcheck --leak-check=yes  --show-reachable=yes  --log-file=result.valgrind tester
valgrind -v --tool=memcheck --leak-check=yes  --show-reachable=yes tester
