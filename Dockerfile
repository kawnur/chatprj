# To build run 'docker build -t chatprj --file <this_file_path> /'
# Build context / used to copy lib by host abspath
FROM fedora
RUN mkdir -p /my/app /my/qt /my/boost /my/cmake

#WORKDIR /my/app

ADD /home/user/sources/qt/qt-everywhere-src-6.4.1.tar.xz /my/qt/
#RUN --mount=type=bind,target=/my/qt,source=/home/user/sources/qt/qt-everywhere-src-6.4.1

ADD /home/user/sources/boost/boost_1_76_0/boost_1_76_0.tar.gz /my/boost/

ADD /home/user/sources/cmake/cmake-3.24.0-linux-x86_64 /my/cmake
#RUN --mount=type=bind,target=/my/cmake,source=/home/user/sources/cmake/cmake-3.24.0-linux-x86_64



RUN sudo dnf groupinstall -y "C Development Tools and Libraries"
#RUN sudo yum install mesa-libGL-devel

#install opengl
#sudo dnf install glew-devel SDL2-devel SDL2_image-devel glm-devel freetype-devel

#install compilers
RUN dnf install -y gcc
RUN dnf install -y g++

#install gpgme
RUN dnf install -y gpgme

#install perl
RUN dnf install -y perl

#install make
RUN dnf install -y make

#install vim
RUN dnf install -y vim

#install tree
RUN dnf install -y tree

#install cmake
RUN ln -s /my/cmake/bin/cmake /usr/bin/cmake


#./configure -prefix $PWD/qtbase -- \
#-DOPENGL_opengl_LIBRARY=/usr/lib64/libOpenGL.so \
#-DOPENGL_glx_LIBRARY=/usr/lib64/libGLX.so \
#-DOPENGL_INCLUDE_DIR=/usr/include/GL/gl.h \
#-DOPENGL_GLX_INCLUDE_DIR=/usr/include/GL/glx.h \
#-DOPENGL_EGL_INCLUDE_DIR=/usr/include/EGL/egl.h \
#-DOPENGL_xmesa_INCLUDE_DIR= \
#-DOPENGL_glx_LIBRARY=/usr/lib64/libGLX.so \
#-DOPENGL_egl_LIBRARY=/usr/lib64/libEGL.so \
#-DOPENGL_glu_LIBRARY=/usr/lib64/libGLU.so \
#-DOPENGL_gl_LIBRARY=/usr/lib64/libGL.so



WORKDIR /my/qt/qt-everywhere-src-6.4.1

# run qt configuration
#RUN /my/qt/qt-everywhere-src-6.4.1/configure -prefix /my/qt/qt-everywhere-src-6.4.1/qtbase -no-opengl
#RUN ./configure -prefix ./qtbase -no-opengl
#RUN ./configure -prefix ./qtbase -libdir /usr/lib64 -debug -no-opengl -- -DCMAKE_INSTALL_PREFIX=/usr/lib64  # TODO reconfigure for release
RUN ./configure -prefix /usr/lib64 -libdir /usr/lib64 -debug -no-opengl  # TODO reconfigure for release

# run qt build
#RUN cmake --build /my/qt/qt-everywhere-src-6.4.1
#RUN cmake --build .
#RUN cmake --build . --parallel --target Widgets  # TODO rebuild in release  # parallel crashes on 25%
RUN cmake --build . --target Widgets  # TODO rebuild in release

#install qt::widgets
RUN cmake --install .

# TODO add clear sources


#build and install boost
WORKDIR /my/boost/boost_1_76_0

RUN ./bootstrap.sh --with-libraries=system,thread,date_time,regex,serialization,chrono,atomic

RUN ./b2 --with-system --with-thread --with-date_time --with-regex --with-serialization --with-chrono --with-atomic \
--libdir=/usr/lib64 --includedir=/usr/include install












