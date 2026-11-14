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

void printRingkasanKategori(List L, const string &prefix) {
    long long total = totalPengeluaran(L, prefix);
    cout << "\nPengeluaran per kategori:\n";
    if (total == 0) {
        cout << "  (belum ada pengeluaran)\n";
        return;
    }
    for (int i = 0; i < JML_KATEGORI; i++) {
        long long sum = 0;
        for (address P = L.first; P != NULL; P = P->next)
            if (P->info.tipe == "Pengeluaran" && P->info.kategori == KATEGORI[i] &&
                cocokPrefix(P->info.tanggal, prefix))
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
    f << S.anggaran << " " << S.batasHarian << " " << S.batasBulanan << " "
      << S.batasTahunan << " " << L.nextId << "\n";
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
        stringstream ss(line);
        ss >> S.anggaran >> S.batasHarian >> S.batasBulanan >> S.batasTahunan >> L.nextId;
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