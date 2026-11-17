#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>
#include <cctype>
#include <cstdlib>
#include <cstddef>
#include "transaksi.h"
using namespace std;

const string KATEGORI[JML_KATEGORI] = {"Makan", "Transport", "Kuliah", "Hiburan", "Kos", "Lainnya"};
const string NAMA_PERIODE[JML_PERIODE] = {"Harian", "Mingguan", "Bulanan", "Tahunan"};

void createList(List &L) {
    L.first = NULL;
    L.nextId = 1;
}

bool isEmpty(List L) { return L.first == NULL; }

address alokasi(infotype x) {
    address P = new Node;
    P->info = x;
    P->next = NULL;
    return P;
}

void dealokasi(address &P) {
    delete P;
    P = NULL;
}

void insertFirst(List &L, address P) {
    P->next = L.first;
    L.first = P;
}

void insertLast(List &L, address P) {
    if (isEmpty(L)) {
        L.first = P;
    } else {
        address Q = L.first;
        while (Q->next != NULL) Q = Q->next;
        Q->next = P;
    }
}

void insertAfter(address Prec, address P) {
    P->next = Prec->next;
    Prec->next = P;
}

void deleteFirst(List &L, address &P) {
    P = L.first;
    if (P != NULL) {
        L.first = P->next;
        P->next = NULL;
    }
}

void deleteLast(List &L, address &P) {
    if (isEmpty(L)) {
        P = NULL;
    } else if (L.first->next == NULL) {
        P = L.first;
        L.first = NULL;
    } else {
        address Q = L.first;
        while (Q->next->next != NULL) Q = Q->next;
        P = Q->next;
        Q->next = NULL;
    }
}

void deleteAfter(address Prec, address &P) {
    P = Prec->next;
    if (P != NULL) {
        Prec->next = P->next;
        P->next = NULL;
    }
}

void clearList(List &L) {
    address P;
    while (!isEmpty(L)) {
        deleteFirst(L, P);
        dealokasi(P);
    }
}

int countNodes(List L) {
    int n = 0;
    for (address P = L.first; P != NULL; P = P->next) n++;
    return n;
}

static bool cocokPrefix(const string &tanggal, const string &prefix) {
    return tanggal.compare(0, prefix.size(), prefix) == 0;
}

static string toLower(string s) {
    for (size_t i = 0; i < s.size(); i++) s[i] = (char)tolower((unsigned char)s[i]);
    return s;
}

address findByID(List L, int id) {
    address P = L.first;
    while (P != NULL) {
        if (P->info.id == id) return P;
        P = P->next;
    }
    return NULL;
}

int searchKeyword(List L, const string &kw) {
    string key = toLower(kw);
    int found = 0;
    for (address P = L.first; P != NULL; P = P->next) {
        if (toLower(P->info.keterangan).find(key) != string::npos ||
            toLower(P->info.kategori).find(key) != string::npos) {
            if (found == 0) printHeader();
            printRow(P->info);
            found++;
        }
    }
    return found;
}

address findMaxPengeluaran(List L, const string &prefix) {
    address maxP = NULL;
    for (address P = L.first; P != NULL; P = P->next) {
        if (P->info.tipe == "Pengeluaran" && cocokPrefix(P->info.tanggal, prefix)) {
            if (maxP == NULL || P->info.nominal > maxP->info.nominal) maxP = P;
        }
    }
    return maxP;
}

long long totalPengeluaran(List L, const string &prefix) {
    long long total = 0;
    for (address P = L.first; P != NULL; P = P->next)
        if (P->info.tipe == "Pengeluaran" && cocokPrefix(P->info.tanggal, prefix))
            total += P->info.nominal;
    return total;
}

long long totalPemasukan(List L, const string &prefix) {
    long long total = 0;
    for (address P = L.first; P != NULL; P = P->next)
        if (P->info.tipe == "Pemasukan" && cocokPrefix(P->info.tanggal, prefix))
            total += P->info.nominal;
    return total;
}

string formatRupiah(long long n) {
    string s = toStr(n < 0 ? -n : n);
    string r;
    int c = 0;
    for (int i = (int)s.size() - 1; i >= 0; i--) {
        r.insert(r.begin(), s[i]);
        if (++c % 3 == 0 && i > 0) r.insert(r.begin(), '.');
    }
    return (n < 0 ? "-Rp " : "Rp ") + r;
}

