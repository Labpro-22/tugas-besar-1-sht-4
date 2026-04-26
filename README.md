[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/b842RB8g)

# Nimonspoli — Tugas Besar 1 IF2010 Pemrograman Berorientasi Objek

Implementasi permainan papan **Nimonspoli**  dalam bahasa **C++17** dengan antarmuka CLI dan GUI (Raylib).

---

## Deskripsi Permainan

Nimonspoli adalah permainan strategi berbasis giliran di mana pemain berkeliling papan berisi 40 petak, membeli properti, membayar sewa, mengikuti lelang, dan mengelola kekayaan. Permainan berakhir ketika semua pemain kecuali satu bangkrut (mode Bankruptcy) atau batas giliran tercapai (mode Max Turn).

---

## Fitur

### Fitur Wajib
- **Papan Permainan** — 40 petak: Street, Railroad, Utility, Festival, Pajak, dan Petak Spesial
- **Kepemilikan Properti** — Beli, gadai, tebus, dan bangun rumah/hotel
- **Sistem Sewa** — Otomatis berdasarkan tipe properti dan jumlah bangunan
- **Lelang** — Dipicu saat pemain tidak membeli properti atau bangkrut ke Bank
- **Festival** — Lipatgandakan sewa properti hingga 8× selama 3 giliran
- **Kartu Kesempatan & Dana Umum** — Efek acak saat mendarat di petak kartu
- **Kartu Kemampuan Spesial** — MoveCard, DiscountCard, ShieldCard, TeleportCard, LassoCard, DemolitionCard
- **Sistem Penjara** — Masuk penjara, keluar dengan denda/double/kartu
- **Pajak** — PPH (pilih flat atau persentase) dan PBM (flat langsung)
- **Kebangkrutan** — Likuidasi aset, pengambilalihan ke pemain/Bank
- **Save/Load** — Simpan dan muat state permainan ke/dari file `.txt`
- **Transaction Logger** — Catat semua aksi signifikan selama permainan
- **Mode Max Turn & Bankruptcy** — Dua kondisi akhir permainan
- **Dadu** — Acak atau manual (`ATUR_DADU`), dengan aturan double dan masuk penjara

### Fitur Bonus
- **GUI (Raylib)** — Antarmuka grafis lengkap dengan renderer layar menu dan gameplay
- **COM (Computer Player)** — Pemain komputer yang dapat bermain secara otomatis
- **Papan Dinamis** — Papan permainan dapat dikustomisasi dari 20 petak hingga 60 petak

---

## Struktur Proyek

