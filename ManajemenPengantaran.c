#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_PESANAN 10
#define MAX_PROSES_PESANAN 10
#define MAX_COMPLETE_PESANAN 10
#define MAX_PENGEMUDI 4
#define STOK_SOSIS 500
#define STOK_SUSU 1000

//FUNGSI MENERIMA DAN MENGELOLA PESANAN
struct Pesanan {
    int id;
    char nama_barang[20];
    int jumlah_barang;
    char alamat_tujuan[50];
    char status[20]; 
    char nama_pengemudi[20];
};
//struct pesanan awal
struct Pesanan pesanan_queue[MAX_PESANAN];
int jumlah_pesanan = 0;  //ini jumlah yg dipesen
int counter_id = 0; // ini id pesanan (1)

//struct pesanan yang udah diproses
struct Pesanan proses_pesanan_queue[MAX_PROSES_PESANAN];
int jumlah_proses_pesanan = 0;

//struct pesanan yang udah selesai
struct Pesanan complete_pesanan_queue[MAX_COMPLETE_PESANAN];
int jumlah_complete_pesanan = 0;

int stok_sosis; 
int stok_susu;

void update_stok(struct Pesanan *pesanan) {
    if (strcmp(pesanan->nama_barang, "Sosis") == 0) {
        printf("Sisa stok sosis yang tersedia: %d\n", stok_sosis -= pesanan->jumlah_barang);
    } else if (strcmp(pesanan->nama_barang, "Susu") == 0) {
        printf("Sisa stok sosis yang tersedia: %d\n", stok_susu -= pesanan->jumlah_barang);
    }
}

void tambah_pesanan() {
    if (jumlah_pesanan == MAX_PESANAN) {
        printf("Antrian pesanan penuh.\n");
        return;
    }

      // Increment counter_id untuk setiap pesanan baru
    counter_id++;

    // Pemesan ga lebih dari 5(biar Queuenya dibatasi)
    if (counter_id > MAX_PESANAN) {
        counter_id = 1; // Reset ke 1 jika sudah mencapai batas maksimum
    }

    int id_pesanan = counter_id;
    struct Pesanan pesanan_baru;
    pesanan_baru.id = id_pesanan;

    int pilihan_barang;
    printf("\nPilih barang yang akan dipesan:\n");
    printf("1. Sosis\n");
    printf("2. Susu\n");
    printf("Masukkan pilihan: ");
    scanf("%d", &pilihan_barang);

    switch (pilihan_barang) {
        case 1:
            strcpy(pesanan_baru.nama_barang, "Sosis"); //menyalin nama barang ke dalam struktur Pesanan
            printf("\nMasukkan jumlah sosis yang akan dipesan (stok tersedia %d): ", stok_sosis);
            scanf("%d", &pesanan_baru.jumlah_barang);
            if (pesanan_baru.jumlah_barang > stok_sosis) {
                printf("Stok sosis tidak tersedia.\n");
                return;
            }
            break;
        case 2:
            strcpy(pesanan_baru.nama_barang, "Susu");
            printf("\nMasukkan jumlah susu yang akan dipesan (stok tersedia %d): ", stok_susu);
            scanf("%d", &pesanan_baru.jumlah_barang);
            if (pesanan_baru.jumlah_barang > stok_susu) {
                printf("Stok susu tidak tersedia.\n");
                return;
            }
            break;
        default:
            printf("Pilihan barang tidak valid.\n");
            return;
    }

//alamat buat graph 
    int pilihan_alamat;
    printf("\nPilih kota tujuan:\n");
    printf("1. Bogor\n");
    printf("2. Depok\n");
    printf("3. Tangerang\n");
    printf("4. Bekasi\n");
    printf("5. Cianjur\n");
    printf("Masukkan pilihan: ");
    scanf("%d", &pilihan_alamat);

    switch (pilihan_alamat) {
        case 1:
            strcpy(pesanan_baru.alamat_tujuan, "Bogor");
            break;
        case 2:
            strcpy(pesanan_baru.alamat_tujuan, "Depok");
            break;
        case 3:
            strcpy(pesanan_baru.alamat_tujuan, "Tangerang");
            break;
        case 4:
            strcpy(pesanan_baru.alamat_tujuan, "Bekasi");
            break;
        case 5:
            strcpy(pesanan_baru.alamat_tujuan, "Cianjur");
            break;
        default:
            printf("Pilihan alamat tujuan tidak valid.\n");
            return;
    }

    strcpy(pesanan_baru.status, "Menunggu Pengiriman");

    pesanan_queue[jumlah_pesanan++] = pesanan_baru;
    printf("Pesanan %d untuk %s sebanyak %d ke %s ditambahkan.\n", id_pesanan, pesanan_baru.nama_barang, pesanan_baru.jumlah_barang, pesanan_baru.alamat_tujuan);
}

