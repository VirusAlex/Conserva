FROM alpine:3.3
RUN apk update && apk upgrade && \ apk add --no-cache bash git openssh
RUN mkdir /inst && cd /inst && wget \
  http://download.qt.io/official_releases/qt/5.6/5.6.0/single/qt-everywhere-opensource-src-5.6.0.tar.gz
RUN mkdir /src && cd /src && tar -xvf /inst/qt-everywhere-opensource-src-5.6.0.tar.gz
RUN ln -s /src/qt-everywhere-opensource-src-5.6.0 /src/qt
RUN cd /src/qt && ./configure \
  -confirm-license -opensource -silent\
  -nomake examples -nomake tests -no-compile-examples \
  -no-xcb \
  -prefix "/usr/local/Qt"
RUN cd /src/qt && make -j9 all
RUN cd /src && git clone https://github.com/VirusAlex/Conserva.git
RUN mkdir -p /usr/local/virus && cd /usr/local/virus && /src/qt/qtbase/bin/qmake /src/Conserva/ConsoleServer.pro && make