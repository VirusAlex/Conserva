FROM alpine:3.3
EXPOSE 2323 22
RUN apk update && apk upgrade && apk add --no-cache bash git openrc openssh build-base qt-dev
RUN rc-update add sshd
RUN /etc/init.d/sshd start
RUN cd / && git clone https://github.com/VirusAlex/Conserva.git && cd Conserva && qmake ConsoleServer.pro && make &
ENTRYPOINT ["/Conserva/ConsoleServer"]