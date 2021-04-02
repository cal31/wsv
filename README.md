# WSV
Bare-bone HTTP/1.1 server (RFC 7230-7231).

Methods:
- GET
- HEAD
- PUT/POST
- DELETE
- TRACE

Request headers:
- Transfer-Encoding
- Content-Length
- Host

Response headers:
- Server [always]
- Content-Type [always except DELETE]
- Content-Length [always]
- Date [always if system call is successful]
- Last-Modified [always except PUT, TRACE and DELETE]
- Location [PUT 201 Created]
- Content-Location [GET]
- Allow [always]

## Compilation (linux only)
`$> make`

You'll obtain a binary named `wsv`.

## Usage
`$> wsv /path/to/file.conf`

## Configuration

Example
```
server {
	listen [::]:8080;

	server_name wsv;

	error_page 404 /404.html;

	location /www/ {
		index index.html;
		client_max_body_size 40;
		limit_except GET HEAD PUT DELETE OPTIONS TRACE {
			deny all;
		}
	}

	location / {
		root /tmp/;
		autoindex on;
		limit_except GET HEAD {
			deny all;
		}
	}
}

server {
	listen [::]:8080;

	server_name localhost:8080;

	location /public/ {
		index index.html;
		limit_except GET PUT POST {
			deny all;
		}
	}
}
```

You can specify multiple `server` block.
- `listen` :
    Ipv6 address.
- `server_name` :
    White-space delimited string.
- `error_page` :
    Maps error codes to the specified file path.
- `location` :
    Tell the server how to handle request for the specified request target.
    - `index` :
        File to be served if no file was specified in the request target.
	- `autoindex`:
		"on" or "off". Creates directory listing if enabled.
    - `root` :
        Directory path prefixed to the request target.
        Defaults to the binary current directory.
    - `client_max_body_size` :
        Defaults to 0 bytes.
    - `limit_except` :
        Allowed methods. If not specified, no methods are allowed.

(Relative paths start at the binary current directory)

### Note(s)
This is one of my very first project and
I was genuinely surprised while reviewing the code.
It has rough edges but half-decent overall.
