![](https://github.com/luv4bytes/cdav/workflows/ubuntu-16.04/badge.svg)
![](https://github.com/luv4bytes/cdav/workflows/ubuntu-18.04/badge.svg)

# cdav
Scriptable CLI WebDAV client for Linux.


cdav aims to supports all operations defined in RFC 4918.
For further information about WebDAV see ![IETF RFC 4918](https://tools.ietf.org/html/rfc4918).

## Currently working on
- ![GitHub-Pages page](https://luv4bytes.github.io/cdav)

## Pending:
- Formatted output
- GUI (cdav-gtk)

## Build

To build cdav make sure to run `make deps` before running `make`. <br>
cdav requires following libraries to be built:

- ![libxml2](http://xmlsoft.org/)
- ![libcurl](https://curl.se/libcurl/)


## Help
To get information and help about arguments and property syntax run `cdav -h` or `cdav` without any arguments.
For detailed information on how to use command files refer to the offical ![GitHub Pages page](https://luv4bytes.github.io/cdav).
