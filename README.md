## Super File System Utility 🤠

![](https://i.pinimg.com/564x/25/1f/7a/251f7a7750a8c0e148098b254bded227.jpg)


### Build ⚔️ Run ⚔️ Clean

Build 🏗️
```shell
make
```

Run 🏃
```shell
./super-fs fs <fs_file>
./super-fs info
```

or you can just run `start.sh` shell-script (recommended)

```shell
./start.sh
```

Clean 🧹
```shell
make clean
```

### Project structure 🧱

```text
.
├── cli
│   ├── cli.o
│   ├── inc
│   │   └── interactive.h
│   ├── Makefile
│   └── src
│       └── interactive.c
├── core
│   ├── core.o
│   ├── inc
│   │   ├── commands.h
│   │   ├── device.h
│   │   ├── info.h
│   │   ├── interactive_mode.h
│   │   └── xfs
│   │       ├── utils.h
│   │       ├── xfs.h
│   │       └── xfs_types.h
│   ├── Makefile
│   └── src
│       ├── commands.c
│       ├── device.c
│       ├── info.c
│       ├── interactive_mode.c
│       ├── main.c
│       └── xfs.c
├── create-loop-dev-file.sh
├── docs
│   ├── initial-xfs-sb-configuration.txt
│   └── report.md
├── Makefile
├── README.md
├── start.sh
└── xfs.fs
```
