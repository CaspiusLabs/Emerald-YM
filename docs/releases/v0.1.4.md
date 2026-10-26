# Emerald YM 0.1.4 — ekran i podświetlenie

Wersja została wgrana na FM-1, tożsamość `FM-1_68000104`. Użytkownik potwierdził, że ekran nie migocze, podświetlenie działa, a cztery pokrętła zmieniają widoczne wartości i paski parametrów.

## Zmiany

- Usunięto cykliczne czyszczenie całego widocznego ekranu. Każdy fragment powstaje w istniejącym buforze canvas i jest wysyłany do LCD jako gotowy kafelek. Niezmienione fragmenty nie są odrysowywane; pełne czyszczenie jest wykonywane tylko przy pierwszym rysowaniu lub jawnym unieważnieniu ekranu, np. po wyjściu z aktualizatora.
- Cztery karty: K1 tempo (80–180 BPM), K2 gęstość, K3 mutacja, K4 transpozycja (-12…+12 półtonów). Duże liczby, jednostki i paski, w tym dwukierunkowy wskaźnik transpozycji.
- Status PLAYING/PAUSED, numer taktu, pasek 16 kroków oraz CPU MAX i XRUN.
- Własny znak Emerald: fasetowany klejnot, jasnoniebieska typografia, ciemnogranatowe panele i plakietka 16-BIT w stylistyce Mega Drive/Genesis. Grafika jest rysowana kodem, bez dodatkowego obrazu lub bufora.
- Podświetlone są używane przyciski: PLAY zielony podczas grania i przygaszony w pauzie; REC przygaszony z krótkim rozjaśnieniem po nowym seedzie; SEQ rozjaśniony przy mutacji 0, przygaszony przy aktywnej mutacji. PLAY czerwony przy przeciążeniu. OCT-/OCT+ rozjaśniają się po równoczesnym naciśnięciu skrótu aktualizacji. Pozostałe klawisze nie udają funkcji, których ta wersja nie ma.
- Odczyt panelu co 20 ms. Kod wysyła zmiany parametrów do silnika tylko wtedy, gdy wartość się zmieniła. Tablice LED są publikowane gotowymi wierszami, bez chwilowego zerowania całego podświetlenia.

## Weryfikacja

Ten sam kod renderera, czcionki i canvas uruchomiono na komputerze z pamięciowym odpowiednikiem LCD. Potwierdzono brak zapisów LCD bez zmiany stanu, dokładnie jeden kafelek po zmianie parametru, mały pasek po zmianie kroku oraz brak czyszczenia widocznego ekranu podczas normalnej pracy. Sprawdzono granice wyjścia i podglądy stanów pauzy, odtwarzania, transpozycji oraz przeciążenia. Log: `build/ui-tests-0.1.4.log`; przykładowy podgląd: `build/host/ui-playing.png`.

Na FM-1 zebrano 62 próbki diagnostyki w ciągu około 60,6 s. W 48 próbkach odtwarzanie było aktywne; użytkownik w tym czasie testował panel, przełączał odtwarzanie i wybrał nowy seed. Maksimum **80%**, **XRUNS 0**, **0 bloków ponad próg 85%**, **0 nieudanych rozruchów**; najdłuższy blok 4653 mikrosekundy. Surowy automatyczny raport ma `passed=false`, ponieważ jego pierwotne kryterium wymagało końcowego numeru taktu większego od początkowego, a REC zgodnie z funkcją zresetował takt do 1. Nie jest to błąd audio: zachowano pełny ślad i ten wynik, a próbę sklasyfikowano jako interaktywną. Nie przedstawiamy jej jako pełnej minuty nieprzerwanego odtwarzania. Wersja 0.1.3 z tym samym algorytmem dźwięku przeszła wcześniej pełną minutę bez przerw.

Użytkownik następnie potwierdził poprawę ekranu, podświetlenie i aktualizację wskaźników przy obracaniu K1–K4. Długie granie i wszystkie kombinacje ustawień nie są wyczerpująco sprawdzone.

## Pakiet i pamięć

`emerald-ym-0.1.4.fwsc`: 609 666 bajtów. Aplikacja: 98 880 bajtów. Dane/BSS: 28 996 / 98 304 bajty. Kod i tablice audio w RAM: 19 392 bajty; cały region RAMTEXT 22 280 / 24 576 bajtów. Pula canvas: 69 120 / 344 064 bajty, bez zwiększenia rozmiaru.

Rdzeń dźwięku pozostaje opisaną w `EMERALD-YM-0.1.3-PL.md` specjalizacją prawdziwej emulacji YM2612: pięć FM + stały DAC, oryginalny zegar NTSC i ochronny próg 85%. Dodano tylko odczyt aktualnego kroku do wskaźnika. Nie zmieniono algorytmu syntezy ani sekwencjonowania.

## Budowanie i testy

`build-ym-windows.ps1` domyślnie buduje 0.1.4. Wymagania i licencje jak w `EMERALD-YM-0.1.3-PL.md`: oficjalny toolchain JieLi Windows, Python z Pillow, trzy referencyjne pliki SDK dołączone w `vendor-sdk`. Wskaż `-Python`, `-Toolchain` i `-Sdk`. Test UI: `tools/test-ui-windows.cmd`; testy dźwięku/OTA: `tools/test-ym-windows.cmd`; weryfikacja pakietu: `tools/verify_ym_package.py`; diagnostyka sprzętu: `tools/ym_hardware_test.py`. Ostatnie narzędzie otwiera tylko zweryfikowany port Emerald YM i używa opisanego `YM-USB-PROTOCOL.md` oraz MIDI Start/Stop. Budowanie i eksport nie uruchamiają instalacji.

Instalacja korzysta z zachowanego mechanizmu Felucca i pozostaje osobną operacją. Nie ma gwarantowanego automatycznego rollbacku. Źródła, logi, mapowanie pamięci i licencje są w archiwum. Połączony firmware GPL-3.0-only, ymfm BSD-3-Clause, własny silnik i adapter MIT, pliki SDK Apache-2.0, czcionki według dołączonych licencji.
