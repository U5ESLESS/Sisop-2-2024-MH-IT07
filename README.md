# Sisop-2-2024-MH-IT07


## Soal 3
Pak Heze adalah seorang admin yang baik. Beliau ingin membuat sebuah program admin yang dapat memantau para pengguna sistemnya. Bantulah Pak Heze untuk membuat program  tersebut!

a.  Nama program tersebut dengan nama admin.c
b. Program tersebut memiliki fitur menampilkan seluruh proses yang dilakukan oleh seorang user dengan menggunakan command:
  ./admin <user>
c. Program dapat memantau proses apa saja yang dilakukan oleh user. Fitur ini membuat program berjalan secara daemon dan berjalan terus menerus. Untuk menjalankan         fitur ini menggunakan command: 
   ./admin -m <user>
   Dan untuk mematikan fitur tersebut menggunakan: 
   ./admin -s <user>
   Program akan mencatat seluruh proses yang dijalankan oleh user di file <user>.log dengan format:
   [dd:mm:yyyy]-[hh:mm:ss]_pid-process_nama-process_GAGAL/JALAN
d. Program dapat menggagalkan proses yang dijalankan user setiap detik secara terus menerus dengan menjalankan: 
  ./admin -c user
  sehingga user tidak bisa menjalankan proses yang dia inginkan dengan baik. Fitur ini dapat dimatikan dengan command:
  ./admin -a user
e. Ketika proses yang dijalankan user digagalkan, program juga akan melog dan menset log tersebut sebagai GAGAL. Dan jika di log menggunakan fitur poin c, log akan
  ditulis dengan JALAN 
