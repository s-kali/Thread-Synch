#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

pthread_mutex_t lock;

struct node *root = NULL;

typedef struct node// Dugum
{
	int data;
	struct node *left;
	struct node *right;
}node;

typedef struct buffer// input file isimlerinin tutuldugu buffer
{
    char intfilename[15];
}buffer;

struct node *init(node *root,int value)// Agaca veri girisi
{
	if(root == NULL)
	{
		node *root = (node*)malloc(sizeof(node));
		root->left = NULL;
		root->right = NULL;
		root->data = value;
		return root;
	}
	
	if(root->data == value)
	{
		return root;
	}
	
	else if(root->data > value)
	{	
		root->left = init(root->left,value);
	}
	
	else if(root->data < value)
	{
		root->right = init(root->right,value);
	}
		
	return root;
	
}
/* agacin L N R biciminde görüntülenmesi icin kullanilir.*/
void sahbatur2(node *root)
{
	if(root==NULL)
		return;
	
	sahbatur2(root->left);
	printf("%d ",root->data);
	sahbatur2(root->right);
}
/*Agacin parametre olarak verilen file'a yazilmasi icin kullanilir.*/
void sahbaturfile(node *root, FILE *f)
{
	if(root==NULL)
		return;
	
	sahbaturfile(root->left,f);
	fprintf(f,"%d\n",root->data);
	sahbaturfile(root->right,f);
}
/*minimum eleman bulan fonksiyon, yeni veri giriþinde silinecek min eleman bulunmasý için kullanýlýr.
Ayni zamanda agactan veri silerken agac yapisi bozulmamasi icin gereklidir.*/
int min(node *root)
{
	while(root->left!=NULL)
	{
		root = root->left;
	}
	return root->data;
}
/*Agactan veri silerken agac yapisi bozulmamasi icin gereklidir.*/
int max(node *root)
{
	while(root->right!=NULL)
	{
		root = root->right;
	}
	return root->data;
}
/* input dosyasyndaki veriler agaçta varsa a?aca eklenmedi?inden
a?açta k dan az veri bulunur. Bu kontrolün sa?lanmasy için find kullanylyr. */
int find(node *root,int value) 
{
	if(root == NULL)
		return -1;
	if(root->data == value)
		return 1;
	if(find(root->right,value)==1)
		return 1;
	if(find(root->left,value)==1)
		return 1;
	return -1;
}
/*Agacta maksimum k kadar deger olacagindan baþta aðaca -k,0 araliginda deger girilir
Daha sonra bu degerlerin yeni gelen degerle degistirilmesi icin del kullanilir.*/
struct node *del(node *root, int x)
{
	if(root==NULL)
	{
		return NULL;
	}
	
	if(root->data == x)
	{
		// Sagi solu bossa degeri sil
		if(root->left == NULL && root->right == NULL)
			return NULL;
		
		// Solu bossa sagdan minimumu sil
		if(root->right!=NULL)
		{
			root->data = min(root->right);
			root->right = del(root->right, min(root->right));
			return root;
		}
		// Sagi bossa soldan maxi sil
		else if(root->left!=NULL)
		{
			root->data = max(root->left);
			root->left = del(root->left, max(root->left));
			return root;
		}	
	}
	if(root->data < x)
	{
		root->right = del(root->right,x);
		return root;
	}
	else if(root->data > x)
	{
		root->left = del(root->left,x);
		return root;
	}	
}

void *worker(void *arg)// Isci threadlerin fonksiyonu
{
    buffer *filename = (buffer *)arg;// Mevcut threadin kullandigi filename'in tutuldugu buffer.
    FILE *fptr = fopen(filename->intfilename,"r");// Mevcut threadin kullandigi file acilmasi.
    char number[15];// File icerisindeki sayi char olarak alýnmakta.

   int inumber;// Char olarak alinan sayi inumber integer sayisina donusturulur.
    
    while(fscanf(fptr,"%s",number)==1)// Dosyanin sonuna gidene kadar döngü.
    { 
        pthread_mutex_lock(&lock); // Threadlerin ayni anda kritik bolgeye erisimini engelleyen mutex.
 		inumber = atoi(number);
        /* crit section */
        if(min(root) < inumber) // Agacin minimum elemani yeni gelen sayidan kücükse,
		{
			if(find(root,inumber)!=1)// Ve sayi agacta bulunmuyorsa,
			{
				root = del(root,min(root));// Minimum olan sayi silinip,
				root = init(root,inumber);// Yeni gelen sayi agaca eklenir.
			}
		}
        /* crit ended*/
        pthread_mutex_unlock(&lock);// Kritik bolge bitti, mutex acildi.

    }
    fclose(fptr); // File kapatildi.
            pthread_exit(NULL);// Mevcut threadin islemi bittigi icin sonlandirildi.
}
/* Program ?uanda ayny sayyyy iki kez eklemiyor, kaldyrylacaksa init fonksiyonundan ko?ul silinmelidir ve worker fonksiyonundan ko?ul kaldyrylmalydyr. */

int main(int argc, char *argv[]) // char argv ile K N infile1...infileN outfile verileri alinir.
{
	int i,j;
	
    int K = atoi(argv[1]);// char ile alinan K ve N degeri atoi ile integere cevirilir.
    int N = atoi(argv[2]); 
	   pthread_t worker_id[N];// N kadar worker thread tanimlanir.
   
   char *outfilename = argv[N+3];// Agacin yazdirilacagi dosya ismi.
    
    buffer list[N];// Her thread basina dosya ismi tutacak buffer.
    
    for(i = 0; i<N; i++)// Argv ile alinan infile isimleri buffere atilir.
	{
    	strcpy(list[i].intfilename,argv[i+3]);
	}
    
    pthread_mutex_init(&lock, NULL);// Mutex tanimlanmasi.

	root = init(root,(i*(-1)));
    
	for(i = 0; i<K; i++) // Agaca -k,0 degerlerinin atanmasi.
		root = init(root,(i*(-1)));

	for(i = 0; i<N; i++)/*N kadar worker thread tanimlanmasi ve fonksiyona parametre olarak
	infile isimlerinin atanmasi.*/
	{
		pthread_create(&worker_id[i],NULL,worker,(void *)&list[i]);	
	}
    	for(j = 0 ; j<N; j++)/* N kadar worker threadin calistirilmasi. */
	{
		pthread_join(worker_id[j],NULL);
	}
	/*Mutex islemini bitirdigi icin kaldirilir.*/
    pthread_mutex_destroy(&lock);
	
	/*outfile dosyasinin olusturulmasi ve yazilip main threadin sonlanmasi.*/
	FILE *f1 = fopen(outfilename,"w+");

	sahbatur2(root);
	sahbaturfile(root,f1);
	
	pthread_exit(NULL);
	return 0;
}
