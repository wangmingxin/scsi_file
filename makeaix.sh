#!/bin/env bash

aixhost=198.88.88.38
ssh $aixhost rm /tmp/scsi_file
scp -r . $aixhost:/tmp/scsi_file 
ssh $aixhost cd /tmp/scsi_file/src/;gmake -f Makefile.aix
scp $aixhost:/tmp/scsi_file/src/*.a .
scp $aixhost:/tmp/scsi_file/src/*.so .

