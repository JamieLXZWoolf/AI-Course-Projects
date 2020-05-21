//
// Created by Jamie on 5/17/2020.
//

#include <iostream>
#include <ctime>
using namespace std;

#define lli long long int
#define TIMES 20

const int C[]={30,50,80,100};

lli getQS4InitConflicts(lli n);

unsigned lli RandSeed = (unsigned) time(nullptr);

lli random(lli max);

void swap(lli &a, lli &b);

lli getConflicts(lli n, const lli pDiags[], const lli nDiags[]);

void initQS1(lli queen[], lli n, lli pDiags[], lli nDiags[]);

void initQS4(lli queen[], lli n, lli m, lli pDiags[], lli nDiags[]);

lli getDelta(lli i, lli j, const lli pDiags[], const lli nDiags[], const lli queen[], lli n);

double QS1(lli n, int & steps, double & initTime);

double QS4(lli n, int & steps, double & initTime);

int main() {
    lli n;
    int steps, sumSteps = 0;
    double totalTime = .0;
    double totalInit = 0;
    cout << "Enter the number of queens:";
    cin>>n;
    for (int i = 0; i < TIMES; i++) {
        double initT = .0;
        cout << "QS4 Round " << i+1 << endl;
        cout << "---------------------------------------------------" << endl;
        totalTime += QS4(n, steps, initT);
        sumSteps+= steps;
        totalInit+=initT;
        cout << "---------------------------------------------------" << endl;
    }
    cout << "QS4 Average initialization time: " << totalInit / TIMES << endl;
    cout << "QS4 Average time used: " << totalTime / TIMES << endl;
    cout << "QS4 Average steps: " << sumSteps / TIMES << endl;
    cout << "---------------------------------------------------" << endl;

    if (n > 100000) {
        int tp;
        cout << "Execute QS1? no [0]" << endl;
        cin >> tp;
        if (!tp) return 0;
    }
    totalTime = totalInit = .0, sumSteps = 0;
    for (int i = 0; i < TIMES; i++) {
        double initT = .0;
        cout << "QS1 Round " << i+1 << endl;
        cout << "---------------------------------------------------" << endl;
        totalTime += QS1(n, steps, initT);
        totalInit += initT;
        sumSteps+= steps;
        cout << "---------------------------------------------------" << endl;
    }
    cout << "QS1 Average initialization time: " << totalInit / TIMES << endl;
    cout << "QS1 Average time used: " << totalTime / TIMES << endl;
    cout << "QS1 Average steps: " << sumSteps / TIMES << endl;
}

lli getQS4InitConflicts(lli n) {
    if(n<=10)	return (n>8) ? 8 : n;
    else if (n<100)		return n;
    else if (n<1000)	return C[0];
    else if (n<10000)	return C[1];
    else if (n<100000)	return C[2];
    else				return C[3];
}

lli random(lli max) {
    unsigned lli x ;
    double i ;

    x = 0x7fffffff;
    x += 1;
    RandSeed *= ((unsigned lli)134775813);
    RandSeed += 1;
    RandSeed = RandSeed % x;
    i = ((double)RandSeed) / (double)0x7fffffff;

    return (lli) ((double) max * i);
}

void swap(lli &a, lli &b) {
    if (a!=b)
    {
        lli t;
        t = a;
        a = b;
        b = t;
    }
}

lli getConflicts(lli n, const lli pDiags[], const lli nDiags[]) {
    lli ans = 0;
    lli i;

    for (i = 0; i < n+n; i++)
    {
        if (pDiags[i] > 1)
            ans += pDiags[i] * (pDiags[i]-1)/2;
        if (nDiags[i] > 1)
            ans += nDiags[i] * (nDiags[i]-1)/2;
    }

    return ans;
}

lli getDelta(lli i, lli j, const lli pDiags[], const lli nDiags[], const lli queen[], lli n) {
    lli delta = 0;
    delta -= pDiags[i - queen[i] + n - 1] - 1;
    delta -= pDiags[j - queen[j] + n - 1] - 1;
    delta -= nDiags[i + queen[i]] - 1;
    delta -= nDiags[j + queen[j]] - 1;

    delta += pDiags[j - queen[i] + n - 1];
    delta += pDiags[i - queen[j] + n - 1];
    delta += nDiags[j + queen[i]];
    delta += nDiags[i + queen[j]];

    if ((i + queen[i] == j + queen[j]) || (i - queen[i] == j - queen[j])) delta+=2;
    return delta;
}

void initQS1(lli queen[], lli n, lli pDiags[], lli nDiags[]) {
    lli i, rg, j;

    for( i = 0; i < n; i++) {
        pDiags[i] = 0;
        nDiags[i] = 0;
        queen[i] = i;
    }
    for (i = n; i < n + n; i++) {
        pDiags[i] = 0;
        nDiags[i] = 0;
    }

    for (i = 0, rg = n; i < n; i++, rg--) {
        j = i + random(rg);
        swap(queen[i], queen[j]);
        pDiags[i - queen[i] + n - 1]++;
        nDiags[i + queen[i]]++;
    }
}

