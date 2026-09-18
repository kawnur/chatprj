# Client container image
# To build run 'docker build -t chatprj --target chatprj --file <this_file_path> /'
# Build context / is used to copy lib by host abspath

# older tag used to match openh264 version
FROM fedora:42 AS chatprj

# RUN mkdir -p /my/app /my/qt /my/boost /my/cmake
RUN mkdir -p /my/app /my/rpm

#ADD /home/user/sources/boost/boost_1_76_0/boost_1_76_0.tar.gz /my/boost/

#ADD /home/user/sources/cmake/cmake-3.24.0-linux-x86_64 /my/cmake

#add source dir
ADD /home/user/sources/rpm /my/rpm

#install openh264
RUN rpm -i /my/rpm/openh264-2.5.1-1.el10_1.x86_64.rpm

#install qt6
RUN dnf install -y qt6-qtbase qt6-qtbase-gui qt6-qtbase-postgresql

#install C Development Tools and Libraries
#RUN sudo dnf groupinstall -y "C Development Tools and Libraries"

#install compilers
RUN dnf install -y gcc
RUN dnf install -y g++

#install gpgme
RUN dnf install -y gpgme

#install GLX support for libglvnd
RUN dnf install -y libglvnd-glx

#install OpenGL support for libglvnd
RUN dnf install -y libglvnd-opengl

#install X.Org X11 XKB parsing library
RUN dnf install -y libxkbcommon

#install libs for X11
RUN dnf install -y qt-x11

#install perl
RUN dnf install -y perl

#install make
RUN dnf install -y make

#install ping
#RUN dnf install -y ping

#install vim
RUN dnf install -y vim

#install iputils
RUN dnf install -y iputils

#install tree
RUN dnf install -y tree

#install less
RUN dnf install -y less

#install cmake
#RUN ln -s /my/cmake/bin/cmake /usr/bin/cmake
RUN dnf install -y cmake

#install boost
#WORKDIR /my/boost/boost_1_76_0

#RUN ./bootstrap.sh --with-libraries=system,thread,date_time,regex,serialization,chrono,atomic

#RUN ./b2 --with-system --with-thread --with-date_time --with-regex --with-serialization \
#--with-chrono --with-atomic --libdir=/usr/lib64 --includedir=/usr/include install
RUN dnf install -y boost-devel

WORKDIR /my/app


# DB container image
# To build run 'docker build -t chatprj_db --target chatprj_db --file <this_file_path> /'
# Build context / is used to copy lib by host abspath

FROM postgres:12 AS chatprj_db

RUN mkdir -p /my/app

WORKDIR /my/app
