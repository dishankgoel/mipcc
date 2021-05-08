int max_prime = 10000;

void seive(int prime[]) {
    int i, j;
    for(i = 0; i <= max_prime; i++) {
        prime[i] = 1;
    }
    for(i = 2; i*i <= max_prime; i++) {
        if(prime[i] == 1) {
            for(j = i*i; j <= max_prime; j = j + i) {
                prime[j] = 0;
            }
        }
    }
    return;
}

void main() {
    int n;
    int prime[10000];
    /* Calculates the seive  */
    seive(prime); 
    print("Get all primes <= N\nEnter N: ");
    scan(n);
    int i;
    print("Primes: ");
    for(i = 2; i <= n; i++) {
        if(prime[i] == 1) {
            print(i, ", ");
        }
    }
    return;
}