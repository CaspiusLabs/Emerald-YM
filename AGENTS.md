# Emerald YM — developer handoff

- Rozmawiaj z użytkownikiem po polsku. Pracujemy nad firmware'em M-VAVE FM-1 na lokalnym Windows.
- Bazą do rozbudowy jest zaakceptowana na urządzeniu wersja `v0.1.4`. `VERSION` jest źródłem numeru wydania. Zobacz README, BUILDING, CHANGELOG i docs/ARCHITECTURE.md.
- Bieżące rozwinięcie to 0.3.1 (indywidualne poziomy i FX); opis w docs/releases/v0.3.1.md.
  Zachowaj 0.1.4 jako osobną zaakceptowaną bazę. Nie myl prób 0.2.x z jej dowodami.
- PSG to ograniczony profil muzyczny Sega: 3 tony i stały biały szum clock/512.
  Nie jest to dowolny interfejs rejestrów/VGM. Optymalizacja jednego zbocza
  na próbkę zakłada dzielnik >=9 (cały obsługiwany zakres MIDI); test pilnuje
  tej granicy. Live używa PSG, nie dodaje szóstego toru FM.
- Kod aktywnej aplikacji: `firmware/src/ym/`; wybrany przez `EN_YM2612` w `firmware/src/main.c`. `firmware/src/app/` i `dsp/` zawierają również odziedziczony kod DUB-1 — nie zakładaj, że opisuje obecną aplikację.
- Buduj: `powershell -ExecutionPolicy Bypass -File .\build-ym-windows.ps1`. Testuj: `powershell -ExecutionPolicy Bypass -File .\test-ym-windows.ps1`. W PowerShell 7 wystarczy wywołać pliki bezpośrednio. Te skrypty nie otwierają MIDI ani nie flashują urządzenia.
- Lokalne zależności są w ignorowanym `.runtime/`: python, toolchain i minimalny sdk. Nie zależ od folderów wcześniejszego czatu. Narzędzia hostowe MSVC są wykrywane przez vswhere.
- Nie podmieniaj `releases/v0.1.4/emerald-ym-0.1.4.fwsc` ani tagu v0.1.4. Nowe zachowanie = nowy numer w VERSION i osobne wydanie. Przy pracy organizacyjnej nie modyfikuj firmware'u na urządzeniu.
- Wgrywanie lub test otwierający MIDI wykonuj w zakresie aktualnego polecenia użytkownika i wcześniejszej autoryzacji; nie uruchamiaj go automatycznie jako części budowania, eksportu lub zwykłych testów. Nie pytaj ponownie o czynności już autoryzowane w trwającym zadaniu.
- USB zawiera też Roland UM-ONE. Testy urządzenia kieruj wyłącznie do zweryfikowanej pary Emerald YM FM-1. Protokół statusu jest w YM-USB-PROTOCOL.md. Nie wymyślaj komunikatów SysEx.
- Audio: 44,1 kHz, bloki 256 ramek, około 5804 us budżetu. Próg ochronny 85% jest celowy; nie usuwaj ani nie podnoś go dla pozornego przejścia testów.
- Krytyczny kod/tablice są w `.audio_hot`, kopiowane do RAM przed startem. Nie zmieniaj mapy pamięci, far-call i guardów bez kontroli linkera i czasu wykonania. Kod flash-off `.ram_text` nadal nie może wołać do XIP.
- Od 0.2.3 główne leaf-settery/gettery mają sekcję `.ym_control` w XIP,
  zbieraną przed wykluczeniem tekstu obiektów emulatora. Wywołuje je pętla
  główna przez istniejące FL_FAR, nie renderer. Budowanie sprawdza ich adresy.
  Granice obszarów RAM i startup kopiujący `.audio_hot` są niezmienione.
- Od 0.3.1 SELECT wybiera źródło, PRESETS jego poziom, FX wybiera DECAY/COLOR/ECHO,
  ALGORITHM zmienia wartość efektu. 12x3 wartości współistnieją; przycisk FX
  nie wyłącza poprzedniego efektu. Brak automatycznego zapisu Flash.
- EMERALD_STATIC_OPN wymaga wyłączonych SSG-EG, operator AM i PM. Obecne patche
  oraz FX tego pilnują. Dodanie tych trybów wymaga adaptacji lub wyłączenia
  optymalizacji i porównania z EMERALD_GENERIC_OPN (tools/test-ym-generic-windows.cmd).
- `.audio_hot.fx` jest zbierana wewnątrz istniejącej `.audio_hot`, objęta tą samą
  kopią startup i ochroną PC; funkcje efektów nie mogą trafić do wykonywalnych
  danych poza tym zakresem. Granice pamięci i blok debug/STATUS bez zmian.
- Echo: osobne wysyłki, jeden powrót stereo ~186 ms, bufory/tagi 32 KiB w pool.
  Testy fizyczne i offline są odrębne. Zachowaj niezaliczony warunek ciągłości
  w interaktywnym raporcie sustain 0.3.0; nie zmieniaj go na zielony.
- Tablica psg_divider ma sekcję `.ym_pitch` w XIP, odczytywaną przy instalacji
  okresu, poza pętlą próbek oscylatora. Okresy pozostają w RAM. Linker i test
  sprzętowy pilnują położenia i czasu; nie zmieniaj zakazu XIP w `.ram_text`.
- Profil emulatora ma 5 FM + stały DAC. `EMERALD_FIXED_DAC` pomija niesłyszalny szósty tor FM. Nie dodawaj dowolnych zapisów rejestrów/przełączania szóstego FM bez usunięcia lub dostosowania tej optymalizacji.
- UI: gotowe kafelki poza ekranem, odświeżanie tylko po zmianie. Nie przywracaj okresowego lcd_fill całego ekranu. Tablice LED publikuj gotowymi wierszami.
- W rendererze nie przydzielaj pamięci i nie dodawaj libm/double. Sprawdzaj no-soft-double, rozmiary, deterministyczność, testy UI/OTA i właściwy zakres zmiany; nie uruchamiaj starych testów scenariuszy DUB-1 jako dowodu działania nowego UI.
- Zachowuj rozróżnienie: testy desktopowe, symulacja flash i testy fizycznego egzemplarza. Raport interaktywny 0.1.4 ma passed=false z powodu resetu taktu przez REC, przy CPU 80%, XRUNS 0 i potwierdzonym UI. Nie zmieniaj surowych dowodów na pozornie zielone.
- `upstream` wskazuje publiczną bazę DUB-1; push jest lokalnie wyłączony. Publikacji na GitHub nie zlecono.
