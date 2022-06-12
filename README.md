## Programın Amacı
  IPC message queue kullanarak server-client yapısında bir sistem hazırlamak. Bu sistemde client server'a sıralanması için her 10 saniyede bir 1000 elemanlı rastgele bir sayı dizisini gönderir ve server ise bu diziyi sıralayıp CLIENT_PID.txt adlı dosyaya basar. Client ise bu dosyayı okuyarak standart çıktıya verir.

## Programın Çalıştırılması
### Derleme (Compiling)
```bash
gcc server.c -lpthread -o server.out
gcc.client.c -o client.out
```
### Koşturma (Running)
Server ve clientlerin her birinin ayrı terminalde koşturulması gerekmektedir.
```bash
./server.out
./client.out
```

## Detaylı Kod Açıklaması
Her iki dosyada da kullanılan önemli define tanımları şunları belirtir:
- ARR_LEN: Oluşturulacak ve sıralanacak rastgele dizinin boyutu
- BLOCK_LEN: Dizinin kaç uzunluğunda bloklar halinde gönderileceğinin bilgisi (sistemde bulunan "msgmax" değişkeninin default olarak 8192 değerinde olması sebebiyle 1000 uzunluğunda dizi tek bir "msgsnd" komutunda gönderilemedi. bu sebeple sistem bilgisini değişmek yerine dizi bloklara bölünerek gönderilmektedir)
- MSG_LEN: IPC haberleşmesinde kullanılacak mesajların boyutu
- PID_LEN: PID değişkeninin diziye çevrildiği durumlardaki dizinin boyutu (PID değerinin kodda maksimum 10 haneli olacağı varsayıldı
  
Yalnızca client tarafındaki define tanımları:
- SLP_TIME: Client process'in kaç saniyede bir rastgele array oluşturup sıralaması için server'a göndereceği
- RNDM_BOUND: Random oluşturulacak sayıların üst sınırı

### server.c
  Server process özel bir key ile mailbox oluşturduktan sonra bu mailboxu dinlemeye başlar. Mailbox'a mesaj geldiği anda (bu mesaj client tarafından içeriği PID olacak olan mesajdır) mesajı parse eder ve client PID değerine ulaşır. Client'a bilgilendirme mesajı gönderdikten sonra bu client için bir worker thread oluşturur ve tekrar mailbox'u dinlemeye devam eder.
  Client için oluşturulmuş worker thread ise bu client'ın PID'sini key olarak alan ayrı bir message queue oluşturur ve client'a bilgilendirme yapar. Ardından client'tan gelecek rastgele diziyi beklemeye başlar. Dizi geldiğinde diziyi bloklar halinde alır ve azalmayan sıralı hale getirir, ardından ISTEMCI_PID.txt dosyasına yazarak client'ı bilgilendirir. Tekrar client'tan gelecek olan diziyi beklemeye devam eder.

### client.c
  Server process ile aynı mailbox keye sahiptir (statik tanımlı) ve server'a bu mailbox üzerinden PID değerini gönderir. Ardından kendisi için açılan message queue bilgisi geldikten sonra bu queue'ya oluşturduğu rastgele diziyi önce standart çıktıya basar, sonra bloklar halinde gönderir. Her gönderdiği bloktan sonra bloğun alındığı bilgisini bekler. Bloklar tamamen gönderilip server'dan dizinin sıralandığı bilgisi geldikten sonra PID.txt dosyasından diziyi okuyarak standart çıktıya basar.

## Senaryo
1. Server statik bir key ile bir mailbox oluşturur ve dinlemeye başlar.
2. Client aynı key ile bağlandığı mailbox'tan server'a PID bilgisini gönderir.
3. Server aldığı bu PID bilgisi ile client için bir message queue oluşturacağı mesajını client'a gönderir, bu iş için bir worker thread görevlendirir ve diğer client'lardan gelecek mesajlar için mailbox'u dinlemeye devam eder. (Yeni client gelirse main thread 2. adımdan devam eder.)
4. Client bu mesajı aldıktan sonra kendi PID'si ile mailbox oluşturur ve server'dan gelecek mesajı bekler.
5. Server taraftaki worker thread ise client'ın PID değeriyle bir message queue oluşturur ve client'a özel queue oluşturduğunun bilgisini buradan gönderir.
6. Client server tarafından gönderilen queue'nin oluştuğu mesajını alır.
7. Worker thread, client tarafından gelecek dizi içeren mesaj için beklemeye başlar.
8. Client öncelikle random bir integer sayı dizisi oluşturup bunu bloklar halinde server tarafına gönderir.
9. Client'tan diziyi alan worker thread diziyi önce sıralar, daha sonra sıraladığı diziyi ISTEMCI_PID.txt dosyasına yazar. Dizinin sıralandığı bilgisini client'a mesaj olarak gönderir.
10. Dizinin sıralandıği bilgisini alan client PID.txt dosyasından diziyi okur ve sıralanmış diziyi standart çıktıya basar. 10 saniye bekler ve döngüye devam eder. (7. adım)
