## Super File System Utility 🤠

![](https://i.pinimg.com/564x/25/1f/7a/251f7a7750a8c0e148098b254bded227.jpg)


### Build ⚔️ Run ⚔️ Clean

Build 🏗️
```shell
make
```

Run 🏃
```shell
./super-fs start
./super-fs info
```

Clean 🧹
```shell
make clean
```

### Project structure 🧱

```text
├── cli
│   ├── inc
│   │   └── interactive.h
│   ├── Makefile
│   └── src
│       └── interactive.c
├── core
│   ├── inc
│   │   ├── device.h
│   │   ├── info.h
│   │   └── interactive_mode.h
│   ├── Makefile
│   └── src
│       ├── device.c
│       ├── info.c
│       ├── interactive_mode.c
│       └── main.c
├── create-loop-dev-file.sh
├── Makefile
├── README.md
```
