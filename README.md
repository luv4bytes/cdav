![](https://github.com/luv4bytes/cdav/workflows/ubuntu-16.04/badge.svg)
![](https://github.com/luv4bytes/cdav/workflows/ubuntu-18.04/badge.svg)

# cdav
Scriptable CLI WebDAV client for Linux.


cdav aims to support all operations defined in RFC 4918.
For further information about WebDAV see <a href="https://tools.ietf.org/html/rfc4918">IETF RFC 4918</a>.

## Currently working on
-  <a href="https://luv4bytes.github.io/cdav">GitHub Pages page</a>

## Pending:
- Formatted output
- GUI (cdav-gtk)

## Build

To build cdav make sure to run `make deps` before running `make`. <br>
cdav requires following libraries to be built:

- <a href="http://xmlsoft.org/">libmxl2</a>
- <a href="https://curl.se/libcurl/">libcurl</a>


## Help
To get information and help about arguments and property syntax run `cdav -h` or `cdav` without any arguments.
For detailed information on how to use command files refer to the offical <a href="https://luv4bytes.github.io/cdav">GitHub Pages page</a>.
