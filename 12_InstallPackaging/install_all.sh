#!/bin/bash

autoreconf -fisv
./configure --prefix=/usr/local
sudo make all install
