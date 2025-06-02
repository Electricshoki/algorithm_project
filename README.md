# HISAT2-style Genome Alignment Simulator

This project contains a simplified implementation of a genome simulation and alignment system inspired by [HISAT2](https://github.com/DaehwanKimLab/hisat2). It includes two main components: genome generation and sequence alignment using FM-index.

## Files

### `genome.c`
Generates a reference genome sequence, applies mutations to produce a personalized genome (`my_genome`), and simulates read and alignment information. This file helps create test data for alignment.

### `hisat2.cpp`
Implements a basic version of the HISAT2 alignment algorithm using C++. It includes suffix array construction, Burrows-Wheeler Transform (BWT), FM-index structures, and read alignment with limited mismatches.  
This file was written in C++ for convenience while following the logic and behavior of the original HISAT2 system.

## Note
This implementation is for educational and experimental use only. It does not cover the full feature set or performance optimizations of the original HISAT2.