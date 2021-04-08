## Отчет по работе с ФС XFS

### Создание файла ФС (loop device)

Для создания файла я использовал самописеый скрипт, который создвавал нулевой файл и монтировал его на первый свободный dev/loop**

```text
magicnum = 0x58465342
blocksize = 4096
dblocks = 16384
rblocks = 0
rextents = 0
uuid = b65c0299-6c57-42b9-a6c4-8998af3d937a
logstart = 8198
rootino = 128
rbmino = 129
rsumino = 130
rextsize = 1
agblocks = 4096
agcount = 4
rbmblocks = 0
logblocks = 1368
versionnum = 0xb4a5
sectsize = 512
inodesize = 512
inopblock = 8
fname = "\000\000\000\000\000\000\000\000\000\000\000\000"
blocklog = 12
sectlog = 9
inodelog = 9
inopblog = 3
agblklog = 12
rextslog = 0
inprogress = 0
imax_pct = 25
icount = 64
ifree = 61
fdblocks = 14984
frextents = 0
uquotino = null
gquotino = null
qflags = 0
flags = 0
shared_vn = 0
inoalignmt = 8
unit = 0
width = 0
dirblklog = 0
logsectlog = 0
logsectsize = 0
logsunit = 1
features2 = 0x18a
bad_features2 = 0x18a
features_compat = 0
features_ro_compat = 0x5
features_incompat = 0x3
features_log_incompat = 0
crc = 0x88e4fba1 (correct)
spino_align = 4
pquotino = null
lsn = 0x100000008
meta_uuid = 00000000-0000-0000-0000-000000000000
```

