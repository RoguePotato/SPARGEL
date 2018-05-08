#!/bin/bash
valgrind -q --tool=memcheck --leak-check=yes ./bin/spargel $1 $2