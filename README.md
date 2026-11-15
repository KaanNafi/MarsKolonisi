# Mars Kolonisi Yaşam Destek Simülatörü

C diliyle yazılmış, metin tabanlı ve tek karakterli komutlarla kontrol edilen bir hayatta kalma oyunudur. Oyuncu, Mars'taki küçük bir araştırma istasyonunda tek başına kalan bir mühendisi yönetir ve kurtarma ekibi gelene kadar **15 gün** hayatta kalmaya çalışır.

Proje, *Algoritma ve Programlama* dersi kapsamında; aritmetik ve mantıksal operatörler, `if / else if / else`, `switch-case` (fall-through), `for` ve `do-while` döngüleri ile `break` / `continue` kullanımını göstermek amacıyla geliştirilmiştir.

## Özellikler

- Üç zorluk seviyesi (Kolay, Normal, Zor); başlangıç değerlerini ve olay eşiklerini değiştirir
- Sağlık, enerji, oksijen, su, yedek parça, modül hasarı ve hava kilidi durumunun takibi
- Büyük/küçük harf duyarsız komut sistemi (`switch-case` fall-through)
- Kum fırtınası simülasyonu (`for` döngüsü, `break`, `continue`)
- Sınırlı denemeli hava kilidi şifresi (`do-while`)
- Her rastgele olayda zar ve eşik değerinin ekrana yazdırılması (`Zar: 37 / Esik: 45 -> BASARILI`)
- Oyun sonunda sonuç raporu ve puan hesabı

## Derleme ve Çalıştırma

```bash
gcc mars_kolonisi.c -o mars_kolonisi
./mars_kolonisi
```

Windows'ta çalıştırmak için `mars_kolonisi.exe` kullanılır. Program yalnızca standart C kütüphanelerini (`stdio.h`, `stdlib.h`, `time.h`) kullanır.

> Not: Konsol karakter kodlaması sorunlarını önlemek için ekran çıktılarında Türkçe karakter kullanılmamıştır.

## Komutlar

Komutlar büyük/küçük harf duyarsızdır (`g` ve `G` aynı işlemi yapar).

| Komut | İşlev | Maliyet |
|:-----:|-------|---------|
| `G` | Keşif görevi: su veya yedek parça bulma şansı, yaralanma riski | 15 enerji, 10 oksijen |
| `O` | Oksijen üret: her 2 birim su 3 birim oksijene dönüşür | 10 enerji, girilen su |
| `T` | Tamir: yedek parça harcayarak modül hasarını azaltır | 10 enerji, yedek parça |
| `D` | Durum raporu ve kritik uyarılar | Tur harcamaz |
| `U` | Uyu / dinlen: enerji ve sağlık artar | 2 oksijen |
| `K` | Kum fırtınasını simüle et | Zorluğa göre değişir |
| `H` | Hava kilidi şifresi (en fazla 3 deneme) | Yanlış deneme başına 5 enerji |
| `Q` | Çıkış | Tur harcamaz |

Tanımsız bir komut girildiğinde uyarı verilir ve tur harcanmaz. Enerjisi yetersiz olan, malzemesi bulunmayan veya iptal edilen komutlar da tur harcamaz.

## Zorluk Seviyeleri

| Değer | Kolay | Normal | Zor |
|-------|:-----:|:------:|:---:|
| Sağlık | 100 | 100 | 90 |
| Enerji | 100 | 90 | 80 |
| Oksijen | 100 | 90 | 80 |
| Su | 60 | 50 | 45 |
| Yedek parça | 4 | 3 | 2 |
| Modül hasarı | 10 | 20 | 30 |
| Fırtına süresi (tur) | 3 | 5 | 7 |
| Fırtına temel hasarı | 5 | 8 | 12 |
| Yaralanma eşiği | 5 | 10 | 15 |

Geçersiz bir zorluk girişinde seçim yeniden istenir.

## Oyun Kuralları

**Zaman akışı:** Geçerli her komut tur sayacını 1 artırır (`D`, `Q` ve tanımsız komutlar hariç). Her 3 turda bir (`tur % 3 == 0`) yeni gün başlar. Gün başında oksijen 8 azalır; modül hasarı 70'in üzerindeyse ek olarak 5 sağlık kaybedilir.

**Sınırlandırma:** Sağlık, enerji, oksijen ve modül hasarı her komuttan sonra `0-100` aralığında tutulur.

**Hava kilidi şifresi:** Şifre günden türetilir: `sifre = 'A' + (gun % 26)`. Doğru şifre kilidi mühürler (`kilitMuhurlu = 1`) ve kum fırtınasındaki oksijen sızıntılarını engeller. Üç deneme de başarısız olursa modül hasarı 20 artar.

**Oyun sonu:**

- Sağlık veya oksijen 0'a düşerse oyun kaybedilir.
- 15. gün tamamlanırsa kurtarma ekibi gelir ve oyun kazanılır.
- Oyun nasıl biterse bitsin (kazanma, kaybetme, `Q` ile çıkış) bir sonuç raporu yazdırılır.

**Puan:**

```
Puan = (gun x 100) + saglik + oksijen - modulHasari
```

## Kodda Kullanılan Yapılar

| Yapı | Kullanıldığı yer |
|------|------------------|
| `switch-case` (fall-through) | Zorluk seçimi ve komut yönetimi (`case 'g': case 'G':`) |
| `if / else if / else` | Tamir kademeleri (hafif, orta, ağır), durum raporu uyarıları |
| İç içe `if` | Uyku sırasında oksijen kontrolü (`U`) |
| Aritmetik operatörler, `%` | Oksijen dönüşümü ve artık su (`O`), gün hesabı, çift tur kontrolü (`K`), şifre (`H`) |
| Mantıksal operatörler (`&&`, `\|\|`) | Keşifte yaralanma ve su bulma eşiği, "TUM SISTEMLER NORMAL" kontrolü |
| `for` + `break` + `continue` | Kum fırtınası (`K`) |
| `do-while` | Ana komut döngüsü, zorluk seçimi, şifre doğrulama (`H`), su miktarı girişi (`O`) |

## Örnek Ekran Çıktısı

```
Zorluk seviyesi secin (K: Kolay, N: Normal, Z: Zor): n
Zorluk: NORMAL

[Gun 1/15 | Tur 0] Saglik:100 Enerji:90 Oksijen:90 Su:50 Parca:3 Hasar:20 Kilit:ACIK
Komut (G/O/T/D/U/K/H/Q): g
Kesif gorevine cikildi: -15 enerji, -10 oksijen.
Yaralanma kontrolu -> Zar: 46 / Esik: 10 -> GUVENLI
Kaynak arama -> Zar: 40 / Su esigi: 55 / Parca esigi: 75 -> SU BULUNDU (+10 su)
```

## Dosya Yapısı

```
.
├── mars_kolonisi.c   # Oyunun tüm kaynak kodu
└── README.md
```
