FROM ubuntu:22.04

ENV TZ=Asia/Tokyo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update && apt-get install -y clang-15 cmake \
&& sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d -b /usr/local/bin
