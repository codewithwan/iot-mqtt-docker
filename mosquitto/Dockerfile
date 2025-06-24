FROM eclipse-mosquitto:2

RUN apk add --no-cache openssh \
    && echo 'root:root123' | chpasswd \
    && mkdir -p /var/run/sshd \
    && ssh-keygen -A \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

RUN echo '#!/bin/sh' > /start.sh \
    && echo 'set -e' >> /start.sh \
    && echo '' >> /start.sh \
    && echo '/usr/sbin/sshd' >> /start.sh \
    && echo '' >> /start.sh \
    && echo 'exec /usr/sbin/mosquitto -c /mosquitto/config/mosquitto.conf' >> /start.sh \
    && chmod +x /start.sh

EXPOSE 22 1883

CMD ["/start.sh"]
