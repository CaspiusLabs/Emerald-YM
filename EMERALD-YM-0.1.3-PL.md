# Emerald YM 0.1.3 — działający prototyp na FM-1

**Firmware został wgrany i przeszedł ponad minutę automatycznego testu na podłączonym M-VAVE FM-1. Użytkownik potwierdził słyszalną, płynną muzykę.**

## Wynik na urządzeniu

- Tożsamość: `FM-1_68000103`; port: `Emerald YM FM-1`.
- Test: 60 odczytów statusu w odstępach co najmniej 1 s, po MIDI Start, potem MIDI Stop. Rzeczywiste granie trwało ponad 60 s. Sekwencer dotarł do taktu 35, obejmując całą aranżację i jej powrót.
- Maksymalne obciążenie bloku: **80%**, najdłuższy blok **4665 mikrosekund**, przy budżecie około **5804 mikrosekund**.
- **XRUNS: 0. Bloki przekraczające ochronne 85%: 0. Licznik nieudanych rozruchów: 0.** Uptime i licznik buforów postępowały bez zarejestrowanego restartu.
- Parametry testu: domyślne 132 BPM, gęstość 100, mutacja 35, transpozycja 0, seed 20261026.

To potwierdza działanie tej konfiguracji na tym egzemplarzu, nie dowodzi stabilności wszystkich parametrów ani wielogodzinnego grania. Pełne próbki statusu są w `build/hardware-0.1.3-test.json`; odczyt jest realizowany przez udokumentowany `YM-USB-PROTOCOL.md`.

## Co gra

Jeden emulowany YM2612, oryginalny zegar NTSC 7 670 454 Hz, pięć kanałów FM i perkusja przez emulowany 8-bitowy DAC szóstego kanału. Bez PSG w tej wersji. Generatywny sekwencer tworzy motywy, harmonię, bas, akordy i aranżację bez końca. Materiał jest oryginalny, bez muzyki czy sampli wyciągniętych z Sonica.

## Co przyspieszono

- Kod i tablice audio są kopiowane do RAM przed inicjalizacją emulatora i uruchomieniem audio. Zastosowano właściwe wywołania pośrednie między flash i RAM oraz uzupełniono istniejący guard wykonywania kodu. Nie poszerzono go na stosy i dane aplikacji.
- Usunięto powtarzane skanowanie wszystkich kanałów przy wyliczaniu wyjścia jednego kanału. Zachowano funkcję obliczeń czterech operatorów, maskowanie aktywności i nieliniowość DAC YM2612.
- Zapisy do DAC pomijają powtórzenie identycznej wartości.
- Profil aplikacji na stałe przeznacza kanał 6 na DAC. Nie kluczuje ani nie udostępnia jego operatorów FM, więc obliczenia ich faz/envelopes są pomijane. DAC i wspólne zegary nadal działają. To specjalizacja pięciu FM + DAC; nie należy jej używać jako uniwersalnego odtwarzacza dowolnych rejestrów/VGM przełączających szósty kanał między FM i DAC. Generyczny rdzeń bez opcji `EMERALD_FIXED_DAC` nadal odlicza sześć torów FM.
- W kompilacji procesora FM-1 wymuszono wbudowanie najczęściej wywoływanych funkcji zegara kanałów/operatorów w miejscu wywołania. Zmniejsza to koszt milionów wywołań bez zmiany ich obliczeń.
- Próg ochronny pozostał **85%**. Nie obniżono zegara układu ani liczby słyszalnych głosów. Komputerowy render pozostaje identyczny bit po bicie: SHA-256 `488849e65ddbc8178f98311bc487abba968b1e7846cfa4c75e24990153866605`.

Pierwsza wersja 0.1.0 dochodziła do 92% i wyciszała się przy XRUNS 0. Samo przeniesienie do RAM w 0.1.1 nie wystarczyło według odczytu użytkownika. W 0.1.2 pomiar USB pokazał 87%, XRUNS 0 i zadziałanie ochrony. 0.1.3 osiągnęła opisany wyżej wynik 80% bez zadziałania ochrony.

## Obsługa

Po rozruchu sekwencer jest zatrzymany. Ustaw małą głośność MASTER i naciśnij PLAY.

