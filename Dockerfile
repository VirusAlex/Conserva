FROM alpine:3.3
EXPOSE 2323
RUN apk update && apk upgrade && apk add --no-cache bash git openssh build-base qt-dev
RUN cd / && git clone https://github.com/VirusAlex/Conserva.git && cd Conserva && qmake ConsoleServer.pro && make
ENTRYPOINT ["/Conserva/ConsoleServer"]