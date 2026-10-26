# Emerald YM 0.1.0 — eksperymentalny firmware dla M-VAVE FM-1

**Pakiet został zbudowany i sprawdzony na komputerze. Nie został wgrany do FM-1. Praca w czasie rzeczywistym na urządzeniu nie jest jeszcze potwierdzona.**

## Co przygotowano

- Prawdziwy rdzeń emulacji YM2612: ymfm Aarona Gilesa, commit `81aec25ccbb98f4873a255f7551ac4dadac59b4a`.
- Pięć głosów FM i perkusja przez emulowany, 8-bitowy DAC szóstego kanału. Brak PSG w tej wersji.
- Generatywny sekwencer działający bez końca: motywy, harmonia E-dur, bas, akordy, melodia, sekcje o różnej gęstości oraz mutacje zależne od seedu. Własna muzyka i syntetyczne sample; bez danych z gier Sonic.
- Baza sprzętowa, USB, mechanizm aktualizacji, watchdog i tryb awaryjny z DUB-1/Felucca. Źródłowy DUB-1: commit `e6fe7b8f8f3114019bb36b5b536ad458eacd2c2d`.
- Zamiast komputerowej listy nut i minutowych buforów: generowanie zdarzeń przy kolejnych krokach, krótkie sample w pamięci programu, małe bufory audio. Nie ma alokacji podczas renderowania.
- Wąska warstwa zgodności C++ dla ymfm: statyczna arena inicjalizowana przed uruchomieniem przerwań. Obliczenia FM rdzenia są zachowane. Zapisywanie stanu emulatora i debugowe pliki WAV są wyłączone na urządzeniu.

## Pliki

`emerald-ym-0.1.0.fwsc` — rzeczywisty, instalowalny pakiet dla AC791N/FM-1. To wersja do pierwszych testów sprzętowych, nie potwierdzone wydanie stabilne.

Identyfikator pakietu i aplikacji: `FM-1_68000100`. Oczekiwana nazwa USB aplikacji: `Emerald YM FM-1` (Windows może zachować dawny alias portu MIDI). VID/PID pozostają `1209:0001`, zgodnie z platformą Felucca. Loader jest niezmieniony.

`Emerald-YM-streaming-preview-60s.wav` — minutowy render tej samej strumieniowej implementacji, zbudowanej dla komputera. Pozwala ocenić zmianę brzmienia po porcie; nie jest nagraniem z FM-1.

`firmware-verification.json` i logi testów — rozmiary, suma SHA-256, wersje bazowe i zakres kontroli.

## Obsługa po ewentualnym wgraniu

1. Ustaw początkowo małą głośność MASTER. Po uruchomieniu sekwencer jest zatrzymany.
2. PLAY uruchamia / zatrzymuje muzykę z łagodnym wyciszeniem. Pauza zachowuje pozycję sekwencera.
3. KNOB 1: tempo 80–180 BPM. KNOB 2: gęstość melodii 0–100. KNOB 3: mutacja 0–100. KNOB 4: transpozycja od -12 do +12 półtonów.
4. REC wybiera nowy seed i zaczyna nową sekwencję. SEQ przełącza mutację między 0 a 35; nie zamraża wszystkich losowych decyzji aranżacji.
5. Ustawienia są robocze i nie zapisują się po wyłączeniu. To prototyp generatora; klawiatura i wejściowe nuty MIDI nie są jeszcze podłączone do silnika.

Na ekranie są tempo, takt, **CPU MAX %** i **XRUNS**. Bufor 256 ramek daje około 5,8 ms na blok. CPU MAX pokazuje najgorszy czas renderowania od uruchomienia jako procent tego budżetu. XRUNS liczy sytuacje, kiedy DMA przeszedł do kolejnej połówki przed zakończeniem renderowania.

Po 8 blokach z obciążeniem powyżej 85% (po początkowym rozgrzaniu) aplikacja trwale wycisza silnik do kolejnego restartu i wyświetla `OVERLOAD: AUDIO STOPPED`. USB oraz panel pozostają obsługiwane. Nie traktuj tego jako potwierdzenia stabilności — to pomoc przy pierwszych pomiarach.

## Co sprawdzono

- Kompilacja na rzeczywisty target `pi32v2 -mcpu=r3 -mfprev1`, oficjalny kompilator JieLi Windows 2.5.3 / clang 4.0.1.
- Aplikacja: **96 636 B**. RAM `.data + .bss`: **28 656 / 98 304 B**. Pula, głównie grafika: **69 120 / 344 064 B**. Stosy i obszary zarezerwowane pozostają w mapie platformy. Wynik nie oznacza jedynie 28 kB całkowitego zużycia RAM.
- Brak dołączonych procedur arytmetyki double. Brak wywołań z kodu flash do XIP podczas jego programowania w sekcji `.ram_text`. Kontrola odwołań do rejestrów sprzętowych poza HAL.
- Ponad 180 sekund renderowania na komputerze: zgodność wyników przy blokach 256 i 63 ramek, powtarzalność seedu od zimnego startu, inny wynik dla innego seedu, pauza i wyciszenie, skrajne ustawienia tempa/transpozycji oraz nowe ziarno bez alokacji. W testowych renderach nie zadziałał limiter.
- Symulacja wejścia OTA na tym pakiecie: handshake, staging loadera, CRC, zatwierdzenie, przerwanie połączenia, brak zatwierdzenia i sprzątanie stagingu.
- Symulacja loadera z innym wcześniejszym obrazem: instalacja nowej aplikacji, zachowanie nagłówka flash, ograniczenie zapisów do dozwolonych okien, odmowa dla uszkodzonego nagłówka lub innego klucza, ponowna instalacja bez zbędnego kasowania.
- Testy instalatora z udawanym urządzeniem przeszły. Porównanie jego obrazu logicznego z wersją JavaScript zostało pominięte przez test bazowy z powodu braku Node.js w środowisku poleceń.
- Trzy binarne składniki SDK odpowiadają dokładnie referencyjnym sumom SHA-256 z DUB-1.

