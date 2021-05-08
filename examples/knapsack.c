// A Dynamic Programming based
// solution for 0-1 Knapsack problem

// A utility function that returns
// maximum of two integers
int max(int a, int b)
{
	if(a > b) {
        return a;
    } else {
        return b;
    }
}

// Returns the maximum value that
// can be put in a knapsack of capacity W
int knapSack(int W, int wt[], int val[], int n)
{
	int i, w;
	int K[101][101];

	// Build table K[][] in bottom up manner
	for (i = 0; i <= n; i++)
	{
		for (w = 0; w <= W; w++)
		{
			if (i == 0 || w == 0)
				K[i][w] = 0;
			else if (wt[i - 1] <= w)
				K[i][w] = max(val[i - 1]
						+ K[i - 1][w - wt[i - 1]],
						K[i - 1][w]);
			else
				K[i][w] = K[i - 1][w];
		}
	}
	return K[n][W];
}

// Driver Code
int main()
{
	int n;
	int W;
	int val[100];
	int wt[100];
    print("Enter number of items: ");
    scan(n);
    print("Enter the total capacity: ");
    scan(W);
    int i;
    for(i = 0; i < n; i++) {
        print("Enter value of item ", i + 1, ": ");
        scan(val[i]);
    }
    for(i = 0; i < n; i++) {
        print("Enter weight of item ", i + 1, ": ");
        scan(wt[i]);
    }
	print("Optimal value: ", knapSack(W, wt, val, n), "\n");
	return 0;
}
