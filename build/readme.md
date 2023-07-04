```                                                        
 m    m                                          m     m
 ##  ##  mmm    mmm   mmmmm  m   m  m mm    mmm  #  #  #
 # ## # "   #  #   "     m"  "m m"  #"  #  "   # " #"# #
 # "" # m"""#   """m   m"     #m#   #   #  m"""#  ## ##"
 #    # "mm"#  "mmm"  #mmmm   "#    #   #  "mm"#  #   # 
                              m"                        
                             ""                         
```
# MaszynaW 
**Maszyna W** to narzędzie edukacyjne mające na celu wizualizację działania prostego procesora. Poniższa instrukcja tłumaczy podstawy jej obsługi


## Interfejs Użytkownika

1. Z lewej strony znajduje się podgląd plików. 

Dwa przyciski powyżej oznaczają kolejno:

- `otwórz plik programu` 

- `otwórz plik rozkazu`

Po wybraniu jednej z opcji pojawiają się  nowe przyciski:

- `edytuj`  Otwiera plit zewnętrznym edytorze, takim jak ten. 

- `asembluj` *(Dla programu)*
- `dodaj do listy` *(Dla rozkazu)*

Podgląd dla programu wskazuje na błędy w składni.
2. W prawym dolnym rogu znajduje się konsola informująca o zmianach w statusie maszyny, można ją przewijać kółkiem myszy.


3. Na środku znajduje się podgląd maszyny, są to rejestry połączone ze sobą odpowiednimi liniami. Oznaczone są również odpowiednie sygnały sterujące. Klikając na rejestr można wprowadzić do niego wartość liczbową. W elementach pamięci można przewijać listę komórek w pamięci. Klikając w puste miejsce w widoku maszyny, można przesunąć widok myszą. Przy pomocy kółka myszy można przybliżać i oddalać widok.
4. Na górze ekranu znajduje się pasek narzędzi. Od lewej są to:

- `resetuj` przywraca maszynę do stanu początkowego, nie kasuje programu z pamięci ale zeruje wszystkie rejestry, wyłącza sygnał *stop*
- `wykonaj takt` wykonuje pojedynczy takt rozkazu (linijkę), w widoku maszyny pokazane jest to przez podświetlenie aktywnych sygnałów
- `wykonaj rokaz` wykonuje pełny cykl jednego rozkazu 
- `wykonaj program` wykonuje cały program do otrzymania sygnału stop lub przekroczenia limitu operacji
- `wczytaj ponownie konfigurację` ładuje od nowa konfigurację maszyny, wiąże się to z usunięciem programu z pamięci i konieczna jest ponowna asemblacja
- `wybierz konfigurację` wyświetla okno dialogowe pozwalające wczytać konfigurację maszyny. Musi ona być zapisana w pliku `.conf` w folderze z plikiem wykonywalnym, lub w folderze `conf/`

- `wybierz motyw` wyświetla okno dialogowe pozwalające wczytać motyw kolorystyczny programu. Musi on być zapisany w pliku `.style` w folderze z plikiem wykonywalnym, lub w folderze `style/`
- `opuść program` wyłącza program
