# DirViewer

Program that prints out the directory in formatted way.

### Example

```
src
 server
 socket
  udp_server.c
  socket.c
```

## Build

```
git clone https://github.com/xirzo/dirviewer
cd dirviewer
mkdir build
cmake -B build
cmake --build build
```

## Use as binary

```
./build/bin/dv

# OR

./build/bin/dv <PATH_TO_DIR>
```

## Use in code

```c

// void dirview(const char *path);

int main(int argc, char *argv[]) {
    if (argc == 1) {
        dirview(".");
    } else {
        while (--argc > 0) {
            dirview(*++argv);
        }
    }

    return 0;
}
```
