#!/bin/bash

set -xe

INPUT=README.md
OUTPUT=documentation.pdf

pandoc $INPUT -f gfm -o $OUTPUT