## Czego jeszcze nie potwierdzono

Nie uruchamiano tego obrazu na FM-1. Nie zmierzono czasu renderowania, opóźnień przerwań, stosu, jakości wyjścia ani obsługi panelu na procesorze urządzenia. Testy aktualizacji symulują flash i transport; nie sprawdzają fizycznej pamięci ani zachowania przy zaniku zasilania. Watchdog i tryb awaryjny zmniejszają trudność diagnostyki, ale nie gwarantują odzyskania urządzenia po każdej awarii.

Pierwszy test sprzętowy powinien sprawdzić rozruch bez grania, potem PLAY przy domyślnych ustawieniach i obserwację CPU MAX / XRUNS. Następnie nowe seedy, tempo i transpozycję. Cel: stałe XRUNS = 0 i wyraźny zapas poniżej limitu. Jeśli występuje przeciążenie, potrzebna będzie optymalizacja rdzenia lub ograniczenie liczby aktywnych głosów; na razie nie ma dowodu, że pełna konfiguracja działa w czasie rzeczywistym.

## Instalacja — osobny następny etap

Przy przygotowywaniu pakietu nie otwierano portów MIDI i nie wysyłano SysEx do urządzenia. Wgrywanie zastąpi obecną aplikację i niesie ryzyko utraty danych lub problemu z rozruchem.

W źródłach jest bazowy instalator `tools/fm1_install.py` dla pakietów Felucca, zależności: `mido`, `python-rtmidi`. Użycie go otwiera MIDI i wysyła SysEx. Powinien wskazywać konkretny port FM-1, nie interfejs Roland UM-ONE. Nie używaj `--force`. Nie dołączono skryptu automatycznie rozpoczynającego wgrywanie.

Zachowano bazowy skrót OCT- i OCT+ przytrzymane przez 5 sekund do trybu aktualizacji oraz tryb awaryjny po nieudanych rozruchach. Możliwości przywrócenia bazowego firmware'u zależą od tego, czy urządzenie nadal dochodzi do odpowiedniego trybu; czasem potrzebne jest narzędzie sprzętowe. Nie obiecujemy automatycznego rollbacku.

## Budowanie i źródła

`build-ym-windows.ps1 -Python PATH -Toolchain PATH -Sdk PATH` buduje wyłącznie lokalne pliki; nie łączy się z FM-1. Wymagane są Python 3 z Pillow, oficjalny toolchain Windows JieLi z `pi32/bin/clang.exe` i `pi32v2-ld.exe` oraz SDK `AC79NN_SDK_V1.2.1_2023-12-13`. Domyślne ścieżki skryptu odpowiadają lokalnemu folderowi roboczemu autora; po rozpakowaniu wskaż własne ścieżki. Narzędzia nie są instalowane systemowo ani dołączone do źródeł.

Dokumentacja oficjalnego kompilatora: https://doc.zh-jieli.com/AC79/zh-cn/master/getting_started/environmental_install/jl_toolchain.html . SDK: https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK . Rdzeń: https://github.com/aaronsgiles/ymfm . Baza: https://github.com/sbegueria/fm1-dub .

Testy strumieniowania i symulacji platformy na Windows: `tools/test-ym-windows.cmd` (wymaga lokalnego MSVC; w skrypcie jest ścieżka do użytej wersji Visual Studio). `tools/verify_ym_package.py` weryfikuje finalny pakiet i symuluje instalację innego obrazu. Oryginalne testy instrumentu/ekranów DUB-1 nie opisują nowej aplikacji i nie są przedstawiane jako jej walidacja.

## Licencje

Cały firmware po połączeniu z bazą DUB-1/Felucca: GPL-3.0-only, `LICENSE`. Zachowano autorstwo platformy w źródłach i `LICENSING.md`. Rdzeń ymfm: BSD-3-Clause, `firmware/src/ym/ymfm/LICENSE`. Własny silnik/sekwencer i wąska warstwa zgodności: MIT, `LICENSE-emerald.txt`. Czcionki: pliki licencji w `assets/fonts`. Składniki boot/konfiguracji JieLi: Apache-2.0; tekst w `JieLi-SDK-LICENSE.txt`. Archiwum zawiera odpowiadające źródła, skrypty i logi; nie zawiera własnościowego kompilatora.
