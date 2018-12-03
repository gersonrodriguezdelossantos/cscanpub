FROM alpine:edge

RUN mkdir /cscanpub && \
  echo '@testing http://nl.alpinelinux.org/alpine/edge/testing' >> /etc/apk/repositories && \
  apk update && \
  apk add --update build-base g++ bash && \
  rm -rf /var/cache/apk/* && \
  LD_LIBRARY_PATH=/usr/local/lib && \
  export LD_LIBRARY_PATH

RUN build.sh

FROM cscanpub

RUN LD_LIBRARY_PATH=/usr/local/lib && \
export LD_LIBRARY_PATH
WORKDIR /cscanpub
CMD ./cscanpub/gen_priv_pub
