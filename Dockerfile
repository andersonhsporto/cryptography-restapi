FROM	ubuntu:rolling

RUN		apt-get update

RUN apt-get install -y build-essential libssl-dev sqlite3 libsqlite3-dev

EXPOSE 8080

COPY . /usr/src/api
WORKDIR /usr/src/api

RUN make

CMD ["./crypto-api"]

# comand to build the image sudo docker build -t crypto-api .
# comand to run the image sudo docker run -p 8080:8080 crypto-api
