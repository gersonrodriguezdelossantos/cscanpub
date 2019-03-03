FROM alpine:edge AS alpinecrypto

RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/main" > /etc/apk/repositories; \
    echo "http://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories; \
    echo "http://dl-cdn.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories 

RUN apk update 

RUN apk add --no-cache --update build-base g++ bash crypto++ crypto++-dev && \
  rm -rf /var/cache/apk/* 

ENV LD_LIBRARY_PATH /usr/local/lib

COPY . /
RUN /build.sh

FROM alpinecrypto

ENV LD_LIBRARY_PATH /usr/local/lib

CMD ./gen_priv_pub
