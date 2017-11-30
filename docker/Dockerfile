FROM ubuntu:14.04

RUN apt-get update && apt-get install -y --force-yes \ 
g++ cmake libboost-dev python-dev libglib2.0-dev libpixman-1-dev liblua5.2-dev swig libcap-dev git vim libattr1-dev wget \
&& rm -rf /var/lib/apt/lists/*

RUN mkdir -p /usr/src/systemc-2.3.0a
WORKDIR /usr/src/systemc-2.3.0a

RUN wget -O systemc-2.3.0a.tar.gz http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.0a.tar.gz \
&& tar xzvf systemc-2.3.0a.tar.gz

RUN cd /usr/src/systemc-2.3.0a/systemc-2.3.0a \
&& ls \
&& mkdir -p /usr/local/systemc-2.3.0/ \
&& mkdir objdir \
&& cd objdir \
&& ../configure --prefix=/usr/local/systemc-2.3.0 \
&& make  \
&& make install

RUN mkdir -p /usr/local/vp-build
COPY ./vp-build /usr/local/vp-build

WORKDIR /usr/local/vp-build