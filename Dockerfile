FROM alpine:edge AS alpinecrypto

RUN echo "http://dl-cdn.alpinelinux.org/alpine/edge/main" > /etc/apk/repositories; \
    echo "http://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories; \
    echo "http://dl-cdn.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories 

RUN apk update 

RUN apk add --no-cache --update build-base g++ bash crypto++ && \
  rm -rf /var/cache/apk/* && \
  LD_LIBRARY_PATH=/usr/local/lib && \
  export LD_LIBRARY_PATH

#RUN build.sh

FROM alpinecrypto

RUN LD_LIBRARY_PATH=/usr/local/lib && \
export LD_LIBRARY_PATH
WORKDIR /cscanpub
CMD ./cscanpub/dist/gen_priv_pub
