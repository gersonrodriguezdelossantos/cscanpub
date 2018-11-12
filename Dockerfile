FROM alpine:edge

RUN mkdir /cscanpub && \
  echo '@testing http://nl.alpinelinux.org/alpine/edge/testing' >> /etc/apk/repositories && \
  apk update && \
  apk add --update build-base g++ && \
  rm -rf /var/cache/apk/* && \
  LD_LIBRARY_PATH=/usr/local/lib && \
  export LD_LIBRARY_PATH

WORKDIR /cscanpub

CMD ./gen_priv_pub
