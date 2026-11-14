#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>
#include <algorithm>
#include "transaksi.h"
using namespace std;

static string trim(const string &s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string bacaString(const string &prompt) {
    string s;
    cout << prompt;
    if (!getline(cin, s)) exit(0);
    return trim(s);
}

long long bacaAngka(const string &prompt, long long minv = 0) {
    while (true) {
        string s = bacaString(prompt);
        long long v;
        if (parseLL(s, v) && v >= minv) return v;
        cout << "  Input tidak valid. Masukkan angka >= " << minv << ".\n";
    }
}

string bacaTanggal(const string &prompt, bool kosongHariIni) {
    while (true) {
        string s = bacaString(prompt);
        if (s.empty() && kosongHariIni) return todayString();
        if (isValidDate(s)) return s;
        cout << "  Format tanggal salah. Gunakan YYYY-MM-DD (contoh 2026-05-20).\n";
    }
}

string pilihKategori() {
    cout << "Kategori:\n";
    for (int i = 0; i < JML_KATEGORI; i++) cout << "  " << i + 1 << ". " << KATEGORI[i] << "\n";
    long long p;
    do { p = bacaAngka("Pilih kategori (1-6): "); } while (p < 1 || p > JML_KATEGORI);
    return KATEGORI[p - 1];
}

string pilihTipe() {
    cout << "Tipe: 1. Pengeluaran  2. Pemasukan\n";
    long long p;
    do { p = bacaAngka("Pilih tipe (1-2): "); } while (p < 1 || p > 2);
    return p == 1 ? "Pengeluaran" : "Pemasukan";
}

void tekanEnter() { bacaString("\nTekan Enter untuk lanjut..."); }

static string hashPass(const string &p) {
    unsigned long h = 5381;
    for (size_t i = 0; i < p.size(); i++) h = h * 33 + (unsigned char)p[i];
    ostringstream o;
    o << h;
    return o.str();
}

static bool namaValid(const string &u) {
    if (u.empty() || u.size() > 20) return false;
    for (size_t i = 0; i < u.size(); i++)
        if (!isalnum((unsigned char)u[i]) && u[i] != '_') return false;
    return true;
}

static bool cariUser(const string &u, string &hashOut) {
    ifstream f("users.txt");
    string nama, h;
    while (f >> nama >> h) {
        if (nama == u) { hashOut = h; return true; }
    }
    return false;
}

void registrasi() {
    cout << "\n=== REGISTRASI AKUN ===\n";
    string u = bacaString("Username 1 sampai 20 karakter (huruf, angka, garis bawah): ");
    if (!namaValid(u)) { cout << "Username tidak valid.\n"; return; }
    string tmp;
    if (cariUser(u, tmp)) { cout << "Username sudah dipakai.\n"; return; }
    string p = bacaString("Password (min 4 karakter): ");
    if (p.size() < 4) { cout << "Password terlalu pendek.\n"; return; }
    ofstream f("users.txt", ios::app);
    f << u << " " << hashPass(p) << "\n";
    cout << "Registrasi berhasil! Silakan login.\n";
}

bool login(string &userOut) {
    cout << "\n=== LOGIN ===\n";
    string u = bacaString("Username: ");
    string p = bacaString("Password: ");
    string h;
    if (cariUser(u, h) && h == hashPass(p)) {
        userOut = u;
        cout << "Login berhasil. Halo, " << u << "!\n";
        return true;
    }
    cout << "Username atau password salah.\n";
    return false;
}

static void cekSatu(const string &nama, long long total, long long batas) {
    if (batas <= 0) return;
    if (total > batas)
        cout << "  [PERINGATAN] Pengeluaran " << nama << " melebihi batas! ("
             << formatRupiah(total) << " / " << formatRupiah(batas) << ")\n";
    else if (total * 100 >= batas * 80)
        cout << "  [HATI-HATI] Pengeluaran " << nama << " sudah " << (total * 100 / batas)
             << "% dari batas (" << formatRupiah(total) << " / " << formatRupiah(batas) << ")\n";
}

void cekPeringatan(List L, Pengaturan S, const string &tanggal) {
    cekSatu("harian", totalPengeluaran(L, tanggal), S.batasHarian);
    cekSatu("bulanan", totalPengeluaran(L, tanggal.substr(0, 7)), S.batasBulanan);
    cekSatu("tahunan", totalPengeluaran(L, tanggal.substr(0, 4)), S.batasTahunan);
}

infotype inputTransaksi(List &L) {
    infotype t;
    t.id = L.nextId;
    t.tanggal = bacaTanggal("Tanggal (YYYY-MM-DD, kosong = hari ini): ", true);
    do { t.keterangan = bacaString("Keterangan: "); } while (t.keterangan.empty());
    for (size_t i = 0; i < t.keterangan.size(); i++)
        if (t.keterangan[i] == '|') t.keterangan[i] = '-';
    t.kategori = pilihKategori();
    t.nominal = bacaAngka("Nominal (Rp): ", 1);
    t.tipe = pilihTipe();
    return t;
}

void menuTambah(List &L, Pengaturan &S) {
    cout << "\n=== TAMBAH TRANSAKSI ===\n";
    cout << "1. Tambah di awal\n2. Tambah di akhir\n3. Tambah setelah ID tertentu\n";
    long long pos;
    do { pos = bacaAngka("Pilih posisi (1-3): "); } while (pos < 1 || pos > 3);

    address prec = NULL;
    if (pos == 3) {
        int idPrec = (int)bacaAngka("Sisipkan setelah ID: ");
        prec = findByID(L, idPrec);
        if (prec == NULL) { cout << "ID " << idPrec << " tidak ditemukan.\n"; return; }
    }

    infotype t = inputTransaksi(L);
    L.nextId++;
    address P = alokasi(t);
    if (pos == 1) insertFirst(L, P);
    else if (pos == 2) insertLast(L, P);
    else insertAfter(prec, P);

    cout << "Transaksi berhasil ditambahkan dengan ID " << t.id << ".\n";
    if (t.tipe == "Pengeluaran") cekPeringatan(L, S, t.tanggal);
}

void menuUbah(List &L, Pengaturan &S) {
    cout << "\n=== UBAH TRANSAKSI ===\n";
    if (isEmpty(L)) { cout << "Belum ada transaksi.\n"; return; }
    int id = (int)bacaAngka("ID transaksi yang diubah: ");
    address P = findByID(L, id);
    if (P == NULL) { cout << "ID tidak ditemukan.\n"; return; }

    printHeader();
    printRow(P->info);
    cout << "\n(kosongkan input untuk mempertahankan nilai lama)\n";

    string s = bacaString("Tanggal baru (YYYY-MM-DD): ");
    if (!s.empty()) {
        if (isValidDate(s)) P->info.tanggal = s;
        else cout << "  Format tanggal salah, tanggal tidak diubah.\n";
    }
    s = bacaString("Keterangan baru: ");
    if (!s.empty()) {
        for (size_t i = 0; i < s.size(); i++) if (s[i] == '|') s[i] = '-';
        P->info.keterangan = s;
    }
    cout << "Ubah kategori? (y/n): ";
    if (bacaString("") == "y") P->info.kategori = pilihKategori();
    s = bacaString("Nominal baru: ");
    if (!s.empty()) {
        long long v;
        if (parseLL(s, v) && v > 0) P->info.nominal = v;
        else cout << "  Nominal tidak valid, tidak diubah.\n";
    }
    cout << "Ubah tipe? (y/n): ";
    if (bacaString("") == "y") P->info.tipe = pilihTipe();

    cout << "Transaksi berhasil diperbarui.\n";
    if (P->info.tipe == "Pengeluaran") cekPeringatan(L, S, P->info.tanggal);
}

void menuHapus(List &L) {
    cout << "\n=== HAPUS TRANSAKSI ===\n";
    if (isEmpty(L)) { cout << "Belum ada transaksi.\n"; return; }
    cout << "1. Delete First\n2. Delete Last\n3. Delete After (hapus transaksi setelah ID tertentu)\n";
    long long pos;
    do { pos = bacaAngka("Pilih (1-3): "); } while (pos < 1 || pos > 3);

    address P = NULL;
    if (pos == 1) {
        deleteFirst(L, P);
    } else if (pos == 2) {
        deleteLast(L, P);
    } else {
        int idPrec = (int)bacaAngka("Hapus transaksi SETELAH ID: ");
        address prec = findByID(L, idPrec);
        if (prec == NULL) { cout << "ID " << idPrec << " tidak ditemukan.\n"; return; }
        deleteAfter(prec, P);
        if (P == NULL) { cout << "Tidak ada transaksi setelah ID " << idPrec << ".\n"; return; }
    }
    cout << "Transaksi berikut dihapus:\n";
    printHeader();
    printRow(P->info);
    dealokasi(P);   // kembalikan memori node
}

void menuCari(List &L) {
    cout << "\n=== CARI TRANSAKSI ===\n";
    if (isEmpty(L)) { cout << "Belum ada transaksi.\n"; return; }
    cout << "1. Berdasarkan ID\n2. Berdasarkan kata kunci (keterangan/kategori)\n";
    long long p;
    do { p = bacaAngka("Pilih (1-2): "); } while (p < 1 || p > 2);
    if (p == 1) {
        int id = (int)bacaAngka("ID: ");
        address P = findByID(L, id);
        if (P == NULL) cout << "Transaksi tidak ditemukan.\n";
        else { printHeader(); printRow(P->info); }
    } else {
        string kw = bacaString("Kata kunci: ");
        int n = searchKeyword(L, kw);
        if (n == 0) cout << "Tidak ada transaksi yang cocok.\n";
        else cout << string(80, '-') << "\nDitemukan " << n << " transaksi.\n";
    }
}

void menuTerbesar(List &L) {
    cout << "\n=== PENGELUARAN TERBESAR ===\n";
    string prefix = bacaString("Periode (YYYY-MM / YYYY, kosong = semua): ");
    address M = findMaxPengeluaran(L, prefix);
    if (M == NULL) { cout << "Tidak ada pengeluaran pada periode tersebut.\n"; return; }
    printHeader();
    printRow(M->info);
}

void menuRingkasan(List &L, Pengaturan &S) {
    cout << "\n=== RINGKASAN BULANAN ===\n";
    string bulan;
    while (true) {
        bulan = bacaString("Bulan (YYYY-MM, kosong = bulan ini): ");
        if (bulan.empty()) { bulan = todayString().substr(0, 7); break; }
        if (bulan.size() == 7 && isValidDate(bulan + "-01")) break;
        cout << "  Format salah. Contoh: 2026-05\n";
    }
    long long out = totalPengeluaran(L, bulan);
    long long in = totalPemasukan(L, bulan);
    cout << "\nPeriode             : " << bulan << "\n";
    cout << "Anggaran bulanan    : " << formatRupiah(S.anggaran) << "\n";
    cout << "Total pemasukan     : " << formatRupiah(in) << "\n";
    cout << "Total pengeluaran   : " << formatRupiah(out) << "\n";
    cout << "Sisa anggaran       : " << formatRupiah(S.anggaran - out) << "\n";
    if (S.anggaran > 0 && out > S.anggaran) cout << "  [!] Pengeluaran melebihi anggaran bulanan.\n";
    printRingkasanKategori(L, bulan);

    address M = findMaxPengeluaran(L, bulan);
    if (M != NULL) {
        cout << "\nPengeluaran terbesar bulan ini: " << M->info.keterangan << " ("
             << formatRupiah(M->info.nominal) << ", " << M->info.tanggal << ")\n";
    }
}

void menuFilter(List &L) {
    cout << "\n=== FILTER RENTANG TANGGAL ===\n";
    string a = bacaTanggal("Tanggal awal  (YYYY-MM-DD): ", false);
    string b = bacaTanggal("Tanggal akhir (YYYY-MM-DD): ", false);
    if (a > b) swap(a, b);
    printRange(L, a, b);
}

void menuPengaturan(Pengaturan &S) {
    cout << "\n=== ATUR ANGGARAN & PERINGATAN ===\n";
    cout << "Anggaran bulanan saat ini : " << formatRupiah(S.anggaran) << "\n";
    cout << "Batas harian              : " << (S.batasHarian ? formatRupiah(S.batasHarian) : "nonaktif") << "\n";
    cout << "Batas bulanan             : " << (S.batasBulanan ? formatRupiah(S.batasBulanan) : "nonaktif") << "\n";
    cout << "Batas tahunan             : " << (S.batasTahunan ? formatRupiah(S.batasTahunan) : "nonaktif") << "\n\n";
    cout << "1. Ubah anggaran bulanan\n2. Atur batas harian\n3. Atur batas bulanan\n"
            "4. Atur batas tahunan\n0. Kembali\n";
    long long p;
    do { p = bacaAngka("Pilih (0-4): "); } while (p > 4);
    switch (p) {
        case 1: S.anggaran = bacaAngka("Anggaran bulanan baru (Rp): "); break;
        case 2: S.batasHarian = bacaAngka("Batas harian (Rp, 0 = nonaktif): "); break;
        case 3: S.batasBulanan = bacaAngka("Batas bulanan (Rp, 0 = nonaktif): "); break;
        case 4: S.batasTahunan = bacaAngka("Batas tahunan (Rp, 0 = nonaktif): "); break;
        default: return;
    }
    cout << "Pengaturan disimpan.\n";
}

void sesiPengguna(const string &user) {
    List L;
    Pengaturan S = {0, 0, 0, 0};
    createList(L);
    string file = "data_" + user + ".txt";

    if (loadList(L, S, file)) {
        cout << "Data dimuat: " << countNodes(L) << " transaksi.\n";
    } else {
        cout << "\nAkun baru terdeteksi. Silakan masukkan anggaran bulanan.\n";
        S.anggaran = bacaAngka("Anggaran bulanan (Rp): ");
    }

    while (true) {
        cout << "\n==============================\n";
        cout << "   DUITKU - Menu Utama (" << user << ")\n";
        cout << "==============================\n";
        cout << " 1. Tambah transaksi\n 2. Tampilkan semua transaksi\n 3. Ubah transaksi\n"
                " 4. Hapus transaksi\n 5. Cari transaksi\n 6. Pengeluaran terbesar\n"
                " 7. Ringkasan bulanan & sisa anggaran\n 8. Filter rentang tanggal\n"
                " 9. Atur anggaran & peringatan\n 0. Simpan & logout\n";
        long long m = bacaAngka("Pilih menu: ");
        switch (m) {
            case 1: menuTambah(L, S); break;
            case 2: cout << "\n=== DAFTAR TRANSAKSI ===\n"; printAll(L); break;
            case 3: menuUbah(L, S); break;
            case 4: menuHapus(L); break;
            case 5: menuCari(L); break;
            case 6: menuTerbesar(L); break;
            case 7: menuRingkasan(L, S); break;
            case 8: menuFilter(L); break;
            case 9: menuPengaturan(S); break;
            case 0:
                if (saveList(L, S, file)) cout << "Data tersimpan ke " << file << ".\n";
                else cout << "Gagal menyimpan data!\n";
                clearList(L);   // dealokasi semua node
                return;
            default: cout << "Menu tidak tersedia.\n";
        }
        if (m != 0) tekanEnter();
    }
}

int main() {
    cout << "========================================\n";
    cout << "   DUITKU - Pengelolaan Keuangan Bulanan\n";
    cout << "========================================\n";
    while (true) {
        cout << "\n1. Login\n2. Registrasi\n0. Keluar\n";
        long long p = bacaAngka("Pilih: ");
        if (p == 1) {
            string user;
            if (login(user)) sesiPengguna(user);
        } else if (p == 2) {
            registrasi();
        } else if (p == 0) {
            cout << "Terima kasih telah menggunakan DUITKU!\n";
            return 0;
        } else {
            cout << "Pilihan tidak tersedia.\n";
        }
    }
}