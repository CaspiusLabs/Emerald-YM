# Historia Emerald YM

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
