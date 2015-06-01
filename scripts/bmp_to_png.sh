#!/bin/bash
find -name *.bmp -type f -exec convert '{}' '{}'.png \;
find -name *.bmp -type f -exec rm '{}' \;