void initQS4(lli queen[], lli n, lli m, lli pDiags[], lli nDiags[]) {
    lli i, rg, j;
    bool *pB = (bool*)malloc(sizeof(bool) * (n + n));
    bool *nB = (bool*)malloc(sizeof(bool) * (n + n));

    for(i = 0; i < n; i++) {
        pDiags[i] = 0;
        nDiags[i] = 0;
        pB[i] = false;
        nB[i] = false;
        queen[i] = i;
    }

    for (i = n; i < n + n; i++) {
        pDiags[i] = 0;
        nDiags[i] = 0;
        pB[i] = false;
        nB[i] = false;
    }

    for (i = 0, rg = n; i < m; i++, rg--) {
        do j = i + random(rg); while (pB[i - queen[j] + n - 1] || nB[i + queen[j]]);
        swap(queen[i], queen[j]);
        pDiags[i - queen[i] + n - 1]++;
        nDiags[i + queen[i]]++;
        pB[i - queen[i] + n - 1] = true;
        nB[i + queen[i]] = true;
    }

    for (i = m, rg = n - m; i < n; i++, rg--) {
        j = i + random(rg);
        swap(queen[i], queen[j]);
        pDiags[i - queen[i] + n - 1]++;
        nDiags[i + queen[i]]++;
    }

    free(pB), free(nB);
}

double QS1(lli n, int & steps, double & initTime) {
    steps = 0;
    lli currentConflicts, delta, i, j;

    lli* queen = (lli*) malloc(sizeof(lli) * n);
    lli* pDiags = (lli*)malloc( sizeof(lli) * (n+n) );
    lli* nDiags = (lli*)malloc( sizeof(lli) * (n+n) );

    clock_t start, finish;
    start = clock();

    initQS1(queen, n, pDiags, nDiags);

    finish = clock();
    initTime = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << "Init Time: "<< initTime << endl;

    currentConflicts = getConflicts(n, pDiags, nDiags);
    while (currentConflicts > 0) {
        delta = 0;

        for ( i = 0; i < n; i++)
            if ((pDiags[i-queen[i]+n-1]==1) && (nDiags[i+queen[i]]==1)) continue;
            else {
                for (j = 0; j < n; j++) {
                    if (i != j) {
                        delta = getDelta(i, j, pDiags, nDiags, queen, n);
                        if (delta < 0) break;
                    }
                }
                if (delta < 0) break;
            }

        if (delta < 0) {
            steps++;
            pDiags[i-queen[i]+n-1]--;
            pDiags[j-queen[j]+n-1]--;
            nDiags[i+queen[i]]--;
            nDiags[j+queen[j]]--;

            swap(queen[i], queen[j]);

            pDiags[i-queen[i]+n-1]++;
            pDiags[j-queen[j]+n-1]++;
            nDiags[i+queen[i]]++;
            nDiags[j+queen[j]]++;
            currentConflicts += delta;
        } else {
            finish = clock();
            cout << "Time: "<< (double)(finish - start) / CLOCKS_PER_SEC<<endl;
            cout << "Reinitialize..." << endl;
            initQS1(queen, n, pDiags, nDiags);
            finish = clock();
            cout << "Init Time: " << (double)(finish - start) / CLOCKS_PER_SEC<<endl;
            currentConflicts = getConflicts(n, pDiags, nDiags);
            cout << "Conflicts: " << currentConflicts << endl;
        }
    }
    finish = clock();
    auto usedTime = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << "Finished. Time: "<< usedTime << " Steps: " << steps << endl;
    free(queen);
    free(pDiags);
    free(nDiags);
    return usedTime;
}

double QS4(lli n, int & steps, double & initTime) {
    steps = 0;
    lli currentConflicts, delta, i, j;
    lli m = n - getQS4InitConflicts(n);
    lli* queen = (lli*) malloc(sizeof(lli) * n);
    lli* pDiags = (lli*)malloc( sizeof(lli) * (n+n) );
    lli* nDiags = (lli*)malloc( sizeof(lli) * (n+n) );

    clock_t start, finish;
    start = clock();

    initQS4(queen, n, m, pDiags, nDiags);

    finish = clock();
    initTime = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << "Init Time: "<< initTime << endl;

    currentConflicts = getConflicts(n, pDiags, nDiags);

    while (currentConflicts > 0) {
        delta = 0;

        for ( i = m; i < n; i++)
            if ((pDiags[i-queen[i]+n-1]==1) && (nDiags[i+queen[i]]==1)) continue;
            else {
                for (j = 0; j < n; j++) {
                    if (i != j) {
                        delta = getDelta(i, j, pDiags, nDiags, queen, n);
                        if (delta < 0) break;
                    }
                }
                if (delta < 0) break;
            }

        if (delta < 0) {
            steps++;
            pDiags[i-queen[i]+n-1]--;
            pDiags[j-queen[j]+n-1]--;
            nDiags[i+queen[i]]--;
            nDiags[j+queen[j]]--;

            swap(queen[i], queen[j]);

            pDiags[i-queen[i]+n-1]++;
            pDiags[j-queen[j]+n-1]++;
            nDiags[i+queen[i]]++;
            nDiags[j+queen[j]]++;

            currentConflicts += delta;

        } else {
            finish = clock();
            cout << "Time: "<< (double)(finish - start) / CLOCKS_PER_SEC<<endl;
            cout << "Reinitialize..." << endl;
            initQS4(queen, n, m, pDiags, nDiags);
            finish = clock();
            cout << "Init Time: " << (double)(finish - start) / CLOCKS_PER_SEC<<endl;
            currentConflicts = getConflicts(n, pDiags, nDiags);
            cout << "Conflicts: " << currentConflicts << endl;
        }
    }

    finish = clock();
    auto usedTime = (double)(finish - start) / CLOCKS_PER_SEC;
    cout << "Finished. Time: "<< usedTime << " Steps: " << steps << endl;

    free(queen);
    free(pDiags);
    free(nDiags);
    return usedTime;
}