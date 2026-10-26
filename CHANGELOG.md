# Historia Emerald YM

## 0.2.4 — 2026-10-26 — indywidualne poziomy

- Po doprecyzowaniu użytkownika: SELECT wybiera instrument, ALGORITHM zmienia
  tylko jego poziom. 12 źródeł: 5 FM, 4 próbki perkusji, PSG ARP, LIVE, NOISE.
- Każde źródło 0–100% domyślnego balansu; PRESETS pozostaje magistralą perkusji.
- Nowy kafelek pokazuje nazwę i poziom wybranego instrumentu. Wybór nie zmienia
  zapisanych w RAM poziomów innych źródeł. CPU/XRUN nadal tylko przez USB.
- Oddzielne testy słyszalności wszystkich 12 źródeł i wyciszania nut live.
- Domyślny balans bitowo identyczny z zaakceptowaną 0.2.2. Tablica strojenia PSG
  przeniesiona do kontrolnego XIP; renderer i okresy tonów nadal w RAM.
- Szczegóły i zakres pomiarów: docs/releases/v0.2.4.md.


## 0.2.3 — 2026-10-26 — mikser melodii i perkusji

- Użytkownik zaakceptował poziom PSG i regulację ALGORITHM w 0.2.2.
- SELECT: 0–100% pięciu melodycznych torów FM, łącznie z basem i akordami.
- PRESETS: 0–100% perkusji DAC i szumu PSG. ALGORITHM pozostaje mikserem PSG.
- Trzy pola z wartościami i paskami zamiast CPU/XRUN i dawnego wiersza podpowiedzi.
  Diagnostyka USB, liczniki i próg ochronny 85% pozostają aktywne.
- Głośność FM nakładana przed dodaniem DAC; przy poziomach 100% dźwięk jest
  bitowo identyczny z 0.2.2. Testowane osobne ścieżki i wyciszanie.
- Leaf-settery/gettery pętli głównej przeniesione do XIP `.ym_control`;
  bez zmiany granic RAM. Linker i skrypt budowania kontrolują adresy.
- Próba na urządzeniu: 30 s ciągłego grania z dwiema/trzema nutami live,
  CPU MAX 84%, XRUNS 0, overload 0. Szczegóły: docs/releases/v0.2.3.md.

## 0.2.2 — 2026-10-26 — wersja rozwojowa

- Po drugim odsłuchu: regulacja miksu PSG pokrętłem ALGORITHM, 0–400%,
  startowo 300% (+9,54 dB względem 0.2.1). Obejmuje arp, live i szum.
- Aktualny poziom widoczny w dolnym kafelku; bez powtarzanych transmisji LCD
  przy braku zmian i bez automatycznego zapisu ustawień.
- Test miksu z trzema głosami live przy 400% bez trafień w limiter.
- Wzór arpeggio 0.2.1 zachowany. Szczegóły: docs/releases/v0.2.2.md.

## 0.2.1 — 2026-10-26 — poprawa arpeggio

- Arpeggio PSG głośniejsze o 6 dB, oktawę niżej, dłuższe nuty i stały wzór
  0–1–2–1 zgodny z akordem podkładu.
- Użytkownik: lepiej muzycznie, lecz nadal za cicho. Podświetlenie klawiszy
  potwierdzone wcześniej w 0.2.0.
- Próba interaktywna ze stress MIDI zachowuje passed=false (PLAY/REC).
  Osobny czysty podkład: 60 sekund ciągłego grania, passed=true, XRUNS 0,
  overload 0; maksimum od bootu 84% obejmuje także wcześniejsze głosy live.
- Skrypt testu zachowuje częściowy raport przy błędzie transportu i pozwala
  wymagać ciągłego grania. Szczegóły: docs/releases/v0.2.1.md.

## 0.2.0 — 2026-10-26 — PSG i klawiatura live

- Własny profil Sega PSG NTSC: trzy dziesięciobitowe dzielniki tonu,
  tłumienie 2 dB, 16-bitowy LFSR i zegar 3579545 Hz. Stały biały szum clock/512.
- Generatywne arpeggio PSG i szum perkusyjny bez odbierania torów FM.
- Klawiatura FM-1 i USB MIDI F3–G5: do trzech głosów live, pierwszeństwo
  nowych klawiszy i powrót do wcześniej trzymanych nut. Live działa w pauzie.
- ARP przełącza PSG. Trzy głosy live zastępują tylko arpeggio PSG.
- LED nut: sekwencer słabo, live jasno. Zachowane gotowe wiersze i kafelkowe UI.
- Testy PSG, allocatora, reseed, zwalniania nut, LED, UI, OTA i loadera.
- Mapa pamięci, próg 85%, bloki 256 i profil pięć FM + stały DAC bez zmian.
- Końcowa próba interaktywna: CPU MAX 84%, XRUNS 0, 47 aktywnych odczytów
  z 60. Osobny pomiar ciągły przerwany timeoutem statusu USB, niezaliczony.
- Pierwszy prototyp przed optymalizacją dochodził do 85%; zachowano rozróżnienie
  wyników. Szczegóły i uwagi odsłuchowe: docs/releases/v0.2.0.md.

## 0.1.4 — 2026-10-26 — zaakceptowana baza

- Prawdziwa emulacja YM2612, pięć głosów FM i perkusja przez DAC; generatywny sekwencer.
- Optymalizacje RAM, pętli operatorów, pojedynczego kanału wyjściowego i powtarzanych zapisów DAC. Oryginalny zegar NTSC i próg 85% zachowane.
- Ekran z odświeżaniem tylko zmienionych kafelków, czterema wskaźnikami parametrów, paskiem kroków, własnym znakiem Emerald i plakietką 16-BIT.
- Podświetlenie PLAY/REC/SEQ i wskazanie ich stanu.
- Diagnostyka USB oraz MIDI Start/Continue/Stop.
- Użytkownik potwierdził dźwięk, ekran bez migotania, LED-y i pokrętła. W próbie interaktywnej: CPU MAX 80%, XRUNS 0, brak przeciążenia. Poprzedni pełny test ciągły 0.1.3: ponad minuta, również 80% / 0 XRUNS.
- Przy porządkowaniu projektu dodano samodzielny lokalny runtime, powtarzalne budowanie, testy, dokumentację, zachowany pakiet i tag v0.1.4. Kod firmware'u nie został przez to zmieniony.

## Wcześniejsze eksperymenty — 2026-10-26

- 0.1.0: pierwsze uruchomienie na FM-1; zgłoszone 92% / 0 XRUNS, zadziałanie ochrony.
- 0.1.1: przeniesienie kodu i tablic do RAM; samo to nie wystarczyło według użytkownika.
- 0.1.2: optymalizacja profilu pięciu FM + DAC i dodanie odczytu statusu; 87% / 0 XRUNS, nadal próg ochronny.
- 0.1.3: wbudowanie często wywoływanych funkcji zegarowych; udany ciągły test i potwierdzony odsłuch.

Te etapy powstały przed utworzeniem osobnego repozytorium Emerald YM i nie są przedstawiane jako osobne historyczne commity. Zachowano ich opisy i dostępne ślady w docs/history. Pierwszy rzeczywisty commit Emerald YM importuje kompletną, działającą wersję 0.1.4 na bazie oryginalnej historii DUB-1.