- PLAY: granie / pauza z łagodnym wyciszeniem.
- REC: nowy seed i początek nowej sekwencji.
- KNOB 1: tempo 80–180 BPM; KNOB 2: gęstość melodii; KNOB 3: mutacja; KNOB 4: transpozycja od -12 do +12 półtonów.
- SEQ przełącza mutację między 0 a 35; nie zamraża każdej losowej decyzji.
- MIDI Start/Continue uruchamia lub wznawia, Stop wycisza. Wejściowe nuty i klawiatura nie są jeszcze podłączone do silnika. Szczegóły USB: `YM-USB-PROTOCOL.md`.

Ekran pokazuje tempo, takt, CPU MAX % i XRUNS. Po ośmiu blokach powyżej 85% (po rozgrzaniu) silnik wycisza się do restartu; odczyt USB nie umożliwia obchodzenia ochrony. Ustawienia nie zapisują się po wyłączeniu. W tej wersji USB Audio jest wyłączone; odsłuch odbywa się przez głośnik lub wyjście słuchawkowe FM-1.

## Pliki, testy i budowanie

Firmware: `emerald-ym-0.1.3.fwsc`, 609 666 bajtów. Aplikacja: 97 040 bajtów. Kod/tablice audio w RAM: 19 372 bajty; cały region kodu RAM zajmuje 22 260 / 24 576 bajtów. Dane/BSS: 28 916 / 98 304 bajty. Pula grafiki: 69 120 / 344 064 bajty.

Przeszły kontrole mapy pamięci, zakresów i obrazu, brak procedur soft-double, kontrola kodu flash-off oraz symulowane testy OTA i loadera. Ponad 180 sekund renderowania na komputerze sprawdziło zgodność przy różnej wielkości bloków, ziarna, pauzę, skrajne parametry, brak alokacji podczas audio i brak trafień w limiter. Powyższy test urządzenia uzupełnia te kontrole, nie zastępuje ich.

`build-ym-windows.ps1` domyślnie buduje 0.1.3. Podaj `-Python`, `-Toolchain`, `-Sdk`; wymagane są Python z Pillow i oficjalny kompilator JieLi Windows 2.5.3 (clang 4.0.1, pi32v2 r3). Dołączony `vendor-sdk` zawiera trzy potrzebne, referencyjne pliki AC79 SDK i może zostać wskazany jako `-Sdk`. Testy komputerowe: `tools/test-ym-windows.cmd`. Weryfikacja pakietu: `tools/verify_ym_package.py`. Test na urządzeniu: `tools/ym_hardware_test.py --seconds 60`; ten ostatni otwiera wyłącznie port Emerald YM, weryfikuje wersję, wysyła opisany status query i standardowy MIDI Start/Stop. Pozostałe narzędzia budowania nie dotykają urządzenia.

Oryginalna baza: DUB-1 `e6fe7b8f8f3114019bb36b5b536ad458eacd2c2d`. Rdzeń ymfm: `81aec25ccbb98f4873a255f7551ac4dadac59b4a`, z opisanymi zmianami lokalnymi i wąską warstwą freestanding. `tools/prepare_ym.py` generuje sample i nagłówki, nie nadpisuje już istniejącej lokalnej kopii rdzenia.

## Aktualizacja i licencje

Zachowano istniejący mechanizm aktualizacji Felucca, watchdog, tryb awaryjny po nieudanych rozruchach i skrót OCT- + OCT+ przytrzymane 5 s. Nie gwarantuje to odzyskania urządzenia po każdej awarii lub przerwaniu programowania. Nie ma automatycznego rollbacku. Kolejne aktualizacje są oddzielną operacją; dostarczone narzędzia nie uruchamiają ich samoczynnie.

Cały połączony firmware: GPL-3.0-only (`LICENSE`, `LICENSING.md`). Rdzeń ymfm: BSD-3-Clause (`firmware/src/ym/ymfm/LICENSE`), lokalne zmiany zachowują tę licencję. Własny silnik/sekwencer i warstwa zgodności: MIT (`LICENSE-emerald.txt`). Czcionki: dołączone licencje. Składniki JieLi: Apache-2.0 (`JieLi-SDK-LICENSE.txt`). Archiwum zawiera odpowiadające źródła, skrypty, logi i binarny pakiet; nie zawiera kompilatora.
