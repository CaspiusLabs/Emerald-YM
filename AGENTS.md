# Emerald YM — developer handoff

- Rozmawiaj z użytkownikiem po polsku. Pracujemy nad firmware'em M-VAVE FM-1 na lokalnym Windows.
- Bazą do rozbudowy jest zaakceptowana na urządzeniu wersja `v0.1.4`. `VERSION` jest źródłem numeru wydania. Zobacz README, BUILDING, CHANGELOG i docs/ARCHITECTURE.md.
- Kod aktywnej aplikacji: `firmware/src/ym/`; wybrany przez `EN_YM2612` w `firmware/src/main.c`. `firmware/src/app/` i `dsp/` zawierają również odziedziczony kod DUB-1 — nie zakładaj, że opisuje obecną aplikację.
- Buduj: `powershell -ExecutionPolicy Bypass -File .\build-ym-windows.ps1`. Testuj: `powershell -ExecutionPolicy Bypass -File .\test-ym-windows.ps1`. W PowerShell 7 wystarczy wywołać pliki bezpośrednio. Te skrypty nie otwierają MIDI ani nie flashują urządzenia.
- Lokalne zależności są w ignorowanym `.runtime/`: python, toolchain i minimalny sdk. Nie zależ od folderów wcześniejszego czatu. Narzędzia hostowe MSVC są wykrywane przez vswhere.
- Nie podmieniaj `releases/v0.1.4/emerald-ym-0.1.4.fwsc` ani tagu v0.1.4. Nowe zachowanie = nowy numer w VERSION i osobne wydanie. Przy pracy organizacyjnej nie modyfikuj firmware'u na urządzeniu.
- Wgrywanie lub test otwierający MIDI wykonuj w zakresie aktualnego polecenia użytkownika i wcześniejszej autoryzacji; nie uruchamiaj go automatycznie jako części budowania, eksportu lub zwykłych testów. Nie pytaj ponownie o czynności już autoryzowane w trwającym zadaniu.
- USB zawiera też Roland UM-ONE. Testy urządzenia kieruj wyłącznie do zweryfikowanej pary Emerald YM FM-1. Protokół statusu jest w YM-USB-PROTOCOL.md. Nie wymyślaj komunikatów SysEx.
- Audio: 44,1 kHz, bloki 256 ramek, około 5804 us budżetu. Próg ochronny 85% jest celowy; nie usuwaj ani nie podnoś go dla pozornego przejścia testów.
- Krytyczny kod/tablice są w `.audio_hot`, kopiowane do RAM przed startem. Nie zmieniaj mapy pamięci, far-call i guardów bez kontroli linkera i czasu wykonania. Kod flash-off `.ram_text` nadal nie może wołać do XIP.
- Profil emulatora ma 5 FM + stały DAC. `EMERALD_FIXED_DAC` pomija niesłyszalny szósty tor FM. Nie dodawaj dowolnych zapisów rejestrów/przełączania szóstego FM bez usunięcia lub dostosowania tej optymalizacji.
- UI: gotowe kafelki poza ekranem, odświeżanie tylko po zmianie. Nie przywracaj okresowego lcd_fill całego ekranu. Tablice LED publikuj gotowymi wierszami.
- W rendererze nie przydzielaj pamięci i nie dodawaj libm/double. Sprawdzaj no-soft-double, rozmiary, deterministyczność, testy UI/OTA i właściwy zakres zmiany; nie uruchamiaj starych testów scenariuszy DUB-1 jako dowodu działania nowego UI.
- Zachowuj rozróżnienie: testy desktopowe, symulacja flash i testy fizycznego egzemplarza. Raport interaktywny 0.1.4 ma passed=false z powodu resetu taktu przez REC, przy CPU 80%, XRUNS 0 i potwierdzonym UI. Nie zmieniaj surowych dowodów na pozornie zielone.
- `upstream` wskazuje publiczną bazę DUB-1; push jest lokalnie wyłączony. Publikacji na GitHub nie zlecono.