//Buat nampilin Queue
void tampilkan_antrian() {
    if (jumlah_pesanan == 0) {
        printf("Tidak ada pesanan dalam antrian.\n");
        return;
    }

    printf("\nAntrian Pesanan di sistem:\n");
    for (int i = 0; i < jumlah_pesanan; i++) {
        // cek order ID ada di proses_pesanan_queue atau tidak
        bool is_processed = false;
        for (int j = 0; j < jumlah_proses_pesanan; j++) {
            if (pesanan_queue[i].id == proses_pesanan_queue[j].id) {
                is_processed = true;
                break;
            }
        }
        // display order selain yang udah diproses
        if (!is_processed) {
            printf("Pesanan %d: %s sebanyak %d ke %s\n", pesanan_queue[i].id, pesanan_queue[i].nama_barang, pesanan_queue[i].jumlah_barang, pesanan_queue[i].alamat_tujuan);
        }
    }
}

// FUNGSI MENGELOLA RUTE PENGIRIMAN
// Struct untuk menyimpan edge dengan node tujuan dan bobot
typedef struct {
    int to;
    int weight;
} Edge;

// Struct untuk priority queue dengan komparator khusus
typedef struct {
    int node;
    int distance;
} PQItem;

// Komparator untuk menentukan mana yang harus diprioritaskan 
bool compare(PQItem a, PQItem b) {
    return a.distance > b.distance;
}

// Priority queue dengan operasi push dan pop
typedef struct {
    PQItem *items;
    int size;
    int capacity;
} PriorityQueue;

PriorityQueue* create_priority_queue(int capacity) {
    PriorityQueue *pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    pq->items = (PQItem*)malloc(sizeof(PQItem) * capacity);
    pq->size = 0; // belum ada item yang ditambahkan
    pq->capacity = capacity; //menyimpan kapasitas maksimum pq
    return pq;
}

