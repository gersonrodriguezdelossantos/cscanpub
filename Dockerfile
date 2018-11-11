FROM alpine:edge

RUN mkdir /cscanpub && \
  apk add --update build-base g++ && \
  rm -rf /var/cache/apk/* && \
  LD_LIBRARY_PATH=/usr/local/lib && \
  export LD_LIBRARY_PATH

WORKDIR /cscanpub

CMD ./gen_priv_pub
