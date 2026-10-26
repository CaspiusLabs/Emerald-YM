# Co jest sprawdzone

## Rozwinięcie 0.2.4

SELECT wybiera jedno z 12 źródeł, ALGORITHM zmienia tylko jego poziom.
Test słyszalności wyklucza początkowy stan przejściowy: każde źródło jest
słyszalne po wyciszeniu pozostałych jedenastu. Domyślny balans nadal jest
bitowo identyczny z 0.2.2. Końcowy test fizyczny: 30 s ciągłego podkładu
z dwiema/trzema nutami live, CPU MAX 85%, XRUNS 0, overload 0. Margines jest
bardzo mały; nie jest to test wszystkich kombinacji regulatorów. Szczegóły
i rozróżnienie prototypu/końcowego pakietu: docs/releases/v0.2.4.md.

## Rozwinięcie 0.2.3

SELECT/PRESETS regulują osobno FM i perkusję. Unity miks zachowuje bitowo
brzmienie 0.2.2; dodatkowe testy potwierdzają osobne ścieżki i wyciszanie.
CPU/XRUN są ukryte na ekranie, lecz liczniki i ochrona pozostają. Testy UI
potwierdzają brak odświeżeń od zmian tych liczników i pojedyncze kafelki miksera.
30 s ciągłego podkładu z dwiema/trzema nutami live: CPU MAX 84%, XRUNS 0,
overload 0, passed=true. Szczegóły w docs/releases/v0.2.3.md.

## Rozwinięcie 0.2.2

Nowy profil PSG, głosy live i LED-y mają osobne testy desktopowe; opis w
docs/releases/v0.2.2.md. Bieżący pakiet wgrano na zweryfikowany Emerald YM FM-1.
30 s ciągłego sekwencera z dwiema/trzema nutami live: CPU MAX 84%, XRUNS 0,
overload 0. Miks 400% bez limiter hits sprawdzono na komputerze, nie jako
osobny test wszystkich ustawień urządzenia. RAMTEXT ma tylko 68 B zapasu.
Nie przenosimy oceny akceptacji 0.1.4 automatycznie na 0.2.2.

## Zaakceptowana baza 0.1.4

Wersja 0.1.4 została wgrana na egzemplarz użytkownika, identyfikator FM-1_68000104. Użytkownik potwierdził muzykę, brak migotania, LED-y i wskaźniki pokręteł. Binarna baza ma SHA-256:

```
5fcae705878056c52588b4af9a98902a556c26488926a98d0cbcd7e6e79bd52a
```

`releases/v0.1.4/hardware-summary.json` zawiera wynik podsumowania. Surowy `hardware-interactive-trace.json` zachowuje `passed=false`: użytkownik testował PLAY i REC, więc takt wrócił do 1 i nie spełnił starego kryterium końcowego numeru taktu. W 62 próbkach, przez około 60,6 s, odtwarzanie było aktywne w 48 próbkach; CPU MAX 80%, XRUNS 0, brak przeciążenia. To próba interaktywna, a nie minuta nieprzerwanego grania. Pełny wcześniejszy ciągły test 0.1.3 z tym samym algorytmem jest w docs/history/hardware-0.1.3-continuous.json.

Kontrole komputerowe obejmują deterministyczność dźwięku przy różnych wielkościach bloków, warianty seedów, pauzę, skrajne parametry, stałe zużycie areny oraz granice sygnału. Test UI uruchamia rzeczywisty renderer z tymi samymi czcionkami przeciw pamięciowemu LCD: brak zbędnych transmisji, atomowe kafelki, brak okresowego czyszczenia i granice ekranu. Testy OTA i loadera symulują transport/flash; nie programują urządzenia.

Po skopiowaniu projektu do samodzielnego folderu powtórzono budowanie z lokalnej .runtime oraz testy. Nowy pakiet jest identyczny bit po bicie z zachowaną bazą. Wyniki nowych uruchomień zapisują się w ignorowanym build/, a nie nadpisują historycznych dowodów wydania.

Nie potwierdzono wszystkich kombinacji parametrów, wielogodzinnego grania ani odporności na przerwanie programowania. Test symulowany nie zastępuje fizycznego pomiaru. Wydania roboczego nie uznawaj za poprawę tylko dlatego, że kompiluje się lub podniesiono próg ochronny.
