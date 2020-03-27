# Całkowanie z zastosowaniem standardu MPI

Równoległe obliczanie całki oznaczonej z zastosowaniem metod numerycznych.

## Treść zadania

Proszę policzyć przy pomocy metody prostokątów/trapezów/simpsona (możecie Państwo wybrać metodę) całkę oznaczoną funkcji jednej zmiennej. Procedura całkowania powinna przyjmować: wskaźnik na całkowaną funkcję, początek i koniec przedziału całkowania oraz liczbę punktów dla których obliczana jest wartość funkcji. Oto jej nagłówek:

```c
double integrate(double (*func)(double), double begin, double end, int num_points);
```

Całkowanie powinno być wykonane równolegle przy użyciu standardu MPI. Wybrany proces główny rozdziela przedział całkowania oraz liczbę punktów, w których będą wykonywane obliczenia pomiędzy siebie i pozostałe procesy. Rozsyła potrzebne informacje do pozostałych procesów oraz wykonuje swoją część obliczeń. Następnie zbiera wyniki od pozostałych procesów i sumuje je ze swoim wynikiem cząstkowym. Ostatecznie wypisuje wynik całkowity na standardowe wyjście. Proszę zwrócić uwagę na to, że każdy z procesów powinien liczyć całkę dla pewnej liczby punktów, która została mu przydzielona przez proces główny (suma punktów z wszystkich procesów powinna być równa liczbie podanej jako argument wejściowy) – zatem proces główny powinien dokonać rozsądnego podziału danych wejściowych pomiędzy procesy.

Program powinien pozwalać na ustawienie początku i końca przedziału całkowania oraz całkowitej liczby punktów. Program powinien działać poprawnie dla dowolnej, zadanej przez prowadzącego, liczby procesów oraz liczby punktów. Komunikacja pomiędzy procesami powinna zostać zrealizowana jedynie przy użyciu funkcji (`MPI_Send`, `MPI_Recv`).

Interfejs programu powinien wyglądać następująco:

`./integrate begin end num_points`

### Zadania do wykonania w ramach ćwiczenia

1. Napisanie procedury całkowania
2. Podział liczby punktów na poszczególne procesy (możliwie równy!)
3. Podział przedziałów całkowania dla poszczególnych procesów
4. Rozesłanie do procesów „robotników” danych umożliwiających obliczenia cząstkowe.
5. Odbiór danych przez procesy.
6. Wykonanie obliczeń w procesie głównym.
7. Wykonanie obliczeń w procesach „robotnikach”
8. Przesłanie obliczeń cząstkowych do procesu głównego.
9. Zsumowanie wszystkich wyników i wyświetlenie wyniku końcowego.
