int main() {
    int i, n, t1 = 0, t2 = 1;
    int nextTerm = t1 + t2;
    print("Enter the number of terms: ");
    scan(n);
    print("Fibonacci Series: ", t1, ", ", t2, ", ");
    for (i = 1; i <= n; ++i) {
        print(nextTerm, ", ");
        t1 = t2;
        t2 = nextTerm;
        nextTerm = t1 + t2;
    }
    return 0;

}
