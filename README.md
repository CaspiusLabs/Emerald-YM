# Emerald YM

Generatywny instrument dla M-VAVE FM-1, inspirowany brzmieniem Mega Drive/Genesis. Emuluje YM2612: pięć głosów FM i perkusja przez DAC. Własna muzyka, UI i znak Emerald.

**Baza do dalszej rozbudowy: v0.1.4**, zaakceptowana na urządzeniu. Tożsamość FM-1_68000104. Sprawdzony pakiet i dowody są w releases/v0.1.4. W próbach: CPU MAX 80%, XRUNS 0; użytkownik potwierdził dźwięk, ekran bez migotania, LED-y oraz wskaźniki pokręteł. Zakres i ograniczenia: [docs/VERIFICATION.md](docs/VERIFICATION.md).

## Szybki start na tym Windowsie

Otwórz PowerShell w folderze projektu:

    .\build-ym-windows.ps1
    .\test-ym-windows.ps1

Pierwsze polecenie wyłącznie buduje; drugie uruchamia testy komputerowe i symulacje. Żadne nie otwiera portu MIDI ani nie wgrywa firmware'u. Nowy build 0.1.4 porównywany jest bit po bicie z zachowanym pakietem.

Lokalna .runtime zawiera gotowy Python, toolchain i minimalny SDK, niezależne od folderu dawnego czatu. Katalog jest pomijany w Git. Po sklonowaniu repo na inną maszynę trzeba uzupełnić te zależności; opis: [BUILDING.md](BUILDING.md).

## Obsługa instrumentu

- PLAY: granie / pauza.
- REC: nowy seed.
- SEQ: przełączenie mutacji 0 / 35.
- K1: tempo, K2: gęstość, K3: mutacja, K4: transpozycja.
- MASTER: głośność.

Ekran pokazuje wartości, paski, takt, kroki, CPU MAX i XRUN. Świecą używane przyciski i ich stany. Szczegółowy opis zaakceptowanej wersji: [docs/releases/v0.1.4.md](docs/releases/v0.1.4.md).

## Rozbudowa i Git

- Główna gałąź: main. Baza: oznaczony tag v0.1.4.
- Twórz gałąź do konkretnej funkcji, np. git switch -c feature/song-structure.
- Numer nowego wydania jest w VERSION. Zachowaj releases/v0.1.4 i tag bez podmiany.
- Historia bazy DUB-1 jest zachowana. Stare tagi v1.x pochodzą z DUB-1; nie są numerami Emerald YM.
- Pierwszy commit Emerald importuje stan 0.1.4, a kolejny porządkuje samodzielny projekt. Wcześniejsze eksperymenty opisano w CHANGELOG; nie sfabrykowano ich commitów.
- Remote upstream służy do odczytu publicznej bazy DUB-1. Push do niego jest wyłączony. Nie utworzono ani nie opublikowano repozytorium na GitHub.
- Eksport źródeł i pełnej historii lokalnej: .runtime\python\python.exe tools\export_ym.py. Wynik trafia do ignorowanego dist.

[AGENTS.md](AGENTS.md) zawiera instrukcje dla kolejnych sesji Codex. [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) pokazuje aktywny kod i jego ograniczenia, [CHANGELOG.md](CHANGELOG.md) historię, a [YM-USB-PROTOCOL.md](YM-USB-PROTOCOL.md) protokół diagnostyczny.

## Projekt w aplikacji Codex

Wybierz dodanie lokalnego projektu/folderu i wskaż ten katalog Emerald-YM. Repozytorium na dysku jest już przygotowane; zarejestrowanie go na pasku bocznym jest oddzielnym krokiem. Do budowania wystarczy wskazany folder; ten czat nie musi pozostać otwarty.

## Pochodzenie i licencje

Baza DUB-1/Felucca, rdzeń ymfm Aarona Gilesa z lokalnymi optymalizacjami. Połączony firmware: GPL-3.0-only. Rdzeń: BSD-3-Clause. Własny silnik i adapter: MIT. SDK: Apache-2.0. Szczegóły i autorstwo: LICENSE, LICENSING.md oraz dołączone pliki licencji.

Praca nad projektem nie wymaga wysyłania żadnych plików do zdalnego repozytorium. Test urządzenia i instalacja pozostają osobnymi, świadomie uruchamianymi działaniami.
