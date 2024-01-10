# Client container image
# To build run 'docker build -t chatprj --file <this_file_path> /'
# Build context / used to copy lib by host abspath
FROM fedora AS chatprj

RUN mkdir -p /my/app /my/qt /my/boost /my/cmake

ADD /home/user/sources/boost/boost_1_76_0/boost_1_76_0.tar.gz /my/boost/

ADD /home/user/sources/cmake/cmake-3.24.0-linux-x86_64 /my/cmake

#install C Development Tools and Libraries
RUN sudo dnf groupinstall -y "C Development Tools and Libraries"

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
RUN dnf install -y ping

#install vim
RUN dnf install -y vim

#install iputils
RUN dnf install -y iputils

#install tree
RUN dnf install -y tree

#install less
RUN dnf install -y less

#install cmake
RUN ln -s /my/cmake/bin/cmake /usr/bin/cmake

#install qt6
RUN dnf install -y qt6-qt3d qt6-qt5compat qt6-qtbase qt6-qtbase-gui \
qt6-qtbase-mysql qt6-qtbase-postgresql qt6-qtcharts qt6-qtconnectivity \
qt6-qtdatavis3d qt6-qtdeclarative qt6-qtimageformats qt6-qtlocation \
qt6-qtlottie qt6-qtmultimedia qt6-qtnetworkauth qt6-qtquick3d \
qt6-qtquicktimeline qt6-qtremoteobjects qt6-qtscxml qt6-qtsensors \
qt6-qtserialbus qt6-qtserialport qt6-qtshadertools qt6-qtsvg qt6-qttools \
qt6-qtvirtualkeyboard qt6-qtwayland qt6-qtwebchannel qt6-qtwebsockets

#build and install boost
WORKDIR /my/boost/boost_1_76_0

RUN ./bootstrap.sh --with-libraries=system,thread,date_time,regex,serialization,chrono,atomic

RUN ./b2 --with-system --with-thread --with-date_time --with-regex --with-serialization --with-chrono --with-atomic \
--libdir=/usr/lib64 --includedir=/usr/include install

WORKDIR /my/app


# DB container image
FROM postgres:12 as chatprj_db

RUN mkdir -p /my/app

WORKDIR /my/app
