# Thread-Synch
Thread synchronization in C 

Main thread tarafından N adet worker thread oluşturan, K adet düğüme sahip global değişken olarak tanımlanmış
(threadler arası paylaşılabilir) bir arama ağacı içeren program.

Agaç, input dosyaları içerisinde şu ana kadar olan en büyük K adet sayıyı tutar.
Ağaca veri eklenmesi N adet worker thread ile sağlanır, bu threadler yine N sayıda input dosyasının her birine atanır.
Her input dosyasına 1 adet worker thread karşılık gelir ve bu threadlerin senkronizasyonu *mutex* kilitleri ile sağlanır.

Sonuç olarak N sayıdaki input dosyasında bulunan K sayıdaki en büyük değerler outfile isimli dosyaya yazdırılır.

Program, part1 K N infile1.txt ...  infileN.txt outfile.txt , gibi parametre alarak çalıştırılır.
