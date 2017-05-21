FROM alpine:3.3
EXPOSE 2323 22
RUN apk update && apk upgrade && apk add --no-cache bash git openssh build-base qt-dev
RUN cd / && git clone https://github.com/VirusAlex/Conserva.git && cd Conserva && qmake ConsoleServer.pro && make
RUN cd /
RUN echo "#!/bin/ash" >> /run.sh
RUN echo "ssh-keygen -A && /usr/sbin/sshd && /Conserva/ConsoleServer" >> /run.sh
#RUN echo "/bin/bash" >> /run.sh
RUN chmod +x /run.sh
RUN sed -i s/#PermitRootLogin.*/PermitRootLogin\ yes/ /etc/ssh/sshd_config
RUN echo "root:root" | chpasswd
ENTRYPOINT ["/run.sh"]