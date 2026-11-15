/*
 * MARS KOLONISI YASAM DESTEK SIMULATORU
 * Algoritma ve Programlama - Metin tabanli C oyunu
 *
 * Oyuncu, Mars'taki bir arastirma istasyonunda tek basina kalan muhendisi
 * yonetir ve kurtarma ekibi gelene kadar 15 gun hayatta kalmaya calisir.
 *
 * Derleme   : gcc mars_kolonisi.c -o mars_kolonisi
 * Calistirma: ./mars_kolonisi   (Windows: mars_kolonisi.exe)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ---------- Sabit degerler ---------- */
#define MAKS_GUN           15   /* 15. gun tamamlaninca kurtarma ekibi gelir */
#define GUN_UZUNLUGU       3    /* 3 tur = 1 gun */
#define GUN_OKSIJEN_KAYBI  8    /* her yeni gun basinda oksijen kaybi */
#define GUN_SAGLIK_KAYBI   5    /* hasar > 70 ise yeni gun basinda saglik kaybi */
#define KRITIK_HASAR       70   /* bu degerin ustu agir hasar sayilir */

#define KESIF_ENERJI       15   /* G komutu enerji maliyeti */
#define KESIF_OKSIJEN      10   /* G komutu oksijen maliyeti */
#define YARALANMA_SAGLIK   10   /* kesifte yaralanirsa kaybedilen saglik */
#define URETIM_ENERJI      10   /* O komutu enerji maliyeti */
#define TAMIR_ENERJI       10   /* T komutu enerji maliyeti */
#define UYKU_OKSIJEN       2    /* U komutunda tuketilen oksijen */
#define SIFRE_HAK          3    /* H komutu deneme hakki */
#define SIFRE_ENERJI       5    /* yanlis sifre denemesinin enerji maliyeti */
#define SIFRE_CEZA         20   /* 3 deneme basarisizsa modul hasari artisi */


/* Degeri 0-100 araligina sinirlar (IF yapilariyla). */
int sinirla(int deger)
{
    if (deger < 0)
    {
        deger = 0;
    }
    if (deger > 100)
    {
        deger = 100;
    }
    return deger;
}

/*
 * Bir satir okur ve tek karakteri dondurur.
 * Satir bos ise veya birden fazla karakter iceriyorsa '?' dondurur.
 * Satirin tamami okunur; boylece tampon bellekte artik karakter kalmaz.
 */
char karakterOku(void)
{
    int c;
    char ilk = '?';
    int karakterSayisi = 0;

    c = getchar();
    if (c == EOF)
    {
        printf("\nGirdi sonlandi, program kapatiliyor.\n");
        exit(0);
    }

    while (c != '\n' && c != EOF)
    {
        if (c != ' ' && c != '\t' && c != '\r')
        {
            karakterSayisi++;
            if (karakterSayisi == 1)
            {
                ilk = (char)c;
            }
        }
        c = getchar();
    }

    if (karakterSayisi != 1)
    {
        return '?';
    }
    return ilk;
}

/*
 * Bir tam sayi okur ve satirin geri kalanini temizler.
 * Sayi okunamazsa -1 dondurur.
 */
int sayiOku(void)
{
    int sayi = 0;
    int sonuc;
    int c;

    sonuc = scanf("%d", &sayi);
    if (sonuc == EOF)
    {
        printf("\nGirdi sonlandi, program kapatiliyor.\n");
        exit(0);
    }

    do
    {
        c = getchar();
    } while (c != '\n' && c != EOF);

    if (sonuc != 1)
    {
        return -1;
    }
    return sayi;
}


