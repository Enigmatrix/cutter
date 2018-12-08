#!/bin/sh

protoc -I=. --cpp_out=. client.proto
