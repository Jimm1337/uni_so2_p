# Gra typu space invaders

### Projekt SO2

## DEMO:
[DEMO_VIDEO (GDRIVE)](https://drive.google.com/file/d/13uWfqpQ1nMKCuiGQmNLuhnzP8vwH-rDa/view?usp=drive_link)

## BUILD:

CMAKE

#### UWAGI:

- Wszystkie biblioteki się same pobiorą i skompilują dla tego projektu, nie trzeba nic pobierać i instalować.
- Nie działa (jest bardzo wolno) po skompilowaniu używając MSVC, pewnie chodzi o implementację std::jthread i std::
  recursive_mutex w libc++
  microsoftu. Należy wybrać normalny kompilator.

## OPIS:

Wielowątkowy 2D shooter, napisany w stylu obiektowym. Strzelamy do przeciwników za pomocą spacji, mamy cooldown .5
sekundy, przeciwnicy strzelają do nas w losowym czasie. Gra kończy się jak skończą nam się życia, po utracie życia
cofamy się na środek. Gra korzysta z tekstur i fontów z folderu res.

## STEROWANIE:

- A - lewo
- D - prawo
- SPACE - strzał

## WSPÓLNE SEKCJE/ZASOBY:

- GPU (1 kontekst)
- INTERAKCJE POMIĘDZY OBIEKTAMI

## WIELOWĄTKOWOŚĆ:

Każdy obiekt ma swój wątek któy jest odpowiedzialny za jego updatowanie. Każdy obiekt ma swój mutex który jest lockowany
za pomocą std::unique_lock w praktycznie każdej jego funkcji. Dostęp do gpu jest regulowany statycznym mutexem w klasie
po której dziedziczą wszystkie inne więc nie ma problemu z dostępem do gpu (statyczne w klasie bazowej są jedne
wszystkich instancji klas potomnych). Updatowanie korzysta z yield wątków updatujących i wątka głównego (rysującego) w
odpowiednich momentach (linux sobie raczej radzi bez tego ale scheduler windowsa ma problem).

## ROZWÓJ PROJEKTU:

- można dodać przeszkody
- można sprawić że przeciwnicy będą się ruszać co jakiś czas
- można poprawić wykorzystanie delta time (nie jest idealne)
- można dodać restartowanie gry po przegranej

## STRUKTURA PROJEKTU:
### Główna klasa:
- GAME

### Komponenty:
> - OBJECT
>> - SPRITE
>> - PROJECTILE
>> - TEXT

> - OBJECT
>> - GRID
>>> - CANVAS
