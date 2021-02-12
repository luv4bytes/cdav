![](https://github.com/luv4bytes/cdav/workflows/ubuntu-16.04/badge.svg)
![](https://github.com/luv4bytes/cdav/workflows/ubuntu-18.04/badge.svg)

# cdav
Interactive and scriptable CLI WebDAV client for Linux.

**The interactive feature is currently in development**

cdav aims to support all operations defined in RFC 4918.
For further information about WebDAV see <a href="https://tools.ietf.org/html/rfc4918">IETF RFC 4918</a>.

## Contribute
If you want to work on `cdav` just fork it and let's go! :smile:  
On the *Issues* tab you can find stuff that can be done. Other than that I'm always open for suggestions!

## Build
cdav requires following libraries:

- <a href="http://xmlsoft.org/">libmxl2</a>
- <a href="https://curl.se/libcurl/">libcurl</a>

To build cdav run these commands:

```
mkdir build
cd build
cmake ..
make
```

## Help
For detailed information on cdav (arguments, commandfile etc.) refer to the offical <a href="https://luv4bytes.github.io/cdav">GitHub Pages page</a>.