```
.
├── src/                          # Kode sumber
│   ├── main.cpp                  # Entry point — memilih mode CLI/GUI
│   ├── controller/               # Layer Controller (jembatan UI ↔ Model)
│   │   ├── CommandController.cpp # Pemrosesan perintah CLI
│   │   ├── GameController.cpp    # Logika alur giliran
│   │   ├── TileController.cpp    # Penanganan efek pendaratan petak
│   │   ├── CardController.cpp    # Penanganan kartu
│   │   └── GUIController/        # Versi GUI dari setiap controller
│   ├── model/                    # Layer Game Logic (inti permainan)
│   │   ├── Game.cpp              # Kelas utama permainan
│   │   ├── Board.cpp             # Papan permainan
│   │   ├── Player.cpp            # Data dan aksi pemain
│   │   ├── Dice.cpp              # Logika dadu
│   │   ├── GameContext.cpp       # Konteks state permainan aktif
│   │   ├── RentContext.cpp       # Konteks perhitungan sewa
│   │   ├── ComputerDecisionMaker.cpp  # Logika keputusan COM
│   │   ├── cards/                # Hierarki kartu (Card → HandCard/InstantCard → ...)
│   │   ├── tiles/                # Hierarki petak (Tile → OwnableTile/ActionTile → ...)
│   │   └── managers/             # Manajer subsistem permainan
│   │       ├── ConfigManager.cpp     # Baca file konfigurasi
│   │       ├── SaveLoadManager.cpp   # Simpan/muat state permainan
│   │       ├── AuctionManager.cpp    # Mekanisme lelang
│   │       ├── BankruptcyManager.cpp # Mekanisme kebangkrutan
│   │       ├── CardManager.cpp       # Manajemen deck kartu
│   │       ├── FestivalManager.cpp   # Efek festival
│   │       ├── JailManager.cpp       # Mekanisme penjara
│   │       ├── LogManager.cpp        # Transaction logger
│   │       ├── MovementManager.cpp   # Pergerakan bidak
│   │       ├── PropertyManager.cpp   # Gadai, tebus, bangun bangunan
│   │       ├── TaxManager.cpp        # Perhitungan pajak
│   │       ├── TurnManager.cpp       # Manajemen giliran
│   │       └── WinConditionManager.cpp # Pengecekan kondisi menang
│   └── view/                     # Layer UI
│       ├── Application.cpp       # Aplikasi CLI
│       ├── UIManager.cpp         # Output CLI berwarna
│       ├── RaylibGuiApp.cpp      # Aplikasi GUI
│       └── raylib/               # Renderer GUI per layar
│           ├── MenuScreenRenderer.cpp
│           ├── GameplayScreenRenderer.cpp
│           ├── OverlayRenderer.cpp
│           └── UiToolkit.cpp
├── include/                      # Header files (struktur sama dengan src/)
├── config/                       # File konfigurasi permainan
│   ├── property.txt              # Data semua properti
│   ├── railroad.txt              # Biaya sewa railroad
│   ├── utility.txt               # Faktor pengali utility
│   ├── tax.txt                   # Nilai pajak PPH & PBM
│   ├── aksi.txt                  # Pemetaan petak aksi
│   ├── special.txt               # Gaji GO dan denda penjara
│   └── misc.txt                  # MAX_TURN dan SALDO_AWAL
├── assets/
│   └── fonts/                    # Font untuk GUI
├── data/                         # File save permainan tersimpan
├── bin/                          # Hasil build (executable)
├── build/                        # Object files hasil kompilasi
└── makefile
```

## Prasyarat

### Semua Platform
- **g++** dengan dukungan C++17
- **Raylib** (untuk mode GUI)
- **make**

