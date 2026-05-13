# Examples

## Juffen

Juffen is the Dutch counting game behind this kind of challenge. Count upward,
but say `Juf` instead of the number when the number is a multiple of 7 or
contains the digit 7.

```platlang
# Print the Juffen sequence for 1 through 50.
funksie contains_seven(number):
    es number % 10 == 7:
        trok woar
    enj

    es number >= 70 en number < 80:
        trok woar
    enj

    trok kwatsj
enj

funksie is_juf(number):
    trok number % 7 == 0 of contains_seven(number)
enj

veur number = 1, 51:
    es is_juf(number):
        aafdrokke("Juf")
    angesj:
        aafdrokke(number)
    enj
enj
```

Output:

```text
1
2
3
4
5
6
Juf
8
9
10
11
12
13
Juf
15
16
Juf
18
19
20
Juf
22
23
24
25
26
Juf
Juf
29
30
31
32
33
34
Juf
36
Juf
38
39
40
41
Juf
43
44
45
46
Juf
48
Juf
50
```

## Greatest Common Divisor

Euclid's algorithm shows reusable global functions, mutation inside loops, and
return values.

```platlang
funksie abs(getal):
    es getal < 0:
        trok -getal
    enj

    trok getal
enj

funksie gcd(a, b):
    a = abs(a)
    b = abs(b)

    zolang b != 0:
        loat rest = a % b
        a = b
        b = rest
    enj

    trok a
enj

aafdrokke(gcd(84, 30))
aafdrokke(gcd(1071, 462))
```

Output:

```text
6
21
```

## Prime Numbers

This example uses a helper function with early returns and a loop that skips
unnecessary candidates.

```platlang
funksie is_prime(n):
    es n < 2:
        trok kwatsj
    enj

    es n == 2:
        trok woar
    enj

    es n % 2 == 0:
        trok kwatsj
    enj

    loat divisor = 3
    zolang divisor * divisor <= n:
        es n % divisor == 0:
            trok kwatsj
        enj

        divisor = divisor + 2
    enj

    trok woar
enj

aafdrokke("Prime numbers below 50:")

veur n = 1, 50:
    es is_prime(n):
        aafdrokke(n)
    enj
enj
```

Output:

```text
Prime numbers below 50:
2
3
5
7
11
13
17
19
23
29
31
37
41
43
47
```

## Sieve of Eratosthenes

Tables can act like mutable arrays. This sieve marks composite numbers and then
prints every prime below a limit.

```platlang
loat limit = 50
loat is_composite = {}

veur n = 0, limit + 1:
    is_composite[n] = kwatsj
enj

loat p = 2
zolang p * p <= limit:
    es !is_composite[p]:
        loat multiple = p * p

        zolang multiple <= limit:
            is_composite[multiple] = woar
            multiple = multiple + p
        enj
    enj

    p = p + 1
enj

aafdrokke("Primes from the sieve:")

veur n = 2, limit + 1:
    es !is_composite[n]:
        aafdrokke(n)
    enj
enj
```

Output:

```text
Primes from the sieve:
2
3
5
7
11
13
17
19
23
29
31
37
41
43
47
```

## Grade Summary

This is a small data-processing program. It stores scores in a table, computes a
total, tracks the minimum and maximum, and classifies the average.

```platlang
loat scores = { 92, 76, 88, 61, 95, 84 }
loat count = 6
loat total = 0
loat lowest = scores[0]
loat highest = scores[0]

veur i = 0, count:
    loat score = scores[i]
    total = total + score

    es score < lowest:
        lowest = score
    enj

    es score > highest:
        highest = score
    enj
enj

loat average = total / count

aafdrokke("average: " + average)
aafdrokke("lowest: " + lowest)
aafdrokke("highest: " + highest)

es average >= 90:
    aafdrokke("grade: A")
angesj:
    es average >= 80:
        aafdrokke("grade: B")
    angesj:
        es average >= 70:
            aafdrokke("grade: C")
        angesj:
            aafdrokke("grade: needs practice")
        enj
    enj
enj
```

Output:

```text
average: 82.6667
lowest: 61
highest: 95
grade: B
```

## Records And Totals

Tables can also act like records. This example models a tiny shopping cart with
nested record values and array-style numeric keys.

```platlang
funksie line_total(item):
    trok item.price * item.quantity
enj

loat cart = {
    { name = "notebook", price = 4.5, quantity = 3 },
    { name = "pencil", price = 1.25, quantity = 6 },
    { name = "bag", price = 18, quantity = 1 },
}

loat count = 3
loat subtotal = 0

veur i = 0, count:
    loat item = cart[i]
    loat total = line_total(item)

    aafdrokke(item.name + ": " + total)
    subtotal = subtotal + total
enj

loat tax = subtotal * 0.21
loat grand_total = subtotal + tax

aafdrokke("subtotal: " + subtotal)
aafdrokke("tax: " + tax)
aafdrokke("total: " + grand_total)
```

Output:

```text
notebook: 13.5
pencil: 7.5
bag: 18
subtotal: 39
tax: 8.19
total: 47.19
```

## Memoized Fibonacci

Recursive functions are useful, but plain recursive Fibonacci repeats a lot of
work. A table can hold previously computed answers.

```platlang
loat memo = {
    [0] = 0,
    [1] = 1,
}

funksie fib(n):
    loat cached = memo[n]

    es cached != niks:
        trok cached
    enj

    loat result = fib(n - 1) + fib(n - 2)
    memo[n] = result

    trok result
enj

veur n = 0, 16:
    aafdrokke("fib(" + n + ") = " + fib(n))
enj
```

Output:

```text
fib(0) = 0
fib(1) = 1
fib(2) = 1
fib(3) = 2
fib(4) = 3
fib(5) = 5
fib(6) = 8
fib(7) = 13
fib(8) = 21
fib(9) = 34
fib(10) = 55
fib(11) = 89
fib(12) = 144
fib(13) = 233
fib(14) = 377
fib(15) = 610
```

## Runtime Type Inspection

`waatis(value)` is useful when data can come from different places or when a
missing table key may produce `niks`.

```platlang
loat person = {
    name = "Mia",
    age = 31,
    active = woar,
}

waatis(person.name)
waatis(person.age)
waatis(person.active)
waatis(person.missing)
waatis(person)
```

Output:

```text
teks
nómmer
woar
niks
tabel
```

Tests use their own fixtures under `tests/`; examples are for readers.
