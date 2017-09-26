# SParGeL
## Smoothed Particle Generator and Loader

### Compiling
1. Clone or fork the repository.
2. Navigate to the root folder.
3. Run `make` the executable is placed in /bin/.

### Usage
1. From root directory `./bin/spargel` **params_file** *input_files*
2. Same from other directory but ensure EoS table path is changed in the parameter file.

### Memory Usage
Analysis will use memory equivalent to the the number of threads multiplied by input file size in binary format. Conversion will double the usage. If memory does become an issue, reduce the number of threads.