### Windows (Native)
- [Raylib untuk Windows](https://www.raylib.com/) — disarankan instal ke `C:\raylib\`

### WSL (Windows Subsystem for Linux)
- WSL2 dengan distro Linux
- Raylib diinstal di sisi Windows
- Compiler `g++` dari w64devkit


## Cara Build dan Run

### 1. Clone Repository

```bash
git https://github.com/Labpro-22/tugas-besar-1-sht-4.git
```

### 2. Atur Path Raylib di `makefile`

Buka file `makefile` dan sesuaikan path Raylib sesuai lokasi instalasi di perangkat Anda.

#### Windows (Native)

Cari bagian `ifeq ($(HOST_PLATFORM),windows)` dan ubah variabel berikut:

```makefile
# Ganti dengan path di mana Raylib terinstall di perangkat Anda
RAYLIB_PATH    ?= C:/raylib/raylib          # <-- path folder raylib
RAYLIB_INCLUDE ?= $(RAYLIB_PATH)/src        # biasanya tidak perlu diubah
RAYLIB_LIB     ?= $(RAYLIB_PATH)/src        # biasanya tidak perlu diubah

# Ganti dengan path folder bin dari w64devkit/MinGW Anda
TOOLCHAIN_BIN  ?= C:/raylib/w64devkit/bin   # <-- path folder bin toolchain
```

#### WSL

Cari bagian `ifeq ($(HOST_PLATFORM),wsl)` dan ubah variabel berikut:

```makefile
# Path Windows dari Raylib (diakses oleh compiler Windows)
RAYLIB_PATH    ?= C:/raylib/raylib          # <-- path Windows (pakai forward slash)
RAYLIB_INCLUDE ?= $(RAYLIB_PATH)/src
RAYLIB_LIB     ?= $(RAYLIB_PATH)/src

# Path toolchain dari sisi WSL (mount Windows drive di /mnt/c/)
TOOLCHAIN_BIN  ?= /mnt/c/raylib/w64devkit/bin   # <-- path Linux menuju folder bin
TOOLCHAIN_PREFIX ?= C:/raylib/w64devkit/bin      # <-- path Windows yang sama
```

### 3. Build

```bash
# Build dan langsung jalankan
make run

# Bersihkan hasil build
make clean

# Build ulang dari awal
make rebuild
```

---

### 4. Menjalankan Program

Setelah build berhasil, executable berada di `bin/game` (Linux/WSL) atau `bin/game.exe` (Windows).

Saat dijalankan tanpa argumen, program akan menampilkan prompt:
```
Pilih mode aplikasi:
1. CLI
2. GUI
0. Keluar
Masukkan pilihan [1/2/0]:
```

---

## Konfigurasi

Semua file konfigurasi berada di folder `config/`. File-file ini dibaca otomatis saat program dijalankan dan **tidak boleh dihapus**.

| File | Keterangan |
|---|---|
| `property.txt` | Data semua properti (ID, kode, nama, jenis, warna, harga, sewa, dsb.) |
| `railroad.txt` | Biaya sewa railroad berdasarkan jumlah railroad dimiliki |
| `utility.txt` | Faktor pengali sewa utility berdasarkan jumlah utility dimiliki |
| `tax.txt` | Nilai pajak PPH (flat & persentase) dan PBM |
| `aksi.txt` | Pemetaan ID → kode, nama, dan jenis petak aksi |
| `special.txt` | Gaji melewati petak GO dan denda keluar penjara |
| `misc.txt` | `MAX_TURN` dan `SALDO_AWAL` tiap pemain |

---

## Perintah Permainan

Berikut daftar perintah yang tersedia selama permainan (mode CLI):

| Perintah | Keterangan |
|---|---|
| `CETAK_PAPAN` | Tampilkan papan permainan berwarna dengan legenda |
| `LEMPAR_DADU` | Lempar dua dadu secara acak |
| `ATUR_DADU X Y` | Set nilai dadu secara manual (X dan Y antara 1–6) |
| `CETAK_AKTA` | Tampilkan akta kepemilikan sebuah properti |
| `CETAK_PROPERTI` | Tampilkan semua properti milik pemain aktif |
| `GADAI` | Gadaikan properti ke Bank |
| `TEBUS` | Tebus properti yang sedang digadaikan |
| `BANGUN` | Bangun rumah atau upgrade ke hotel |
| `GUNAKAN_KEMAMPUAN` | Aktifkan Kartu Kemampuan Spesial (sebelum lempar dadu) |
| `SIMPAN <nama_file>` | Simpan state permainan ke file |
| `CETAK_LOG [N]` | Tampilkan log transaksi (opsional: N entri terakhir) |

Perintah yang dipicu **otomatis** oleh sistem (tidak diketik manual):
`BELI`, `BAYAR_SEWA`, `BAYAR_PAJAK`, `LELANG`, `FESTIVAL`, `BANGKRUT`, `MENANG`, `KESEMPATAN/DANA_UMUM`, `DROP_KARTU_KEMAMPUAN`

---

## Bonus yang Diimplementasikan

### GUI (Raylib)
Antarmuka grafis lengkap menggunakan library Raylib. Semua fungsionalitas CLI tersedia di GUI, termasuk papan permainan visual, panel informasi pemain, overlay lelang, dan input perintah. Implementasi GUI hanya mengatur tampilan — game state tetap dikelola oleh layer model.

### COM (Computer Player)
Pemain komputer (`ComputerDecisionMaker`) yang dapat memainkan seluruh aksi permainan secara otomatis: melempar dadu, memutuskan beli/tidak, mengikuti lelang, memilih festival, mengelola kartu, hingga melakukan likuidasi saat terancam bangkrut. Semua keputusan COM ditampilkan jelas ke layar.

### Papan Dinamis
Papan permainan dapat dikustomisasi dengan ukuran berkisar antara 20-60 petak

---

## Anggota Kelompok

| NIM | Nama |
|---|---|
| 13524010 | Audric Yusuf Maynard Simatupang |
| 13524022 | Ishak Palentino Napitupulu |
| 13524052 | Raynard Fausta |
| 13524062 | Nathan Edward Christofer Marpaung |
| 13524112 | Richard Samuel Simanullang |
