.PHONY: all clean release

all: sudodeckd

sudodeckd:
	go build -ldflags="-s -w" -o sudodeckd .

release:
	GOOS=linux GOARCH=amd64 go build -ldflags="-s -w" -o release/sudodeckd-linux-amd64 .
	GOOS=linux GOARCH=arm64 go build -ldflags="-s -w" -o release/sudodeckd-linux-arm64 .
	GOOS=windows GOARCH=amd64 go build -ldflags="-s -w" -o release/sudodeckd-windows-amd64.exe .
	GOOS=darwin GOARCH=amd64 go build -ldflags="-s -w" -o release/sudodeckd-darwin-amd64 .
	GOOS=darwin GOARCH=arm64 go build -ldflags="-s -w" -o release/sudodeckd-darwin-arm64 .

clean:
	rm -f sudodeckd release/sudodeckd-*