void printHeader() {
    cout << left << setw(5) << "ID" << setw(12) << "Tanggal" << setw(24) << "Keterangan"
         << setw(11) << "Kategori" << setw(12) << "Tipe" << right << setw(16) << "Nominal" << left << "\n";
    cout << string(80, '-') << "\n";
}

void printRow(const infotype &t) {
    string k = t.keterangan;
    if (k.size() > 22) k = k.substr(0, 21) + ".";
    cout << left << setw(5) << t.id << setw(12) << t.tanggal << setw(24) << k
         << setw(11) << t.kategori << setw(12) << t.tipe
         << right << setw(16) << formatRupiah(t.nominal) << left << "\n";
}

void printAll(List L) {
    if (isEmpty(L)) {
        cout << "Belum ada transaksi.\n";
        return;
    }
    printHeader();
    for (address P = L.first; P != NULL; P = P->next) printRow(P->info);
    cout << string(80, '-') << "\n";
    cout << "Total transaksi: " << countNodes(L) << "\n";
}

void printRange(List L, const string &awal, const string &akhir) {
    int n = 0;
    long long in = 0, out = 0;
    for (address P = L.first; P != NULL; P = P->next) {
        if (P->info.tanggal >= awal && P->info.tanggal <= akhir) {
            if (n == 0) printHeader();
            printRow(P->info);
            n++;
            if (P->info.tipe == "Pemasukan") in += P->info.nominal;
            else out += P->info.nominal;
        }
    }
    if (n == 0) {
        cout << "Tidak ada transaksi pada rentang tersebut.\n";
        return;
    }
    cout << string(80, '-') << "\n";
    cout << "Jumlah transaksi : " << n << "\n";
    cout << "Total pemasukan  : " << formatRupiah(in) << "\n";
    cout << "Total pengeluaran: " << formatRupiah(out) << "\n";
}

// ---------- fungsi rentang periode ----------
static void isiTm(const string &tgl, int tambahHari, tm &t) {
    t = tm();
    t.tm_year = atoi(tgl.substr(0, 4).c_str()) - 1900;
    t.tm_mon = atoi(tgl.substr(5, 2).c_str()) - 1;
    t.tm_mday = atoi(tgl.substr(8, 2).c_str()) + tambahHari;
    t.tm_hour = 12;               // jam 12 siang supaya aman dari masalah DST
    mktime(&t);                   // normalisasi tanggal + isi tm_wday
}

string geserHari(const string &tgl, int n) {
    tm t;
    isiTm(tgl, n, t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
    return string(buf);
}

static int hariDalamMinggu(const string &tgl) {   // 0 = Senin ... 6 = Minggu
    tm t;
    isiTm(tgl, 0, t);
    return (t.tm_wday + 6) % 7;
}

void rentangPeriode(int periode, const string &tgl, string &awal, string &akhir) {
    if (periode == 0) {
        awal = akhir = tgl;
    } else if (periode == 1) {
        awal = geserHari(tgl, -hariDalamMinggu(tgl));
        akhir = geserHari(awal, 6);
    } else if (periode == 2) {
        awal = tgl.substr(0, 7) + "-01";
        string bulanDepan = geserHari(awal, 31).substr(0, 7) + "-01";
        akhir = geserHari(bulanDepan, -1);
    } else {
        awal = tgl.substr(0, 4) + "-01-01";
        akhir = tgl.substr(0, 4) + "-12-31";
    }
}

string labelRentang(const string &awal, const string &akhir) {
    return awal == akhir ? awal : awal + " s/d " + akhir;
}

long long totalPengeluaranRange(List L, const string &awal, const string &akhir) {
    long long total = 0;
    for (address P = L.first; P != NULL; P = P->next)
        if (P->info.tipe == "Pengeluaran" && P->info.tanggal >= awal && P->info.tanggal <= akhir)
            total += P->info.nominal;
    return total;
}

long long totalPemasukanRange(List L, const string &awal, const string &akhir) {
    long long total = 0;
    for (address P = L.first; P != NULL; P = P->next)
        if (P->info.tipe == "Pemasukan" && P->info.tanggal >= awal && P->info.tanggal <= akhir)
            total += P->info.nominal;
    return total;
}

address findMaxPengeluaranRange(List L, const string &awal, const string &akhir) {
    address maxP = NULL;
    for (address P = L.first; P != NULL; P = P->next)
        if (P->info.tipe == "Pengeluaran" && P->info.tanggal >= awal && P->info.tanggal <= akhir)
            if (maxP == NULL || P->info.nominal > maxP->info.nominal) maxP = P;
    return maxP;
}

void printRingkasanKategori(List L, const string &awal, const string &akhir) {
    long long total = totalPengeluaranRange(L, awal, akhir);
    cout << "\nPengeluaran per kategori:\n";
    if (total == 0) {
        cout << "  (belum ada pengeluaran)\n";
        return;
    }
    for (int i = 0; i < JML_KATEGORI; i++) {
        long long sum = 0;
        for (address P = L.first; P != NULL; P = P->next)
            if (P->info.tipe == "Pengeluaran" && P->info.kategori == KATEGORI[i] &&
                P->info.tanggal >= awal && P->info.tanggal <= akhir)
                sum += P->info.nominal;
        int pct = (int)(sum * 100 / total);
        cout << "  " << left << setw(10) << KATEGORI[i] << right << setw(16) << formatRupiah(sum)
             << " " << setw(3) << pct << "% " << string(pct / 5, '#') << left << "\n";
    }
}

string toStr(long long n) {
    ostringstream o;
    o << n;
    return o.str();
}

bool parseLL(const string &s, long long &out) {
    if (s.empty() || s.size() > 18) return false;
    size_t i = 0;
    bool neg = false;
    if (s[0] == '-') {
        neg = true;
        i = 1;
        if (s.size() == 1) return false;
    }
    long long v = 0;
    for (; i < s.size(); i++) {
        if (!isdigit((unsigned char)s[i])) return false;
        v = v * 10 + (s[i] - '0');
    }
    out = neg ? -v : v;
    return true;
}

bool isValidDate(const string &s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)s[i])) return false;
    }
    int y = atoi(s.substr(0, 4).c_str()), m = atoi(s.substr(5, 2).c_str()), d = atoi(s.substr(8, 2).c_str());
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1) return false;
    int hari[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool kabisat = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    int maks = hari[m - 1] + ((m == 2 && kabisat) ? 1 : 0);
    return d <= maks;
}

