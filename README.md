# Całkowanie z zastosowaniem standardu MPI

Równoległe obliczanie całki oznaczonej z zastosowaniem metod numerycznych.

## Treść zadania

Proszę policzyć przy pomocy metody prostokątów/trapezów/Simpsona (możecie Państwo wybrać metodę) całkę oznaczoną funkcji jednej zmiennej. Procedura całkowania powinna przyjmować: wskaźnik na całkowaną funkcję, początek i koniec przedziału całkowania oraz liczbę punktów dla których obliczana jest wartość funkcji. Oto jej nagłówek:

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

## Realizacja zadania

> Podczas pracy z _Open MPI_ zainstalowanym przy pomocy brew po zakończeniu programu pojawiał się błąd. Pomogło ustawienie zmiennej środowiskowej `PMIX_MCA_gds=hash`

Głowna cześć zadania jest realizowane przez funkcję `double integrate(double (*func)(double), double begin, double end, int num_points)`.

Jej wykonanie przebiega różnie w zależności od tego czy aktualny proces jest procesem głównym.
Proces główny przygotowuje punkty, w których będzie liczona wartość funkcji oraz możliwie równy podział wygenerowanych punktów między siebie i pozostałe procesy.
Do podziału został wykorzystany algorytm, który napisałem na potrzeby wcześniejszych zadań.  
Po przygotowaniu podziału proces główny przesyła do wszystkich pozostałych punkty, na których mają one dokonać obliczeń. Jest to wykonane za pomocą 2 instrukcji `MPI_Send()`, gdzie pierwsza przesyła ilość danych a druga dane.

W między czasie procesy nie będące procesem głównym oczekują na dane przy pomocy funkcji `MPI_Recv()`.

Po otrzymaniu danych, każdy proces wykonuje na nich funkcję `integrateRange()`, która liczy wartość całki korzystając z podanych punktów jako węzłów. Po zakończeniu obliczeń procesy przesyłają swoje wyniki do procesu głównego, gdzie są one sumowane do wartości całkowitej.
