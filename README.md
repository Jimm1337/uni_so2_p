# Gra typu space invaders
### Projekt SO2

## BUILD
```bash
$ mkdir build && cd build
$ cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=ninja
$ ninja
```

## RUN
```bash
$ ./src/space_invaders
```

## OPIS
Wielowątkowe space invaders, napisane w stylu obiektowym.

### WSPÓLNE SEKCJE:
- GPU
- KBD
- INTERAKCJE POMIĘDZY OBIEKTAMI

### WĄTKI:
Każdy obiekt ma swój wątek któy jest odpowiedzialny za jego updatowanie.
