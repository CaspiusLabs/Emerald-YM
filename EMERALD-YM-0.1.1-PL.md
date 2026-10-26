# Emerald YM 0.1.1 — optymalizacja po pierwszym teście FM-1

Wersja 0.1.0 uruchomiła się na urządzeniu. Użytkownik zgłosił CPU MAX 92%, XRUNS 0 i komunikat OVERLOAD AUDIO STOPPED. Zadziałał próg ochronny 85%; nie zgłoszono przekroczenia terminu bufora. Nie jest to jeszcze potwierdzenie stabilnego grania.

W 0.1.1 przeniesiono kod YM2612, sekwencera, tablice i krótkie sample z XIP do szybkiej RAM. Nie zmieniono rdzenia obliczeń FM, oryginalnego zegara NTSC, liczby głosów ani progu ochronnego. Render komputerowy pozostaje identyczny bit po bicie.

## Pakiet i pamięć

- Pakiet: `emerald-ym-0.1.1.fwsc`, tożsamość `FM-1_68000101`.
- Aplikacja: 96 696 bajtów.
- Kod i tablice audio w RAM: 19 500 bajtów. Razem z kodem obsługi flash: 22 388 / 24 576 bajtów regionu RAMTEXT.
- Dane i BSS: 28 656 / 98 304 bajtów. Pula grafiki: 69 120 / 344 064 bajtów.
- Nowa sekcja `.audio_hot` jest kopiowana z obrazu do RAM przed inicjalizacją emulatora i uruchomieniem przerwań audio. Guard wykonywalnej pamięci RAM obejmuje ciągły region kodu flash i audio; stosy i dane aplikacji pozostają poza nim. Wywołania z XIP do RAM używają mechanizmu pośredniego platformy, aby nie przekroczyć zakresu bezpośredniego skoku procesora.
- Oddzielna sekcja `.ram_text` obsługująca flash nadal przechodzi kontrolę braku wywołań. Kod audio nie jest wykonywany podczas programowania flash.

## Testy przed wgraniem

Przeszły kontrole mapy pamięci i obrazu, brak soft-double, testy strumieniowania (deterministyczność przy różnej wielkości bloków, zmiana seedu, pauza, skrajne parametry, brak alokacji podczas audio), symulacja OTA i loadera dla zmiany obrazu z 0.1.0 na 0.1.1. Przebieg testowy nie trafiał w limiter. SHA-256 renderu WAV w obu wersjach: `488849e65ddbc8178f98311bc487abba968b1e7846cfa4c75e24990153866605`.

## Test sprzętowy

Po wgraniu ustaw małą głośność i naciśnij PLAY. Po 20–30 sekundach odczytaj CPU MAX %, XRUNS i ewentualny komunikat. Zmiana ma poprawić czas renderowania; jej skuteczność na FM-1 wymaga ponownego pomiaru i nie wynika z samego przeniesienia kodu.

Wersja startuje z zatrzymanym sekwencerem. PLAY włącza/wyłącza granie; REC wybiera nowy seed. K1 reguluje tempo, K2 gęstość, K3 mutację, K4 transpozycję. Ochrona po ośmiu blokach powyżej 85% wycisza silnik do restartu. Poprzednia instrukcja `EMERALD-YM-PL.md` opisuje pozostałe funkcje i ograniczenia wersji eksperymentalnej; jej informacja o nieprzeprowadzonej instalacji dotyczy stanu przed pierwszym testem 0.1.0.

## Budowanie

`build-ym-windows.ps1` domyślnie buduje teraz wersję 0.1.1. Wskaż parametry `-Python`, `-Toolchain`, `-Sdk` odpowiadające lokalnym narzędziom. Źródła zawierają tylko trzy potrzebne pliki SDK w `vendor-sdk`, więc ten katalog może być wskazany jako `-Sdk`. Wymagany jest oficjalny kompilator JieLi Windows i Python z Pillow. Testy: `tools/test-ym-windows.cmd`, weryfikacja pakietu: `tools/verify_ym_package.py`. Narzędzia budowania, eksportu i testów nie otwierają portów MIDI. Instalacja jest oddzielną operacją `tools/fm1_install.py`.

Licencje: cały połączony firmware GPL-3.0-only; ymfm BSD-3-Clause; własny silnik i warstwa zgodności MIT; pliki SDK Apache-2.0; czcionki według dołączonych licencji. Zachowano odpowiednie teksty i źródła z wersji 0.1.0.
