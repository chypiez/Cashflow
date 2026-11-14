#ifndef TRANSAKSI_H
#define TRANSAKSI_H

#include <string>
using namespace std;

const int JML_KATEGORI = 6;
extern const string KATEGORI[JML_KATEGORI];

struct Transaksi {
    int id;            
    string tanggal;     
    string keterangan;  
    string kategori;     
    long long nominal;   
    string tipe;         
};
typedef Transaksi infotype;

typedef struct Node *address;   
struct Node {
    infotype info;
    address next;               
};

struct List {
    address first;             
    int nextId;               
};

struct Pengaturan {
    long long anggaran;         
    long long batasHarian;      
    long long batasBulanan;
    long long batasTahunan;
};

void createList(List &L);
bool isEmpty(List L);
address alokasi(infotype x);
void dealokasi(address &P);             

void insertFirst(List &L, address P);
void insertLast(List &L, address P);
void insertAfter(address Prec, address P);

void deleteFirst(List &L, address &P);
void deleteLast(List &L, address &P);
void deleteAfter(address Prec, address &P);

void clearList(List &L);                  
int countNodes(List L);

address findByID(List L, int id);                     
int searchKeyword(List L, const string &kw);           
address findMaxPengeluaran(List L, const string &prefix = "");
long long totalPengeluaran(List L, const string &prefix = "");
long long totalPemasukan(List L, const string &prefix = "");

string formatRupiah(long long n);
void printHeader();
void printRow(const infotype &t);
void printAll(List L);
void printRange(List L, const string &awal, const string &akhir);
void printRingkasanKategori(List L, const string &prefix);

string toStr(long long n);                
bool parseLL(const string &s, long long &out); 
bool isValidDate(const string &s);          
string todayString();

bool saveList(List L, Pengaturan S, const string &file);
bool loadList(List &L, Pengaturan &S, const string &file);

#endif