// Menambahkan item prioritas terendah ke priority queue
void pq_push(PriorityQueue *pq, PQItem item) {
    int i = pq->size++; // indeks untuk item baru yang ditambahkan
    // membandingkan item baru dengan parentnya (menjaga min-heap)
    // jika bobot parent lebih besar, tukar posisi, item baru ke depan
    while (i > 0 && compare(pq->items[(i - 1) / 2], item)) { 
        pq->items[i] = pq->items[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    pq->items[i] = item;
}

// Mengeluarkan item prioritas tertinggi dari priority queue
PQItem pq_pop(PriorityQueue *pq) {
    PQItem result = pq->items[0]; // mengambil item terdepan
    PQItem lastItem = pq->items[--pq->size]; // mengambil item terakhir melalui indeksnya
    int i = 0;
    while (2 * i + 1 < pq->size) { // memeriksa apakah ada anak kiri untuk node pada indeks i
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int minIndex = (right < pq->size && compare(pq->items[left], pq->items[right])) ? right : left;
        if (!compare(lastItem, pq->items[minIndex])) break;
        pq->items[i] = pq->items[minIndex];
        i = minIndex;
    }
    pq->items[i] = lastItem;
    return result;
    
}

// Fungsi untuk mencari jalur terpendek menggunakan Dijkstra
typedef struct {
    int *distances; // Menyimpan jarak terpendek
    int *previous; // Menyimpan node sebelumnya
} DijkstraResult;

DijkstraResult dijkstra(Edge **graph, int n, int start, int end) {
    int *distances = (int*)malloc(sizeof(int) * n); 
    int *previous = (int*)malloc(sizeof(int) * n); 
    for (int i = 0; i < n; i++) {
        distances[i] = INT_MAX;
        previous[i] = -1;
    }
    distances[start] = 0;

    PriorityQueue *pq = create_priority_queue(n * 2);
    pq_push(pq, (PQItem){start, 0});

    while (pq->size > 0) {
        PQItem current = pq_pop(pq);
        int currentNode = current.node;
        int currentDistance = current.distance;

        // Proses semua tetangga dari node saat ini
        Edge *edge = graph[currentNode];
        while (edge->to != -1) { // Menandakan akhir edge
            int neighbor = edge->to;
            int newDistance = currentDistance + edge->weight;

            if (newDistance < distances[neighbor]) {
                distances[neighbor] = newDistance;
                previous[neighbor] = currentNode; // Menyimpan node sebelumnya
                pq_push(pq, (PQItem){neighbor, newDistance});
            }

            edge++;
        }
    }

    free(pq->items);
    free(pq);
    return (DijkstraResult){distances, previous};
}

// Fungsi untuk mendapatkan rute dari node sebelumnya
int* get_path(int *previous, int start, int end, int *path_length) {
    int *path = (int*)malloc(sizeof(int) * 100); // Ukuran awal yang cukup besar
    int path_count = 0;
    int currentNode = end;

    while (currentNode != -1) {
        path[path_count++] = currentNode;
        currentNode = previous[currentNode];
    }

    // Balik urutan
    for (int i = 0; i < path_count / 2; i++) {
        int temp = path[i];
        path[i] = path[path_count - 1 - i];
        path[path_count - 1 - i] = temp;
    }

    *path_length = path_count;
    return path;
}

/*hasil rute yang dari fungsi sebelumnya disimpan ke linked list
 kemudian tiap barang bergerak per lokasi, print notifikasi status dan lokasi barang
*/

/* Menyimpan hasil djiksta ke dalam linked list*/
// Struct untuk node dalam linked list
struct ListNode {
    int data;
    char lokasi[20]; // nama lokasi
    struct ListNode* next;
};

// Fungsi untuk menambahkan node baru ke linked list
void tambahNode(struct ListNode** head_node, int new_data, char* new_lokasi) {
    struct ListNode* new_node = (struct ListNode*)malloc(sizeof(struct ListNode));
    new_node->data = new_data;
    strcpy(new_node->lokasi, new_lokasi); // untuk masukin input nama lokasi ke lokasi di linkedlist
    new_node->next = NULL;

    if (*head_node == NULL) {
        *head_node = new_node;
        return;
    }

    struct ListNode* last_node = *head_node;
    while (last_node->next != NULL) {
        last_node = last_node->next;
    }

    last_node->next = new_node;
}

struct ListNode* list_rute = NULL; //buat listnya globally
// Fungsi untuk menyimpan hasil rute dari array ke linked list
struct ListNode* masukkanRute(int* rute, char (*lokasi)[20], int jarak_rute) {
    //iterasi untuk melihat semua indeks array hasil djikstra di awal
    for (int i = 0; i < jarak_rute; i++) {
        tambahNode(&list_rute, rute[i], lokasi[rute[i]]); //masukin ke linked list
    }
    return list_rute;
}

void menu();
void mulai_pengantaran();

/* Jadwalin pengiriman: kasih pengemudi dan rute, opsi proses pengantaran*/

// Fungsi untuk men-generate jadwal; driver dan rute
void jadwalkan_pengantaran(){
    char lokasi[][20] = {"Jakarta", "Bogor", "Depok", "Tangerang", "Bekasi", "Cianjur"};
    // Contoh graf dengan 6 node (0-5)
    int n = 6;
    Edge **graph = (Edge**)malloc(sizeof(Edge) * n);

    for (int i = 0; i < n; i++) {
        graph[i] = (Edge*)malloc(sizeof(Edge) * 5); // Kapasitas awal
        memset(graph[i], -1, sizeof(Edge) * 5); // Menggunakan sentinel
    }

    // Tambah edge
    graph[0][0] = (Edge){1, 7};
    graph[0][1] = (Edge){2, 9};
    graph[0][2] = (Edge){5, 14};
    graph[1][0] = (Edge){2, 10};
    graph[1][1] = (Edge){3, 15};
    graph[2][0] = (Edge){3, 11};
    graph[2][1] = (Edge){5, 2};
    graph[3][0] = (Edge){4, 6};
    graph[5][0] = (Edge){4, 9};

    if (jumlah_pesanan == 0) {
        printf("Tidak ada pesanan untuk dijadwalkan pengantaran.\n");
        return;
    }
    int start = 0; // lokasi awal selalu Jakarta (index 0)
    
    // tampilin list order yang udah ada
    printf("\nPilih pesanan yang akan dijadwalkan pengantaran:\n");
    for (int i = 0; i < jumlah_pesanan; i++) {
        printf("%d. Pesanan %d: %s sebanyak %d ke %s\n", i + 1, pesanan_queue[i].id, pesanan_queue[i].nama_barang, pesanan_queue[i].jumlah_barang, pesanan_queue[i].alamat_tujuan);
    }

    // pilih order
    int pilihan_order;
    printf("\nMasukkan nomor pesanan: ");
    scanf("%d", &pilihan_order);
    if (pilihan_order < 1 || pilihan_order > jumlah_pesanan) {
        printf("Nomor pesanan tidak valid.\n");
        return;
    }

    int end = -1;
    for (int i = 0; i < n; i++) {
        if (strcmp(pesanan_queue[pilihan_order - 1].alamat_tujuan, lokasi[i]) == 0) {
            end = i;
            break;
        }
    }
    if (end == -1) {
        printf("Lokasi tujuan tidak valid.\n");
        return;
    }

    DijkstraResult result = dijkstra(graph, n, start, end);

    printf("\nJarak terpendek: %d\n", result.distances[end]);

    int path_length;
    int *path = get_path(result.previous, start, end, &path_length);

    // simpan rute terpendek ke linked list
    struct ListNode* list_rute = masukkanRute(path, lokasi, path_length);

    // cetak isi linked list
    struct ListNode* current_node = list_rute;
    printf("Rute terpendek: ");
    while (current_node != NULL) {
        printf("%s ", lokasi[current_node->data]);
        current_node = current_node->next;
    }
    printf("\n");


    int mulaipengantaran;
    printf("\nMulai Pengantaran?\n");
    printf("1. Ya\n");
    printf("2. Tidak. Kembali ke menu\n");
    scanf("%d", &mulaipengantaran);
    switch (mulaipengantaran) {
        case 1:
            mulai_pengantaran(list_rute, &pesanan_queue[pilihan_order - 1]);
            //hapus dari queue pesanan awal
            for (int i = pilihan_order - 1; i < jumlah_pesanan - 1; i++) {
                pesanan_queue[i] = pesanan_queue[i + 1];
            }
            jumlah_pesanan--;
            break;
        case 2:
            menu();
            break;
        default:
            printf("Pilihan tidak valid. Silakan coba lagi.\n");
            break;
    }
    // Bebaskan memori
    free(result.distances);
    free(result.previous);
    free(path); // Free the path memory
    for (int i = 0; i < n; i++) {
        free(graph[i]);
    }
    free(graph);

    // Bebaskan memori dari linked list
    struct ListNode* temp;
    while (list_rute != NULL) {
        temp = list_rute;
        list_rute = list_rute->next;
        free(temp);
    }
}

// Queue pengemudi
char pengemudi_queue[MAX_PENGEMUDI][20] = {"Dicky", "Ghani", "Syira", "Zara"};
int pengemudi_queue_front = 0;
int pengemudi_queue_back = MAX_PENGEMUDI - 1;
int pengemudi_count = MAX_PENGEMUDI;

// Fungsi assign pengemudi ke pengantaran
char* assign_pengemudi() {
    if (pengemudi_count == 0) {
        printf("Tidak ada pengemudi yang tersedia untuk melakukan pengantaran.\n");
        return NULL;
    }
    char* assigned_pengemudi = (char*)malloc(sizeof(char) * 20);
    strcpy(assigned_pengemudi, pengemudi_queue[pengemudi_queue_front]);
    pengemudi_queue_front = (pengemudi_queue_front + 1) % MAX_PENGEMUDI;
    pengemudi_count--;

    printf("\nPengemudi %s ditugaskan untuk melakukan pengantaran.\n", assigned_pengemudi);

    return assigned_pengemudi;
}

// Fungsi untuk mengembalikan pengemudi ke queue
void kembalikan_pengemudi(char* nama_pengemudi) {
    if (pengemudi_count == MAX_PENGEMUDI) {
        printf("Queue pengemudi penuh.\n");
        return;
    }
    strcpy(pengemudi_queue[pengemudi_queue_back], nama_pengemudi);
    pengemudi_queue_back = (pengemudi_queue_back + 1) % MAX_PENGEMUDI;
    pengemudi_count++;

}

//Fungsi untuk memulai pengantaran (mindahin dari queue pesanan awal ke queue pesanan yang udah diproses)
void mulai_pengantaran(struct ListNode *list_rute, struct Pesanan *pesanan){
    struct Pesanan *new_proses_pesanan = malloc(sizeof(struct Pesanan));
    if (new_proses_pesanan == NULL) {
        return;
    }
    new_proses_pesanan->id = pesanan->id;
    strcpy(new_proses_pesanan->nama_barang, pesanan->nama_barang);
    new_proses_pesanan->jumlah_barang = pesanan->jumlah_barang;
    strcpy(new_proses_pesanan->alamat_tujuan, pesanan->alamat_tujuan);
    strcpy(new_proses_pesanan->status, pesanan->status);

    char* assigned_pengemudi = assign_pengemudi();
    if (assigned_pengemudi != NULL) {
        strcpy(pesanan->nama_pengemudi, assigned_pengemudi);
        free(assigned_pengemudi);
    }
    proses_pesanan_queue[jumlah_proses_pesanan++] = *pesanan;

    free(new_proses_pesanan);

    list_rute = list_rute->next;
    printf("\nPesanan %d sedang dalam perjalanan menuju %s.\n", pesanan->id, list_rute->lokasi);
    strcpy(pesanan->status, "Sedang Dikirim");
    strcpy(pesanan->alamat_tujuan, list_rute->lokasi);
    printf("Status pengantaran: %s\n", pesanan->status);

}

//fungsi untuk update lokasi pengantaran
void proses_pengantaran(){
    // tampilin list order yang udah dimulai pengantarannya
    if (jumlah_proses_pesanan == 0) {
        printf("Tidak ada pesanan untuk diproses.\n");
        return;
    }
    printf("\nPilih pesanan yang akan diproses:\n");
    for (int i = 0; i < jumlah_proses_pesanan; i++) {
        printf("%d. Pesanan %d: %s sebanyak %d ke %s\n", i + 1, proses_pesanan_queue[i].id, proses_pesanan_queue[i].nama_barang, proses_pesanan_queue[i].jumlah_barang, proses_pesanan_queue[i].alamat_tujuan);
    }

    // pilih order
    int pilihan_proses_order;
    printf("\nMasukkan nomor pesanan: ");
    scanf("%d", &pilihan_proses_order);
    if (pilihan_proses_order < 1 || pilihan_proses_order > jumlah_proses_pesanan) {
        printf("Nomor pesanan tidak valid.\n");
        return;
    }

    struct Pesanan *pesanan = &proses_pesanan_queue[pilihan_proses_order - 1];   
     
    list_rute = list_rute->next;
    if(list_rute->next == NULL){
        printf("\nPesanan %d telah sampai di tujuan %s.\n", pesanan->id, list_rute->lokasi);
        strcpy(pesanan->status, "Pesanan Telah Sampai");
        strcpy(pesanan->alamat_tujuan, list_rute->lokasi);
        printf("Status pengantaran: %s\n", pesanan->status);
        printf("Stok barang diperbarui.\n");
        update_stok(pesanan);
        complete_pesanan_queue[jumlah_complete_pesanan++] = *pesanan;
        jumlah_proses_pesanan--;

        kembalikan_pengemudi(pesanan->nama_pengemudi);

    }else{
        printf("\nPesanan %d sedang transit di %s.\n", pesanan->id, list_rute->lokasi);
        strcpy(pesanan->status, "Sedang Transit");
        strcpy(pesanan->alamat_tujuan, list_rute->lokasi);
        printf("Status pengantaran: %s\n", pesanan->status);
    }
}    

void melacak_status() {
    if (jumlah_pesanan == 0 && jumlah_proses_pesanan == 0 && jumlah_complete_pesanan == 0){
        printf("Tidak ada pesanan dalam sistem.\n");
        menu();
    }
    int id;
    printf("Masukkan ID pesanan yang ingin dilacak: ");
    scanf("%d", &id);
    bool found = false;

    for (int i = 0; i < jumlah_proses_pesanan; i++) {
        if (proses_pesanan_queue[i].id == id) {
            printf("Pesanan %d: %s sebanyak %d sedang berada di %s. \nStatus: %s\n",
                   proses_pesanan_queue[i].id, proses_pesanan_queue[i].nama_barang,
                   proses_pesanan_queue[i].jumlah_barang, proses_pesanan_queue[i].alamat_tujuan,
                   proses_pesanan_queue[i].status);
            found = true;
            break;
        }else if(pesanan_queue[i].id == id){
            printf("Pesanan %d: %s sebanyak %d masih berada di Warehouse Jakarta. \nStatus: Belum dikirim\n",
                   proses_pesanan_queue[i].id, proses_pesanan_queue[i].nama_barang,
                   proses_pesanan_queue[i].jumlah_barang);
            found = true;       
        }
    }
    for (int i = 0; i < jumlah_complete_pesanan; i++) {
        if (complete_pesanan_queue[i].id == id) {
            printf("Pesanan %d: %s sebanyak %d telah sampai di %s. \nStatus: %s\n",
                   complete_pesanan_queue[i].id, complete_pesanan_queue[i].nama_barang,
                   complete_pesanan_queue[i].jumlah_barang, complete_pesanan_queue[i].alamat_tujuan,
                   complete_pesanan_queue[i].status);
            found = true;
            break;
        }
    }

    if (!found) {
        printf("Pesanan dengan ID %d tidak ditemukan.\n", id);
    }
}

void menu() {
    int pilihan;
    do {
        printf("\nSistem pengantaran Startup Frozen Food Jabodetabekjur\n");
        printf("\n===== Menu =====\n");
        printf("1. Tambah Pesanan\n");
        printf("2. Tampilkan Antrian\n");
        printf("3. Jadwalkan Pengantaran\n");
        printf("4. Proses Pengantaran\n");
        printf("5. Lacak Pengantaran\n");
        printf("0. Keluar\n");
        printf("Masukkan pilihan: ");
        if (scanf("%d", &pilihan) != 1) {
            printf("Input tidak valid. Silakan coba lagi.\n");
            // Membersihkan buffer stdin
            while (getchar() != '\n');
        }

        switch (pilihan) {
            case 1:
                tambah_pesanan();
                break;
            case 2:
                tampilkan_antrian();
                break;
            case 3:
                jadwalkan_pengantaran();
                break;
            case 4:
                proses_pengantaran();
                break;
            case 5:
                melacak_status();
                break;
            case 0:
                printf("Keluar dari program.\n");
                break;
            default:
                printf("Pilihan tidak valid. Silakan coba lagi.\n");
                break;
        }
    } while (pilihan != 0);
}

// Contoh penggunaan
int main() {
    stok_sosis = STOK_SOSIS;
    stok_susu = STOK_SUSU;
    menu();
    return 0;
}
