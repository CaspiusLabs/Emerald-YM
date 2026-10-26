# Mapa projektu

| Obszar | Rola |
|---|---|
| firmware/src/ym/ym_engine.cpp | Strumieniowy sekwencer, 5 FM + DAC, konwersja próbkowania, stała arena inicjalizacji |
| firmware/src/ym/ym_psg.h | Od 0.2.0: profil Sega PSG, trzy tony i stały biały szum, przydział głosów live |
| firmware/src/ym/ymfm | Rdzeń ymfm i lokalne optymalizacje dla używanego profilu |
| firmware/src/ym/ym_tables.h | Zatwierdzone tablice strojenia i oryginalne krótkie sample |
| firmware/src/ym/main.c, ym_loop.c | Start, przerwania audio, watchdog, pomiar czasu, obsługa panelu i USB |
| firmware/src/ym/ym_ui.c, ym_leds.c | Kafelki UI i podświetlenie |
| firmware/hal | Obsługa procesora, wyświetlacza, panelu, DMA, flash i guardów |
| firmware/app.ld | Rozmieszczenie XIP, .ram_text, .audio_hot, danych, puli i stosów |
| firmware/loader, src/ota.c, src/usb.c | Zachowana platforma aktualizacji Felucca/DUB-1 |
| firmware/src/ym/ym_engine.cpp: YM_CONTROL_CODE | Od 0.2.3 leaf-settery/gettery pętli głównej w XIP `.ym_control`; bez wywołań z renderera |
| tools/build.py, fm1pkg_make.py | Kompilacja, kontrola obrazu i składanie .fwsc |
| tests/ym_stream_test.c, ym_ui_test.c | Aktywne testy nowego silnika i UI na komputerze |
| releases/v0.1.4 | Niezmienny sprawdzony pakiet, suma kontrolna i dowody weryfikacji |
| docs/history | Archiwalne opisy i migracja z bazy DUB-1 |
| .runtime | Lokalne narzędzia, pomijane przez Git; nie są częścią firmware'u |

Układ YM2612 pracuje z zegarem 7 670 454 Hz. Rdzeń generuje przy zegar/144, a aplikacja konwertuje do 44,1 kHz i wypełnia 256 ramek stereo w przerwaniu audio. Czas takiego bloku to około 5,8 ms. Ochrona po ośmiu blokach przekraczających 85% wycisza silnik do restartu.

Wybrany profil zawsze wykorzystuje kanał 6 jako DAC. Optymalizacja EMERALD_FIXED_DAC nie jest dowolnym odtwarzaczem wszystkich strumieni rejestrów YM2612; pięć torów FM i DAC są odtwarzane z zachowaniem porównanego wyniku. Rozszerzanie o szósty tor FM, odtwarzacz VGM lub inne tryby wymaga ponownego przemyślenia tej granicy.

Kod generujący dźwięk nie przydziela pamięci w czasie grania. Konstrukcja obiektów rdzenia odbywa się przed uruchomieniem IRQ, we własnej stałej arenie. W renderowaniu nie ma double ani libm. Kod i tablice audio są w RAM, osobno od funkcji flash-off, a wejścia z XIP używają far-call platformy.

UI korzysta z istniejącego canvasu 240 × 144 jako bufora kafelków. Nie ma pełnego dodatkowego framebufferu. CV jest synchronizowany z DMA przed ponownym zapisaniem. Logo jest rysowane kodem. Zwykła zmiana parametru nie czyści całego LCD.

Odziedziczone katalogi app/dsp, host i web nadal zawierają kod poprzedniego instrumentu DUB-1. Pozostają dla zależności platformy i zachowania pochodzenia, ale nie opisują bieżącego generatora. Wybór aplikacji odbywa się przez EN_YM2612. Dawny skrypt jednorazowej migracji jest zachowany wyłącznie jako plik tekstowy w docs/history; nie uruchamiaj go do przygotowywania nowej wersji.