int main(void)
{
    /* ---------- Durum degiskenleri ---------- */
    int saglik = 0, enerji = 0, oksijen = 0;
    int su = 0, yedekParca = 0, modulHasari = 0;
    int kilitMuhurlu = 0;
    int gun = 1, tur = 0;

    /* ---------- Zorluk seviyesine bagli degerler ---------- */
    int firtinaTurSayisi = 0;     /* kum firtinasinin tur sayisi */
    int firtinaHasari = 0;        /* firtinada tur basina temel hasar */
    int firtinaKorumaEsigi = 0;   /* firtinadan korunma icin temel esik */
    int yaralanmaEsigi = 0;       /* kesifte yaralanma esigi */
    int suEsigiYuksek = 0;        /* enerji ve oksijen iyiyken su bulma esigi */
    int suEsigiDusuk = 0;         /* enerji veya oksijen dusukken su bulma esigi */
    int parcaEsigi = 0;           /* yedek parca bulma icin eklenen aralik */

    /* ---------- Oyun akisi ---------- */
    char zorluk = '?', komut = '?';
    int gecerliSecim = 0;
    int oyunDurumu = 0;           /* 0: devam, 1: kaybetti, 2: kazandi, 3: cikis */
    int turHarcandi = 0;          /* komut tur harcadi mi? */
    int kurtarmaGeldi = 0;
    int puan = 0;

    /* ---------- Komutlarda kullanilan gecici degiskenler ---------- */
    int zar = 0, esik = 0;
    int bulunanSu = 0;                              /* G */
    int miktar = 0, artikSu = 0;                    /* O */
    int kullanilanSu = 0, uretilenOksijen = 0;
    int gerekliParca = 0, harcananParca = 0;        /* T */
    int onarim = 0, onarilan = 0;
    int enerjiArtis = 0, saglikArtis = 0;           /* U */
    int i = 0, carpan = 1, hasar = 0;               /* K */
    int hasarliTurSayisi = 0;
    int baslangicSaglik = 0, baslangicOksijen = 0, baslangicHasar = 0;
    int deneme = 0;                                 /* H */
    char sifre = 'A', girilen = '?';

    srand((unsigned int)time(NULL));

    printf("==============================================\n");
    printf("   MARS KOLONISI YASAM DESTEK SIMULATORU\n");
    printf("==============================================\n");
    printf("Mars'ta tek basina kaldiniz. Kurtarma ekibi %d gun sonra gelecek.\n", MAKS_GUN);
    printf("Her %d tur 1 gun sayilir. Hayatta kalin!\n\n", GUN_UZUNLUGU);

    /* ---------- Zorluk seviyesi secimi (SWITCH-CASE) ---------- */
    do
    {
        printf("Zorluk seviyesi secin (K: Kolay, N: Normal, Z: Zor): ");
        zorluk = karakterOku();
        gecerliSecim = 1;

        switch (zorluk)
        {
            case 'k':
            case 'K':
                printf("Zorluk: KOLAY\n");
                saglik = 100; enerji = 100; oksijen = 100;
                su = 60; yedekParca = 4; modulHasari = 10;
                firtinaTurSayisi = 3; firtinaHasari = 5; firtinaKorumaEsigi = 40;
                yaralanmaEsigi = 5;
                suEsigiYuksek = 65; suEsigiDusuk = 40; parcaEsigi = 25;
                break;
            case 'n':
            case 'N':
                printf("Zorluk: NORMAL\n");
                saglik = 100; enerji = 90; oksijen = 90;
                su = 50; yedekParca = 3; modulHasari = 20;
                firtinaTurSayisi = 5; firtinaHasari = 8; firtinaKorumaEsigi = 30;
                yaralanmaEsigi = 10;
                suEsigiYuksek = 55; suEsigiDusuk = 30; parcaEsigi = 20;
                break;
            case 'z':
            case 'Z':
                printf("Zorluk: ZOR\n");
                saglik = 90; enerji = 80; oksijen = 80;
                su = 45; yedekParca = 2; modulHasari = 30;
                firtinaTurSayisi = 7; firtinaHasari = 12; firtinaKorumaEsigi = 20;
                yaralanmaEsigi = 15;
                suEsigiYuksek = 45; suEsigiDusuk = 25; parcaEsigi = 15;
                break;
            default:
                printf("Gecersiz secim! Lutfen K, N veya Z girin.\n");
                gecerliSecim = 0;
                break;
        }
    } while (gecerliSecim == 0);

    printf("\nKomutlar: G: Kesif | O: Oksijen uret | T: Tamir | D: Durum raporu\n");
    printf("          U: Uyu/dinlen | K: Kum firtinasi | H: Hava kilidi sifresi | Q: Cikis\n");

    /* ---------- Ana oyun dongusu (DO-WHILE) ---------- */
    do
    {
        turHarcandi = 0;
        kurtarmaGeldi = 0;

        printf("\n[Gun %d/%d | Tur %d] Saglik:%d Enerji:%d Oksijen:%d Su:%d Parca:%d Hasar:%d Kilit:",
               gun, MAKS_GUN, tur, saglik, enerji, oksijen, su, yedekParca, modulHasari);
        if (kilitMuhurlu == 1)
        {
            printf("MUHURLU\n");
        }
        else
        {
            printf("ACIK\n");
        }
        printf("Komut (G/O/T/D/U/K/H/Q): ");
        komut = karakterOku();

        /* ---------- Komut yonetimi (SWITCH-CASE, fall-through) ---------- */
        switch (komut)
        {
            /* ===== G: Kesif gorevi ===== */
            case 'g':
            case 'G':
                if (enerji < KESIF_ENERJI)
                {
                    printf("Yetersiz enerji! Kesif icin en az %d enerji gerekir. Komut iptal edildi.\n",
                           KESIF_ENERJI);
                }
                else
                {
                    turHarcandi = 1;
                    enerji -= KESIF_ENERJI;
                    oksijen -= KESIF_OKSIJEN;
                    printf("Kesif gorevine cikildi: -%d enerji, -%d oksijen.\n",
                           KESIF_ENERJI, KESIF_OKSIJEN);

                    /* Yaralanma kontrolu: (zar < esik || modulHasari > 70) */
                    zar = rand() % 100;
                    printf("Yaralanma kontrolu -> Zar: %d / Esik: %d -> ", zar, yaralanmaEsigi);
                    if (zar < yaralanmaEsigi || modulHasari > KRITIK_HASAR)
                    {
                        saglik -= YARALANMA_SAGLIK;
                        printf("YARALANDINIZ (-%d saglik)", YARALANMA_SAGLIK);
                        if (modulHasari > KRITIK_HASAR)
                        {
                            printf(" [modul hasari %d'in ustunde]", KRITIK_HASAR);
                        }
                        printf("\n");
                    }
                    else
                    {
                        printf("GUVENLI\n");
                    }

                    /* Kaynak arama: esik, enerji ve oksijen durumuna gore belirlenir */
                    if (enerji > 40 && oksijen > 30)
                    {
                        esik = suEsigiYuksek;
                    }
                    else
                    {
                        esik = suEsigiDusuk;
                    }
                    zar = rand() % 100;
                    printf("Kaynak arama -> Zar: %d / Su esigi: %d / Parca esigi: %d -> ",
                           zar, esik, esik + parcaEsigi);
                    if (zar < esik)
                    {
                        bulunanSu = 10 + zar % 8;  /* 10-17 birim su */
                        su += bulunanSu;
                        printf("SU BULUNDU (+%d su)\n", bulunanSu);
                    }
                    else if (zar < esik + parcaEsigi)
                    {
                        yedekParca++;
                        printf("YEDEK PARCA BULUNDU (+1 parca)\n");
                    }
                    else
                    {
                        printf("ELI BOS DONULDU\n");
                    }
                }
                break;

            /* ===== O: Oksijen uretimi ===== */
            case 'o':
            case 'O':
                if (oksijen >= 100)
                {
                    printf("Oksijen deposu zaten dolu (100). Komut iptal edildi.\n");
                }
                else if (su < 2)
                {
                    printf("Yetersiz su! Oksijen uretimi icin en az 2 birim su gerekir. Komut iptal edildi.\n");
                }
                else if (enerji < URETIM_ENERJI)
                {
                    printf("Yetersiz enerji! Oksijen uretimi icin en az %d enerji gerekir. Komut iptal edildi.\n",
                           URETIM_ENERJI);
                }
                else
                {
                    printf("Her 2 birim su 3 birim oksijene donusur. Depodaki su: %d\n", su);
                    do
                    {
                        printf("Kac birim su kullanilsin? (2-%d arasi, iptal icin 0): ", su);
                        miktar = sayiOku();
                        if (miktar > su)
                        {
                            printf("Mevcut sudan fazla miktar girilemez! (Depoda %d birim su var)\n", su);
                        }
                        else if (miktar != 0 && miktar < 2)
                        {
                            printf("Gecersiz miktar! En az 2 birim su girilmelidir.\n");
                        }
                    } while (miktar != 0 && (miktar < 2 || miktar > su));

                    if (miktar == 0)
                    {
                        printf("Oksijen uretimi iptal edildi.\n");
                    }
                    else
                    {
                        artikSu = miktar % 2;                    /* tek sayi ise 1 birim artar */
                        kullanilanSu = miktar - artikSu;
                        uretilenOksijen = (kullanilanSu / 2) * 3;

                        turHarcandi = 1;
                        enerji -= URETIM_ENERJI;
                        su -= kullanilanSu;

                        printf("%d birim su kullanildi -> +%d oksijen, -%d enerji.\n",
                               kullanilanSu, uretilenOksijen, URETIM_ENERJI);
                        if (artikSu == 1)
                        {
                            printf("Tek sayi girildi: 1 birim su artik olarak depoda kaldi.\n");
                        }
                        if (oksijen + uretilenOksijen > 100)
                        {
                            printf("Oksijen en fazla 100 olabilir, fazlasi bosa gitti.\n");
                        }
                        oksijen += uretilenOksijen;
                    }
                }
                break;

            /* ===== T: Tamir ===== */
            case 't':
            case 'T':
                if (yedekParca <= 0)
                {
                    printf("Yedek parca yok! Tamir yapilamaz. Komut iptal edildi.\n");
                }
                else if (modulHasari == 0)
                {
                    printf("Modul hasarsiz, tamire gerek yok. Komut iptal edildi.\n");
                }
                else if (enerji < TAMIR_ENERJI)
                {
                    printf("Yetersiz enerji! Tamir icin en az %d enerji gerekir. Komut iptal edildi.\n",
                           TAMIR_ENERJI);
                }
                else
                {
                    turHarcandi = 1;
                    enerji -= TAMIR_ENERJI;

                    /* Hasar kademesi: IF - ELSE IF - ELSE */
                    if (modulHasari <= 30)
                    {
                        printf("Hasar seviyesi: HAFIF (%d)\n", modulHasari);
                        gerekliParca = 1;
                        onarim = 15;
                    }
                    else if (modulHasari <= 70)
                    {
                        printf("Hasar seviyesi: ORTA (%d)\n", modulHasari);
                        gerekliParca = 2;
                        onarim = 25;
                    }
                    else
                    {
                        printf("Hasar seviyesi: AGIR (%d)\n", modulHasari);
                        gerekliParca = 3;
                        onarim = 40;
                    }

                    harcananParca = gerekliParca;
                    if (yedekParca < gerekliParca)
                    {
                        harcananParca = yedekParca;
                        printf("Bu kademe icin %d parca gerekli, elinizde %d var: kismi onarim yapiliyor.\n",
                               gerekliParca, yedekParca);
                    }

                    onarilan = onarim * harcananParca / gerekliParca;
                    if (onarilan > modulHasari)
                    {
                        onarilan = modulHasari;
                    }
                    yedekParca -= harcananParca;
                    modulHasari -= onarilan;
                    printf("%d parca harcandi, %d hasar onarildi, -%d enerji.\n",
                           harcananParca, onarilan, TAMIR_ENERJI);
                }
                break;

            /* ===== D: Durum raporu (tur harcamaz) ===== */
            case 'd':
            case 'D':
                printf("\n========== DURUM RAPORU ==========\n");
                printf("Gun        : %d/%d\n", gun, MAKS_GUN);
                printf("Tur        : %d\n", tur);
                printf("Saglik     : %d/100\n", saglik);
                printf("Enerji     : %d/100\n", enerji);
                printf("Oksijen    : %d/100\n", oksijen);
                printf("Su         : %d birim\n", su);
                printf("Yedek parca: %d\n", yedekParca);
                printf("Modul hasari: %d/100\n", modulHasari);
                if (kilitMuhurlu == 1)
                {
                    printf("Hava kilidi: MUHURLU\n");
                }
                else
                {
                    printf("Hava kilidi: ACIK\n");
                }
                printf("----------------------------------\n");

                /* Kritik seviyedeki her deger icin ayri uyari */
                if (oksijen < 25)
                {
                    printf("UYARI: Oksijen kritik seviyede (< 25)!\n");
                }
                if (saglik < 30)
                {
                    printf("UYARI: Saglik kritik seviyede (< 30)!\n");
                }
                if (enerji < 20)
                {
                    printf("UYARI: Enerji kritik seviyede (< 20)!\n");
                }
                if (modulHasari > KRITIK_HASAR)
                {
                    printf("UYARI: Modul hasari kritik seviyede (> %d)!\n", KRITIK_HASAR);
                }
                /* Hepsi guvenli aralikta ise tek bir && kosulu */
                if (oksijen >= 25 && saglik >= 30 && enerji >= 20 && modulHasari <= KRITIK_HASAR)
                {
                    printf("TUM SISTEMLER NORMAL\n");
                }
                break;

            /* ===== U: Uyu / dinlen ===== */
            case 'u':
            case 'U':
                turHarcandi = 1;

                /* Ne kadar yorgunsa o kadar cok enerji toplar */
                enerjiArtis = 30 + (100 - enerji) / 4;
                if (enerji + enerjiArtis > 100)
                {
                    enerjiArtis = 100 - enerji;
                }
                enerji += enerjiArtis;
                printf("Dinlendiniz: +%d enerji.\n", enerjiArtis);

                /* Oksijen 20'nin altindaysa saglik artmaz, azalir (ic ice IF) */
                if (oksijen < 20)
                {
                    if (oksijen < 10)
                    {
                        saglik -= 10;
                        printf("Oksijen cok dusuk (< 10)! Uyurken saglik -10.\n");
                    }
                    else
                    {
                        saglik -= 5;
                        printf("Oksijen yetersiz (< 20)! Saglik artmadi, -5 saglik.\n");
                    }
                }
                else
                {
                    saglikArtis = 8 + (100 - saglik) / 10;
                    if (saglik + saglikArtis > 100)
                    {
                        saglikArtis = 100 - saglik;
                    }
                    saglik += saglikArtis;
                    printf("Saglik +%d.\n", saglikArtis);
                }

                oksijen -= UYKU_OKSIJEN;
                printf("Oksijen tuketimi devam ediyor: -%d oksijen.\n", UYKU_OKSIJEN);
                break;

            /* ===== K: Kum firtinasi (FOR dongusu) ===== */
            case 'k':
            case 'K':
                turHarcandi = 1;
                baslangicSaglik = saglik;
                baslangicOksijen = oksijen;
                baslangicHasar = modulHasari;
                hasarliTurSayisi = 0;

                printf("\n*** KUM FIRTINASI BASLADI (%d tur) ***\n", firtinaTurSayisi);

                for (i = 1; i <= firtinaTurSayisi; i++)
                {
                    /* Cift numarali turlarda ruzgar siddetlenir, hasar 2 katina cikar */
                    carpan = 1;
                    if (i % 2 == 0)
                    {
                        carpan = 2;
                    }
                    hasar = firtinaHasari * carpan;

                    /* Korunma esigi oyuncunun durumuna (modul hasarina) baglidir */
                    esik = firtinaKorumaEsigi;
                    if (modulHasari <= 30)
                    {
                        esik += 15;
                    }
                    else if (modulHasari > KRITIK_HASAR)
                    {
                        esik -= 15;
                    }

                    zar = rand() % 100;
                    printf("Firtina turu %d/%d", i, firtinaTurSayisi);
                    if (carpan == 2)
                    {
                        printf(" [RUZGAR SIDDETLENDI: hasar x2]");
                    }
                    printf(" -> Zar: %d / Esik: %d -> ", zar, esik);

                    if (zar < esik)
                    {
                        printf("KORUNDUNUZ, zarar yok\n");
                    }
                    else if (zar < 65)
                    {
                        /* Oksijen sizintisi: kilit muhurluyse bu tur atlanir */
                        if (kilitMuhurlu == 1)
                        {
                            printf("Oksijen sizintisi, hava kilidi muhurlu: engellendi\n");
                            continue;
                        }
                        oksijen -= hasar;
                        hasarliTurSayisi++;
                        printf("OKSIJEN SIZINTISI (-%d oksijen)\n", hasar);
                    }
                    else if (zar < 85)
                    {
                        modulHasari += hasar;
                        hasarliTurSayisi++;
                        printf("MODUL HASARI (+%d hasar)\n", hasar);
                    }
                    else
                    {
                        saglik -= hasar;
                        hasarliTurSayisi++;
                        printf("CAN KAYBI (-%d saglik)\n", hasar);
                    }

                    /* Saglik veya oksijen 0'a duserse firtina hemen biter */
                    if (saglik <= 0 || oksijen <= 0)
                    {
                        printf("Kritik seviye! Firtina erken sonlandi.\n");
                        break;
                    }
                }

                saglik = sinirla(saglik);
                oksijen = sinirla(oksijen);
                modulHasari = sinirla(modulHasari);

                printf("\n--- FIRTINA OZETI ---\n");
                printf("Hasar alinan tur sayisi : %d\n", hasarliTurSayisi);
                printf("Kaybedilen saglik       : %d\n", baslangicSaglik - saglik);
                printf("Kaybedilen oksijen      : %d\n", baslangicOksijen - oksijen);
                printf("Modul hasari artisi     : %d\n", modulHasari - baslangicHasar);
                break;

            /* ===== H: Hava kilidi sifresi (DO-WHILE) ===== */
            case 'h':
            case 'H':
                if (kilitMuhurlu == 1)
                {
                    printf("Hava kilidi zaten muhurlu. Komut iptal edildi.\n");
                }
                else if (enerji < SIFRE_HAK * SIFRE_ENERJI)
                {
                    printf("Yetersiz enerji! Sifre denemeleri icin en az %d enerji gerekir. Komut iptal edildi.\n",
                           SIFRE_HAK * SIFRE_ENERJI);
                }
                else
                {
                    turHarcandi = 1;
                    sifre = 'A' + (gun % 26);   /* sifre gun sayisindan turetilir */
                    deneme = 0;
                    girilen = '?';

                    printf("Hava kilidi sifre paneli acildi.\n");
                    printf("Ipucu: Sifre, alfabede A harfinden bugunun gun sayisi (%d) kadar ilerideki harftir.\n",
                           gun);
                    printf("%d deneme hakkiniz var, her yanlis denemede -%d enerji.\n",
                           SIFRE_HAK, SIFRE_ENERJI);

                    do
                    {
                        printf("Deneme %d/%d - Sifre harfi: ", deneme + 1, SIFRE_HAK);
                        girilen = karakterOku();

                        /* Kucuk harf girisi buyuk harfe cevrilir */
                        if (girilen >= 'a' && girilen <= 'z')
                        {
                            girilen = girilen - 'a' + 'A';
                        }

                        /* Harf olmayan giris deneme hakkini harcamaz */
                        if (girilen < 'A' || girilen > 'Z')
                        {
                            printf("Gecersiz giris! Yalnizca A-Z arasi tek bir harf girin (hak harcanmadi).\n");
                            continue;
                        }

                        deneme++;
                        if (girilen != sifre)
                        {
                            enerji -= SIFRE_ENERJI;
                            if (girilen < sifre)
                            {
                                printf("Yanlis sifre! Ipucu: ileri git. (-%d enerji)\n", SIFRE_ENERJI);
                            }
                            else
                            {
                                printf("Yanlis sifre! Ipucu: geri git. (-%d enerji)\n", SIFRE_ENERJI);
                            }
                        }
                    } while (girilen != sifre && deneme < SIFRE_HAK);   /* tek mantiksal ifade */

                    if (girilen == sifre)
                    {
                        kilitMuhurlu = 1;
                        printf("SIFRE DOGRU! Hava kilidi muhurlendi (%d. denemede).\n", deneme);
                    }
                    else
                    {
                        modulHasari += SIFRE_CEZA;
                        printf("%d deneme de basarisiz! Kilit sistemi zarar gordu: modul hasari +%d.\n",
                               SIFRE_HAK, SIFRE_CEZA);
                    }
                }
                break;

            /* ===== Q: Cikis ===== */
            case 'q':
            case 'Q':
                printf("Cikis komutu verildi.\n");
                oyunDurumu = 3;
                break;

            /* ===== Tanimsiz komut: tur harcanmaz ===== */
            default:
                printf("Tanimsiz komut! Gecerli komutlar: G, O, T, D, U, K, H, Q (tur harcanmadi).\n");
                break;
        }

        /* ---------- Sinirlandirma: her komuttan sonra 0-100 ---------- */
        saglik = sinirla(saglik);
        enerji = sinirla(enerji);
        oksijen = sinirla(oksijen);
        modulHasari = sinirla(modulHasari);

        /* ---------- Zaman akisi ---------- */
        if (turHarcandi == 1)
        {
            tur++;

            /* Her 3 turda bir yeni gun baslar (oyuncu hayattaysa) */
            if (tur % GUN_UZUNLUGU == 0 && saglik > 0 && oksijen > 0)
            {
                gun++;
                if (gun > MAKS_GUN)
                {
                    gun = MAKS_GUN;
                    kurtarmaGeldi = 1;
                }
                else
                {
                    printf("\n>>> YENI GUN BASLADI: Gun %d <<<\n", gun);
                    oksijen -= GUN_OKSIJEN_KAYBI;
                    printf("Gun basi oksijen tuketimi: -%d oksijen.\n", GUN_OKSIJEN_KAYBI);
                    if (modulHasari > KRITIK_HASAR)
                    {
                        saglik -= GUN_SAGLIK_KAYBI;
                        printf("Modul hasari %d'in ustunde: -%d saglik.\n", KRITIK_HASAR, GUN_SAGLIK_KAYBI);
                    }
                    saglik = sinirla(saglik);
                    oksijen = sinirla(oksijen);
                }
            }
        }

        /* ---------- Oyun sonu kontrolu (her turun sonunda) ---------- */
        if (oyunDurumu == 0)
        {
            if (saglik <= 0 || oksijen <= 0)
            {
                oyunDurumu = 1;
            }
            else if (kurtarmaGeldi == 1)
            {
                oyunDurumu = 2;
            }
        }

    } while (oyunDurumu == 0);

    /* ---------- Sonuc raporu ---------- */
    puan = (gun * 100) + saglik + oksijen - modulHasari;

    printf("\n==============================================\n");
    printf("                SONUC RAPORU\n");
    printf("==============================================\n");
    if (oyunDurumu == 2)
    {
        printf("KAZANDINIZ! %d gun hayatta kaldiniz, kurtarma ekibi geldi.\n", MAKS_GUN);
    }
    else if (oyunDurumu == 1)
    {
        printf("KAYBETTINIZ! ");
        if (saglik == 0)
        {
            printf("Saglik 0'a dustu. ");
        }
        if (oksijen == 0)
        {
            printf("Oksijen 0'a dustu. ");
        }
        printf("\n");
    }
    else
    {
        printf("Simulasyondan cikildi.\n");
    }
    printf("Hayatta kalinan gun: %d\n", gun);
    printf("Toplam tur         : %d\n", tur);
    printf("Son durum          : Saglik:%d Enerji:%d Oksijen:%d Su:%d Parca:%d Hasar:%d\n",
           saglik, enerji, oksijen, su, yedekParca, modulHasari);
    printf("Puan = (gun x 100) + saglik + oksijen - modulHasari\n");
    printf("     = (%d x 100) + %d + %d - %d = %d\n", gun, saglik, oksijen, modulHasari, puan);
    printf("==============================================\n");

    return 0;
}

