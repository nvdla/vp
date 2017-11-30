#! /bin/bash

if [ "$1" == "" ] ;
then
    echo "No docker image specified"
    echo "Usage: $0 [DockerImagePath]"
    exit
else
    echo "Using $1 docker image"
fi
docker_img=$1
if [ -f "$docker_img" ] ;
then
    sudo docker load -i $docker_img
    sudo docker run -it -v /home:/home vp/ubuntu14.04:1.0
else
    echo "$docker_img is not valid"
    exit
fi