string todayString() {
    time_t t = time(NULL);
    tm *lt = localtime(&t);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", lt);
    return string(buf);
}

bool saveList(List L, Pengaturan S, const string &file) {
    ofstream f(file.c_str());
    if (!f) return false;
    f << S.periodeUtama;
    for (int i = 0; i < JML_PERIODE; i++) f << " " << S.anggaran[i];
    f << " " << L.nextId << "\n";
    for (address P = L.first; P != NULL; P = P->next) {
        const infotype &t = P->info;
        f << t.id << "|" << t.tanggal << "|" << t.keterangan << "|" << t.kategori << "|"
          << t.nominal << "|" << t.tipe << "\n";
    }
    return true;
}

bool loadList(List &L, Pengaturan &S, const string &file) {
    ifstream f(file.c_str());
    if (!f) return false;
    string line;
    if (!getline(f, line)) return false;
    {
        vector<long long> n;
        stringstream ss(line);
        long long v;
        while (ss >> v) n.push_back(v);
        if (n.size() == 6) {                      // format baru
            S.periodeUtama = (int)n[0];
            for (int i = 0; i < JML_PERIODE; i++) S.anggaran[i] = n[i + 1];
            L.nextId = (int)n[5];
        } else if (n.size() == 5) {               // format lama (anggaran bulanan saja)
            S.periodeUtama = 2;
            S.anggaran[0] = n[1];
            S.anggaran[1] = 0;
            S.anggaran[2] = n[0] > 0 ? n[0] : n[2];
            S.anggaran[3] = n[3];
            L.nextId = (int)n[4];
        } else return false;
        if (S.periodeUtama < 0 || S.periodeUtama >= JML_PERIODE) S.periodeUtama = 2;
    }
    address tail = NULL;
    while (getline(f, line)) {
        if (line.empty()) continue;
        vector<string> parts;
        stringstream ss(line);
        string item;
        while (getline(ss, item, '|')) parts.push_back(item);
        if (parts.size() != 6) continue;
        infotype t;
        long long idv, nom;
        if (!parseLL(parts[0], idv) || !parseLL(parts[4], nom)) continue;
        t.id = (int)idv;
        t.tanggal = parts[1];
        t.keterangan = parts[2];
        t.kategori = parts[3];
        t.nominal = nom;
        t.tipe = parts[5];
        address P = alokasi(t);
        if (tail == NULL) L.first = P;
        else tail->next = P;
        tail = P;
    }
    return true;
}
