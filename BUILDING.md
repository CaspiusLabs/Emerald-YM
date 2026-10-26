# Budowanie Emerald YM na Windows

## Gotowe środowisko lokalne

    .\build-ym-windows.ps1
    .\test-ym-windows.ps1

VERSION określa numer pakietu; bazą jest 0.1.4. Wynik: build/emerald-ym-0.1.4.fwsc. Logi, obrazy testów UI i render WAV są w ignorowanym build/. Test pakietu wymaga identyczności ze sprawdzonym releases/v0.1.4/emerald-ym-0.1.4.fwsc. Nie podmieniaj tego pliku, jeśli nowy kod zmienia zachowanie — nadaj nową wersję.

Skrypty budowania/testów komputerowych nie używają urządzenia USB/MIDI. Nie uruchamiaj starych tests/run_tests.sh lub host/build_host.sh jako walidacji nowego instrumentu: dotyczą również dawnego DUB-1.

## Zależności

Lokalnie przygotowano:

- .runtime/python: przenośny Python 3.12.10, Pillow 13.0.0, mido 1.3.3, python-rtmidi 1.5.8, packaging 26.3.
- .runtime/toolchain: oficjalny JieLi Windows 2.5.3, clang 4.0.1, pi32v2 r3.
- .runtime/sdk: uboot.boot, cfg_tool.bin i cfg/eq_cfg_hw.bin z AC79NN_SDK_V1.2.1_2023-12-13; budowanie sprawdza referencyjne sumy.
- Zainstalowane Visual Studio / Build Tools z C++, wykrywane przez vswhere; są potrzebne do testów desktopowych, nie do samego firmware'u.

Narzędzia są pomijane w Git, nie kopiują się w zwykłym clone ani w eksporcie źródeł. Ich wersje i źródła: tools/runtime-manifest.json. Oficjalny toolchain: https://pkgman.jieliapp.com/s/win-toolchain . SDK: https://gitee.com/Jieli-Tech/fw-AC79_AIoT_SDK . Python: https://www.python.org/ftp/python/3.12.10/ .

Przy przygotowaniu innego komputera umieść przenośne narzędzia w powyższej strukturze lub podaj własne ścieżki:

    .\build-ym-windows.ps1 -Python C:\tools\python\python.exe -Toolchain C:\tools\jieli -Sdk C:\tools\ac79

Nie trzeba instalować WSL ani Dockera dla sprawdzonego wariantu Windows. Kompilator może wymagać uruchomienia poza ograniczeniami procesu agenta; to nadal lokalne budowanie, nie programowanie urządzenia.

## Kontrole

Budowanie sprawdza mapę pamięci, rozmiary obrazu i RAM, kod flash-off, zakresy oraz brak soft-double. Testy desktopowe obejmują deterministyczność i strumieniowanie dźwięku, renderowanie UI, symulacje OTA/loadera i udawany transport instalatora. Są odpowiednie dla tego projektu, lecz nie dowodzą czasu wykonania na procesorze FM-1.

Historyczne dowody sprzętowe i ograniczenia opisano w docs/VERIFICATION.md. Oryginalna instrukcja bazowego DUB-1 jest w docs/history/dub-1/BUILDING.md i nie jest bieżącym quickstartem.

## Eksport i powrót do bazy

Po zapisaniu zmian w Git:

    .runtime\python\python.exe tools\export_ym.py --ref v0.1.4

Powstają archiwum źródeł wskazanego commitu/tagu, lokalny Git bundle i sumy kontrolne w dist/. Bundle przechowuje historię Git bez zależności .runtime.

Do sprawdzenia bazy bez nadpisywania bieżącej pracy:

    git switch -c inspect-v0.1.4 v0.1.4

Wgrywanie nie jest częścią tych poleceń. Gdy jest zlecone, używa się tools/fm1_install.py z konkretnym portem FM-1 i sprawdzonym pakietem. Protokół diagnostyki i MIDI Start/Stop opisuje YM-USB-PROTOCOL.md.
